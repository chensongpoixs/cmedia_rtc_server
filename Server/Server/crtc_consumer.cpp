/***********************************************************************************************
created: 		2023-01-15

author:			chensong

purpose:		_C_RTC_TRACK_DESCRIPTION_H_


************************************************************************************************/
#include "crtc_consumer.h"
#include "crtc_transport.h"
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

	void crtc_consumer::send_rtp_packet(RTC::RtpPacket* packet)
	{
		uint8_t payload_type  = packet->GetPayloadType();
		if (m_sync_required && m_rtp_params.params.type == "video" && packet->IsKeyFrame())
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
		if (m_rtp_params.params.rtx_ssrc == origSsrc)
		{
			 
			packet->SetPayloadType(m_rtp_params.params.rtx_payload_type);
		}
		else
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
			// Send the packet.
			//this->listener->OnConsumerSendRtpPacket(this, packet);
			m_rtc_ptr->send_rtp_data(packet);
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
		//packet->SetSsrc(origSsrc);
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

}