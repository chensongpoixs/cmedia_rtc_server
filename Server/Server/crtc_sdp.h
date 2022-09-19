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
#include "Producer.hpp"
//#include "RtcSdp.pb.h"
#include "crtc_sdp_define.h"
#include "DtlsTransport.hpp"
namespace chen {
	class crtc_sdp
	{
	private:
		
	public:
		crtc_sdp()
			/*: m_session_td()
			, m_session_description()*/
			: m_client_sdp("")
			, m_current_pos(0)
			, m_media_datas()
			, m_rtp_parameter()
			, m_finger_print()
		{}
		~crtc_sdp();
		bool init(const std::string & sdp);


		void destroy();
	public:
		const std::vector< RTC::RtpParameters> & get_rtp_parameters() const { return m_media_datas; }
		const RTC::DtlsTransport::Fingerprint  get_finger_print() const { return  m_finger_print; }
	public:
		std::string get_webrtc_sdp() const ;
	protected:
	private:
		//bool _parse_session_description(const std::string & session_sdp_description);
		//bool _parse_media_description(const std::string & media_sdp_description);

		bool  _get_line_data(size_t & read_size);
		
	private:
		void _config_media();
	private:
		bool  _handler_sdp_v(const uint8_t * line_data, size_t line_data_size);
		bool  _handler_sdp_o(const uint8_t * line_data, size_t line_data_size);
		bool  _handler_sdp_t(const uint8_t * line_data, size_t line_data_size);
		bool  _handler_sdp_a(const uint8_t * line_data, size_t line_data_size);
		bool  _handler_sdp_m(const uint8_t * line_data, size_t line_data_size);
		bool  _handler_sdp_c(const uint8_t * line_data, size_t line_data_size);
		//bool  _handler_sdp_a(const uint8_t * line_data, size_t line_data_size);
	private:
		bool _parse_line_value_array(const uint8_t * line_data, size_t cur_index, size_t line_data_size, std::vector<std::string> & data);

		// key=value; key=value;...
		bool _parse_value_array(const std::string & value, RTC::Parameters & data);
		//bool _handler_sdp_a_group(const uint8_t * line_data, size_t cur_index, size_t line_data_size);
		//bool _handler_sdp_a_msid_semantic(const uint8_t * line_data, size_t cur_index, size_t line_data_size);
		//bool _parse_connection_data(const std::string& line, Socket_Address & addr/*, rtc::SocketAddress* addr*/);


		 
	private:
		//Transport_Description			m_session_td;
		//Session_Description				m_session_description;
		std::string							m_client_sdp;
		size_t								m_current_pos;
		std::vector< RTC::RtpParameters>	m_media_datas;
		RTC::RtpParameters					m_rtp_parameter;

		// crypto
		RTC::DtlsTransport::Fingerprint	    m_finger_print;
	};
}
#endif // _C_RTC_SDP_H_