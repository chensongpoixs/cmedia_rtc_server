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
#include "cmedia_desc.h"
namespace chen {
	//class crtc_sdp
	//{
	//private:
	//	
	//public:
	//	crtc_sdp()
	//		/*: m_session_td()
	//		, m_session_description()*/
	//		: m_client_sdp("")
	//		, m_current_pos(0)
	//		, m_media_datas()
	//		, m_rtp_parameter()
	//		, m_finger_print()
	//	{}
	//	~crtc_sdp();
	//	bool init(const std::string & sdp);


	//	void destroy();
	//public:
	//	const std::vector< RTC::RtpParameters> & get_rtp_parameters() const { return m_media_datas; }
	//	const RTC::DtlsTransport::Fingerprint  get_finger_print() const { return  m_finger_print; }
	//public:
	//	std::string get_webrtc_sdp() const ;
	//protected:
	//private:
	//	//bool _parse_session_description(const std::string & session_sdp_description);
	//	//bool _parse_media_description(const std::string & media_sdp_description);

	//	bool  _get_line_data(size_t & read_size);
	//	
	//private:
	//	void _config_media();
	//private:
	//	bool  _handler_sdp_v(const uint8_t * line_data, size_t line_data_size);
	//	bool  _handler_sdp_o(const uint8_t * line_data, size_t line_data_size);
	//	bool  _handler_sdp_t(const uint8_t * line_data, size_t line_data_size);
	//	bool  _handler_sdp_a(const uint8_t * line_data, size_t line_data_size);
	//	bool  _handler_sdp_m(const uint8_t * line_data, size_t line_data_size);
	//	bool  _handler_sdp_c(const uint8_t * line_data, size_t line_data_size);
	//	//bool  _handler_sdp_a(const uint8_t * line_data, size_t line_data_size);
	//private:
	//	bool _parse_line_value_array(const uint8_t * line_data, size_t cur_index, size_t line_data_size, std::vector<std::string> & data);

	//	// key=value; key=value;...
	//	bool _parse_value_array(const std::string & value, RTC::Parameters & data);
	//	//bool _handler_sdp_a_group(const uint8_t * line_data, size_t cur_index, size_t line_data_size);
	//	//bool _handler_sdp_a_msid_semantic(const uint8_t * line_data, size_t cur_index, size_t line_data_size);
	//	//bool _parse_connection_data(const std::string& line, Socket_Address & addr/*, rtc::SocketAddress* addr*/);


	//	 
	//private:
	//	//Transport_Description			m_session_td;
	//	//Session_Description				m_session_description;
	//	std::string							m_client_sdp;
	//	size_t								m_current_pos;
	//	std::vector< RTC::RtpParameters>	m_media_datas;
	//	RTC::RtpParameters					m_rtp_parameter;

	//	// crypto
	//	RTC::DtlsTransport::Fingerprint	    m_finger_print;
	//};


	class crtc_sdp
	{
	public:
		explicit  crtc_sdp()
			: m_in_media_session(false)
			, m_version("")
			, m_username("")
			, m_session_id("")
			, m_session_version("")
			, m_nettype("")
			, m_addrtype("")
			, m_unicast_address("")
			, m_session_name("")
			, m_start_time(0)
			, m_end_time(0)
			, m_connection("")
			, m_session_info()
			, m_session_config()
			, m_session_negotiate()
			, m_groups()
			, m_group_policy("")
			, m_ice_lite("a=ice-lite")
			, m_msid_semantic("")
			, m_msids()
			, m_media_descs()
		{}
	  virtual	~crtc_sdp(){}


	public:
		int32 parse(const std::string sdp_str);


		int32 encode(std::ostringstream& os);
	public:
		std::vector<cmedia_desc*> find_media_descs(const std::string& type);
	public:
		bool is_unified() const;
		// TODO: FIXME: will be fixed when use single pc.
		int32 update_msid(std::string id);
	public:
		void set_ice_ufrag(const std::string& ufrag);
		void set_ice_pwd(const std::string& pwd);
		void set_dtls_role(const std::string& dtls_role);
		void set_fingerprint_algo(const std::string& algo);
		void set_fingerprint(const std::string& fingerprint);
		void add_candidate(const std::string& protocol, const std::string& ip, const int& port, const std::string& type);

		std::string get_ice_ufrag() const;
		std::string get_ice_pwd() const;
		std::string get_dtls_role() const;
	private:
	private:
		int32 _parse_line(const std::string& line);
		int32 _parse_origin(const std::string& content);
		int32 _parse_version(const std::string& content);
		int32 _parse_session_name(const std::string& content);
		int32 _parse_timing(const std::string& content);
		int32 _parse_attribute(const std::string& content);
		int32 _parse_gb28181_ssrc(const std::string& content);
		int32 _parse_media_description(const std::string& content);
		int32 _parse_attr_group(const std::string& content);
	private:
		bool							m_in_media_session;
	public:
		// version
		std::string						m_version;

		// origin
		std::string						m_username;
		std::string						m_session_id;
		std::string						m_session_version;
		std::string						m_nettype;
		std::string						m_addrtype	;
		std::string						m_unicast_address;

		// session_name
		std::string						m_session_name;

		// timing
		int64							m_start_time;
		int64							m_end_time;

		// Connection data, see https://www.ietf.org/rfc/rfc4566.html#section-5.7
		std::string						m_connection;

		csession_info					m_session_info;
		csession_config					m_session_config;
		csession_config					m_session_negotiate;

		std::vector<std::string>		m_groups;
		std::string						m_group_policy;

		std::string						m_ice_lite;
		std::string						m_msid_semantic;
		std::vector<std::string>		m_msids;

		// m-line, media sessions
		std::vector<cmedia_desc>		m_media_descs;
	};

 

}
#endif // _C_RTC_SDP_H_