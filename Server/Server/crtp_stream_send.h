/***********************************************************************************************
created: 		2022-08-10

author:			chensong

purpose:		room

Copyright boost
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

#ifndef _C_RTP_STREAM_SEND_H_
#define _C_RTP_STREAM_SEND_H_
#include <string>
#include "cnet_type.h"
#include "crtp_rtcp.h"
#include "RtpPacket.hpp"
#include "SenderReport.hpp"
#include "crate_calculator.h"
#include "ReceiverReport.hpp"
#include "crtp_stream.h"
#include "FeedbackRtpNack.hpp"
#include "RateCalculator.hpp"

namespace chen {
	class crtc_consumer;
	class crtp_stream_send : public crtp_stream
	{
	public:
		struct StorageItem
		{
			// Cloned packet.
			RTC::RtpPacket* packet{ nullptr };
			// Memory to hold the cloned packet (with extra space for RTX encoding).
			uint8  store[RTC::MtuSize + 100];
			// Last time this packet was resent.
			uint64  resentAtMs{ 0u };
			// Number of times this packet was resent.
			uint8  sentTimes{ 0u };
			// Whether the packet has been already RTX encoded.
			bool rtxEncoded{ false };
		};
	public:
		crtp_stream_send(crtc_consumer * ptr, const crtp_stream::crtp_stream_params & params)
		: m_rtc_consumer_ptr(ptr)
		, crtp_stream(params)
		, m_lost_prior_score(0u)
		, m_sent_prior_score(0u)
		, m_buffer(65536, nullptr)
		, m_buffer_start_idx(0u)
		, m_buffer_size(0u)
		, m_storage(600u)
		, m_rtx_seq(0u)
		{}
		~crtp_stream_send();
	public:
		void set_rtx(uint8 payload_type, uint32 ssrc);

		bool receive_packet(RTC::RtpPacket* packet);

		//void receive_nack();//crtcp_nack
		void receive_nack(RTC::RTCP::FeedbackRtpNackPacket* nackPacket);
		/*void ReceiveKeyFrameRequest(RTC::RTCP::FeedbackPs::MessageType messageType);*/
		void receive_rtcp_receiver_report(RTC::RTCP::ReceiverReport* report);
		/*RTC::RTCP::SenderReport* GetRtcpSenderReport(uint64_t nowMs);*/
		//RTC::RTCP::SdesChunk* GetRtcpSdesChunk();



		void receive_key_frame_request(RTC::RTCP::FeedbackPs::MessageType messageType);
	public:
		virtual void pause() ;
		virtual void resume();
		virtual uint32  get_bitrate(uint64  nowMs);
	protected:

	private:
		void _store_packet(RTC::RtpPacket * packet);
		void _clear_buffer();
		void UpdateBufferStartIdx();
		void UpdateScore(RTC::RTCP::ReceiverReport* report);
		 void FillRetransmissionContainer(uint16_t seq, uint16_t bitmask);
	private:
		crtc_consumer	*			m_rtc_consumer_ptr;
		uint32						m_lost_prior_score{ 0u }; // Packets lost at last interval for score calculation.
		uint32						m_sent_prior_score{ 0u }; // Packets sent at last interval for score calculation.
		std::vector<StorageItem*>	m_buffer;
		uint16						m_buffer_start_idx{ 0u };
		size_t						m_buffer_size{ 0u };
		std::vector<StorageItem>	m_storage;
		uint16						m_rtx_seq{ 0u };
		 RTC::RtpDataCounter		m_transmission_counter;
	};
} 
#endif //_C_RTP_STREAM_SEND_H_