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
#include "crtp_stream_recv.h"
#include "cuv_util.h"
#include "SeqManager.hpp"
#include "crtp_stream_define.h"
#include "crtp_stream.h"
#include "cbuffer.h"
#include "cav1.h"
#include "H264.hpp"
#include "FeedbackRtpNack.hpp"
 #include "crtc_producer.h"
#include "cvideo_rtp_depacketizer_av1.h"
namespace chen {

	/* Static. */

	 

	crtp_stream_recv::crtp_stream_recv(crtc_producer *ptr, const crtp_stream::crtp_stream_params & params)
		: m_rtc_producer_ptr(ptr)
		, crtp_stream(params)
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
		, m_rtt(0)
		, m_has_rtt(false)
		, m_packet_buffer()
		//, m_dav1d_decoder()
		, m_timestamp(0)
		, m_stoped(false)
	{
		m_packet_buffer.init(500);
		if ( params.use_nack)
		{
			  nackGenerator.reset(new RTC::NackGenerator(this));
		}
		// Run the RTP inactivity periodic timer (use a different timeout if DTX is
		// enabled).
		//m_inactivityCheckPeriodicTimer = new ctimer(this);
		m_inactive = false;

		//if (!this->params.useDtx)
		/*{
			m_inactivityCheckPeriodicTimer->Start(InactivityCheckInterval);
		}*/
		m_timestamp = uv_util::GetTimeMs() + InactivityCheckInterval;

		//m_dav1d_decoder.init();
		 
	}

	crtp_stream_recv::~crtp_stream_recv()
	{
		//if (m_inactivityCheckPeriodicTimer)
		//{
		//	// Close the RTP inactivity check periodic timer.
		//	delete this->m_inactivityCheckPeriodicTimer;
		//	m_inactivityCheckPeriodicTimer = NULL;
		//}
		m_stoped = true;
		
	}
 
	void crtp_stream_recv::destroy()
	{
		if (nackGenerator)
		{
			nackGenerator->destroy();
			nackGenerator.reset(NULL);
		}
		m_stoped = true;
		// Close the RTP inactivity check periodic timer.
		//if (m_inactivityCheckPeriodicTimer)
		//{
		//	// Close the RTP inactivity check periodic timer.
		//	delete this->m_inactivityCheckPeriodicTimer;
		//	m_inactivityCheckPeriodicTimer = NULL;
		//}
	}

