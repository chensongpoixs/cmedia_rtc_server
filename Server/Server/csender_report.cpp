/***********************************************************************************************
created: 		2023-04-03

author:			chensong

purpose:		api_rtc_publish


************************************************************************************************/
#include "csender_report.h"
#include "ccommon_header.h"
#include "cbyte_io.h"
#include "clog.h"
namespace chen {
	namespace rtcp {
		//    Sender report (SR) (RFC 3550).
		//     0                   1                   2                   3
		//     0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
		//    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		//    |V=2|P|    RC   |   PT=SR=200   |             length            |
		//    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		//  0 |                         SSRC of sender                        |
		//    +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
		//  4 |              NTP timestamp, most significant word             |
		//    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		//  8 |             NTP timestamp, least significant word             |
		//    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		// 12 |                         RTP timestamp                         |
		//    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		// 16 |                     sender's packet count                     |
		//    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		// 20 |                      sender's octet count                     |
		// 24 +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+

		csender_report::csender_report()
			: sender_ssrc_(0),
			rtp_timestamp_(0),
			sender_packet_count_(0),
			sender_octet_count_(0) {}

		csender_report::csender_report(const csender_report&) = default;
		csender_report::csender_report(csender_report&&) = default;
		csender_report& csender_report::operator=(const csender_report&) = default;
		csender_report& csender_report::operator=(csender_report&&) = default;
		csender_report::~csender_report() = default;

		bool csender_report::Parse(const CommonHeader& packet)
		{
			RTC_DCHECK_EQ(packet.type(), kPacketType);

			const uint8_t report_block_count = packet.count();
			if (packet.payload_size_bytes() < kSenderBaseLength/*24*/ + report_block_count * creport_block::kLength /*24*/)
			{
				WARNING_EX_LOG("Packet is too small to contain all the data.");
				return false;
			}
			// Read SenderReport header.
			const uint8_t* const payload = packet.payload();
			// 发送端ssrc
			sender_ssrc_ = ByteReader<uint32_t>::ReadBigEndian(&payload[0]);
			uint32_t secs = ByteReader<uint32_t>::ReadBigEndian(&payload[4]);
			uint32_t frac = ByteReader<uint32_t>::ReadBigEndian(&payload[8]);
			ntp_.Set(secs, frac);
			// rtp 网络时间戳
			rtp_timestamp_ = ByteReader<uint32_t>::ReadBigEndian(&payload[12]);
			// 发送的总包数
			sender_packet_count_ = ByteReader<uint32_t>::ReadBigEndian(&payload[16]);
			// 总共发送数据包量
			sender_octet_count_ = ByteReader<uint32_t>::ReadBigEndian(&payload[20]);
			report_blocks_.resize(report_block_count);
			const uint8_t* next_block = payload + kSenderBaseLength;
			for (creport_block& block : report_blocks_)
			{
				bool block_parsed = block.Parse(next_block, creport_block::kLength);
				RTC_DCHECK(block_parsed);
				next_block += creport_block::kLength;
			}
			// Double check we didn't read beyond provided buffer.
			RTC_DCHECK_LE(next_block - payload, static_cast<ptrdiff_t>(packet.payload_size_bytes()));
			return true;
		}

		size_t csender_report::BlockLength() const {
			return kHeaderLength + kSenderBaseLength +
				report_blocks_.size() * creport_block::kLength;
		}

		bool csender_report::Create(uint8_t* packet,
			size_t* index,
			size_t max_length,
			PacketReadyCallback callback) const {
			while (*index + BlockLength() > max_length) {
				if (!OnBufferFull(packet, index, callback))
					return false;
			}
			const size_t index_end = *index + BlockLength();

			CreateHeader(report_blocks_.size(), kPacketType, HeaderLength(), packet,
				index);
			// Write SenderReport header.
			ByteWriter<uint32_t>::WriteBigEndian(&packet[*index + 0], sender_ssrc_);
			ByteWriter<uint32_t>::WriteBigEndian(&packet[*index + 4], ntp_.seconds());
			ByteWriter<uint32_t>::WriteBigEndian(&packet[*index + 8], ntp_.fractions());
			ByteWriter<uint32_t>::WriteBigEndian(&packet[*index + 12], rtp_timestamp_);
			ByteWriter<uint32_t>::WriteBigEndian(&packet[*index + 16],
				sender_packet_count_);
			ByteWriter<uint32_t>::WriteBigEndian(&packet[*index + 20],
				sender_octet_count_);
			*index += kSenderBaseLength;
			// Write report blocks.
			for (const creport_block& block : report_blocks_) {
				block.Create(packet + *index);
				*index += creport_block::kLength;
			}
			// Ensure bytes written match expected.
			RTC_DCHECK_EQ(*index, index_end);
			return true;
		}

		bool csender_report::AddReportBlock(const creport_block& block) {
			if (report_blocks_.size() >= kMaxNumberOfReportBlocks) {
				WARNING_EX_LOG("Max report blocks reached.");
				return false;
			}
			report_blocks_.push_back(block);
			return true;
		}

		bool csender_report::SetReportBlocks(std::vector<creport_block> blocks) {
			if (blocks.size() > kMaxNumberOfReportBlocks)
			{
				WARNING_EX_LOG( "Too many report blocks (%u)for sender report.",blocks.size());
				return false;
			}
			report_blocks_ = std::move(blocks);
			return true;
		}

	}
}