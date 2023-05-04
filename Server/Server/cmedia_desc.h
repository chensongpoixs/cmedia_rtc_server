/***********************************************************************************************
created: 		2022-12-21

author:			chensong

purpose:		media_desc

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


	struct ccodec_bitrate_param
	{
		uint32  m_min_bitrate;
		uint32  m_start_bitrate;
		uint32  m_max_bitrate;
		ccodec_bitrate_param()
			: m_min_bitrate(0)
			, m_start_bitrate(0)
			, m_max_bitrate(0) {}
	};



	int32 parse_h264_fmtp(const std::string& fmtp, ch264_specific_param& h264_param);

	int32 parse_codec_bitrate_fmtp(const std::string& fmtp, ccodec_bitrate_param& bitrate_param);

	class cmedia_payload_type
	{
	public:
		explicit cmedia_payload_type(int32 payload_type)
			: m_payload_type(payload_type)
			, m_encoding_name("")
			, m_clock_rate(0)
			, m_encoding_param("")
			, m_rtcp_fb()
			, m_format_specific_param("")
			, m_codec_bitrate_param("")
			//, m_apt(0)
			, m_rtx(0)
			, m_red(0)
			, m_codec_parameter_map(){}
		/*virtual*/ ~cmedia_payload_type();
		uint32 encode(std::ostringstream & os);

		public:
		int32					m_payload_type;
		std::string				m_encoding_name;
		int32					m_clock_rate;
		std::string				m_encoding_param;
		std::vector<std::string>m_rtcp_fb;
		std::string				m_format_specific_param;
		std::string			    m_codec_bitrate_param;
		//uint32					m_apt;
		uint32					m_rtx;
		uint32					m_red;
		std::map<std::string, std::string> m_codec_parameter_map;
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
			, m_connection("c=IN IP4 0.0.0.0")
			, m_candidates()
			, m_ssrc_groups()
			, m_ssrc_infos()
			, m_extmaps() 
			, m_sctp_port(5000)
			, m_max_message_size(10000){}
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
		std::string							m_type; // audio  or video  or applicaiton datachannel 
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
		// datachannel 
		uint32								m_sctp_port; // datachannel defaulef 5000
		uint64								m_max_message_size;



	private:

	};
}

#endif // _C_MEDIA_DESC_H_