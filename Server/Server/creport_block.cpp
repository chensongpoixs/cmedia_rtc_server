﻿/***********************************************************************************************
created: 		2023-04-03

author:			chensong

purpose:		api_rtc_publish


************************************************************************************************/
#include "creport_block.h"
#include "cbyte_io.h"
namespace chen {
	namespace rtcp {

		// From RFC 3550, RTP: A Transport Protocol for Real-Time Applications.
		//
		// RTCP report block (RFC 3550).
		//
		//     0                   1                   2                   3
		//     0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
		//    +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
		//  0 |                 SSRC_1 (SSRC of first source)                 |
		//    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		//  4 | fraction lost |       cumulative number of packets lost       |
		//    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		//  8 |           extended highest sequence number received           |
		//    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		// 12 |                      interarrival jitter                      |
		//    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		// 16 |                         last SR (LSR)                         |
		//    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		// 20 |                   delay since last SR (DLSR)                  |
		// 24 +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
		creport_block::creport_block()
			: source_ssrc_(0),
			fraction_lost_(0),
			cumulative_lost_(0),
			extended_high_seq_num_(0),
			jitter_(0),
			last_sr_(0),
			delay_since_last_sr_(0) {}

		bool creport_block::Parse(const uint8_t* buffer, size_t length) 
		{
			RTC_DCHECK(buffer != nullptr);
			if (length < creport_block::kLength)
			{
				WARNING_EX_LOG( "Report Block should be 24 bytes long");
				return false;
			}
			// 接收到的媒体源ssrc
			source_ssrc_ = ByteReader<uint32_t>::ReadBigEndian(&buffer[0]);
			// TODO@chensong 2022-10-19  丢包率 fraction_lost
			/**
			TODO@chensong 2023-03-07  
			某时刻收到的有序包的数量Count = transmitted-retransmitte,当前时刻为Count2,上一时刻为Count1;

			接收端以一定的频率发送RTCP包（RR、REMB、NACK等）时，会统计两次发送间隔之间(fraction)的接收包信息。

			接收端发送的RR包中包含两个丢包:

			一个是fraction_lost，是两次统计间隔间的丢包率(以256为基数换算成8bit)。

			一个是cumulative number of packets lost，是总的累积丢包。 
			**/
			fraction_lost_ = buffer[4];
			// 接收开始丢包总数， 迟到包不算丢包，重传有可以导致负数
			cumulative_lost_ = ByteReader<int32_t, 3>::ReadBigEndian(&buffer[5]);
			// 低16位表示收到的最大seq，高16位表示seq循环次数
			extended_high_seq_num_ = ByteReader<uint32_t>::ReadBigEndian(&buffer[8]);
			// rtp包到达时间间隔的统计方差
			jitter_ = ByteReader<uint32_t>::ReadBigEndian(&buffer[12]);
			// ntp时间戳的中间32位
			last_sr_ = ByteReader<uint32_t>::ReadBigEndian(&buffer[16]);
			// 记录上一个接收SR的时间与上一个发送SR的时间差
			delay_since_last_sr_ = ByteReader<uint32_t>::ReadBigEndian(&buffer[20]);

			return true;
		}

		void creport_block::Create(uint8_t* buffer) const {
			// Runtime check should be done while setting cumulative_lost.
			RTC_DCHECK_LT(cumulative_lost_signed(),
				(1 << 23));  // Have only 3 bytes for it.

			ByteWriter<uint32_t>::WriteBigEndian(&buffer[0], source_ssrc());
			ByteWriter<uint8_t>::WriteBigEndian(&buffer[4], fraction_lost());
			ByteWriter<int32_t, 3>::WriteBigEndian(&buffer[5], cumulative_lost_signed());
			ByteWriter<uint32_t>::WriteBigEndian(&buffer[8], extended_high_seq_num());
			ByteWriter<uint32_t>::WriteBigEndian(&buffer[12], jitter());
			ByteWriter<uint32_t>::WriteBigEndian(&buffer[16], last_sr());
			ByteWriter<uint32_t>::WriteBigEndian(&buffer[20], delay_since_last_sr());
		}

		bool creport_block::SetCumulativeLost(int32_t cumulative_lost) {
			// We have only 3 bytes to store it, and it's a signed value.
			if (cumulative_lost >= (1 << 23) || cumulative_lost < -(1 << 23)) {
				WARNING_EX_LOG( "Cumulative lost is too big to fit into Report Block");
				return false;
			}
			cumulative_lost_ = cumulative_lost;
			return true;
		}

		uint32_t creport_block::cumulative_lost() const {
			if (cumulative_lost_ < 0) {
				WARNING_EX_LOG( "Ignoring negative value of cumulative_lost");
				return 0;
			}
			return cumulative_lost_;
		}

	}
}