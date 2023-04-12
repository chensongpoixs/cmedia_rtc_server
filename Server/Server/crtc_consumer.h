/***********************************************************************************************
created: 		2023-01-15

author:			chensong

purpose:		_C_RTC_TRACK_DESCRIPTION_H_


************************************************************************************************/

#ifndef _C_RTC_CONSUMER_H_
#define _C_RTC_CONSUMER_H_
#include "cnet_type.h"
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include "crtc_sdp.h"
#include "cmedia_desc.h"
#include "crtx_paylod_des.h"
#include "cred_payload.h"
#include "cvideo_payload.h"
#include "caudio_payload.h"
#include "cred_payload.h"
#include "crtp_stream_send.h"
#include "crtc_producer.h"
namespace chen {
	class crtc_consumer
	{
	public:
		crtc_consumer(crtc_transport * ptr, const std::string & kind, const crtc_producer::crtp_params& params);
			
		~crtc_consumer();

	public:
		void send_rtp_packet(RTC::RtpPacket* packet);


		void receive_rtcp_receiver_report(RTC::RTCP::ReceiverReport* report);
		float get_rtt();
	protected:
	private:
		crtc_transport			*			m_rtc_ptr;
		std::string							m_kind;
		crtc_producer::crtp_params			m_rtp_params;

		crtp_stream_send *					m_rtp_stream_send_ptr;

		bool								m_sync_required;

		RTC::SeqManager<uint16>				m_rtp_seq_manager;
	};
}

#endif // _C_RTC_CONSUMER_H_