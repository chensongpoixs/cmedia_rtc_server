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