/***********************************************************************************************
created: 		2022-08-10

author:			chensong

purpose:		room

Copyright boost
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