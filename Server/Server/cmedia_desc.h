/***********************************************************************************************
created: 		2022-12-21

author:			chensong

purpose:		media_desc


************************************************************************************************/

#ifndef _C_MEDIA_DESC_H_
#define _C_MEDIA_DESC_H_
#include "cnet_type.h"
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
namespace chen {


	// STUN info 
	struct ccandidate
	{
		std::string m_protocol;
		std::string m_ip; // host
		int32		m_port;
		std::string m_type; // TCP  or UDP 
	};


	struct csession_config 
	{
		std::string m_dtls_role;
		std::string m_dtls_version;
		csession_config() 
			: m_dtls_role("")
			, m_dtls_version(""){}
	};


	class csession_info 
	{
	public:
		 explicit csession_info ()
			 : m_ice_ufrag("")
			 , m_ice_pwd("")
			 , m_ice_options("")
			 , m_fingerprint_algo("")
			 , m_fingerprint("")
			 , m_setup("")
		 {}
		 virtual ~csession_info(){}

	public:
		int32 parse_attribute(const std::string & attribute, const std::string & value);
		int32 encode(std::ostringstream & os);

		bool operator==(const csession_info & rhs);
		csession_info & operator=(csession_info other);
	public:
	public:

		std::string m_ice_ufrag;
		std::string m_ice_pwd;
		std::string m_ice_options;
		std::string m_fingerprint_algo;
		std::string m_fingerprint;
		std::string m_setup;

	protected: 
	private:

	};

	class cssrc_info
	{
	public:
		explicit cssrc_info()
			: m_ssrc(0)
			, m_cname("")
			, m_msid("")
			, m_msid_tracker("")
			, m_mslabel("")
			, m_label(""){}
		cssrc_info(uint32 ssrc, std::string cname, std::string stream_id, std::string track_id);
		
		virtual ~cssrc_info(){}

	public:
		int32  encode(std::ostringstream & os);

	public:
		// See https://webrtchacks.com/sdp-anatomy/
		uint32_t						m_ssrc;
		// See https://webrtchacks.com/sdp-anatomy/
		// a=ssrc:3570614608 cname:4TOk42mSjXCkVIa6
		std::string						m_cname;
		// See https://webrtchacks.com/sdp-anatomy/
		// a=ssrc:3570614608 msid:lgsCFqt9kN2fVKw5wg3NKqGdATQoltEwOdMS 35429d94-5637-4686-9ecd-7d0622261ce8
		// a=ssrc:3570614608 msid:{msid_} {msid_tracker_}
		std::string						m_msid;
		std::string						m_msid_tracker;
		// See https://webrtchacks.com/sdp-anatomy/
		// a=ssrc:3570614608 mslabel:lgsCFqt9kN2fVKw5wg3NKqGdATQoltEwOdMS
		std::string						m_mslabel;
		// See https://webrtchacks.com/sdp-anatomy/
		// a=ssrc:3570614608 label:35429d94-5637-4686-9ecd-7d0622261ce8
		std::string						m_label;
	protected:
	private:
	};

	class cssrc_group
	{
	public:
		explicit cssrc_group()
			: m_semantic("")
			, m_ssrcs() {}

		cssrc_group(const std::string & usage, const std::vector<uint32_t> & ssrcs);
		virtual ~cssrc_group(){}

	public:
		int32 encode(std::ostringstream& os);

	public:
		// e.g FIX, FEC, SIM.
		std::string						m_semantic;
		// SSRCs of this type. 
		std::vector<uint32_t>			m_ssrcs;

	protected:
	private:
	};
	

	struct ch264_specific_param
	{
		std::string m_profile_level_id;
		std::string m_packetization_mode;
		std::string m_level_asymmerty_allow;
		ch264_specific_param()
			: m_profile_level_id("")
			, m_packetization_mode("")
			, m_level_asymmerty_allow(""){}
	};



	int32 parse_h264_fmtp(const std::string& fmtp, ch264_specific_param& h264_param);

	class cmedia_payload_type
	{
	public:
		explicit cmedia_payload_type(int32 payload_type)
			: m_payload_type(payload_type)
			, m_encoding_name("")
			, m_clock_rate(0)
			, m_encoding_param("")
			, m_rtcp_fb()
			, m_format_specific_param(){}
		virtual ~cmedia_payload_type();
		uint32 encode(std::ostringstream & os);

		public:
		int32					m_payload_type;
		std::string				m_encoding_name;
		int32					m_clock_rate;
		std::string				m_encoding_param;
		std::vector<std::string>m_rtcp_fb;
		std::string				m_format_specific_param;
	};
	class cmedia_desc
	{
	public:
		explicit cmedia_desc(const std::string & type)
			: m_session_info()
			, m_type(type)
			, m_port(0)
			, m_rtcp_mux(false)
			, m_rtcp_rsize(false)
			, m_sendonly(false)
			, m_recvonly(false)
			, m_sendrecv(false)
			, m_inactive(false)
			, m_mid("")
			, m_msid("")
			, m_msid_tracker("")
			, m_protos("")
			, m_payload_types()
			, m_connection("")
			, m_candidates()
			, m_ssrc_groups()
			, m_ssrc_infos()
			, m_extmaps() {}
		virtual ~cmedia_desc(){}

	public:
		int32 parse_line(const std::string & line);


		int32 encode(std::ostringstream & os);
		
		const std::map<int, std::string>& get_extmaps() const { return m_extmaps; }
		int32 update_msid(std::string id);

		bool is_audio() const { return m_type == "audio"; }
		bool is_video() const { return m_type == "video"; }
	public:
		std::vector<cmedia_payload_type> find_media_with_encoding_name(const std::string& encoding_name) const;

		cmedia_payload_type * find_media_with_payload_type(int32 payload_type);

		cssrc_info& fetch_or_create_ssrc_info(uint32_t ssrc);
	public:

	private:
	private:
		int32 _parse_attribute(const std::string& content);
		int32 _parse_attr_rtpmap(const std::string& value);
		int32 _parse_attr_rtcp(const std::string& value);
		int32 _parse_attr_rtcp_fb(const std::string& value);
		int32 _parse_attr_fmtp(const std::string& value);
		int32 _parse_attr_mid(const std::string& value);
		int32 _parse_attr_msid(const std::string& value);
		int32 _parse_attr_ssrc(const std::string& value);
		int32 _parse_attr_ssrc_group(const std::string& value);
		int32 _parse_attr_extmap(const std::string& value);

	public:
		csession_info						m_session_info;
		std::string							m_type;
		int32								m_port;
		
		bool								m_rtcp_mux;
		bool								m_rtcp_rsize;

		bool								m_sendonly;
		bool								m_recvonly;
		bool								m_sendrecv;
		bool								m_inactive;

		std::string							m_mid;
		std::string							m_msid;
		std::string							m_msid_tracker;
		std::string							m_protos;
		std::vector<cmedia_payload_type>	m_payload_types;
		std::string							m_connection;

		std::vector<ccandidate>				m_candidates;
		std::vector<cssrc_group>			m_ssrc_groups;
		std::vector<cssrc_info>				m_ssrc_infos;

		std::map<int32, std::string>		m_extmaps;




	private:

	};
}

#endif // _C_MEDIA_DESC_H_