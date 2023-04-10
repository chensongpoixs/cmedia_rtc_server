/***********************************************************************************************
created: 		2022-08-10

author:			chensong

purpose:		room

Copyright boost
************************************************************************************************/
#include "crtp_stream_recv.h"
#include "cuv_util.h"
#include "SeqManager.hpp"
#include "crtp_stream_define.h"
#include "crtp_stream.h"
namespace chen {

	/* Static. */

	 

	crtp_stream_recv::crtp_stream_recv(const crtp_stream::crtp_stream_params & params)
		: crtp_stream(params)
		, m_expected_prior(0u)
		, m_expected_prior_score(0u)
		, m_received_prior(0u)
		, m_received_prior_score(0u)
		, m_last_sr_timestamp(0u)
		, m_last_sr_received(0u)
		, m_transit(0u)
		, m_jitter(0u)
		, m_fir_seq_number(0u)
		, m_reported_packet_lost(0u)
		, m_inactive(false)
	{
		if ( params.use_nack)
		{
			// nackGenerator.reset(new RTC::NackGenerator(this));
		}
	}

	crtp_stream_recv::~crtp_stream_recv()
	{
	}
 
	bool crtp_stream_recv::receive_packet(RTC::RtpPacket * packet)
	{
		// Call the parent method.
		if (!crtp_stream::receive_packet(packet))
		{
			WARNING_EX_LOG("rtp, packet discarded");

			return false;
		}

		// Process the packet at codec level.
		//if (packet->GetPayloadType() == GetPayloadType())
		//{
		//	RTC::Codecs::Tools::ProcessRtpPacket(packet, GetMimeType());
		//	//packet->Dump();
		//}

		// Pass the packet to the NackGenerator.
		//if (this->params.useNack)
		//{
		//	// If there is RTX just provide the NackGenerator with the packet.
		//	if (HasRtx())
		//	{
		//		this->nackGenerator->ReceivePacket(packet, /*isRecovered*/ false);
		//	}
		//	// If there is no RTX and NackGenerator returns true it means that it
		//	// was a NACKed packet.
		//	else if (this->nackGenerator->ReceivePacket(packet, /*isRecovered*/ false))
		//	{
		//		// Mark the packet as retransmitted and repaired.
		//		RTC::RtpStream::PacketRetransmitted(packet);
		//		RTC::RtpStream::PacketRepaired(packet);
		//	}
		//}
		_calculate_jitter(packet->GetTimestamp());
		// Increase transmission counter.
		//this->transmissionCounter.Update(packet);
		// Increase media transmission counter.
		m_media_transmission_counter.Update(packet);

		if (m_inactive)
		{
			m_inactive = false;
		reset_score(10, true);
		}

		// Restart the inactivityCheckPeriodicTimer.
		/*if (this->inactivityCheckPeriodicTimer)
		{
			this->inactivityCheckPeriodicTimer->Restart();
		}*/
		return false;
	}
	bool crtp_stream_recv::receive_rtx_packet(RTC::RtpPacket * packet)
	{
		if (!m_params.use_nack)
		{
			WARNING_EX_LOG( "NACK not supported");

			return false;
		}

		cassert(packet->GetSsrc() == m_params.rtx_ssrc, "invalid ssrc on RTX packet");

		// Check that the payload type corresponds to the one negotiated.
		if (packet->GetPayloadType() != m_params.rtx_payload_type)
		{
			WARNING_EX_LOG( "rtx ignoring RTX packet with invalid payload type [ssrc:%" PRIu32 ", seq:%" PRIu16
				", pt:%" PRIu8 "]",
				packet->GetSsrc(),
				packet->GetSequenceNumber(),
				packet->GetPayloadType());

			return false;
		}

		if (has_rtx())
		{
			if (!m_rtx_stream_ptr->receive_packet(packet))
			{
				WARNING_EX_LOG( "RTX packet discarded");

				return false;
			}
		}

//#if MS_LOG_DEV_LEVEL == 3
		// Get the RTX packet sequence number for logging purposes.
		uint16 rtxSeq = packet->GetSequenceNumber();
//#endif

		// Get the original RTP packet.
		if (!packet->RtxDecode(m_params.payload_type, m_params.ssrc))
		{
			DEBUG_EX_LOG(
				"ignoring empty RTX packet [ssrc:%" PRIu32 ", seq:%" PRIu16 ", pt:%" PRIu8 "]",
				packet->GetSsrc(),
				packet->GetSequenceNumber(),
				packet->GetPayloadType());

			return false;
		}

		DEBUG_EX_LOG(
			"received RTX packet [ssrc:%" PRIu32 ", seq:%" PRIu16 "] recovering original [ssrc:%" PRIu32
			", seq:%" PRIu16 "]",
			m_params.rtx_ssrc,
			rtxSeq,
			packet->GetSsrc(),
			packet->GetSequenceNumber());

		// If not a valid packet ignore it.
		if (!crtp_stream::update_seq(packet))
		{
			WARNING_EX_LOG( "rtx invalid RTX packet [ssrc:%" PRIu32 ", seq:%" PRIu16 "]",
				packet->GetSsrc(),
				packet->GetSequenceNumber());

			return false;
		}

		// Process the packet at codec level.
		//if (packet->GetPayloadType() == GetPayloadType())
		//	RTC::Codecs::Tools::ProcessRtpPacket(packet, GetMimeType());

		// Mark the packet as retransmitted.
		crtp_stream::packet_retransmitted(packet);

		// Pass the packet to the NackGenerator and return true just if this was a
		// NACKed packet.
		//if (this->nackGenerator->ReceivePacket(packet, /*isRecovered*/ true))
		{
			// Mark the packet as repaired.
			crtp_stream::packet_repaired(packet);

			// Increase transmission counter.
			m_media_transmission_counter.Update(packet);

			// Not inactive anymore.
			if (this->m_inactive)
			{
				this->m_inactive = false;

				reset_score(10, /*notify*/ true);
			}

			// Restart the inactivityCheckPeriodicTimer.
			//if (this->inactivityCheckPeriodicTimer)
			//	this->inactivityCheckPeriodicTimer->Restart();

			return true;
		}

		return false;
		return true;
	}

