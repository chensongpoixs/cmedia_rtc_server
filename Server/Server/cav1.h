#ifndef _C_AV1_H_
#define _C_AV1_H_
/*
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
*/
//#include "common.hpp"
#include "PayloadDescriptorHandler.hpp"
#include "RtpPacket.hpp"
#include "cnet_type.h"

// AV1 format:
//
// RTP payload syntax:
//     0 1 2 3 4 5 6 7
//    +-+-+-+-+-+-+-+-+
//    |Z|Y| W |N|-|-|-| (REQUIRED)
//    +=+=+=+=+=+=+=+=+ (REPEATED W-1 times, or any times if W = 0)
//    |1|             |
//    +-+ OBU fragment|
//    |1|             | (REQUIRED, leb128 encoded)
//    +-+    size     |
//    |0|             |
//    +-+-+-+-+-+-+-+-+
//    |  OBU fragment |
//    |     ...       |
//    +=+=+=+=+=+=+=+=+
//    |     ...       |
//    +=+=+=+=+=+=+=+=+ if W > 0, last fragment MUST NOT have size field
//    |  OBU fragment |
//    |     ...       |
//    +=+=+=+=+=+=+=+=+
//
//
// OBU syntax:
//     0 1 2 3 4 5 6 7
//    +-+-+-+-+-+-+-+-+
//    |0| type  |X|S|-| (REQUIRED)
//    +-+-+-+-+-+-+-+-+
// X: | TID |SID|-|-|-| (OPTIONAL)
//    +-+-+-+-+-+-+-+-+
//    |1|             |
//    +-+ OBU payload |
// S: |1|             | (OPTIONAL, variable length leb128 encoded)
//    +-+    size     |
//    |0|             |
//    +-+-+-+-+-+-+-+-+
//    |  OBU payload  |
//    |     ...       |
namespace chen
{
	class cav1
	{
	public:
		struct PayloadDescriptor : public RTC::Codecs::PayloadDescriptor
		{
			/* Pure virtual methods inherited from RTC::Codecs::PayloadDescriptor. */
			~PayloadDescriptor() = default;

			void Dump() const override {}
			//Z|Y| W |N
			uint8_t z : 1;          // first obu element is continuation of the previous OBU.
			uint8_t y : 1;          //   last obu element will be continuated in the next packet.
			uint8_t w : 2;          //  obu elements size [0, 3]
			uint8_t n : 1; // keyframe
			 
			bool isKeyFrame{ false };
		};

	public:
		static cav1::PayloadDescriptor* Parse(
			const uint8_t* data,
			size_t len,
			RTC::RtpPacket::FrameMarking* frameMarking = nullptr,
			uint8_t frameMarkingLen = 0);
		static void ProcessRtpPacket(RTC::RtpPacket* packet);



		static int32 RtpStartsNewCodedVideoSequence(uint8 aggregation_header) { return aggregation_header & 0b0000'1000u; }
		static bool RtpStartsWithFragment(uint8 aggregation_header) { return aggregation_header & 0b1000'0000u; }
		static bool RtpEndsWithFragment(uint8 aggregation_header) { return aggregation_header & 0b0100'0000u; }
	public:
		class EncodingContext : public RTC::Codecs::EncodingContext
		{
		public:
			explicit EncodingContext(RTC::Codecs::EncodingContext::Params& params)
				: RTC::Codecs::EncodingContext(params)
			{
			}
			~EncodingContext() = default;

			/* Pure virtual methods inherited from RTC::Codecs::EncodingContext. */
		public:
			void SyncRequired() override
			{
			}
		};

	public:
		class PayloadDescriptorHandler : public RTC::Codecs::PayloadDescriptorHandler
		{
		public:
			explicit PayloadDescriptorHandler(PayloadDescriptor* payloadDescriptor): payloadDescriptor(payloadDescriptor){}
			~PayloadDescriptorHandler() = default;

		public:
			void Dump() const override
			{
				this->payloadDescriptor->Dump();
			}
			bool Process(RTC::Codecs::EncodingContext* encodingContext, uint8_t* data, bool& marker) override { return false; }
			void Restore(uint8_t* data) override {}
			uint8_t GetSpatialLayer() const override
			{
				return 0u;
			}
			uint8_t GetTemporalLayer() const override
			{
				return 0;
			}
			bool IsKeyFrame() const override
			{
				return this->payloadDescriptor->isKeyFrame;
			}

		private:
			std::unique_ptr<PayloadDescriptor> payloadDescriptor;
		};
	};
}
#endif // 