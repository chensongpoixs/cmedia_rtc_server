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
		//m_media_transmission_counter.Update(packet);

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
		return false;
	}

	crtcp_rr crtp_stream_recv::get_rtcp_receiver_report()
	{
		return crtcp_rr();
	}

	crtcp_rr crtp_stream_recv::get_rtx_rtcp_receiver_report()
	{
		return crtcp_rr();
	}

	void crtp_stream_recv::receive_rtcp_sender_report(crtcp_sr * report)
	{
	}

	void crtp_stream_recv::receive_rtx_rtcp_sender_report(crtcp_sr * report)
	{
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