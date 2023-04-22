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

#ifndef _C_RTP_STREAM_RECV_H_
#define _C_RTP_STREAM_RECV_H_
#include <string>
#include "cnet_type.h"
#include "crtp_rtcp.h"
#include "RtpPacket.hpp"
#include "SenderReport.hpp"
#include "crate_calculator.h"
#include "crtp_stream.h"
#include "ReceiverReport.hpp"
#include "ctimer.h"
#include "NackGenerator.hpp"
//#include "crtc_producer.h"
namespace chen {
	
	class crtc_producer;
	class crtp_stream_recv : public crtp_stream, public ctimer::Listener, public RTC::NackGenerator::Listener
	{
	public:
		explicit crtp_stream_recv(crtc_producer * ptr, const crtp_stream::crtp_stream_params & params);
		
		virtual ~crtp_stream_recv();

	public:
		 
	public:
		bool  receive_packet(RTC::RtpPacket* packet);
		bool  receive_rtx_packet(RTC::RtpPacket* packet);
		RTC::RTCP::ReceiverReport* get_rtcp_receiver_report();
		RTC::RTCP::ReceiverReport* get_rtx_rtcp_receiver_report();
		 void receive_rtcp_sender_report(RTC::RTCP::SenderReport* report);
		 void receive_rtx_rtcp_sender_report(RTC::RTCP::SenderReport* report);
		//void ReceiveRtcpXrDelaySinceLastRr(RTC::RTCP::DelaySinceLastRr::SsrcInfo* ssrcInfo);



		 void request_key_frame();
		/* void pause();
		 void pesume();*/
		 virtual void pause() ;
		 virtual void resume()  ;
		// virtual uint32  get_bitrate(uint64  nowMs)  ;
		 uint32  get_bitrate(uint64  nowMs)  
		 {
			 return m_media_transmission_counter .GetBitrate(nowMs);
		 }
		 /* uint32  get_bitrate(uint64  nowMs, uint8  spatialLayer, uint8  temporalLayer) override
		  {
			  return m_transmission_counter.GetBitrate(nowMs, spatialLayer, temporalLayer);
		  }*/
		 /*uint32  get_spatialLayerBitrate(uint64_t nowMs, uint8_t spatialLayer) override
		 {
			 return this->transmissionCounter.GetSpatialLayerBitrate(nowMs, spatialLayer);
		 }*/
		 /*uint32  GetLayerBitrate(uint64_t nowMs, uint8_t spatialLayer, uint8_t temporalLayer) override
		 {
			 return this->transmissionCounter.GetLayerBitrate(nowMs, spatialLayer, temporalLayer);
		 }*/
		 /* Pure virtual methods inherited from Timer. */
	protected:
		void OnTimer(ctimer* timer)  ;
	public:
		//virtual void set_rtx(uint8 payload_type, uint32 ssrc);
		void del_rtx();
	protected:
		void OnNackGeneratorNackRequired(const std::vector<uint16>& seqNumbers)  ;
		void OnNackGeneratorKeyFrameRequired()  ;
	private:

		void _calculate_jitter(uint32 rtpTimestamp);

		 
		void update_score();
 
		

	private:
		crtc_producer * m_rtc_producer_ptr;
		uint32			m_expected_prior{ 0u };      // Packets expected at last interval.
		uint32			m_expected_prior_score{ 0u }; // Packets expected at last interval for score calculation.
		uint32			m_received_prior{ 0u };      // Packets received at last interval.
		uint32			m_received_prior_score{ 0u }; // Packets received at last interval for score calculation.
		uint32			m_last_sr_timestamp{ 0u };    // The middle 32 bits out of 64 in the NTP
										   // timestamp received in the most recent
										   // sender report.
		uint64			m_last_sr_received{ 0u };     // Wallclock time representing the most recent
										   // sender report arrival.
		int32			m_transit{ 0u };             // Relative transit time for prev packet.
		uint32			m_jitter{ 0u };
		uint8			m_fir_seq_number{ 0u };
		uint32			m_reported_packet_lost{ 0u };
		std::unique_ptr<RTC::NackGenerator> nackGenerator;
		ctimer*			m_inactivityCheckPeriodicTimer{ nullptr };
		bool			m_inactive{ false };
		//TransmissionCounter transmissionCounter;      // Valid media + valid RTX.
		RtpDataCounter m_media_transmission_counter; // Just valid media.


		 
	};
}

#endif // _C_RTP_STREAM_RECV_H_