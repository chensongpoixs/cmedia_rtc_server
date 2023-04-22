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

#ifndef _C_REPORT_BLOCK_H_
#define _C_REPORT_BLOCK_H_
#include "cnet_type.h"
#include "clog.h"
#include "cfunction_view.h"
#include "carray_view.h"
#include "buffer.h"

namespace chen {
	namespace rtcp {
		// A ReportBlock represents the Sender Report packet from
		// RFC 3550 section 6.4.1.
		class creport_block {
		public:
			static const size_t kLength = 24;

			creport_block();
			~creport_block() {}

			bool Parse(const uint8_t* buffer, size_t length);

			// Fills buffer with the ReportBlock.
			// Consumes ReportBlock::kLength bytes.
			void Create(uint8_t* buffer) const;

			void SetMediaSsrc(uint32_t ssrc) { source_ssrc_ = ssrc; }
			void SetFractionLost(uint8_t fraction_lost) {
				fraction_lost_ = fraction_lost;
			}
			bool SetCumulativeLost(int32_t cumulative_lost);
			void SetExtHighestSeqNum(uint32_t ext_highest_seq_num) {
				extended_high_seq_num_ = ext_highest_seq_num;
			}
			void SetJitter(uint32_t jitter) { jitter_ = jitter; }
			void SetLastSr(uint32_t last_sr) { last_sr_ = last_sr; }
			void SetDelayLastSr(uint32_t delay_last_sr) {
				delay_since_last_sr_ = delay_last_sr;
			}

			uint32_t source_ssrc() const { return source_ssrc_; }
			uint8_t fraction_lost() const { return fraction_lost_; }
			int32_t cumulative_lost_signed() const { return cumulative_lost_; }
			// Deprecated - returns max(0, cumulative_lost_), not negative values.
			uint32_t cumulative_lost() const;
			uint32_t extended_high_seq_num() const { return extended_high_seq_num_; }
			uint32_t jitter() const { return jitter_; }
			uint32_t last_sr() const { return last_sr_; }
			uint32_t delay_since_last_sr() const { return delay_since_last_sr_; }

		private:
			uint32_t source_ssrc_;     // 32 bits 接收到的每个媒体源
			uint8_t fraction_lost_;    // 8 bits representing a fixed point value 0..1 上一次报告之后从SSRC_n来包的漏包比列
			int32_t cumulative_lost_;  // Signed 24-bit value 自接收开始漏包总数，迟到包不算漏包，重传有可以导致负数
			uint32_t extended_high_seq_num_;  // 32 bits  低16位表式收到的最大seq，高16位表式seq循环次数 
			uint32_t jitter_;                 // 32 bits RTP包到达时间间隔的统计方差
			uint32_t last_sr_;                // 32 bits NTP时间戳的中间32位
			uint32_t delay_since_last_sr_;    // 32 bits, units of 1/65536 seconds//记录接收SR的时间与发送SR的时间差
		};
	}
}

#endif //