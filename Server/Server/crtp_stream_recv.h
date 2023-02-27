/***********************************************************************************************
created: 		2022-08-10

author:			chensong

purpose:		room

Copyright boost
************************************************************************************************/

#ifndef _C_RTP_STREAM_RECV_H_
#define _C_RTP_STREAM_RECV_H_
#include <string>
#include "cnet_type.h"
#include "crtp_rtcp.h"
#include "RtpPacket.hpp"
#include "SenderReport.hpp"
namespace chen {
	class crtp_stream_recv
	{
	public:
		explicit crtp_stream_recv()
		: m_expectedPrior(0u)
		, m_expectedPriorScore(0u)
		, m_receivedPrior(0u)
		, m_receivedPriorScore(0u)
		, m_lastSrTimestamp(0u)
		, m_lastSrReceived(0u)
		, m_transit(0u)
		, m_jitter(0u)
		, m_firSeqNumber(0u)
		, m_reportedPacketLost(0u)
		, m_inactive(false){}
		virtual ~crtp_stream_recv();

	public:
		bool  receive_packet(RTC::RtpPacket* packet);
		bool  receive_rtx_packet(RTC::RtpPacket* packet);
		//RTC::RTCP::ReceiverReport* GetRtcpReceiverReport();
		//RTC::RTCP::ReceiverReport* GetRtxRtcpReceiverReport();
		void receive_rtcp_sender_report(RTC::RTCP::SenderReport* report);
		void receive_rtx_rtcp_sender_report(RTC::RTCP::SenderReport* report);
		//void ReceiveRtcpXrDelaySinceLastRr(RTC::RTCP::DelaySinceLastRr::SsrcInfo* ssrcInfo);

	protected:
	private:

		void _calculate_jitter(uint32 rtpTimestamp);
	private:
		uint32			m_expectedPrior{ 0u };      // Packets expected at last interval.
		uint32			m_expectedPriorScore{ 0u }; // Packets expected at last interval for score calculation.
		uint32			m_receivedPrior{ 0u };      // Packets received at last interval.
		uint32			m_receivedPriorScore{ 0u }; // Packets received at last interval for score calculation.
		uint32			m_lastSrTimestamp{ 0u };    // The middle 32 bits out of 64 in the NTP
										   // timestamp received in the most recent
										   // sender report.
		uint64			m_lastSrReceived{ 0u };     // Wallclock time representing the most recent
										   // sender report arrival.
		int32			m_transit{ 0u };             // Relative transit time for prev packet.
		uint32			m_jitter{ 0u };
		uint8			m_firSeqNumber{ 0u };
		uint32			m_reportedPacketLost{ 0u };
		//std::unique_ptr<RTC::NackGenerator> nackGenerator;
		//Timer* inactivityCheckPeriodicTimer{ nullptr };
		bool			m_inactive{ false };
		//TransmissionCounter transmissionCounter;      // Valid media + valid RTX.
		//RTC::RtpDataCounter mediaTransmissionCounter; // Just valid media.
	};
}

#endif // _C_RTP_STREAM_RECV_H_