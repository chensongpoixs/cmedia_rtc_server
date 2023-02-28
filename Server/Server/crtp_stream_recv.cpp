/***********************************************************************************************
created: 		2022-08-10

author:			chensong

purpose:		room

Copyright boost
************************************************************************************************/
#include "crtp_stream_recv.h"
#include "cuv_util.h"
#include "SeqManager.hpp"
namespace chen {

	/* Static. */

	static const	uint16		MaxDropout{ 3000 };
	static const	uint16		MaxMisorder{ 1500 };
	static const	uint32		RtpSeqMod{ 1 << 16 };
	static const	size_t		ScoreHistogramLength{ 24 };

	crtp_stream_recv::~crtp_stream_recv()
	{
	}
	bool crtp_stream_recv::init(uint32 ssrc, uint8 initial_score)
	{
		m_ssrc = ssrc;
		m_activeSinceMs = uv_util::GetTimeMs();

		return true;
	}
	bool crtp_stream_recv::receive_packet(RTC::RtpPacket * packet)
	{
		// Call the parent method.
		if (!_receive_packet(packet))
		{
			WARNING_EX_LOG("rtp, packet discarded");

			return false;
		}
		_calculate_jitter(packet->GetTimestamp());

		// Increase media transmission counter.
		m_mediaTransmissionCounter.Update(packet);

		if (m_inactive)
		{
			m_inactive = false;
			_reset_score(10, true);
		}
		return false;
	}
	bool crtp_stream_recv::receive_rtx_packet(RTC::RtpPacket * packet)
	{
		return false;
	}
	RTC::RTCP::ReceiverReport * crtp_stream_recv::get_rtcp_receiver_report()
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

		auto* report = new RTC::RTCP::ReceiverReport();

		report->SetSsrc(m_ssrc);

		uint32_t prevPacketsLost = m_packetsLost;

		// Calculate Packets Expected and Lost.
		auto expected = getexpected_packets();

		if (expected > m_mediaTransmissionCounter.GetPacketCount())
		{
			m_packetsLost = expected - m_mediaTransmissionCounter.GetPacketCount();
		}
		else
		{
			m_packetsLost = 0u;
		}

		// Calculate Fraction Lost.
		uint32_t expectedInterval = expected - m_expectedPrior;

		m_expectedPrior = expected;

		uint32_t receivedInterval = m_mediaTransmissionCounter.GetPacketCount() - m_receivedPrior;

		m_receivedPrior = m_mediaTransmissionCounter.GetPacketCount();

		int32_t lostInterval = expectedInterval - receivedInterval;

		if (expectedInterval == 0 || lostInterval <= 0)
		{
			m_fractionLost = 0;
		}
		else
		{
			m_fractionLost = std::round((static_cast<double>(lostInterval << 8) / expectedInterval));
		}
		// Worst remote fraction lost is not worse than local one.
		if (worstRemoteFractionLost <= m_fractionLost)
		{
			m_reportedPacketLost += (m_packetsLost - prevPacketsLost);

			report->SetTotalLost(m_reportedPacketLost);
			report->SetFractionLost(m_fractionLost);
		}
		else
		{
			// Recalculate packetsLost.
			uint32_t newLostInterval = (worstRemoteFractionLost * expectedInterval) >> 8;

			m_reportedPacketLost += newLostInterval;

			report->SetTotalLost(m_reportedPacketLost);
			report->SetFractionLost(worstRemoteFractionLost);
		}

		// Fill the rest of the report.
		report->SetLastSeq(static_cast<uint32_t>(m_maxSeq) + m_cycles);
		report->SetJitter(m_jitter);

		if (m_lastSrReceived != 0)
		{
			// Get delay in milliseconds.
			auto delayMs = static_cast<uint32_t>(uv_util::GetTimeMs() - m_lastSrReceived);
			// Express delay in units of 1/65536 seconds.
			uint32_t dlsr = (delayMs / 1000) << 16;

			dlsr |= uint32_t{ (delayMs % 1000) * 65536 / 1000 };

			report->SetDelaySinceLastSenderReport(dlsr);
			report->SetLastSenderReport(m_lastSrTimestamp);
		}
		else
		{
			report->SetDelaySinceLastSenderReport(0);
			report->SetLastSenderReport(0);
		}

