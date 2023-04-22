/***********************************************************************************************
created: 		2023-04-03

author:			chensong

purpose:		api_rtc_publish
输赢不重要，答案对你们有什么意义才重要。

光阴者，百代之过客也，唯有奋力奔跑，方能生风起时，是时代造英雄，英雄存在于时代。或许世人道你轻狂，可你本就年少啊。 看护好，自己的理想和激情。


我可能会遇到很多的人，听他们讲好2多的故事，我来写成故事或编成歌，用我学来的各种乐器演奏它。
然后还可能在一个国家遇到一个心仪我的姑娘，她可能会被我帅气的外表捕获，又会被我深邃的内涵吸引，在某个下雨的夜晚，她会全身淋透然后要在我狭小的住处换身上的湿衣服。
3小时候后她告诉我她其实是这个国家的公主，她愿意向父皇求婚。我不得已告诉她我是穿越而来的男主角，我始终要回到自己的世界。
然后我的身影慢慢消失，我看到她眼里的泪水，心里却没有任何痛苦，我才知道，原来我的心被丢掉了，我游历全世界的原因，就是要找回自己的本心。
于是我开始有意寻找各种各样失去心的人，我变成一块砖头，一颗树，一滴水，一朵白云，去听大家为什么会失去自己的本心。
我发现，刚出生的宝宝，本心还在，慢慢的，他们的本心就会消失，收到了各种黑暗之光的侵蚀。
从一次争论，到嫉妒和悲愤，还有委屈和痛苦，我看到一只只无形的手，把他们的本心扯碎，蒙蔽，偷走，再也回不到主人都身边。
我叫他本心猎手。他可能是和宇宙同在的级别 但是我并不害怕，我仔细回忆自己平淡的一生 寻找本心猎手的痕迹。
沿着自己的回忆，一个个的场景忽闪而过，最后发现，我的本心，在我写代码的时候，会回来。
安静，淡然，代码就是我的一切，写代码就是我本心回归的最好方式，我还没找到本心猎手，但我相信，顺着这个线索，我一定能顺藤摸瓜，把他揪出来。

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