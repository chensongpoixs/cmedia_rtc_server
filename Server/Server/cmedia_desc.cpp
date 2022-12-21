﻿/***********************************************************************************************
created: 		2022-12-21

author:			chensong

purpose:		media_desc


************************************************************************************************/
#include "cmedia_desc.h"
#include "crtc_sdp_define.h"
#include "clog.h"
#include "cmedia_error.h"
#include "crtc_sdp_util.h"
namespace chen {


#define FETCH(is,word, error_code, error_message) \
if (!(is >> word)) {\
	WARNING_EX_LOG(error_message); \
    return error_code; \
}\

#define FETCH_WITH_DELIM(is,word,delim, error_code, error_message) \
if (!getline(is,word,delim)) {\
      WARNING_EX_LOG(error_message); \
    return error_code; \
}\


	/*cmedia_desc::cmedia_desc()
	{
	}
	cmedia_desc::~cmedia_desc()
	{
	}*/
	cmedia_payload_type::~cmedia_payload_type()
	{
	}
	uint32 cmedia_payload_type::encode(std::ostringstream & os)
	{
		//return uint32();
		os << "a=rtpmap:" << m_payload_type << " " << m_encoding_name << "/" << m_clock_rate;

		if (!m_encoding_param.empty())
		{
			os << "/" << m_encoding_param;
		}

		os << kCRLF;

		// 支持feackback协议
		for (std::vector<std::string>::iterator iter = m_rtcp_fb.begin(); iter != m_rtcp_fb.end(); ++iter)
		{
			os << "a=rtcp-fb:" << m_payload_type << " " << *iter << kCRLF;
		}


		if (!m_format_specific_param.empty())
		{
			// TODO: FIXME: Remove the test code bellow.
		  // << ";x-google-max-bitrate=6000;x-google-min-bitrate=5100;x-google-start-bitrate=5000"
			os << "a=fmtp:" << m_payload_type << " " << m_format_specific_param << kCRLF;
		}
		return 0;
	}
	cssrc_info::cssrc_info(uint32 ssrc, std::string cname, std::string stream_id, std::string track_id)
	{
		m_ssrc = ssrc;
		m_cname = cname;
		m_msid = stream_id;
		m_msid_tracker = track_id;
		m_mslabel = m_msid;
		m_label = m_msid_tracker;

	}
	int32 cssrc_info::encode(std::ostringstream & os)
	{

		if (0 != m_ssrc)
		{
			WARNING_EX_LOG("invalid ssrc");
			return EMediaRtcSdpInvalidSsrc;
		}
		// See AnnexF at page 101 of https://openstd.samr.gov.cn/bzgk/gb/newGbInfo?hcno=469659DC56B9B8187671FF08748CEC89
   // Encode the bellow format:
   //      a=ssrc:0100008888 cname:0100008888
   //      a=ssrc:0100008888 label:gb28181
   // As GB28181 format:
   //      y=0100008888
		if (m_label == "gb28181") 
		{
			os << "y=" << (m_cname.empty() ? rtc_sdp_util::fmt("%u", m_ssrc) : m_cname) << kCRLF;
			return 0;
		}
		os << "a=ssrc:" << m_ssrc << " cname:" << m_cname << kCRLF;
		if (!m_msid.empty()) 
		{
			os << "a=ssrc:" << m_ssrc << " msid:" << m_msid;
			if (!m_msid_tracker.empty()) 
			{
				os << " " << m_msid_tracker;
			}
			os << kCRLF;
		}
		if (!m_mslabel.empty())
		{
			os << "a=ssrc:" << m_ssrc << " mslabel:" << m_mslabel << kCRLF;
		}
		if (!m_label.empty()) 
		{
			os << "a=ssrc:" << m_ssrc << " label:" << m_label << kCRLF;
		}
		return 0;
	}
	cssrc_group::cssrc_group(const std::string & semantic, const std::vector<uint32_t>& ssrcs)
	{
		m_semantic = semantic;
		m_ssrcs = ssrcs;
	}
	int32 cssrc_group::encode(std::ostringstream & os)
	{
		if (m_semantic.empty())
		{
			WARNING_EX_LOG("invalid semantics !!!");
			return EMediaRtcSdpInvalidSsrc;
		}
		if (0 == m_ssrcs.size())
		{
			WARNING_EX_LOG("invalid ssrcs !!!");
			return EMediaRtcSdpInvalidSemantics;
		}

		os << "a=ssrc_group:" << m_semantic;
		for (size_t i = 0; i < m_ssrcs.size(); ++i)
		{
			os << " " << m_ssrcs[i];
		}

		return 0;
	}
	int32 parse_h264_fmtp(const std::string & fmtp, ch264_specific_param & h264_param)
	{
		std::vector<std::string> vec =  rtc_sdp_util:: string_split(fmtp, ";");
		for (size_t i = 0; i < vec.size(); ++i) 
		{
			std::vector<std::string> kv = rtc_sdp_util::string_split(vec[i], "=");
			if (kv.size() != 2) continue;

			if (kv[0] == "profile-level-id") 
			{
				h264_param.m_profile_level_id = kv[1];
			}
			else if (kv[0] == "packetization-mode") 
			{
				// 6.3.  Non-Interleaved Mode
				// This mode is in use when the value of the OPTIONAL packetization-mode
				// media type parameter is equal to 1.  This mode SHOULD be supported.
				// It is primarily intended for low-delay applications.  Only single NAL
				// unit packets, STAP-As, and FU-As MAY be used in this mode.  STAP-Bs,
				// MTAPs, and FU-Bs MUST NOT be used.  The transmission order of NAL
				// units MUST comply with the NAL unit decoding order.
				// @see https://tools.ietf.org/html/rfc6184#section-6.3
				h264_param.m_packetization_mode = kv[1];
			}
			else if (kv[0] == "level-asymmetry-allowed") 
			{
				h264_param.m_level_asymmerty_allow = kv[1];
			}
		}

		if (h264_param.m_profile_level_id.empty()) 
		{
			WARNING_EX_LOG("no h264 param: profile-level-id");
			return EMediaRtcSdpInvalidH264ParamProfileLevelId;
			//return srs_error_new(ERROR_RTC_SDP_DECODE, "no h264 param: profile-level-id");
		}
		if (h264_param.m_packetization_mode.empty()) 
		{
			WARNING_EX_LOG("no h264 param: packetization-mode");
			return EMediaRtcSdpInvalidH264ParamPacketizationMode;
			//return srs_error_new(ERROR_RTC_SDP_DECODE, "no h264 param: packetization-mode");
		}
		if (h264_param.m_level_asymmerty_allow.empty()) 
		{
			WARNING_EX_LOG("no h264 param: level-asymmetry-allowed");
			return EMediaRtcSdpInvalidH264ParamLevelAsymmetryAllowed;

			//return srs_error_new(ERROR_RTC_SDP_DECODE, "no h264 param: level-asymmetry-allowed");
		}

		return 0;
	}
	int32 csession_info::parse_attribute(const std::string & attribute, const std::string & value)
	{
		if (attribute == "ice-ufrag") 
		{
			m_ice_ufrag = value;
		}
		else if (attribute == "ice-pwd") 
		{
			m_ice_pwd = value;
		}
		else if (attribute == "ice-options") 
		{
			m_ice_options = value;
		}
		else if (attribute == "fingerprint") 
		{
			std::istringstream is(value);
			FETCH(is, m_fingerprint_algo, EMediaRtcSdpInvalidSessionAttributeFingerprintAlgo, "EMediaRtcSdpInvalidSessionAttributeFingerprintAlgo");
			FETCH(is, m_fingerprint, EMediaRtcSdpInvalidSessionAttributeFingerprint, "EMediaRtcSdpInvalidSessionAttributeFingerprint");
		}
		else if (attribute == "setup") 
		{
			// @see: https://tools.ietf.org/html/rfc4145#section-4
			m_setup = value;
		}
		else {
			NORMAL_EX_LOG("ignore attribute=%s, value=%s", attribute.c_str(), value.c_str());
		}
		return 0;
	}
	int32 csession_info::encode(std::ostringstream & os)
	{
		if (!m_ice_ufrag.empty())
		{
			os << "a=ice-ufrag:" << m_ice_ufrag << kCRLF;
		}

		if (!m_ice_pwd.empty())
		{
			os << "a=ice-pwd:" << m_ice_pwd << kCRLF;
		}

		// For ICE-lite, we never set the trickle.
		if (!m_ice_options.empty())
		{
			os << "a=ice-options:" << m_ice_options << kCRLF;
		}

		if (!m_fingerprint_algo.empty() && !m_fingerprint.empty()) 
		{
			os << "a=fingerprint:" << m_fingerprint_algo << " " << m_fingerprint << kCRLF;
		}

		if (!m_setup.empty()) 
		{
			os << "a=setup:" << m_setup << kCRLF;
		}
		return 0;
	}
	bool csession_info::operator==(const csession_info & rhs)
	{
		return m_ice_ufrag  == rhs.m_ice_ufrag  &&
			m_ice_pwd  == rhs.m_ice_pwd  &&
			m_ice_options  == rhs.m_ice_options  &&
			m_fingerprint_algo  == rhs.m_fingerprint_algo  &&
			m_fingerprint  == rhs.m_fingerprint  &&
			m_setup  == rhs.m_setup ;
	}
	csession_info & csession_info::operator=(csession_info other)
	{
		// TODO: 在此处插入 return 语句

		std::swap(m_ice_ufrag , other.m_ice_ufrag);
		std::swap(m_ice_pwd , other.m_ice_pwd);
		std::swap(m_ice_options , other.m_ice_options);
		std::swap(m_fingerprint_algo , other.m_fingerprint_algo);
		std::swap(m_fingerprint , other.m_fingerprint);
		std::swap(m_setup , other.m_setup);
		return *this;
	}
	int32 cmedia_desc::parse_line(const std::string & line)
	{
		std::string content = line.substr(2);

		switch (line[0])
		{
		case 'a':
		{
			return _parse_attribute(content);
			break;
		}
		case 'c': 
		{
			// TODO: process c-line
			break;
		}
		default:
			NORMAL_EX_LOG("ignore media line=%s", line.c_str());
			break;
		}


		return 0;
	}
	int32 cmedia_desc::encode(std::ostringstream & os)
	{
		int32 err = 0;
		os << "m=" << m_type << " " << m_port << " " << m_protos;
		for (std::vector<cmedia_payload_type>::iterator iter = m_payload_types.begin(); iter != m_payload_types.end(); ++iter) 
		{
			os << " " << iter->m_payload_type;
		}

		os << kCRLF;

		// TODO:nettype and address type
		if (!m_connection.empty()) 
		{
			os << m_connection << kCRLF;
		}

		if ((err = m_session_info.encode(os)) != 0) 
		{
			WARNING_EX_LOG("encode session info failed");
			return EMediaRtcSdpEncodeSessionInfoFailed;
			//return srs_error_wrap(err, "encode session info failed");
		}

		if (!m_mid.empty()) 
		{
			os << "a=mid:" << m_mid << kCRLF;
		}
		if (!m_msid.empty()) 
		{
			os << "a=msid:" << m_msid;

			if (!m_msid_tracker.empty()) 
			{
				os << " " << m_msid_tracker;
			}

			os << kCRLF;
		}

		for (std::map<int32, std::string>::iterator it = m_extmaps.begin(); it != m_extmaps.end(); ++it) 
		{
			os << "a=extmap:" << it->first << " " << it->second << kCRLF;
		}
		if (m_sendonly) 
		{
			os << "a=sendonly" << kCRLF;
		}
		if (m_recvonly) 
		{
			os << "a=recvonly" << kCRLF;
		}
		if (m_sendrecv)
		{
			os << "a=sendrecv" << kCRLF;
		}
		if (m_inactive) 
		{
			os << "a=inactive" << kCRLF;
		}

		if (m_rtcp_mux)
		{
			os << "a=rtcp-mux" << kCRLF;
		}

		if (m_rtcp_rsize)
		{
			os << "a=rtcp-rsize" << kCRLF;
		}

		for (std::vector<cmedia_payload_type>::iterator iter = m_payload_types.begin(); iter != m_payload_types.end(); ++iter) 
		{
			if ((err = iter->encode(os)) != 0) 
			{
				WARNING_EX_LOG("encode media payload failed");
				return EMediaRtcSdpEncodeMediaPayloadFailed;
				//return srs_error_wrap(err, "encode media payload failed");
			}
		}

		for (std::vector<cssrc_info>::iterator iter = m_ssrc_infos.begin(); iter != m_ssrc_infos.end(); ++iter)
		{
			cssrc_info& ssrc_info = *iter;

			if ((err = ssrc_info.encode(os)) != 0) 
			{
				WARNING_EX_LOG("encode ssrc failed");
				return EMediaRtcSdpEncodeSsrcFailed;
				//return srs_error_wrap(err, "encode ssrc failed");
			}
		}

		int foundation = 0;
		int component_id = 1; /* RTP */
		for (std::vector<ccandidate>::iterator iter = m_candidates.begin(); iter != m_candidates.end(); ++iter)
		{
			// @see: https://tools.ietf.org/html/draft-ietf-ice-rfc5245bis-00#section-4.2
			uint32_t priority = (1 << 24)*(126) + (1 << 8)*(65535) + (1)*(256 - component_id);

			// See ICE TCP at https://www.rfc-editor.org/rfc/rfc6544
			if (iter->m_protocol == "tcp") {
				os << "a=candidate:" << foundation++ << " "
					<< component_id << " tcp " << priority << " "
					<< iter->m_ip << " " << iter->m_port
					<< " typ " << iter->m_type
					<< " tcptype passive"
					<< kCRLF;
				continue;
			}

			// @see: https://tools.ietf.org/id/draft-ietf-mmusic-ice-sip-sdp-14.html#rfc.section.5.1
			os << "a=candidate:" << foundation++ << " "
				<< component_id << " udp " << priority << " "
				<< iter->m_ip << " " << iter->m_port
				<< " typ " << iter->m_type
				<< " generation 0" << kCRLF;

			NORMAL_EX_LOG("local SDP candidate line=%s", os.str().c_str());
		}

		return 0;
		//return int32();
	}
	int32 cmedia_desc::update_msid(std::string id)
	{
		for (std::vector<cssrc_info>::iterator it = m_ssrc_infos.begin(); it != m_ssrc_infos.end(); ++it) 
		{
			cssrc_info& info = *it;

			info.m_msid = id;
			info.m_mslabel = id;
		}

		return 0;
		//return int32();
	}
	cmedia_payload_type * cmedia_desc::find_media_with_payload_type(int32 payload_type)
	{
		for (size_t i = 0; i < m_payload_types.size(); ++i) 
		{
			if (m_payload_types[i].m_payload_type == payload_type)
			{
				return &m_payload_types[i];
			}
		}

		return NULL;
		return nullptr;
	}
	cssrc_info & cmedia_desc::fetch_or_create_ssrc_info(uint32_t ssrc)
	{
		// TODO: 在此处插入 return 语句
		for (size_t i = 0; i < m_ssrc_infos.size(); ++i) 
		{
			if (m_ssrc_infos[i].m_ssrc == ssrc) 
			{
				return m_ssrc_infos[i];
			}
		}

		cssrc_info ssrc_info;
		ssrc_info.m_ssrc = ssrc;
		m_ssrc_infos.push_back(ssrc_info);

		return m_ssrc_infos.back();
	}
	int32 cmedia_desc::_parse_attribute(const std::string & content)
	{
		std::string attribute = "";
		std::string value = "";
		size_t pos = content.find_first_of(":");

		if (pos != std::string::npos)
		{
			attribute = content.substr(0, pos);
			value = content.substr(pos + 1);
		}
		else 
		{
			attribute = content;
		}

		if (attribute == "extmap") 
		{
			return _parse_attr_extmap(value);
		}
		else if (attribute == "rtpmap") 
		{
			return _parse_attr_rtpmap(value);
		}
		else if (attribute == "rtcp") 
		{
			return _parse_attr_rtcp(value);
		}
		else if (attribute == "rtcp-fb")
		{
			return _parse_attr_rtcp_fb(value);
		}
		else if (attribute == "fmtp") 
		{
			return _parse_attr_fmtp(value);
		}
		else if (attribute == "mid") 
		{
			return _parse_attr_mid(value);
		}
		else if (attribute == "msid") 
		{
			return _parse_attr_msid(value);
		}
		else if (attribute == "ssrc") 
		{
			return _parse_attr_ssrc(value);
		}
		else if (attribute == "ssrc-group") 
		{
			return _parse_attr_ssrc_group(value);
		}
		else if (attribute == "rtcp-mux") 
		{
			m_rtcp_mux = true;
		}
		else if (attribute == "rtcp-rsize") 
		{
			m_rtcp_rsize = true;
		}
		else if (attribute == "recvonly") 
		{
			m_recvonly = true;
		}
		else if (attribute == "sendonly") 
		{
			m_sendonly = true;
		}
		else if (attribute == "sendrecv") 
		{
			m_sendrecv = true;
		}
		else if (attribute == "inactive") 
		{
			m_inactive = true;
		}
		else {
			return m_session_info.parse_attribute(attribute, value);
		}

		return 0;
		//return int32();
	}
	int32 cmedia_desc::_parse_attr_rtpmap(const std::string & value)
	{
		// @see: https://tools.ietf.org/html/rfc4566#page-25
   // a=rtpmap:<payload type> <encoding name>/<clock rate> [/<encoding parameters>]

		std::istringstream is(value);

		int payload_type = 0;
		FETCH(is, payload_type, EMediaRtcSdpInvalidAttrRtpmapPayloadType, "EMediaRtcSdpInvalidAttrRtpmapPayloadType");

		cmedia_payload_type* payload = find_media_with_payload_type(payload_type);
		if (payload == NULL)
		{
			WARNING_EX_LOG("can not find payload %d when pase rtpmap", payload_type);
			return EMediaRtcSdpInvalidAttrRtpmapPayloadType;
			//return srs_error_new(ERROR_RTC_SDP_DECODE, "can not find payload %d when pase rtpmap", payload_type);
		}

		std::string word;
		FETCH(is, word, EMediaRtcSdpInvalidAttrRtpmap, "EMediaRtcSdpInvalidAttrPraseRtpmap");

		std::vector<std::string> vec = rtc_sdp_util::  split_str(word, "/");
		if (vec.size() < 2) 
		{
			WARNING_EX_LOG("invalid rtpmap line=%s", value.c_str());
			return EMediaRtcSdpInvalidAttrRtpmap;
			//return srs_error_new(ERROR_RTC_SDP_DECODE, "invalid rtpmap line=%s", value.c_str());
		}

		payload->m_encoding_name = vec[0];
		payload->m_clock_rate = ::atoi(vec[1].c_str());

		if (vec.size() == 3) 
		{
			payload->m_encoding_param = vec[2];
		}

		return 0;
		//return int32();
	}
	int32 cmedia_desc::_parse_attr_rtcp(const std::string & value)
	{
		return 0;
	}
	int32 cmedia_desc::_parse_attr_rtcp_fb(const std::string & value)
	{
		// @see: https://tools.ietf.org/html/rfc5104#section-7.1

		std::istringstream is(value);

		int payload_type = 0;
		FETCH(is, payload_type, EMediaRtcSdpInvalidAttrRtcpfbPayloadType, "EMediaRtcSdpInvalidAttrRtcpfbPrasePayloadType");

		cmedia_payload_type* payload = find_media_with_payload_type(payload_type);
		if (payload == NULL) 
		{
			WARNING_EX_LOG("can not find payload %d when pase rtcp-fb", payload_type);
			return EMediaRtcSdpInvalidAttrRtcpfbPayloadType;
		//	return srs_error_new(ERROR_RTC_SDP_DECODE, "can not find payload %d when pase rtcp-fb", payload_type);
		}

		std::string rtcp_fb = is.str().substr(is.tellg());
		rtc_sdp_util:: skip_first_spaces(rtcp_fb);

		payload->m_rtcp_fb.push_back(rtcp_fb);

		return 0;
		//return int32();
	}
	int32 cmedia_desc::_parse_attr_fmtp(const std::string & value)
	{
		// @see: https://tools.ietf.org/html/rfc4566#page-30
   // a=fmtp:<format> <format specific parameters>

		std::istringstream is(value);

		int payload_type = 0;
		FETCH(is, payload_type, EMediaRtcSdpInvalidAttrFmtpPayloadType, "EMediaRtcSdpInvalidAttrFmtpParsePayloadType");

		cmedia_payload_type* payload = find_media_with_payload_type(payload_type);
		if (payload == NULL) 
		{
			WARNING_EX_LOG("can not find payload %d when pase fmtp", payload_type);
			return EMediaRtcSdpInvalidAttrFmtpPayloadType;
		//	return srs_error_new(ERROR_RTC_SDP_DECODE, "can not find payload %d when pase fmtp", payload_type);
		}

		std::string word;
		FETCH(is, word, EMediaRtcSdpInvalidAttrFmtpSpecificParam, "EMediaRtcSdpInvalidAttrFmtpParseSpecificParam");

		payload->m_format_specific_param = word;

		return 0;
		//return int32();
	}
	int32 cmedia_desc::_parse_attr_mid(const std::string & value)
	{
		// @see: https://tools.ietf.org/html/rfc3388#section-3
		std::istringstream is(value);
		// mid_ means m-line id
		FETCH(is, m_mid, EMediaRtcSdpInvalidAttrParseMid, "EMediaRtcSdpInvalidAttrParseMid");
		NORMAL_EX_LOG("mid=%s", m_mid.c_str());
		return 0;
		//return int32();
	}
	int32 cmedia_desc::_parse_attr_msid(const std::string & value)
	{
		// @see: https://tools.ietf.org/id/draft-ietf-mmusic-msid-08.html#rfc.section.2
		// TODO: msid and msid_tracker
		 
		std::istringstream is(value);
		// msid_ means media stream id
		FETCH(is, m_msid, EMediaRtcSdpInvalidAttrParseMsid, "EMediaRtcSdpInvalidAttrParseMsid");
		is >> m_msid_tracker;
		return 0;
	}
	int32 cmedia_desc::_parse_attr_ssrc(const std::string & value)
	{
		// @see: https://tools.ietf.org/html/rfc5576#section-4.1

		std::istringstream is(value);

		uint32_t ssrc = 0;
		FETCH(is, ssrc, EMediaRtcSdpInvalidAttrParseSsrc, "EMediaRtcSdpInvalidAttrParseSsrc");

		std::string ssrc_attr = "";
		FETCH_WITH_DELIM(is, ssrc_attr, ':', EMediaRtcSdpInvalidAttrParseSsrcAttr, "EMediaRtcSdpInvalidAttrParseSsrcAttr");
		rtc_sdp_util:: skip_first_spaces(ssrc_attr);

		std::string ssrc_value = is.str().substr(is.tellg());
		rtc_sdp_util:: skip_first_spaces(ssrc_value);

		cssrc_info& ssrc_info = fetch_or_create_ssrc_info(ssrc);

		if (ssrc_attr == "cname") 
		{
			// @see: https://tools.ietf.org/html/rfc5576#section-6.1
			ssrc_info.m_cname = ssrc_value;
			ssrc_info.m_ssrc = ssrc;
		}
		else if (ssrc_attr == "msid") 
		{
			// @see: https://tools.ietf.org/html/draft-alvestrand-mmusic-msid-00#section-2
			std::vector<std::string> vec = rtc_sdp_util:: split_str(ssrc_value, " ");
			if (vec.empty()) 
			{
				WARNING_EX_LOG("invalid ssrc line=%s", value.c_str());
				return EMediaRtcSdpInvalidSsrc;
				//return srs_error_new(ERROR_RTC_SDP_DECODE, "invalid ssrc line=%s", value.c_str());
			}

			ssrc_info.m_msid = vec[0];
			if (vec.size() > 1) 
			{
				ssrc_info.m_msid_tracker = vec[1];
			}
		}
		else if (ssrc_attr == "mslabel") 
		{
			ssrc_info.m_mslabel = ssrc_value;
		}
		else if (ssrc_attr == "label") 
		{
			ssrc_info.m_label = ssrc_value;
		}

		return 0;
		//return int32();
	}
	int32 cmedia_desc::_parse_attr_ssrc_group(const std::string & value)
	{
		// @see: https://tools.ietf.org/html/rfc5576#section-4.2
   // a=ssrc-group:<semantics> <ssrc-id> ...

		std::istringstream is(value);

		std::string semantics;
		FETCH(is, semantics, EMediaRtcSdpInvalidAttrParseSsrcGroupSemantics, "EMediaRtcSdpInvalidAttrParseSsrcGroupSemantics");

		std::string ssrc_ids = is.str().substr(is.tellg());
		rtc_sdp_util:: skip_first_spaces(ssrc_ids);

		std::vector<std::string> vec = rtc_sdp_util:: split_str(ssrc_ids, " ");
		if (vec.size() == 0) 
		{
			WARNING_EX_LOG("invalid ssrc-group line=%s", value.c_str());
			return EMediaRtcSdpInvalidAttrParseSsrcGroupSsrcId;
			//return srs_error_new(ERROR_RTC_SDP_DECODE, "invalid ssrc-group line=%s", value.c_str());
		}

		std::vector<uint32_t> ssrcs;
		for (size_t i = 0; i < vec.size(); ++i) 
		{
			std::istringstream in_stream(vec[i]);
			uint32_t ssrc = 0;
			in_stream >> ssrc;
			ssrcs.push_back(ssrc);
		}
		m_ssrc_groups.push_back(cssrc_group(semantics, ssrcs));

		return 0;
		//return int32();
	}
	int32 cmedia_desc::_parse_attr_extmap(const std::string & value)
	{
		std::istringstream is(value);
		int32 id = 0;
		FETCH(is, id, EMediaRtcSdpInvalidAttrParseExtmapId, "EMediaRtcSdpInvalidAttrParseExtmapId");
		if (m_extmaps.end() != m_extmaps.find(id)) 
		{
			WARNING_EX_LOG("duplicate ext id: %d", id);
			return EMediaRtcSdpInvalidAttrParseExtmapId;
			//return srs_error_new(ERROR_RTC_SDP_DECODE, "duplicate ext id: %d", id);
		}
		std::string ext;
		FETCH(is, ext, EMediaRtcSdpInvalidAttrParseExtmapUri, "EMediaRtcSdpInvalidAttrParseExtmapUri");
		m_extmaps[id] = ext;
		return 0;
		//return int32();
	}
}