		return report;
	}
	void crtp_stream_recv::receive_rtcp_sender_report(RTC::RTCP::SenderReport * report)
	{
		m_lastSrReceived = uv_util::GetTimeMs();
		m_lastSrTimestamp = report->GetNtpSec() << 16;
		m_lastSrTimestamp += report->GetNtpFrac() >> 16;

		// Update info about last Sender Report.
		rtc_time::Ntp ntp; // NOLINT(cppcoreguidelines-pro-type-member-init)

		ntp.seconds = report->GetNtpSec();
		ntp.fractions = report->GetNtpFrac();

		m_lastSenderReportNtpMs = rtc_time::Ntp2TimeMs(ntp);
		m_lastSenderReporTs = report->GetRtpTs();

		// Update the score with the current RR.
		update_score();
	}
	void crtp_stream_recv::receive_rtx_rtcp_sender_report(RTC::RTCP::SenderReport * report)
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

	bool crtp_stream_recv::_receive_packet(RTC::RtpPacket * packet)
	{
		uint16_t seq = packet->GetSequenceNumber();

		// If this is the first packet seen, initialize stuff.
		if (!m_started)
		{
			_init_seq(seq);

			m_started = true;
			m_maxSeq = seq - 1;
			m_maxPacketTs = packet->GetTimestamp();
			m_maxPacketMs = uv_util::GetTimeMs();
		}

		// If not a valid packet ignore it.
		if (!update_seq(packet))
		{
			WARNING_EX_LOG(" rtp, invalid packet [ssrc:%" PRIu32 ", seq:%" PRIu16 "]",
				packet->GetSsrc(),
				packet->GetSequenceNumber());

			return false;
		}

		// Update highest seen RTP timestamp.
		if (RTC::SeqManager<uint32>::IsSeqHigherThan(packet->GetTimestamp(), m_maxPacketTs))
		{
			m_maxPacketTs = packet->GetTimestamp();
			m_maxPacketMs = uv_util::GetTimeMs();
		}
		return true;
	}

	void crtp_stream_recv::_reset_score(uint8 score, bool notify)
	{
		m_scores.clear();

		if (m_score != score)
		{
			auto previousScore = m_score;

			m_score = score;

			// If previous score was 0 (and new one is not 0) then update activeSinceMs.
			if (previousScore == 0u)
			{
				m_activeSinceMs = uv_util::GetTimeMs();
			}
			// Notify the listener.
			//if (notify)
				//this->listener->OnRtpStreamScore(this, score, previousScore);
		}
	}

	void crtp_stream_recv::update_score()
	{
		// Calculate number of packets expected in this interval.
		auto totalExpected = getexpected_packets();
		uint32_t expected = totalExpected - m_expectedPriorScore;

		m_expectedPriorScore = totalExpected;

		// Calculate number of packets received in this interval.
		auto totalReceived = m_mediaTransmissionCounter.GetPacketCount();
		uint32_t received = totalReceived - m_receivedPriorScore;

		m_receivedPriorScore = totalReceived;

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
		auto totalRepaired = m_packetsRepaired;
		uint32_t repaired = totalRepaired - m_repairedPriorScore;

		m_repairedPriorScore = totalRepaired;

		// Calculate number of packets retransmitted in this interval.
		auto totatRetransmitted = m_packetsRetransmitted;
		uint32_t retransmitted = totatRetransmitted - m_retransmittedPriorScore;

		m_retransmittedPriorScore = totatRetransmitted;

		if (m_inactive)
		{
			return;
		}

		// We didn't expect more packets to come.
		if (expected == 0)
		{
			update_score(10);

			return;
		}

		if (lost > received)
		{
			lost = received;
		}

		if (repaired > lost)
		{
			// TODO@chensong 2023-02-28   RTX packet add ===>
			/*if (HasRtx())
			{
				repaired = lost;
				retransmitted -= repaired - lost;
			}
			else*/
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

		cassert_desc(retransmitted >= repaired, "repaired packets cannot be more than retransmitted ones");

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

		 update_score(score);
	}

	bool crtp_stream_recv::update_seq(RTC::RtpPacket * packet)
	{
		uint16 seq = packet->GetSequenceNumber();
		uint16 udelta = seq - m_maxSeq;

		// If the new packet sequence number is greater than the max seen but not
		// "so much bigger", accept it.
		// NOTE: udelta also handles the case of a new cycle, this is:
		//    maxSeq:65536, seq:0 => udelta:1
		if (udelta < MaxDropout)
		{
			// In order, with permissible gap.
			if (seq < m_maxSeq)
			{
				// Sequence number wrapped: count another 64K cycle.
				m_cycles += RtpSeqMod;
			}

			m_maxSeq = seq;
		}
		// Too old packet received (older than the allowed misorder).
		// Or to new packet (more than acceptable dropout).
		else if (udelta <= RtpSeqMod - MaxMisorder)
		{
			// The sequence number made a very large jump. If two sequential packets
			// arrive, accept the latter.
			if (seq == m_badSeq)
			{
				// Two sequential packets. Assume that the other side restarted without
				// telling us so just re-sync (i.e., pretend this was the first packet).
				WARNING_EX_LOG( "rtp too bad sequence number, re-syncing RTP [ssrc:%" PRIu32 ", seq:%" PRIu16 "]",
					packet->GetSsrc(),
					packet->GetSequenceNumber());

				_init_seq(seq);

				m_maxPacketTs = packet->GetTimestamp();
				m_maxPacketMs = uv_util::GetTimeMs();
			}
			else
			{
				WARNING_EX_LOG(" rtp, bad sequence number, ignoring packet [ssrc:%" PRIu32 ", seq:%" PRIu16 "]",
					packet->GetSsrc(),
					packet->GetSequenceNumber());

				m_badSeq = (seq + 1) & (RtpSeqMod - 1);

				// Packet discarded due to late or early arriving.
				m_packetsDiscarded++;

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

	void crtp_stream_recv::update_score(uint8 score)
	{
		// Add the score into the histogram.
		if (m_scores.size() == ScoreHistogramLength)
		{
			m_scores.erase(m_scores.begin());
		}

		uint8 previousScore = m_score;

		// Compute new effective score taking into accout entries in the histogram.
		m_scores.push_back(score);

		/*
		 * Scoring mechanism is a weighted average.
		 *
		 * The more recent the score is, the more weight it has.
		 * The oldest score has a weight of 1 and subsequent scores weight is
		 * increased by one sequentially.
		 *
		 * Ie:
		 * - scores: [1,2,3,4]
		 * - this->scores = ((1) + (2+2) + (3+3+3) + (4+4+4+4)) / 10 = 2.8 => 3
		 */

		size_t weight{ 0 };
		size_t samples{ 0 };
		size_t totalScore{ 0 };
		// 加权平均数  计算 哈
		// 1. 权重越大 判断的占有标量越大哈
		for (auto score : m_scores)
		{
			weight++;
			samples += weight;
			totalScore += weight * score;
		}

		// clang-tidy "thinks" that this can lead to division by zero but we are
		// smarter.
		// NOLINTNEXTLINE(clang-analyzer-core.DivideZero)  // Round 函数即四舍五入取偶
		m_score = static_cast<uint8_t>(std::round(static_cast<double>(totalScore) / samples));

		// Call the listener if the global score has changed.
		if (m_score != previousScore)
		{
			DEBUG_EX_LOG(" score, [added score:%" PRIu8 ", previous computed score:%" PRIu8 ", new computed score:% " PRIu8 "] (calling listener)",
				score,
				previousScore,
				m_score);

			// If previous score was 0 (and new one is not 0) then update activeSinceMs.
			if (previousScore == 0u)
			{
				m_activeSinceMs = uv_util::GetTimeMs();
			}

			//this->listener->OnRtpStreamScore(this, this->score, previousScore);
		}
		else
		{
#if MS_LOG_DEV_LEVEL == 3
			MS_DEBUG_TAG(
				score,
				"[added score:%" PRIu8 ", previous computed score:%" PRIu8 ", new computed score:%" PRIu8
				"] (no change)",
				score,
				previousScore,
				this->score);
#endif
		}
	}

	void crtp_stream_recv::packet_retransmitted(RTC::RtpPacket * packet)
	{
		m_packetsRetransmitted++;
	}

	void crtp_stream_recv::packet_repaired(RTC::RtpPacket * packet)
	{
		m_packetsRepaired++;
	}

	void crtp_stream_recv::_init_seq(uint16 seq)
	{
		// Initialize/reset RTP counters.
		m_baseSeq = seq;
		m_maxSeq = seq;
		m_badSeq = RtpSeqMod + 1; // So seq == badSeq is false.
	}

}