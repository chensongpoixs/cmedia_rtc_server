/***********************************************************************************************
created: 		2023-03-12

author:			chensong

purpose:		rtc_producer


************************************************************************************************/

#ifndef _C_RTC_PRODUCER_H_
#define _C_RTC_PRODUCER_H_
#include "cnet_type.h"
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include "crtc_sdp.h"
#include "crtp_stream.h"
#include "crtp_stream_recv.h"
#include "crtp_stream_send.h"

namespace chen {
	class crtc_transport;
	class crtc_producer
	{
	public:
		struct crtp_params 
		{
			std::string mid;
			crtp_stream::crtp_stream_params   params;

		};
	public:
		explicit crtc_producer(crtc_transport *ptr, const std::string & kind, const crtp_params& params, const std::map<uint32, uint32> & ssrc_table)
		: m_kind(kind)
		, m_params(params)
		, m_ssrc_rtp_stream_map()
		, m_rtx_ssrc_rtp_stream_map()
		, m_rtc_ptr(ptr)
		, m_server_ssrc_map(ssrc_table)
		{}
		virtual ~crtc_producer();

	public:

		std::string get_kind() const { return  m_kind; }


		 bool    receive_rtp_packet(RTC::RtpPacket* packet);
		void receive_rtcp_sender_report(RTC::RTCP::SenderReport* report);

		void request_key_frame(uint32 mapped_ssrc);

		bool  mangle_rtp_packet(RTC::RtpPacket * packet, const crtp_stream::crtp_stream_params & params);
		const crtp_params & get_rtcp_params() const { return m_params; }
	public:

	private:
		crtp_stream_recv * _get_rtp_stream(RTC::RtpPacket * packet);
		crtp_stream_recv*  _create_rtp_stream(RTC::RtpPacket* packet);
		
	protected:
	private:
		std::string									m_kind;
		crtp_params									m_params;

		std::map<uint32, crtp_stream_recv*>			m_ssrc_rtp_stream_map;
		std::map<uint32, crtp_stream_recv*>			m_rtx_ssrc_rtp_stream_map;
		crtc_transport *							m_rtc_ptr;
		std::map<uint32, uint32>					m_server_ssrc_map;
	};
}

#endif //