	bool crtp_stream_recv::receive_packet(RTC::RtpPacket * packet)
	{
		// Call the parent method.
		if (!crtp_stream::receive_packet(packet))
		{
			WARNING_EX_LOG("rtp, packet discarded[seq = %u]", packet->GetSequenceNumber());

			return false;
		}
		if (m_params.type == EMediaVideo)
		{
			//NORMAL_EX_LOG("[payload_type = %u][seq =%u]", packet->GetPayloadType(), packet->GetSequenceNumber());
			if (m_params.subtype != EDataH264)
			{
				cav1::ProcessRtpPacket(packet);
#if 0
				auto timestamp =
					std::chrono::duration_cast<std::chrono::milliseconds>(
						std::chrono::system_clock::now().time_since_epoch())
					.count();
				if (packet->GetPayloadLength() > 0)
				{
					cvcm_packet vcm_data;
					vcm_data.m_data_ptr = new uint8[packet->GetPayloadLength()];
					memcpy(vcm_data.m_data_ptr, packet->GetPayload(), packet->GetPayloadLength());
					vcm_data.m_size_bytes = packet->GetPayloadLength();
					vcm_data.m_seq_num = packet->GetSequenceNumber();
					vcm_data.m_timestamp = packet->GetTimestamp();
					vcm_data.m_video_header.is_first_packet_in_frame = packet->is_first_packet_in_frame();
					vcm_data.m_video_header.is_last_packet_in_frame = packet->is_last_packet_in_frame();
					cvideo_rtp_depacketizer_av1  rtp_depacketizer_av1;
					std::vector<cvcm_packet> fonds_frames = m_packet_buffer.insert_packet(&vcm_data);
					std::vector<cvcm_encoded_frame> vcm_encoded_frame = rtp_depacketizer_av1.AssembleFrame(fonds_frames);
					m_packet_buffer.remove_packet(fonds_frames);
					for (cvcm_encoded_frame& encoded_frame : vcm_encoded_frame)
					{
#if 0
						static FILE * out_file_ptr = ::fopen("./av1/test_webrtc.mp4", "wb+");;
						if (out_file_ptr)
						{
							//static bool ret = true;
							//if (ret)
							{
								uint8 p[2] = { 0X12u, 0u };
								::fwrite(&p[0], 1, 2, out_file_ptr);
								::fflush(out_file_ptr);
								//ret = false;
							}
							::fwrite(encoded_frame.data(), 1, encoded_frame.data_size(), out_file_ptr);
							//	NORMAL_EX_LOG("[rtp_packet][rtc_count = %u][packet_size = %u]", rtc_count, frame_data.data_size());
							::fflush(out_file_ptr);

						}
#endif
						m_dav1d_decoder.Decode(encoded_frame, false, 0);
						encoded_frame.free();
					}


				}

				auto timestamp_curr = std::chrono::duration_cast<std::chrono::milliseconds>(
					std::chrono::system_clock::now().time_since_epoch())
					.count();
				NORMAL_EX_LOG(" AV1 packet encoder render  %u], seq = %u, rtp = %u", (timestamp_curr - timestamp), packet->GetSequenceNumber(), packet->GetTimestamp());
#endif
			}
			else
			{ 
				RTC::Codecs::H264::ProcessRtpPacket(packet);
			}
#if 0
			
			
			
			/*static FILE * out_file_ptr = ::fopen("test_chensong_av1.mp4", "wb+");
			if (out_file_ptr)
			{
				::fwrite(packet->GetPayload(), 1, packet->GetPayloadLength(), out_file_ptr);
				::fflush(out_file_ptr);
			}*/
#endif //
		}
		// Process the packet at codec level.
		//if (packet->GetPayloadType() == GetPayloadType())
		//{
		//	RTC::Codecs::Tools::ProcessRtpPacket(packet, GetMimeType());
		//	//packet->Dump();
		//}

		// Pass the packet to the NackGenerator.
		if (m_params.use_nack)
		{
			
			// If there is RTX just provide the NackGenerator with the packet.
			if (has_rtx())
			{
				this->nackGenerator->ReceivePacket(packet, /*isRecovered*/ false);
			}
			// If there is no RTX and NackGenerator returns true it means that it
			// was a NACKed packet.
			else if (this->nackGenerator->ReceivePacket(packet, /*isRecovered*/ false))
			{
				// Mark the packet as retransmitted and repaired.
				//           packet_retransmitted
				crtp_stream::packet_retransmitted(packet);
				crtp_stream::packet_repaired(packet);
			}
		}
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
		/*if (this->m_inactivityCheckPeriodicTimer)
		{
			this->m_inactivityCheckPeriodicTimer->Restart();
		}*/
		m_timestamp = uv_util::GetTimeMs() + InactivityCheckInterval;
		return true;
	}
	bool crtp_stream_recv::receive_rtx_packet(RTC::RtpPacket * packet)
	{
		if (!m_params.use_nack)
		{
			WARNING_EX_LOG( "NACK not supported");

			return false;
		}
		//NORMAL_EX_LOG("");
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
		if (m_params.type == EMediaVideo)
		{
			//RTC::Codecs::H264::ProcessRtpPacket(packet);
			//cav1::ProcessRtpPacket(packet);
			if (m_params.subtype != EDataH264)
			{
				cav1::ProcessRtpPacket(packet);
#if 0
				 auto timestamp =
					std::chrono::duration_cast<std::chrono::milliseconds>(
						std::chrono::system_clock::now().time_since_epoch())
					.count();
				if (packet->GetPayloadLength() > 0)
				{
					cvcm_packet vcm_data;
					vcm_data.m_data_ptr = new uint8[packet->GetPayloadLength()];
					memcpy(vcm_data.m_data_ptr, packet->GetPayload(), packet->GetPayloadLength());
					vcm_data.m_size_bytes = packet->GetPayloadLength();
					vcm_data.m_seq_num = packet->GetSequenceNumber();
					vcm_data.m_timestamp = packet->GetTimestamp();
					vcm_data.m_video_header.is_first_packet_in_frame = packet->is_first_packet_in_frame();
					vcm_data.m_video_header.is_last_packet_in_frame = packet->is_last_packet_in_frame();
					cvideo_rtp_depacketizer_av1  rtp_depacketizer_av1;
					std::vector<cvcm_packet> fonds_frames = m_packet_buffer.insert_packet(&vcm_data);
					std::vector<cvcm_encoded_frame> vcm_encoded_frame = rtp_depacketizer_av1.AssembleFrame(fonds_frames);
					m_packet_buffer.remove_packet(fonds_frames);
					for (cvcm_encoded_frame& encoded_frame : vcm_encoded_frame)
					{
#if 0
						static FILE * out_file_ptr = ::fopen("./av1/test_webrtc.mp4", "wb+");;
						if (out_file_ptr)
						{
							//static bool ret = true;
							//if (ret)
							{
								uint8 p[2] = { 0X12u, 0u };
								::fwrite(&p[0], 1, 2, out_file_ptr);
								::fflush(out_file_ptr);
								//ret = false;
							}
							::fwrite(encoded_frame.data(), 1, encoded_frame.data_size(), out_file_ptr);
							//	NORMAL_EX_LOG("[rtp_packet][rtc_count = %u][packet_size = %u]", rtc_count, frame_data.data_size());
							::fflush(out_file_ptr);

						}
#endif
						m_dav1d_decoder.Decode(encoded_frame, false, 0);
						encoded_frame.free();
					}


				}

				auto timestamp_curr = std::chrono::duration_cast<std::chrono::milliseconds>(
					std::chrono::system_clock::now().time_since_epoch())
					.count();
				NORMAL_EX_LOG(" AV1 packet encoder render  %u", (timestamp_curr - timestamp));
#endif
			}
			else
			{
				RTC::Codecs::H264::ProcessRtpPacket(packet);
			}
		}

		// Mark the packet as retransmitted.
		crtp_stream::packet_retransmitted(packet);

		// Pass the packet to the NackGenerator and return true just if this was a
		// NACKed packet.
		if (this->nackGenerator->ReceivePacket(packet, /*isRecovered*/ true))
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
			/*if (this->m_inactivityCheckPeriodicTimer)
			{
				this->m_inactivityCheckPeriodicTimer->Restart();
			}*/
			m_timestamp = uv_util::GetTimeMs() + InactivityCheckInterval;
			return true;
		}

