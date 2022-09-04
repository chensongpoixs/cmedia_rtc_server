/***********************************************************************************************
created: 		2022-09-03

author:			chensong

purpose:		crtc_sdp


************************************************************************************************/

#ifndef _C_RTC_SDP_H_
#define _C_RTC_SDP_H_
#include <iostream>
#include <cstdbool>
#include <cstdint>
#include <string>
#include "RtcSdp.pb.h"
#include "crtc_sdp_define.h"
namespace chen {
	class crtc_sdp
	{
	public:
		crtc_sdp()
			: m_session_td()
			, m_session_description()
			, m_current_pos(0)
		{}
		~crtc_sdp();
		bool init(const std::string & sdp);

	protected:
	private:
		bool _parse_session_description(const std::string & session_sdp_description);
		bool _parse_media_description(const std::string & media_sdp_description);


	private:
		bool _parse_connection_data(const std::string& line/*, rtc::SocketAddress* addr*/);


		bool _parse_group_attribute(const std::string& line/*,
			cricket::SessionDescription* desc,
			SdpParseError* error*/);
		bool _parse_ice_options(const std::string& line,
			Transport_Description& transport_options);
		bool _parse_fingerprint_attribute(
			const std::string& line,
			Ssl_Fignerpint* fingerprint);

		bool _parse_dtls_setup(const std::string& line,
			Transport_Description& role);

		bool _parse_extmap(const std::string& line,
			Rtp_Extension* extmap);



		/////////////////////////////
		bool _parse_content(const std::string& message,
			const  MediaType media_type,
			int mline_index,
			const std::string& protocol,
			const std::vector<int>& payload_types,
			size_t* pos,
			std::string* content_name,
			bool* bundle_only,
			int* msid_signaling,  
			Transport_Description& transport);
	private:
		Transport_Description			m_session_td;
		Session_Description				m_session_description;

		size_t							m_current_pos;
	};
}
#endif // _C_RTC_SDP_H_