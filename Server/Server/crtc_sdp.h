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
	public:
		const std::vector< RTC::RtpParameters> & get_rtp_parameters() const { return m_media_datas; }
		const RTC::DtlsTransport::Fingerprint& get_finger_print() const { m_finger_print; }
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


		//bool _parse_group_attribute(const std::string& line/*,
		//	cricket::SessionDescription* desc,
		//	SdpParseError* error*/);
		//bool _parse_ice_options(const std::string& line,
		//	Transport_Description& transport_options);
		//bool _parse_fingerprint_attribute(
		//	const std::string& line,
		//	Ssl_Fignerpint* fingerprint);

		//bool _parse_dtls_setup(const std::string& line,
		//	Transport_Description& role);

		//bool _parse_extmap(const std::string& line,
		//	Rtp_Extension* extmap);



		///////////////////////////////
		//bool _parse_content(const std::string& message,
		//	const  MediaType media_type,
		//	int mline_index,
		//	const std::string& protocol,
		//	const std::vector<int>& payload_types,
		//	size_t* pos,
		//	std::string* content_name,
		//	bool* bundle_only,
		//	int* msid_signaling,  
		//	Transport_Description& transport);


		//bool _parse_fmtp_attributes(const std::string& line,
		//	const MediaType media_type,
		//	Content_Info& content_info);


		//bool _parse_fmtp_param(const std::string& line,
		//	std::string* parameter,
		//	std::string* value);


		//bool _parse_sctp_port(const std::string& line,
		//	int* sctp_port);

		//bool _parse_sctp_max_message_size(const std::string& line,
		//	int* max_message_size);

		//bool _parse_ssrc_group_attribute(const std::string& line,
		//	std::vector<Ssrc_Group>* ssrc_groups);

		//bool _parse_ssrc_attribute(const std::string& line,
		//	/*SsrcInfoVec*/std::vector<Rtc_Ssrc_Info> * ssrc_infos,
		//	int* msid_signaling);


		//bool _parse_crypto_attribute(const std::string& line,
		//	Media_Content_Description& media_desc);


		//void _update_codec(MediaType media_type, Content_Info & content_info, int32_t payload_type, std::map<std::string , std::string> & codec_params);
	
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