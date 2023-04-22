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