		return false; 
	}

	RTC::RTCP::ReceiverReport* crtp_stream_recv::get_rtcp_receiver_report()
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
		//crtcp_rr report;
		//report.set_ssrc(get_ssrc());
		  report->SetSsrc(get_ssrc()) ;
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

			//report.set_lost_packets(m_reported_packet_lost);
			//report.set_lost_rate(m_fraction_lost);
			report->SetTotalLost(m_reported_packet_lost);
			report->SetFractionLost(m_fraction_lost);
		}
		else
		{
			// Recalculate packetsLost.
			uint32_t newLostInterval = (worstRemoteFractionLost * expectedInterval) >> 8;

			m_reported_packet_lost += newLostInterval;
			report->SetTotalLost(m_reported_packet_lost);
			report->SetFractionLost(worstRemoteFractionLost);
			//report.set_lost_packets(m_reported_packet_lost);
			//report.set_lost_rate(worstRemoteFractionLost);
		}

		// Fill the rest of the report.
		//report.set_highest_sn(static_cast<uint32 >(m_max_seq) + m_cycles);
		//report.set_jitter(m_jitter);
		report->SetLastSeq(static_cast<uint32_t>(m_max_seq) + m_cycles);
		report->SetJitter(m_jitter);


		if (m_last_sr_received != 0)
		{
			// Get delay in milliseconds.
			auto delayMs = static_cast<uint32_t>(uv_util::GetTimeMs() - m_last_sr_received);
			// Express delay in units of 1/65536 seconds.
			uint32_t dlsr = (delayMs / 1000) << 16;

			dlsr |= uint32_t{ (delayMs % 1000) * 65536 / 1000 };

			//report.set_dlsr(dlsr);
			//report.set_lsr(m_last_sr_timestamp);
			report->SetDelaySinceLastSenderReport(dlsr);
			report->SetLastSenderReport(m_last_sr_timestamp);
		}
		else
		{
			//report.set_dlsr(0);
			//report.set_lsr(0);
			report->SetDelaySinceLastSenderReport(0);
			report->SetLastSenderReport(0);
		}

		return report;
	}

	RTC::RTCP::ReceiverReport* crtp_stream_recv::get_rtx_rtcp_receiver_report()
	{
		if (has_rtx())
		{
			return  m_rtx_stream_ptr->get_rtcp_receiver_report();
		}
		return NULL;
	}

	void crtp_stream_recv::receive_rtcp_sender_report(RTC::RTCP::SenderReport* report)
	{
		m_last_sr_received = uv_util::GetTimeMs();
		m_last_sr_timestamp = report->GetNtpSec() << 16;
		m_last_sr_timestamp += report->GetNtpFrac() >> 16;

		// Update info about last Sender Report.
		rtc_time::Ntp ntp; // NOLINT(cppcoreguidelines-pro-type-member-init)

		ntp.seconds = report->GetNtpSec();
		ntp.fractions = report->GetNtpFrac();

		m_last_sender_report_ntp_ms = rtc_time::Ntp2TimeMs(ntp);
		m_last_sender_repor_ts = report->GetRtpTs();

		// Update the score with the current RR.
		update_score();
	}

	void crtp_stream_recv::receive_rtx_rtcp_sender_report(RTC::RTCP::SenderReport* report)
	{
		if (has_rtx())
		{
			m_rtx_stream_ptr->receive_rtcp_sender_report(report);
		}
	}

	void crtp_stream_recv::receive_rtcp_xrdelay_since_lastrr(RTC::RTCP::DelaySinceLastRr::SsrcInfo * ssrcInfo)
	{
		// Get the NTP representation of the current timestamp.
		uint64_t nowMs = uv_util::GetTimeMs();
		auto ntp = rtc_time::TimeMs2Ntp(nowMs);

		// Get the compact NTP representation of the current timestamp.
		uint32_t compactNtp = (ntp.seconds & 0x0000FFFF) << 16;

		compactNtp |= (ntp.fractions & 0xFFFF0000) >> 16;

		uint32_t lastRr = ssrcInfo->GetLastReceiverReport();
		uint32_t dlrr = ssrcInfo->GetDelaySinceLastReceiverReport();

		// RTT in 1/2^16 second fractions.
		uint32_t rtt{ 0 };

		// If no Receiver Extended Report was received by the remote endpoint yet,
		// ignore lastRr and dlrr values in the Sender Extended Report.
		if (lastRr && dlrr && (compactNtp > dlrr + lastRr))
		{
			rtt = compactNtp - dlrr - lastRr;
		}

		// RTT in milliseconds.
		this->m_rtt = static_cast<float>(rtt >> 16) * 1000;
		this->m_rtt += (static_cast<float>(rtt & 0x0000FFFF) / 65536) * 1000;

		if (this->m_rtt > 0.0f)
		{
			this->m_has_rtt = true;
		}

		// Tell it to the NackGenerator.
		if (this->m_params.use_nack)
		{
			this->nackGenerator->UpdateRtt(static_cast<uint32_t>(this->m_rtt));
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
		/*if (this->m_inactivityCheckPeriodicTimer)
		{
			this->m_inactivityCheckPeriodicTimer->Stop();
		}*/
		m_stoped = true;
		 if (m_params.use_nack)
		 {
			 this->nackGenerator->Reset();
		 }

		// Reset jitter.
		m_transit = 0;
		m_jitter = 0;
	}

	void crtp_stream_recv::resume()
	{
		if (/*this->m_inactivityCheckPeriodicTimer &&*/ !this->m_inactive)
		{
			//this->m_inactivityCheckPeriodicTimer->Restart();
		 }
	}
	 
	/*void crtp_stream_recv::set_rtx(uint8 payload_type, uint32 ssrc)
	{
	}*/

	void crtp_stream_recv::OnTimer(ctimer * timer)
	{
		if (!m_stoped &&/*timer == this->m_inactivityCheckPeriodicTimer*/ m_timestamp < uv_util::GetTimeMs())
		{
			m_inactive = true;

			if (get_score() != 0)
			{
				WARNING_EX_LOG("RTP inactivity detected, resetting score to 0 [ssrc:%" PRIu32 "]", get_ssrc());
			}

			reset_score(0, /*notify*/ true);
			m_timestamp = uv_util::GetTimeMs() + InactivityCheckInterval;
		}
	}

	void crtp_stream_recv::del_rtx()
	{
		if (m_rtx_stream_ptr)
		{
			delete m_rtx_stream_ptr;
			m_rtx_stream_ptr = NULL;
		}
	}

	void crtp_stream_recv::OnNackGeneratorNackRequired(const std::vector<uint16>& seqNumbers)
	{
	//	cassert_desc(m_params.use_nack, "NACK required but not supported");

		DEBUG_EX_LOG( "triggering NACK [PayloadType:%" PRIu32 ",ssrc:%" PRIu32 ", first seq:%" PRIu16 ", num packets:%zu]",
			 get_payload_type(), m_params.ssrc,
			seqNumbers[0],
			seqNumbers.size());

		RTC::RTCP::FeedbackRtpNackPacket packet(0, get_ssrc());

		auto it = seqNumbers.begin();
		const auto end = seqNumbers.end();
		size_t numPacketsRequested{ 0 };

		while (it != end)
		{
			uint16_t seq;
			uint16_t bitmask{ 0 };

			seq = *it;
			++it;
			//////////////////////////////////////////////////////////////
			//[uint16_t ]	[uint16_t]
			//[seq]			[0000 0000 0000 0000 ]
			//////////////////////////////////////////////////////////////
			while (it != end)
			{
				uint16_t shift = *it - seq - 1;

				if (shift > 15)
				{
					break;
				}

				bitmask |= (1 << shift);
				++it;
			}

			auto* nackItem = new RTC::RTCP::FeedbackRtpNackItem(seq, bitmask);

			packet.AddItem(nackItem);

			numPacketsRequested += nackItem->CountRequestedPackets();
		}

		// Ensure that the RTCP packet fits into the RTCP buffer.
		if (packet.GetSize() > RTC::RTCP::BufferSize)
		{
			WARNING_EX_LOG("rtx, cannot send RTCP NACK packet, size too big (%zu bytes)", packet.GetSize());

			return;
		}
		// 发送feedback 个数
		m_nack_count++;
		// 发送一共nack个数
		m_nack_packet_count += numPacketsRequested;

		packet.Serialize(RTC::RTCP::Buffer);
		if (m_rtc_producer_ptr)
		{
			m_rtc_producer_ptr->OnProducerSendRtcpPacket(&packet);
		}
		// Notify the listener.
		//static_cast<RTC::RtpStreamRecv::Listener*>(this->listener)->OnRtpStreamSendRtcpPacket(this, &packet);
		//crtcp_nack rtcp_nack(get_ssrc());
		//RTC::RTCP::FeedbackRtpNackPacket packet(0, GetSsrc());
		/*for (std::vector<uint16>::const_iterator iter = seqNumbers.begin(); iter != seqNumbers.end(); ++iter)
		{
			rtcp_nack.add_lost_sn(*iter);
		}

		char buffer[RTC::RTCP::BufferSize] = {0};
		cbuffer stream(&buffer[0], sizeof(buffer));
		rtcp_nack.encode(&stream);*/
		// auto it = seqNumbers.begin();
		// const auto end = seqNumbers.end();
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

		//m_nack_count++;
		

	//	packet.Serialize(RTC::RTCP::Buffer);
		//m_nack_packet_count += m_num_packets_requested;
		// Notify the listener.
		// static_cast<RTC::RtpStreamRecv::Listener*>(this->listener)->OnRtpStreamSendRtcpPacket(this, &packet);
	}

	void crtp_stream_recv::OnNackGeneratorKeyFrameRequired()
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

		//cassert(retransmitted >= repaired, "repaired packets cannot be more than retransmitted ones");

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