	crtcp_rr crtp_stream_recv::get_rtcp_receiver_report()
	{
		uint8_t worstRemoteFractionLost{ 0 };

		//if (this->params.useInBandFec)
		//{
		//	// Notify the listener so we'll get the worst remote fraction lost.
		//	static_cast<RTC::RtpStreamRecv::Listener*>(this->listener)
		//		->OnRtpStreamNeedWorstRemoteFractionLost(this, worstRemoteFractionLost);

		//	if (worstRemoteFractionLost > 0)
		//		MS_DEBUG_TAG(rtcp, "using worst remote fraction lost:%" PRIu8, worstRemoteFractionLost);
		//}

		//auto* report = new RTC::RTCP::ReceiverReport();
		crtcp_rr report;
		report.set_ssrc(get_ssrc());

		uint32_t prevPacketsLost = m_packets_lost;

		// Calculate Packets Expected and Lost.
		uint32 expected = get_expected_packets();

		if (expected > m_media_transmission_counter.GetPacketCount())
		{
			m_packets_lost = expected - m_media_transmission_counter.GetPacketCount();
		}
		else
		{
			m_packets_lost = 0u;
		}

		// Calculate Fraction Lost.
		uint32_t expectedInterval = expected - m_expected_prior;

		m_expected_prior   = expected;

		uint32_t receivedInterval = m_media_transmission_counter  .GetPacketCount() - m_received_prior;

		m_received_prior = m_media_transmission_counter.GetPacketCount();

		int32_t lostInterval = expectedInterval - receivedInterval;

		if (expectedInterval == 0 || lostInterval <= 0)
		{
			m_fraction_lost = 0;
		}
		else
		{
			m_fraction_lost = std::round((static_cast<double>(lostInterval << 8) / expectedInterval));
		}

		// Worst remote fraction lost is not worse than local one.
		if (worstRemoteFractionLost <= m_fraction_lost)
		{
			m_reported_packet_lost += (m_packets_lost - prevPacketsLost);

			report.set_lost_packets(m_reported_packet_lost);
			report.set_lost_rate(m_fraction_lost);
		}
		else
		{
			// Recalculate packetsLost.
			uint32_t newLostInterval = (worstRemoteFractionLost * expectedInterval) >> 8;

			m_reported_packet_lost += newLostInterval;

			report.set_lost_packets(m_reported_packet_lost);
			report.set_lost_rate(worstRemoteFractionLost);
		}

		// Fill the rest of the report.
		report.set_highest_sn(static_cast<uint32 >(m_max_seq) + m_cycles);
		report.set_jitter(m_jitter);

		if (m_last_sr_received != 0)
		{
			// Get delay in milliseconds.
			auto delayMs = static_cast<uint32_t>(uv_util::GetTimeMs() - m_last_sr_received);
			// Express delay in units of 1/65536 seconds.
			uint32_t dlsr = (delayMs / 1000) << 16;

			dlsr |= uint32_t{ (delayMs % 1000) * 65536 / 1000 };

			report.set_dlsr(dlsr);
			report.set_lsr(m_last_sr_timestamp);
		}
		else
		{
			report.set_dlsr(0);
			report.set_lsr(0);
		}

		return report;
	}

