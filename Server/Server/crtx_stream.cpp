/***********************************************************************************************
created: 		2022-08-10

author:			chensong

purpose:		room

Copyright boost
************************************************************************************************/
#include "crtx_stream.h"
#include "crtp_stream_define.h"
#include "cseq_manager.h"
#include "SeqManager.hpp"
namespace chen {
	/*crtx_stream::crtx_stream(const crtx_params & params)
	{
	}*/
	crtx_stream::~crtx_stream()
	{
	}
	bool crtx_stream::receive_packet(RTC::RtpPacket * packet)
	{
		uint16  seq = packet->GetSequenceNumber();

		// If this is the first packet seen, initialize stuff.
		if (!m_started )
		{
			_init_seq(seq);

			m_started = true;
			m_max_seq = seq - 1;
			m_max_packet_ts = packet->GetTimestamp();
			m_max_packet_ms = uv_util::GetTimeMs();
		}

		// If not a valid packet ignore it.
		if (!update_seq(packet))
		{
			WARNING_EX_LOG( "rtx invalid packet [ssrc:%" PRIu32 ", seq:%" PRIu16 "]",
				packet->GetSsrc(),
				packet->GetSequenceNumber());

			return false;
		}

		// Update highest seen RTP timestamp.
		if (RTC::SeqManager<uint32>::IsSeqHigherThan(packet->GetTimestamp(), m_max_packet_ts))
		{
			m_max_packet_ts = packet->GetTimestamp();
			m_max_packet_ms = uv_util::GetTimeMs();
		}

		// Increase packet count.
		++m_packets_count;

		return true;
	}
	RTC::RTCP::ReceiverReport* crtx_stream::get_rtcp_receiver_report()
	{
		//crtcp_rr report;
		//report.set_ssrc(get_ssrc());
		auto* report = new RTC::RTCP::ReceiverReport();

		report->SetSsrc(get_ssrc());
		
		uint32  prevPacketsLost = m_packets_lost;

		// Calculate Packets Expected and Lost.
		auto expected = get_expected_packets();
		if (expected > m_packets_count)
		{
			m_packets_lost = expected - m_packets_count;
		}
		else
		{
			m_packets_lost = 0u;
		}

		// Calculate Fraction Lost.
		uint32_t expectedInterval = expected - m_expected_prior;

		m_expected_prior = expected;

		uint32_t receivedInterval = m_packets_count - m_received_prior;

		m_received_prior = m_packets_count;

		int32_t lostInterval = expectedInterval - receivedInterval;

		if (expectedInterval == 0 || lostInterval <= 0)
		{
			m_fraction_lost = 0;
		}
		else
		{
			m_fraction_lost = std::round((static_cast<double>(lostInterval << 8) / expectedInterval));

		}
		m_reported_packet_lost += (m_packets_lost - prevPacketsLost);

		//report.set_lost_packets(m_reported_packet_lost);
		//report.set_lost_rate(m_fraction_lost);
		report->SetTotalLost(m_reported_packet_lost);
		report->SetFractionLost(m_fraction_lost);

		report->SetLastSeq(static_cast<uint32>(m_max_seq) + m_cycles);

		//report.set_highest_sn(static_cast<uint32 >(m_max_seq) + m_cycles);
		//report.set_jitter(0);
		report->SetJitter(0);

		if (m_last_sr_received != 0)
		{
			// Get delay in milliseconds.
			uint32 delayMs = static_cast<uint32>(uv_util::GetTimeMs() - m_last_sr_received);
			// Express delay in units of 1/65536 seconds.
			uint32_t dlsr = (delayMs / 1000) << 16;

			dlsr |= uint32_t{ (delayMs % 1000) * 65536 / 1000 };

			report->SetDelaySinceLastSenderReport(dlsr);
			report->SetLastSenderReport(m_last_sr_timestamp);
			//report.set_dlsr(dlsr);
			//report.set_lsr(m_last_sr_timestamp);
		}
		else
		{
			report->SetDelaySinceLastSenderReport(0);
			report->SetLastSenderReport(0);
			//report.set_dlsr(0);
			//report.set_lsr(0);
		}
		return report;
	}
	void crtx_stream::receive_rtcp_sender_report(RTC::RTCP::SenderReport* report)
	{
		m_last_sr_received = uv_util::GetTimeMs();
		m_last_sr_timestamp = report->GetNtpSec() << 16;
		m_last_sr_timestamp += report->GetNtpFrac() >> 16;

		 
	}
	bool crtx_stream::update_seq(RTC::RtpPacket * packet)
	{
		uint16  seq = packet->GetSequenceNumber();
		uint16  udelta = seq - m_max_seq;

		// If the new packet sequence number is greater than the max seen but not
		// "so much bigger", accept it.
		// NOTE: udelta also handles the case of a new cycle, this is:
		//    maxSeq:65536, seq:0 => udelta:1
		if (udelta < MaxDropout)
		{
			// In order, with permissible gap.
			if (seq < m_max_seq)
			{
				// Sequence number wrapped: count another 64K cycle.
				m_cycles += RtpSeqMod;
			}

			m_max_seq = seq;
		}
		// Too old packet received (older than the allowed misorder).
		// Or to new packet (more than acceptable dropout).
		else if (udelta <= RtpSeqMod - MaxMisorder)
		{
			// The sequence number made a very large jump. If two sequential packets
			// arrive, accept the latter.
			if (seq == m_bad_seq)
			{
				// Two sequential packets. Assume that the other side restarted without
				// telling us so just re-sync (i.e., pretend this was the first packet).
				WARNING_EX_LOG( "rtx too bad sequence number, re-syncing RTP [ssrc:%" PRIu32 ", seq:%" PRIu16 "]",
					packet->GetSsrc(),
					packet->GetSequenceNumber());

				_init_seq(seq);

				m_max_packet_ts = packet->GetTimestamp();
				m_max_packet_ms = uv_util::GetTimeMs();
			}
			else
			{
				WARNING_EX_LOG( "rtx bad sequence number, ignoring packet [ssrc:%" PRIu32 ", seq:%" PRIu16 "]",
					packet->GetSsrc(),
					packet->GetSequenceNumber());

				m_bad_seq = (seq + 1) & (RtpSeqMod - 1);

				// Packet discarded due to late or early arriving.
				++m_packets_discarded;

				return false;
			}
		}
		// Acceptable misorder.
		else
		{
			// Do nothing.
		}

		return true;
	}
	void crtx_stream::_init_seq(uint16 seq)
	{
		m_base_seq = seq;
		m_max_seq = seq;
		m_bad_seq = RtpSeqMod + 1;
	}
}