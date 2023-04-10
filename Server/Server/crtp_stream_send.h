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
namespace chen {
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
		crtp_stream_send(const crtp_stream::crtp_stream_params & params) 
		: crtp_stream(params)
		, m_lost_prior_score(0u)
		, m_sent_prior_score(0u)
		, m_buffer()
		, m_buffer_start_idx(0u)
		, m_buffer_size(0u)
		, m_storage()
		, m_rtx_seq(0u)
		{}
		~crtp_stream_send();
	public:
		void set_rtx(uint8 payload_type, uint32 ssrc);

		bool receive_packet(RTC::RtpPacket* packet);

		//void receive_nack();
		/*void ReceiveNack(RTC::RTCP::FeedbackRtpNackPacket* nackPacket);
		void ReceiveKeyFrameRequest(RTC::RTCP::FeedbackPs::MessageType messageType);
		void ReceiveRtcpReceiverReport(RTC::RTCP::ReceiverReport* report);
		RTC::RTCP::SenderReport* GetRtcpSenderReport(uint64_t nowMs);*/
		//RTC::RTCP::SdesChunk* GetRtcpSdesChunk();


	protected:

	private:
		void _store_packet(RTC::RtpPacket * packet);
		void _clear_buffer();
		
	private:

		uint32						m_lost_prior_score{ 0u }; // Packets lost at last interval for score calculation.
		uint32						m_sent_prior_score{ 0u }; // Packets sent at last interval for score calculation.
		std::vector<StorageItem*>	m_buffer;
		uint16						m_buffer_start_idx{ 0u };
		size_t						m_buffer_size{ 0u };
		std::vector<StorageItem>	m_storage;
		uint16						m_rtx_seq{ 0u };
		//RTC::RtpDataCounter transmissionCounter;
	};
} 
#endif //_C_RTP_STREAM_SEND_H_