	crtcp_rr crtp_stream_recv::get_rtx_rtcp_receiver_report()
	{
		if (has_rtx())
		{
			return  m_rtx_stream_ptr->get_rtcp_receiver_report();
		}
		return crtcp_rr();
	}

	void crtp_stream_recv::receive_rtcp_sender_report(crtcp_sr * report)
	{
		m_last_sr_received = uv_util::GetTimeMs();
		m_last_sr_timestamp = report->get_ntp() << 16;
		m_last_sr_timestamp += report->get_ntp() >> 16;

		// Update info about last Sender Report.
		rtc_time::Ntp ntp; // NOLINT(cppcoreguidelines-pro-type-member-init)

		ntp.seconds = report->get_ntp() << 16;
		ntp.fractions = report->get_ntp() >> 16;

		m_last_sender_report_ntp_ms = rtc_time::Ntp2TimeMs(ntp);
		m_last_sender_repor_ts = report->get_rtp_ts();

		// Update the score with the current RR.
		update_score();
	}

	void crtp_stream_recv::receive_rtx_rtcp_sender_report(crtcp_sr * report)
	{
		if (has_rtx())
		{
			m_rtx_stream_ptr->receive_rtcp_sender_report(*report);
		}
	}

	void crtp_stream_recv::request_key_frame()
	{
		if (m_params.use_pli)
		{
			DEBUG_EX_LOG( "sending PLI [ssrc:%" PRIu32 "]", get_ssrc());

			// Sender SSRC should be 0 since there is no media sender involved, but
			// some implementations like gstreamer will fail to process it otherwise.
			/*RTC::RTCP::FeedbackPsPliPacket packet(get_ssrc(), get_ssrc());

			packet.Serialize(RTC::RTCP::Buffer);

			m_pli_count++;*/

			// Notify the listener.
			//static_cast<RTC::RtpStreamRecv::Listener*>(this->listener)->OnRtpStreamSendRtcpPacket(this, &packet);
		}
		else if (m_params.use_fir)
		{
			DEBUG_EX_LOG( "sending FIR [ssrc:%" PRIu32 "]", get_ssrc());

			// Sender SSRC should be 0 since there is no media sender involved, but
			// some implementations like gstreamer will fail to process it otherwise.
			/*RTC::RTCP::FeedbackPsFirPacket packet(GetSsrc(), GetSsrc());
			auto* item = new RTC::RTCP::FeedbackPsFirItem(GetSsrc(), ++this->firSeqNumber);

			packet.AddItem(item);
			packet.Serialize(RTC::RTCP::Buffer);*/

			m_fir_count++;

			// Notify the listener.
			//static_cast<RTC::RtpStreamRecv::Listener*>(this->listener)->OnRtpStreamSendRtcpPacket(this, &packet);
		}
	}

	void crtp_stream_recv::pause()
	{
		//if (this->inactivityCheckPeriodicTimer)
		//	this->inactivityCheckPeriodicTimer->Stop();

		//if (this->params.useNack)
		//	this->nackGenerator->Reset();

		// Reset jitter.
		m_transit = 0;
		m_jitter = 0;
	}

	void crtp_stream_recv::pesume()
	{
		//if (this->inactivityCheckPeriodicTimer && !this->inactive)
			//this->inactivityCheckPeriodicTimer->Restart();
	}
	 
	void crtp_stream_recv::onnack_generator_nack_required(const std::vector<uint16>& seqNumbers)
	{
		cassert(m_params.use_nack, "NACK required but not supported");

		DEBUG_EX_LOG( "triggering NACK [ssrc:%" PRIu32 ", first seq:%" PRIu16 ", num packets:%zu]",
			m_params.ssrc,
			seqNumbers[0],
			seqNumbers.size());

		//RTC::RTCP::FeedbackRtpNackPacket packet(0, GetSsrc());

		//auto it = seqNumbers.begin();
		//const auto end = seqNumbers.end();
		//size_t numPacketsRequested{ 0 };

		//while (it != end)
		//{
		//	uint16_t seq;
		//	uint16_t bitmask{ 0 };

		//	seq = *it;
		//	++it;

		//	while (it != end)
		//	{
		//		uint16_t shift = *it - seq - 1;

		//		if (shift > 15)
		//			break;

		//		bitmask |= (1 << shift);
		//		++it;
		//	}

		//	auto* nackItem = new RTC::RTCP::FeedbackRtpNackItem(seq, bitmask);

		//	packet.AddItem(nackItem);

		//	numPacketsRequested += nackItem->CountRequestedPackets();
		//}

		//// Ensure that the RTCP packet fits into the RTCP buffer.
		//if (packet.GetSize() > RTC::RTCP::BufferSize)
		//{
		//	WARNING_EX_LOG( "cannot send RTCP NACK packet, size too big (%zu bytes)", packet.GetSize());

		//	return;
		//}

		m_nack_count++;
		/*m_nack_packet_count += numPacketsRequested;

		packet.Serialize(RTC::RTCP::Buffer);
*/
		// Notify the listener.
		// static_cast<RTC::RtpStreamRecv::Listener*>(this->listener)->OnRtpStreamSendRtcpPacket(this, &packet);
	}

