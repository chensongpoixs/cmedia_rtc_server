/***********************************************************************************************
created: 		2022-08-10

author:			chensong

purpose:		room

Copyright boost
************************************************************************************************/
#include "crtp_stream.h"
#include "crtp_stream_define.h"
#include "SeqManager.hpp"
#include "crtx_stream.h"
namespace chen {
	/*crtp_stream::crtp_stream(const crtp_stream_params & params)
	{
	}*/
	crtp_stream::~crtp_stream()
	{
	}
	void crtp_stream::set_rtx(uint8 payload_type, uint32 ssrc)
	{
		/*m_params.rtx_payload_type = payload_type;
		m_params.rtx_ssrc = ssrc;*/

		if (has_rtx())
		{
			delete m_rtx_stream_ptr;
			m_rtx_stream_ptr = nullptr;
		}

		// Set RTX stream params.
		 crtx_stream::crtx_params rtx_params;
		 rtx_params.clock_rate = 9000;
		 rtx_params.payload_type = m_params.rtx_payload_type;
		 rtx_params.ssrc = m_params.rtx_ssrc;
		 rtx_params.type = "rtx";
		 rtx_params.subtype = "rtx";

		//params.ssrc = ssrc;
		//params.payloadType = payloadType;
		//params.mimeType.type = GetMimeType().type;
		//params.mimeType.subtype = RTC::RtpCodecMimeType::Subtype::RTX;
		///*params.clockRate = GetClockRate();
		//params.rrid = GetRid();
		//params.cname = GetCname();*/

		//// Tell the RtpCodecMimeType to update its string based on current type and subtype.
		//params.mimeType.UpdateMimeType();

		 m_rtx_stream_ptr = new crtx_stream(rtx_params);
	}
	bool crtp_stream::receive_packet(RTC::RtpPacket * packet)
	{
		uint16  seq = packet->GetSequenceNumber();

		// If this is the first packet seen, initialize stuff.
		if (!m_started)
		{
			_init_seq(seq);

			m_started = true;
			m_max_seq = seq - 1;
			m_max_packet_ts  = packet->GetTimestamp();
			m_max_packet_ms = uv_util::GetTimeMs();
		}

		// If not a valid packet ignore it.
		if (!update_seq(packet))
		{
			WARNING_EX_LOG( "invalid packet [ssrc:%" PRIu32 ", seq:%" PRIu16 "]",
				packet->GetSsrc(),
				packet->GetSequenceNumber());

			return false;
		}

		// Update highest seen RTP timestamp.
		if (RTC::SeqManager<uint32_t>::IsSeqHigherThan(packet->GetTimestamp(), m_max_packet_ts))
		{
			m_max_packet_ts = packet->GetTimestamp();
			m_max_packet_ms = uv_util::GetTimeMs();
		}

		return true;
		return true;
	}
	void crtp_stream::reset_score(uint8 score, bool notify)
	{
	}
	bool crtp_stream::update_seq(RTC::RtpPacket * packet)
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
				WARNING_EX_LOG("rtx too bad sequence number, re-syncing RTP [ssrc:%" PRIu32 ", seq:%" PRIu16 "]",
					packet->GetSsrc(),
					packet->GetSequenceNumber());

				_init_seq(seq);

				m_max_packet_ts = packet->GetTimestamp();
				m_max_packet_ms = uv_util::GetTimeMs();
			}
			else
			{
				WARNING_EX_LOG("rtx bad sequence number, ignoring packet [ssrc:%" PRIu32 ", seq:%" PRIu16 "]",
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
	void crtp_stream::update_score(uint8 score)
	{
		// Add the score into the histogram.
		if ( m_scores.size() == ScoreHistogramLength)
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
		for (uint8 score : m_scores)
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
			WARNING_EX_LOG( "[added score:%" PRIu8 ", previous computed score:%" PRIu8 ", new computed score:%" PRIu8
				"] (calling listener)",
				score,
				previousScore,
				m_score);

			// If previous score was 0 (and new one is not 0) then update activeSinceMs.
			if (previousScore == 0u)
			{
				m_active_since_ms = uv_util::GetTimeMs();
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
	void crtp_stream::packet_retransmitted(RTC::RtpPacket * packet)
	{
		++m_packets_retransmitted;
	}
	void crtp_stream::packet_repaired(RTC::RtpPacket * packet)
	{
		++m_packets_repaired;
	}
	void crtp_stream::_init_seq(uint16 seq)
	{
		m_base_seq = seq;
		m_max_seq = seq;
		m_bad_seq = RtpSeqMod + 1;
	}
}