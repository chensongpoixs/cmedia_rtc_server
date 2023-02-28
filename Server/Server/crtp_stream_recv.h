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
#include "crate_calculator.h"
#include "ReceiverReport.hpp"
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
		bool init(uint32 ssrc, uint8 initial_score);
	public:
		bool  receive_packet(RTC::RtpPacket* packet);
		bool  receive_rtx_packet(RTC::RtpPacket* packet);
		RTC::RTCP::ReceiverReport* get_rtcp_receiver_report();
		//RTC::RTCP::ReceiverReport* GetRtxRtcpReceiverReport();
		void receive_rtcp_sender_report(RTC::RTCP::SenderReport* report);
		void receive_rtx_rtcp_sender_report(RTC::RTCP::SenderReport* report);
		//void ReceiveRtcpXrDelaySinceLastRr(RTC::RTCP::DelaySinceLastRr::SsrcInfo* ssrcInfo);

	protected:
	private:

		void _calculate_jitter(uint32 rtpTimestamp);

		bool  _receive_packet(RTC::RtpPacket* packet);

		void _reset_score(uint8  score, bool notify);

		void update_score();
	public:
		bool update_seq(RTC::RtpPacket* packet);
		void update_score(uint8 score);
		void packet_retransmitted(RTC::RtpPacket* packet);
		void packet_repaired(RTC::RtpPacket* packet);
		uint32  getexpected_packets() const
		{
			return (m_cycles + m_maxSeq) - m_baseSeq + 1;
		}

	private:
		void _init_seq(uint16 seq);


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
		RtpDataCounter m_mediaTransmissionCounter; // Just valid media.


		///////////////////////////////////////////////////////////////////////////////////////
		// Others.
		//   https://tools.ietf.org/html/rfc3550#appendix-A.1 stuff.
		uint16				m_maxSeq{ 0u };      // Highest seq. number seen.
		uint32				m_cycles{ 0u };      // Shifted count of seq. number cycles.
		uint32				m_baseSeq{ 0u };     // Base seq number.
		uint32				m_badSeq{ 0u };      // Last 'bad' seq number + 1.
		uint32				m_maxPacketTs{ 0u }; // Highest timestamp seen.
		uint64				m_maxPacketMs{ 0u }; // When the packet with highest timestammp was seen.
		uint32				m_packetsLost{ 0u };
		uint8				m_fractionLost{ 0u };
		size_t				m_packetsDiscarded{ 0u };
		size_t				m_packetsRetransmitted{ 0u };
		size_t				m_packetsRepaired{ 0u };
		size_t				m_nackCount{ 0u };
		size_t				m_nackPacketCount{ 0u };
		size_t				m_pliCount{ 0u };
		size_t				m_firCount{ 0u };
		size_t				m_repairedPriorScore{ 0u }; // Packets repaired at last interval for score calculation.
		size_t				m_retransmittedPriorScore{ 0u }; // Packets retransmitted at last interval for score calculation.
		uint64				m_lastSenderReportNtpMs{ 0u }; // NTP timestamp in last Sender Report (in ms).
		uint32				m_lastSenderReporTs{ 0u };     // RTP timestamp in last Sender Report.
		float				m_rtt{ 0 };
		bool				m_hasRtt{ false };




		//////////////////////////private://///////////////////////////////
		// Score related.
		uint8					m_score{ 0u };
		std::vector<uint8>		m_scores;
		// Whether at least a RTP packet has been received.
		bool					m_started{ false };
		// Last time since the stream is active.
		uint64					m_activeSinceMs{ 0u };



		uint32					m_ssrc;
	};
}

#endif // _C_RTP_STREAM_RECV_H_