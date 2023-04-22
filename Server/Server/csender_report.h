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

#ifndef _C_SENDER_REPORT_H_
#define _C_SENDER_REPORT_H_
#include "cnet_type.h"
#include "clog.h"
#include "cfunction_view.h"
#include "carray_view.h"
#include "buffer.h"
#include "crtcp_packet.h"
#include "cntp_time.h"
#include "creport_block.h"
#include <vector>

namespace chen {

	namespace rtcp {
		class  CommonHeader;
		class csender_report : public crtcp_packet
		{
		public:
			static const  uint8 kPacketType = 200;
			static const  size_t kMaxNumberOfReportBlocks = 0x1f;
		public:
			csender_report();
			csender_report(const csender_report&);
			csender_report(csender_report&&);
			csender_report& operator=(const csender_report&);
			csender_report& operator=(csender_report&&);
			~csender_report() override;

			// Parse assumes header is already parsed and validated.
			bool Parse(const CommonHeader& packet);

			void SetSenderSsrc(uint32_t ssrc) { sender_ssrc_ = ssrc; }
			void SetNtp(NtpTime ntp) { ntp_ = ntp; }
			void SetRtpTimestamp(uint32_t rtp_timestamp) {
				rtp_timestamp_ = rtp_timestamp;
			}
			void SetPacketCount(uint32_t packet_count) {
				sender_packet_count_ = packet_count;
			}
			void SetOctetCount(uint32_t octet_count) {
				sender_octet_count_ = octet_count;
			}
			bool AddReportBlock(const creport_block& block);
			bool SetReportBlocks(std::vector<creport_block> blocks);
			void ClearReportBlocks() { report_blocks_.clear(); }

			uint32_t sender_ssrc() const { return sender_ssrc_; }
			NtpTime ntp() const { return ntp_; }
			uint32_t rtp_timestamp() const { return rtp_timestamp_; }
			uint32_t sender_packet_count() const { return sender_packet_count_; }
			uint32_t sender_octet_count() const { return sender_octet_count_; }

			const std::vector<creport_block>& report_blocks() const {
				return report_blocks_;
			}

			size_t BlockLength() const override;

			bool Create(uint8_t* packet,
				size_t* index,
				size_t max_length,
				PacketReadyCallback callback) const override;

		private:
			static constexpr size_t kSenderBaseLength = 24;

			uint32_t sender_ssrc_;
			NtpTime ntp_;
			uint32_t rtp_timestamp_;
			uint32_t sender_packet_count_;
			uint32_t sender_octet_count_;
			std::vector<creport_block> report_blocks_;
		};
	}
}

#endif