	void crtp_stream_recv::onnack_generator_key_frame_required()
	{
		DEBUG_EX_LOG( "requesting key frame [ssrc:%" PRIu32 "]", m_params.ssrc);

		request_key_frame();
	}

	void crtp_stream_recv::_calculate_jitter(uint32 rtpTimestamp)
	{
		int32 transit = static_cast<int32>(uv_util::GetTimeMs() - (rtpTimestamp * 1000 / 9000/*this->params.clockRate*/));
		int32 d = transit - m_transit;

		// First transit calculation, save and return.
		if (m_transit == 0)
		{
			m_transit = transit;

			return;
		}

		m_transit = transit;

		if (d < 0)
		{
			d = -d;
		}

		m_jitter += (1. / 16.) * (static_cast<double>(d) - m_jitter);
	}

	 
	 
	 
	 
	void crtp_stream_recv::update_score( )
	{
		// Calculate number of packets expected in this interval.
		uint32 totalExpected = get_expected_packets();
		uint32  expected = totalExpected -   m_expected_prior_score;

		m_expected_prior_score = totalExpected;

		// Calculate number of packets received in this interval.
		auto totalReceived = m_media_transmission_counter.GetPacketCount();
		uint32_t received = totalReceived - m_received_prior_score;

		m_received_prior_score = totalReceived;

		// Calculate number of packets lost in this interval.
		uint32_t lost;

		if (expected < received)
		{
			lost = 0;
		}
		else
		{
			lost = expected - received;
		}

		// Calculate number of packets repaired in this interval.
		size_t totalRepaired = m_packets_repaired;
		uint32 repaired = totalRepaired - m_received_prior_score;

		m_received_prior_score = totalRepaired;

		// Calculate number of packets retransmitted in this interval.
		size_t totatRetransmitted = m_packets_retransmitted;
		uint32_t retransmitted = totatRetransmitted - m_retransmitted_prior_score;

		m_retransmitted_prior_score = totatRetransmitted;

		if (m_inactive)
		{
			return;
		}

		// We didn't expect more packets to come.
		if (expected == 0)
		{
			crtp_stream::update_score(10);

			return;
		}

		if (lost > received)
			lost = received;

		if (repaired > lost)
		{
			if (has_rtx())
			{
				repaired = lost;
				retransmitted -= repaired - lost;
			}
			else
			{
				lost = repaired;
			}
		}

#if MS_LOG_DEV_LEVEL == 3
		MS_DEBUG_TAG(
			score,
			"[totalExpected:%" PRIu32 ", totalReceived:%zu, totalRepaired:%zu",
			totalExpected,
			totalReceived,
			totalRepaired);

		MS_DEBUG_TAG(
			score,
			"fixed values [expected:%" PRIu32 ", received:%" PRIu32 ", lost:%" PRIu32
			", repaired:%" PRIu32 ", retransmitted:%" PRIu32,
			expected,
			received,
			lost,
			repaired,
			retransmitted);
#endif

		auto repairedRatio = static_cast<float>(repaired) / static_cast<float>(received);
		auto repairedWeight = std::pow(1 / (repairedRatio + 1), 4);

		cassert(retransmitted >= repaired, "repaired packets cannot be more than retransmitted ones");

		if (retransmitted > 0)
		{
			repairedWeight *= static_cast<float>(repaired) / retransmitted;
		}

		lost -= repaired * repairedWeight;

		auto deliveredRatio = static_cast<float>(received - lost) / static_cast<float>(received);
		auto score = static_cast<uint8_t>(std::round(std::pow(deliveredRatio, 4) * 10));

#if MS_LOG_DEV_LEVEL == 3
		MS_DEBUG_TAG(
			score,
			"[deliveredRatio:%f, repairedRatio:%f, repairedWeight:%f, new lost:%" PRIu32 ", score:%" PRIu8
			"]",
			deliveredRatio,
			repairedRatio,
			repairedWeight,
			lost,
			score);
#endif

		crtp_stream::update_score(score);
	}
 

}