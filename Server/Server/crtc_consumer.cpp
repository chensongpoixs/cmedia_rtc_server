/***********************************************************************************************
created: 		2023-01-15

author:			chensong

purpose:		_C_RTC_TRACK_DESCRIPTION_H_

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
#include "crtc_consumer.h"
#include "crtc_transport.h"

#include "ccfg.h"



namespace chen {
	crtc_consumer::crtc_consumer(crtc_transport * ptr, const std::string & kind, const crtc_producer::crtp_params & params)
	  : m_rtc_ptr(ptr)
	  , m_kind(kind)
		,m_rtp_params(params)
			, m_rtp_stream_send_ptr(NULL)
			, m_sync_required(true)
			, m_rtp_seq_manager()
	{
		m_rtp_stream_send_ptr = new crtp_stream_send(this, m_rtp_params.params);
		if (m_rtp_params.params.rtx_ssrc)
		{
			m_rtp_stream_send_ptr->set_rtx(m_rtp_params.params.rtx_payload_type, m_rtp_params.params.rtx_ssrc);
		}
	}
	 
	crtc_consumer::~crtc_consumer()
	{
		
	}

	void crtc_consumer::destroy()
	{
		if (m_rtp_stream_send_ptr)
		{
			delete m_rtp_stream_send_ptr;
			m_rtp_stream_send_ptr = NULL;
		}
		//m_rtp_seq_manager.destroy();
	}

	void crtc_consumer::send_rtp_packet(RTC::RtpPacket* packet)
	{
#if RTP_PACKET_MCS
		std::chrono::steady_clock::time_point cur_time_ms;
		std::chrono::steady_clock::time_point pre_time = std::chrono::steady_clock::now();
		std::chrono::steady_clock::duration dur;
		std::chrono::microseconds ms;
#endif 
		uint8_t payload_type  = packet->GetPayloadType();
		if (m_sync_required && m_rtp_params.params.type == EMediaVideo && !packet->IsKeyFrame())
		{
			WARNING_EX_LOG("keyframe ---> return");
			return;
		}
		// Whether this is the first packet after re-sync.
		bool isSyncPacket = m_sync_required;

		// Sync sequence number and timestamp if required.
		if (isSyncPacket)
		{
			if (packet->IsKeyFrame())
			{
				DEBUG_EX_LOG("rtp, sync key frame received");
			}

			m_rtp_seq_manager.Sync(packet->GetSequenceNumber() - 1);

			this->m_sync_required = false;
		}

		// Update RTP seq number and timestamp.
		uint16_t seq;

		this->m_rtp_seq_manager.Input(packet->GetSequenceNumber(), seq);

		// Save original packet fields.
		auto origSsrc = packet->GetSsrc();
		auto origSeq = packet->GetSequenceNumber();

		// Rewrite packet.
		//packet->SetSsrc(this->rtpParameters.encodings[0].ssrc);
		packet->SetSequenceNumber(seq);
		packet->SetSsrc(m_rtp_params.params.ssrc);
		/*if (m_rtp_params.params.rtx_ssrc == origSsrc)
		{

			packet->SetPayloadType(m_rtp_params.params.rtx_payload_type);
		}
		else*/
		{
			packet->SetPayloadType(m_rtp_params.params.payload_type);
		}
		
		if (isSyncPacket)
		{
			DEBUG_EX_LOG( "rtp sending sync packet [ssrc:%" PRIu32 ", seq:%" PRIu16 ", ts:%" PRIu32
				"] from original [seq:%" PRIu16 "]",
				packet->GetSsrc(),
				packet->GetSequenceNumber(),
				packet->GetTimestamp(),
				origSeq);
		}

		// Process the packet.
		if (m_rtp_stream_send_ptr->receive_packet(packet))
		{
#if RTP_PACKET_MCS
			cur_time_ms = std::chrono::steady_clock::now();
			dur = cur_time_ms - pre_time;
			ms = std::chrono::duration_cast<std::chrono::microseconds>(dur);
			if (ms.count() > 5)
			{
				WARNING_EX_LOG("udp   consumer recv seq  $$$$$$$$$$$$$$$$$$$$ packet [microseconds = %" PRIi64 "]", ms.count());
			}
			pre_time = cur_time_ms;
#endif 
			// Send the packet.
			//this->listener->OnConsumerSendRtpPacket(this, packet);
			//NORMAL_EX_LOG("client [send seq = %u][ssrc = %u][seq = %u][timestamp = %u]", origSeq, packet->GetSsrc(), packet->GetSequenceNumber(), packet->GetTimestamp());
			m_rtc_ptr->send_rtp_data(packet);
#if RTP_PACKET_MCS
			cur_time_ms = std::chrono::steady_clock::now();
			dur = cur_time_ms - pre_time;
			ms = std::chrono::duration_cast<std::chrono::microseconds>(dur);
			if (ms.count() > 5)
			{
				WARNING_EX_LOG("udp send consumer $$$$$$$$$$$$$$$$$$$$ packet [microseconds = %" PRIi64 "]", ms.count());
			}
#endif 
			// May emit 'trace' event.
			//EmitTraceEventRtpAndKeyFrameTypes(packet);
		}
		else
		{
			WARNING_EX_LOG( "rtp failed to send packet [ssrc:%" PRIu32 ", seq:%" PRIu16 ", ts:%" PRIu32
				"] from original [seq:%" PRIu16 "]",
				packet->GetSsrc(),
				packet->GetSequenceNumber(),
				packet->GetTimestamp(),
				origSeq);
		}

		// Restore packet fields.
		packet->SetSsrc(origSsrc);
		packet->SetSequenceNumber(origSeq);
	}

	void crtc_consumer::receive_nack(RTC::RTCP::FeedbackRtpNackPacket * nackPacket)
	{
		m_rtp_stream_send_ptr->receive_nack(nackPacket);
	}

	void crtc_consumer::receive_rtcp_receiver_report(RTC::RTCP::ReceiverReport * report)
	{
		m_rtp_stream_send_ptr->receive_rtcp_receiver_report(report);
	}

	float crtc_consumer::get_rtt()
	{
		return m_rtp_stream_send_ptr->get_rtt();
	}

	void crtc_consumer::receive_key_frame_request(RTC::RTCP::FeedbackPs::MessageType messageType, uint32_t ssrc)
	{
		switch (messageType)
		{
			case RTC::RTCP::FeedbackPs::MessageType::PLI:
			{
				//EmitTraceEventPliType(ssrc); 
				break;
			} 
			case RTC::RTCP::FeedbackPs::MessageType::FIR:
			{
				//EmitTraceEventFirType(ssrc); 
				break;
			}

			default:;
		}

		m_rtp_stream_send_ptr ->receive_key_frame_request(messageType);
		//RequestKeyFrame();
		request_key_frame();
	}

	void crtc_consumer::get_rtcp(RTC::RTCP::CompoundPacket * packet, uint64_t nowMs)
	{
		//MS_TRACE();
		////DEBUG_EX_LOG("");
		//MS_ASSERT(rtpStream == this->rtpStream, "RTP stream does not match");

		if (static_cast<float>((nowMs - this->m_last_rtcp_send_time) * 1.15) < 50)
			return;

		auto* report = m_rtp_stream_send_ptr->get_rtcp_sender_report(nowMs);

		if (!report)
			return;

		packet->AddSenderReport(report);

		// Build SDES chunk for this sender.
		/*auto* sdesChunk = this->rtpStream->GetRtcpSdesChunk();

		packet->AddSdesChunk(sdesChunk);*/

		this->m_last_rtcp_send_time = nowMs;
	}

	void crtc_consumer::OnRtpStreamRetransmitRtpPacket(crtp_stream * stream, RTC::RtpPacket * packet)
	{
		if (m_rtc_ptr)
		{
			m_rtc_ptr->OnConsumerRetransmitRtpPacket(this, packet);
		}
	}

	void crtc_consumer::request_key_frame()
	{
		if (m_kind != "video")
		{
			return;
		}
		if (m_rtc_ptr)
		{
			m_rtc_ptr->OnTransportConsumerKeyFrameRequested();
		}
	}

	uint32 crtc_consumer::get_desired_bitrate() const
	{
		if (m_kind != "video")
		{
			return 0u;
		}
		auto nowMs = uv_util::GetTimeMs();
		auto desiredBitrate = this->m_rtp_stream_send_ptr->get_bitrate(nowMs);

		// If consumer.rtpParameters.encodings[0].maxBitrate was given and it's
		// greater than computed one, then use it.
		//auto maxBitrate = this->rtpParameters.encodings[0].maxBitrate;
		static const uint32 maxBitrate = g_cfg.get_uint32(ECI_RtcMaxBitrate);
		if (maxBitrate > desiredBitrate)
		{
			desiredBitrate = maxBitrate;
		 }

		return desiredBitrate;
		return uint32();
	}

}