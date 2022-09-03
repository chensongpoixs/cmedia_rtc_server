/***********************************************************************************************
created: 		2022-09-03

author:			chensong

purpose:		crtc_sdp


************************************************************************************************/
#include "crtc_sdp.h"
#include "crtc_sdp_util.h"
#include "crtc_sdp_define.h"
#include <absl/utility/utility.h>
#include <absl/types/optional.h>

#if defined(_MSC_VER)
#include <ws2spi.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <in6addr.h>
#endif // 
//#include <win>
namespace chen {
	//////////////////////////////////////////////////////////////////

	// Line type
	// RFC 4566
	// An SDP session description consists of a number of lines of text of
	// the form:
	// <type>=<value>
	// where <type> MUST be exactly one case-significant character.
	static const int kLinePrefixLength = 2;  // Length of <type>=
	static const char kLineTypeVersion = 'v';
	static const char kLineTypeOrigin = 'o';
	static const char kLineTypeSessionName = 's';
	static const char kLineTypeSessionInfo = 'i';
	static const char kLineTypeSessionUri = 'u';
	static const char kLineTypeSessionEmail = 'e';
	static const char kLineTypeSessionPhone = 'p';
	static const char kLineTypeSessionBandwidth = 'b';
	static const char kLineTypeTiming = 't';
	static const char kLineTypeRepeatTimes = 'r';
	static const char kLineTypeTimeZone = 'z';
	static const char kLineTypeEncryptionKey = 'k';
	static const char kLineTypeMedia = 'm';
	static const char kLineTypeConnection = 'c';
	static const char kLineTypeAttributes = 'a';

	// Attributes
	static const char kAttributeGroup[] = "group";
	static const char kAttributeMid[] = "mid";
	static const char kAttributeMsid[] = "msid";
	static const char kAttributeBundleOnly[] = "bundle-only";
	static const char kAttributeRtcpMux[] = "rtcp-mux";
	static const char kAttributeRtcpReducedSize[] = "rtcp-rsize";
	static const char kAttributeSsrc[] = "ssrc";
	static const char kSsrcAttributeCname[] = "cname";
	static const char kAttributeExtmapAllowMixed[] = "extmap-allow-mixed";
	static const char kAttributeExtmap[] = "extmap";
	// draft-alvestrand-mmusic-msid-01
	// a=msid-semantic: WMS
	// This is a legacy field supported only for Plan B semantics.
	static const char kAttributeMsidSemantics[] = "msid-semantic";
	static const char kMediaStreamSemantic[] = "WMS";
	static const char kSsrcAttributeMsid[] = "msid";
	static const char kDefaultMsid[] = "default";
	static const char kNoStreamMsid[] = "-";
	static const char kSsrcAttributeMslabel[] = "mslabel";
	static const char kSSrcAttributeLabel[] = "label";
	static const char kAttributeSsrcGroup[] = "ssrc-group";
	static const char kAttributeCrypto[] = "crypto";
	static const char kAttributeCandidate[] = "candidate";
	static const char kAttributeCandidateTyp[] = "typ";
	static const char kAttributeCandidateRaddr[] = "raddr";
	static const char kAttributeCandidateRport[] = "rport";
	static const char kAttributeCandidateUfrag[] = "ufrag";
	static const char kAttributeCandidatePwd[] = "pwd";
	static const char kAttributeCandidateGeneration[] = "generation";
	static const char kAttributeCandidateNetworkId[] = "network-id";
	static const char kAttributeCandidateNetworkCost[] = "network-cost";
	static const char kAttributeFingerprint[] = "fingerprint";
	static const char kAttributeSetup[] = "setup";
	static const char kAttributeFmtp[] = "fmtp";
	static const char kAttributeRtpmap[] = "rtpmap";
	static const char kAttributeSctpmap[] = "sctpmap";
	static const char kAttributeRtcp[] = "rtcp";
	static const char kAttributeIceUfrag[] = "ice-ufrag";
	static const char kAttributeIcePwd[] = "ice-pwd";
	static const char kAttributeIceLite[] = "ice-lite";
	static const char kAttributeIceOption[] = "ice-options";
	static const char kAttributeSendOnly[] = "sendonly";
	static const char kAttributeRecvOnly[] = "recvonly";
	static const char kAttributeRtcpFb[] = "rtcp-fb";
	static const char kAttributeSendRecv[] = "sendrecv";
	static const char kAttributeInactive[] = "inactive";
	// draft-ietf-mmusic-sctp-sdp-26
	// a=sctp-port, a=max-message-size
	static const char kAttributeSctpPort[] = "sctp-port";
	static const char kAttributeMaxMessageSize[] = "max-message-size";
	// draft-ietf-mmusic-sdp-simulcast-13
	// a=simulcast
	static const char kAttributeSimulcast[] = "simulcast";
	// draft-ietf-mmusic-rid-15
	// a=rid
	static const char kAttributeRid[] = "rid";

	// Experimental flags
	static const char kAttributeXGoogleFlag[] = "x-google-flag";
	static const char kValueConference[] = "conference";

	// Candidate
	static const char kCandidateHost[] = "host";
	static const char kCandidateSrflx[] = "srflx";
	static const char kCandidatePrflx[] = "prflx";
	static const char kCandidateRelay[] = "relay";
	static const char kTcpCandidateType[] = "tcptype";

	// rtc::StringBuilder doesn't have a << overload for chars, while rtc::split and
	// rtc::tokenize_first both take a char delimiter. To handle both cases these
	// constants come in pairs of a chars and length-one strings.
	static const char kSdpDelimiterEqual[] = "=";
	static const char kSdpDelimiterEqualChar = '=';
	static const char kSdpDelimiterSpace[] = " ";
	static const char kSdpDelimiterSpaceChar = ' ';
	static const char kSdpDelimiterColon[] = ":";
	static const char kSdpDelimiterColonChar = ':';
	static const char kSdpDelimiterSemicolon[] = ";";
	static const char kSdpDelimiterSemicolonChar = ';';
	static const char kSdpDelimiterSlashChar = '/';
	static const char kNewLine[] = "\n";
	static const char kNewLineChar = '\n';
	static const char kReturnChar = '\r';
	static const char kLineBreak[] = "\r\n";

	// TODO(deadbeef): Generate the Session and Time description
	// instead of hardcoding.
	static const char kSessionVersion[] = "v=0";
	// RFC 4566
	static const char kSessionOriginUsername[] = "-";
	static const char kSessionOriginSessionId[] = "0";
	static const char kSessionOriginSessionVersion[] = "0";
	static const char kSessionOriginNettype[] = "IN";
	static const char kSessionOriginAddrtype[] = "IP4";
	static const char kSessionOriginAddress[] = "127.0.0.1";
	static const char kSessionName[] = "s=-";
	static const char kTimeDescription[] = "t=0 0";
	static const char kAttrGroup[] = "a=group:BUNDLE";
	static const char kConnectionNettype[] = "IN";
	static const char kConnectionIpv4Addrtype[] = "IP4";
	static const char kConnectionIpv6Addrtype[] = "IP6";
	static const char kMediaTypeVideo[] = "video";
	static const char kMediaTypeAudio[] = "audio";
	static const char kMediaTypeData[] = "application";
	static const char kMediaPortRejected[] = "0";
	// draft-ietf-mmusic-trickle-ice-01
	// When no candidates have been gathered, set the connection
	// address to IP6 ::.
	// TODO(perkj): FF can not parse IP6 ::. See http://crbug/430333
	// Use IPV4 per default.
	static const char kDummyAddress[] = "0.0.0.0";
	static const char kDummyPort[] = "9";
	// RFC 3556
	static const char kApplicationSpecificMaximum[] = "AS";

	static const char kDefaultSctpmapProtocol[] = "webrtc-datachannel";

	// This is a non-standardized media transport settings.
	// This setting is going to be set in the offer. There may be one or more
	// a=x-mt: settings, and they are in the priority order (the most preferred on
	// top). x-mt setting format depends on the media transport, and is generated by
	// |MediaTransportInterface::GetTransportParametersOffer|.
	static const char kMediaTransportSettingLine[] = "x-mt";
/////////////////////////////////////////////////////////////////////////////////////////////////






	static bool IsLineType(const std::string& message,
		const char type,
		size_t line_start) {
		if (message.size() < line_start + kLinePrefixLength) {
			return false;
		}
		const char* cmessage = message.c_str();
		return (cmessage[line_start] == type &&
			cmessage[line_start + 1] == kSdpDelimiterEqualChar);
	}
	static bool IsLineType(const std::string& line, const char type) {
		return IsLineType(line, type, 0);
	}
	static bool GetLine(const std::string& message,
		size_t* pos,
		std::string* line) {
		size_t line_begin = *pos;
		size_t line_end = message.find(kNewLine, line_begin);
		if (line_end == std::string::npos) {
			return false;
		}
		// Update the new start position
		*pos = line_end + 1;
		if (line_end > 0 && (message.at(line_end - 1) == kReturnChar)) {
			--line_end;
		}
		*line = message.substr(line_begin, (line_end - line_begin));
		const char* cline = line->c_str();
		// RFC 4566
		// An SDP session description consists of a number of lines of text of
		// the form:
		// <type>=<value>
		// where <type> MUST be exactly one case-significant character and
		// <value> is structured text whose format depends on <type>.
		// Whitespace MUST NOT be used on either side of the "=" sign.
		//
		// However, an exception to the whitespace rule is made for "s=", since
		// RFC4566 also says:
		//
		//   If a session has no meaningful name, the value "s= " SHOULD be used
		//   (i.e., a single space as the session name).
		if (line->length() < 3 || !islower(cline[0]) ||
			cline[1] != kSdpDelimiterEqualChar ||
			(cline[0] != kLineTypeSessionName &&
				cline[2] == kSdpDelimiterSpaceChar)) {
			*pos = line_begin;
			return false;
		}
		return true;
	}
	static bool GetLineWithType(const std::string& message,
		size_t* pos,
		std::string* line,
		const char type) {
		if (!IsLineType(message, type, *pos)) 
		{
			return false;
		}

		if (!GetLine(message, pos, line))
		{
			return false;
		}

		return true;
	}
	static bool HasAttribute(const std::string& line,
		const std::string& attribute) {
		if (line.compare(kLinePrefixLength, attribute.size(), attribute) == 0) {
			// Make sure that the match is not only a partial match. If length of
			// strings doesn't match, the next character of the line must be ':' or ' '.
			// This function is also used for media descriptions (e.g., "m=audio 9..."),
			// hence the need to also allow space in the end.
			//RTC_CHECK_LE(kLinePrefixLength + attribute.size(), line.size());
			if ((kLinePrefixLength + attribute.size()) == line.size() ||
				line[kLinePrefixLength + attribute.size()] == kSdpDelimiterColonChar ||
				line[kLinePrefixLength + attribute.size()] == kSdpDelimiterSpaceChar) {
				return true;
			}
		}
		return false;
	}



	// Get value only from <attribute>:<value>.
	static bool GetValue(const std::string& message,
		const std::string& attribute,
		std::string* value/*,
		SdpParseError* error*/) 
	{
		std::string leftpart;
		if (!rtc_sdp_util::tokenize_first(message, kSdpDelimiterColonChar, &leftpart, value)) {
			return false;// ParseFailedGetValue(message, attribute, error);
		}
		// The left part should end with the expected attribute.
		if (leftpart.length() < attribute.length() ||
			leftpart.compare(leftpart.length() - attribute.length(),
				attribute.length(), attribute) != 0) 
		{
			return false;// ParseFailedGetValue(message, attribute, error);
		}
		return true;
	}


	template <typename T,
		typename std::enable_if<std::is_arithmetic<T>::value &&
		!std::is_same<T, bool>::value,
		int>::type = 0>
		static bool FromString(const std::string& s, T* t) {
		//RTC_DCHECK(t);
		absl::optional<T> result =  std::atol(s.c_str())/*StringToNumber<T>(s)*/;

		if (result)
			*t = *result;

		return result.has_value();
	}

	template <class T>
	static bool GetValueFromString(const std::string& line,
		const std::string& s,
		T* t ) {
		if (!FromString(s, t)) 
		{
			/*rtc::StringBuilder description;
			description << "Invalid value: " << s << ".";*/
			return false; // ParseFailed(line, description.str(), error);
		}
		return true;
	}


	static bool IsValidPort(int port) {
		return port >= 0 && port <= 65535;
	}

	bool IsValidRtpPayloadType(int payload_type) {
		return payload_type >= 0 && payload_type <= 127;
	}

	static bool GetPayloadTypeFromString(const std::string& line,
		const std::string& s,
		int* payload_type) {
		return GetValueFromString(line, s, payload_type) &&
			IsValidRtpPayloadType(*payload_type);
	}

	crtc_sdp::~crtc_sdp()
	{
	}
	bool crtc_sdp::init(const std::string & sdp)
	{
		
		// 1. Session Description
		if (!_parse_session_description(sdp))
		{
			// error 
			return false;
		}
		return true;
	}
	bool crtc_sdp::_parse_session_description(const std::string & session_sdp_description)
	{
		m_session_description.set_msid_supported(false);
		m_session_description.set_extmap_allow_mixed(false);
		std::string line;
		// RFC 4566
		// v=  (protocol version)
		if (!GetLineWithType(session_sdp_description, &m_current_pos, &line, kLineTypeVersion)) 
		{
			return false;/*ParseFailedExpectLine(message, *pos, kLineTypeVersion, std::string(),
				error);*/
		}
		// RFC 4566
		// o=<username> <sess-id> <sess-version> <nettype> <addrtype>
		// <unicast-address>
		if (!GetLineWithType(session_sdp_description, &m_current_pos, &line, kLineTypeOrigin)) 
		{
			return false; // ParseFailedExpectLine(message, *pos, kLineTypeOrigin, std::string(),
				//error);
		}
		std::vector<std::string> fields;
		rtc_sdp_util::split(line.substr(kLinePrefixLength), kSdpDelimiterSpaceChar, &fields);
		const size_t expected_fields = 6;
		if (fields.size() != expected_fields) 
		{
			return  false;// ParseFailedExpectFieldNum(line, expected_fields, error);
		}
		/**session_id = fields[1];
		*session_version = fields[2];*/


		// RFC 4566
		// s=  (session name)
		if (!GetLineWithType(session_sdp_description, &m_current_pos, &line, kLineTypeSessionName)) 
		{
			return false; // ParseFailedExpectLine(message, *pos, kLineTypeSessionName,
				//std::string(), error);
		}

		// absl::optional lines
		// Those are the optional lines, so shouldn't return false if not present.
		// RFC 4566
		// i=* (session information)
		GetLineWithType(session_sdp_description, &m_current_pos, &line, kLineTypeSessionInfo);

		// RFC 4566
		// u=* (URI of description)
		GetLineWithType(session_sdp_description, &m_current_pos, &line, kLineTypeSessionUri);

		// RFC 4566
		// e=* (email address)
		GetLineWithType(session_sdp_description, &m_current_pos, &line, kLineTypeSessionEmail);

		// RFC 4566
		// p=* (phone number)
		GetLineWithType(session_sdp_description, &m_current_pos, &line, kLineTypeSessionPhone);

		// RFC 4566
		// c=* (connection information -- not required if included in
		//      all media)
		if (GetLineWithType(session_sdp_description, &m_current_pos, &line, kLineTypeConnection)) 
		{
			if (!_parse_connection_data(line)) 
			{
				return false;
			}
		}

		// RFC 4566
		// b=* (zero or more bandwidth information lines)
		while (GetLineWithType(session_sdp_description, &m_current_pos, &line, kLineTypeSessionBandwidth)) 
		{
			// By pass zero or more b lines.
		}

		// RFC 4566
		// One or more time descriptions ("t=" and "r=" lines; see below)
		// t=  (time the session is active)
		// r=* (zero or more repeat times)
		// Ensure there's at least one time description
		if (!GetLineWithType(session_sdp_description, &m_current_pos, &line, kLineTypeTiming)) 
		{
			return false;//ParseFailedExpectLine(message, *pos, kLineTypeTiming, std::string(),
				//error);
		}
		while (GetLineWithType(session_sdp_description, &m_current_pos, &line, kLineTypeRepeatTimes)) 
		{
			// By pass zero or more r lines.
		}
		// Go through the rest of the time descriptions
		while (GetLineWithType(session_sdp_description, &m_current_pos, &line, kLineTypeTiming)) 
		{
			while (GetLineWithType(session_sdp_description, &m_current_pos, &line, kLineTypeRepeatTimes)) 
			{
				// By pass zero or more r lines.
			}
		}


		// RFC 4566
		// z=* (time zone adjustments)
		GetLineWithType(session_sdp_description, &m_current_pos, &line, kLineTypeTimeZone);

		// RFC 4566
		// k=* (encryption key)
		GetLineWithType(session_sdp_description, &m_current_pos, &line, kLineTypeEncryptionKey);


		// RFC 4566
		// a=* (zero or more session attribute lines)
		Transport_Description  session_td;
		std::vector<Rtp_Extension>	   session_extmaps;
		while (GetLineWithType(session_sdp_description, &m_current_pos, &line, kLineTypeAttributes)) 
		{
			if (HasAttribute(line, kAttributeGroup)) {
				if (!_parse_group_attribute(line/*, desc*/)) {
					return false;
				}
			}
			else if (HasAttribute(line, kAttributeIceUfrag)) {
				if (!GetValue(line, kAttributeIceUfrag, (session_td.mutable_ice_ufrag()) )) {
					return false;
				}
			}
			else if (HasAttribute(line, kAttributeIcePwd)) {
				if (!GetValue(line, kAttributeIcePwd,  (session_td.mutable_ice_pwd()) )) {
					return false;
				}
			}
			else if (HasAttribute(line, kAttributeIceLite)) 
			{
				//session_td->ice_mode = cricket::ICEMODE_LITE;
				session_td.set_ice_mode(ICEMODE_LITE);
			}
			else if (HasAttribute(line, kAttributeIceOption)) {
				if (!_parse_ice_options(line, session_td)) {
					return false;
				}
			}
			else if (HasAttribute(line, kAttributeFingerprint)) {
				if (session_td.has_identity_fingerprint()/*session_td->identity_fingerprint.get()*/) 
				{
					return false; // ParseFailed(
					//	line,
						//"Can't have multiple fingerprint attributes at the same level.",
						//error);
				}
				//std::unique_ptr<rtc::SSLFingerprint> fingerprint;
				if (!_parse_fingerprint_attribute(line, session_td.mutable_identity_fingerprint()/*&fingerprint*/))
				{
					return false;
				}
				//session_td->identity_fingerprint = std::move(fingerprint);
			}
			else if (HasAttribute(line, kAttributeSetup)) {
				if (!_parse_dtls_setup(line, session_td   /*&(session_td->connection_role), error*/)) {
					return false;
				}
			}
			else if (HasAttribute(line, kAttributeMsidSemantics)) {
				std::string semantics;
				if (!GetValue(line, kAttributeMsidSemantics, &semantics )) {
					return false;
				}
				m_session_description.set_msid_supported(true);
				/*desc->set_msid_supported(
					CaseInsensitiveFind(semantics, kMediaStreamSemantic));*/
			}
			else if (HasAttribute(line, kAttributeExtmapAllowMixed)) {
			//	desc->set_extmap_allow_mixed(true);
				m_session_description.set_extmap_allow_mixed(true);
				 
				/*Extmap_Allow_Mixed media_level_setting =
					true ?  kSession
					:  kNo;*/
				//for (auto& content : m_session_description.contents()) 
				//{
				//	// Do not set to kNo if the current setting is kMedia.
				//	if (m_session_description.extmap_allow_mixed() || content.mutable_media_description()->extmap_allow_mixed_enum() !=
				//		MediaContentDescription::kMedia) {
				//		content.media_description()->set_extmap_allow_mixed_enum(
				//			media_level_setting);
				//	}
				//}
			}
			else if (HasAttribute(line, kAttributeExtmap)) {
				Rtp_Extension extmap;
				if (!_parse_extmap(line, &extmap)) {
					return false;
				}
				session_extmaps.push_back(extmap);
			}
			else if (HasAttribute(line, kMediaTransportSettingLine)) {
				std::string transport_name;
				std::string transport_setting;
				/*if (!ParseMediaTransportLine(line, &transport_name, &transport_setting,
					error)) {
					return false;
				}*/

				//for (const auto& setting : desc->MediaTransportSettings()) {
				//	if (setting.transport_name == transport_name) {
				//		// Ignore repeated transport names rather than failing to parse so
				//		// that in the future the same transport could have multiple configs.
				//		RTC_LOG(INFO) << "x-mt line with repeated transport, transport_name="
				//			<< transport_name;
				//		return true;
				//	}
				//}
				//desc->AddMediaTransportSetting(transport_name, transport_setting);
			}
		}



		return true;
	}
	bool crtc_sdp::_parse_media_description(const std::string & media_sdp_description)
	{
		std::string line;
		int mline_index = -1;
		int msid_signaling = 0;

		// Zero or more media descriptions
		// RFC 4566
		// m=<media> <port> <proto> <fmt>
		while (GetLineWithType(media_sdp_description, &m_current_pos, &line, kLineTypeMedia)) 
		{
			++mline_index;

			std::vector<std::string> fields;
			rtc_sdp_util::split(line.substr(kLinePrefixLength), kSdpDelimiterSpaceChar, &fields);

			const size_t expected_min_fields = 4;
			if (fields.size() < expected_min_fields) 
			{
				return false; // ParseFailedExpectMinFieldNum(line, expected_min_fields, error);
			}
			bool port_rejected = false;
			// RFC 3264
			// To reject an offered stream, the port number in the corresponding stream
			// in the answer MUST be set to zero.
			if (fields[1] == kMediaPortRejected) {
				port_rejected = true;
			}

			int port = 0;
			if (!FromString<int>(fields[1], &port) || !IsValidPort(port))
			{
				return false; // ParseFailed(line, "The port number is invalid", error);
			}
			std::string protocol = fields[2];

			// <fmt>
			std::vector<int> payload_types;
			if (rtc_sdp_util::is_rtp(protocol)) 
			{
				for (size_t j = 3; j < fields.size(); ++j) 
				{
					// TODO(wu): Remove when below bug is fixed.
					// https://bugzilla.mozilla.org/show_bug.cgi?id=996329
					if (fields[j].empty() && j == fields.size() - 1) 
					{
						continue;
					}

					int pl = 0;
					if (!GetPayloadTypeFromString(line, fields[j], &pl))
					{
						return false;
					}
					payload_types.push_back(pl);
				}
			}

			// Make a temporary TransportDescription based on |session_td|.
			// Some of this gets overwritten by ParseContent.
			/*TransportDescription transport(
				session_td.transport_options, session_td.ice_ufrag, session_td.ice_pwd,
				session_td.ice_mode, session_td.connection_role,
				session_td.identity_fingerprint.get());*/
			Transport_Description transport;
			//transport.set_transport_options()
			Media_Content_Description content;
			std::string content_name;
			bool bundle_only = false;
			int section_msid_signaling = 0;
			if (HasAttribute(line, kMediaTypeVideo)) {
				/*content = ParseContentDescription<VideoContentDescription>(
					message, cricket::MEDIA_TYPE_VIDEO, mline_index, protocol,
					payload_types, pos, &content_name, &bundle_only,
					&section_msid_signaling, &transport, candidates, error);*/
			}
			else if (HasAttribute(line, kMediaTypeAudio)) {
				/*content = ParseContentDescription<AudioContentDescription>(
					message, cricket::MEDIA_TYPE_AUDIO, mline_index, protocol,
					payload_types, pos, &content_name, &bundle_only,
					&section_msid_signaling, &transport, candidates, error);*/
			}
			/*else if (HasAttribute(line, kMediaTypeData)) {
				std::unique_ptr<DataContentDescription> data_desc =
					ParseContentDescription<DataContentDescription>(
						message, cricket::MEDIA_TYPE_DATA, mline_index, protocol,
						payload_types, pos, &content_name, &bundle_only,
						&section_msid_signaling, &transport, candidates, error);

				if (data_desc && IsDtlsSctp(protocol)) {
					int p;
					if (rtc::FromString(fields[3], &p)) {
						if (!AddOrModifySctpDataCodecPort(data_desc.get(), p)) {
							return false;
						}
					}
					else if (fields[3] == kDefaultSctpmapProtocol) {
						data_desc->set_use_sctpmap(false);
					}
				}

				content = std::move(data_desc);
			}*/
			else {
			//	RTC_LOG(LS_WARNING) << "Unsupported media type: " << line;
				continue;
			}
			//if (!content.get()) {
			//	// ParseContentDescription returns NULL if failed.
			//	return false;
			//}

			msid_signaling |= section_msid_signaling;

			bool content_rejected = false;
			// A port of 0 is not interpreted as a rejected m= section when it's
			// used along with a=bundle-only.
			if (bundle_only) {
				if (!port_rejected) {
					// Usage of bundle-only with a nonzero port is unspecified. So just
					// ignore bundle-only if we see this.
					bundle_only = false;
				/*	RTC_LOG(LS_WARNING)
						<< "a=bundle-only attribute observed with a nonzero "
						"port; this usage is unspecified so the attribute is being "
						"ignored.";*/
				}
			}
			else {
				// If not using bundle-only, interpret port 0 in the normal way; the m=
				// section is being rejected.
				content_rejected = port_rejected;
			}

			if (rtc_sdp_util::is_rtp(protocol)) {
				// Set the extmap.
				/*if (!session_extmaps.empty() &&
					!content->rtp_header_extensions().empty()) {
					return ParseFailed("",
						"The a=extmap MUST be either all session level or "
						"all media level.",
						error);
				}
				for (size_t i = 0; i < session_extmaps.size(); ++i) {
					content->AddRtpHeaderExtension(session_extmaps[i]);
				}*/
			}
			//content->set_protocol(protocol);

			// Use the session level connection address if the media level addresses are
			// not specified.
			//rtc::SocketAddress address;
			//address = content->connection_address().IsNil()
			//	? session_connection_addr
			//	: content->connection_address();
			//address.SetPort(port);
			//content->set_connection_address(address);

			//desc->AddContent(content_name,
			//	IsDtlsSctp(protocol) ? MediaProtocolType::kSctp
			//	: MediaProtocolType::kRtp,
			//	content_rejected, bundle_only, content.release());
			//// Create TransportInfo with the media level "ice-pwd" and "ice-ufrag".
			//desc->AddTransportInfo(TransportInfo(content_name, transport));
		}

		/*desc->set_msid_signaling(msid_signaling);

		size_t end_of_message = message.size();
		if (mline_index == -1 && *pos != end_of_message) {
			ParseFailed(message, *pos, "Expects m line.", error);
			return false;
		}*/
		return true;
	}



	bool crtc_sdp::_parse_connection_data(const std::string& line/*, rtc::SocketAddress* addr*/ )
	{
		// Parse the line from left to right.
		std::string token;
		std::string rightpart;
		// RFC 4566
		// c=<nettype> <addrtype> <connection-address>
		// Skip the "c="
		if (!rtc_sdp_util::tokenize_first(line, kSdpDelimiterEqualChar, &token, &rightpart)) 
		{
			return false; // ParseFailed(line, "Failed to parse the network type.", error);
		}

		// Extract and verify the <nettype>
		if (!rtc_sdp_util::tokenize_first(rightpart, kSdpDelimiterSpaceChar, &token,
			&rightpart) ||
			token != kConnectionNettype) 
		{
			return false; // ParseFailed(line,
				//"Failed to parse the connection data. The network type "
				//"is not currently supported.",
				//error);
		}

		// Extract the "<addrtype>" and "<connection-address>".
		if (!rtc_sdp_util::tokenize_first(rightpart, kSdpDelimiterSpaceChar, &token,
			&rightpart)) {
			return false; // ParseFailed(line, "Failed to parse the address type.", error);
		}

		// The rightpart part should be the IP address without the slash which is used
		// for multicast.
		if (rightpart.find('/') != std::string::npos) 
		{
			return false; // ParseFailed(line,
				//"Failed to parse the connection data. Multicast is not "
				//"currently supported.",
				//error);
		}


		in_addr addr;
		if (rtc_sdp_util::cinet_pton(AF_INET, rightpart.c_str(), &addr) == 0) 
		{
			in6_addr addr6;
			if (rtc_sdp_util::cinet_pton(AF_INET6, rightpart.c_str(), &addr6) == 0) 
			{
				//*out = IPAddress();
				return false;
			}
			//*out = IPAddress(addr6);
		}
		else 
		{
			//*out = IPAddress(addr);
		}
		//addr->SetIP(rightpart);

		// Verify that the addrtype matches the type of the parsed address.
		/*if ((addr->family() == AF_INET && token != "IP4") ||
			(addr->family() == AF_INET6 && token != "IP6")) {
			addr->Clear();
			return ParseFailed(
				line,
				"Failed to parse the connection data. The address type is mismatching.",
				error);
		}*/




		return true;
	}
	bool crtc_sdp::_parse_group_attribute(const std::string & line)
	{
		//RTC_DCHECK(desc != NULL);

		// RFC 5888 and draft-holmberg-mmusic-sdp-bundle-negotiation-00
		// a=group:BUNDLE video voice
		std::vector<std::string> fields;
		rtc_sdp_util::split(line.substr(kLinePrefixLength), kSdpDelimiterSpaceChar, &fields);
		std::string semantics;
		if (!GetValue(fields[0], kAttributeGroup, &semantics)) 
		{
			return false;
		}
		//cricket::ContentGroup group(semantics);
		::Content_Group* group_ptr  =m_session_description.add_content_groups();
		for (size_t i = 1; i < fields.size(); ++i)
		{
			if (group_ptr)
			{
				*group_ptr->add_content_names() = fields[i];
			}
			//group.AddContentName(fields[i]);
		}
		//desc->AddGroup(group);
		return true;
	}
	bool crtc_sdp::_parse_ice_options(const std::string & line, Transport_Description& transport_options)
	{
		std::string ice_options;
		if (!GetValue(line, kAttributeIceOption, &ice_options))
		{
			return false;
		}
		std::vector<std::string> fields;
		rtc_sdp_util::split(ice_options, kSdpDelimiterSpaceChar, &fields);
		for (size_t i = 0; i < fields.size(); ++i) 
		{
			*transport_options.add_transport_options() = fields[i];
			//transport_options->push_back(fields[i]);
		}
		return true;
	}
	bool crtc_sdp::_parse_fingerprint_attribute(const std::string & line, Ssl_Fignerpint * fingerprint)
	{
		if (!IsLineType(line, kLineTypeAttributes) ||
			!HasAttribute(line, kAttributeFingerprint)) 
		{
			return false;// ParseFailedExpectLine(line, 0, kLineTypeAttributes,
				//kAttributeFingerprint, error);
		}

		std::vector<std::string> fields;
		rtc_sdp_util::split(line.substr(kLinePrefixLength), kSdpDelimiterSpaceChar, &fields);
		const size_t expected_fields = 2;
		if (fields.size() != expected_fields) 
		{
			return false; // ParseFailedExpectFieldNum(line, expected_fields, error);
		}

		// The first field here is "fingerprint:<hash>.
		std::string algorithm;
		if (!GetValue(fields[0], kAttributeFingerprint, &algorithm)) 
		{
			return false;
		}
		if (fingerprint)
		{
			fingerprint->set_algorithm(algorithm);
			fingerprint->set_digest(fields[1]);
		}
		// Downcase the algorithm. Note that we don't need to downcase the
		// fingerprint because hex_decode can handle upper-case.
		//absl::c_transform(algorithm, algorithm.begin(), ::tolower);

		// The second field is the digest value. De-hexify it.
		/**fingerprint =
			rtc::SSLFingerprint::CreateUniqueFromRfc4572(algorithm, fields[1]);
		if (!*fingerprint) {
			return ParseFailed(line, "Failed to create fingerprint from the digest.",
				error);
		}*/

		return true;
	}
	bool crtc_sdp::_parse_dtls_setup(const std::string & line, Transport_Description & role)
	{
		// setup-attr           =  "a=setup:" role
		// role                 =  "active" / "passive" / "actpass" / "holdconn"
		std::vector<std::string> fields;
		rtc_sdp_util::split(line.substr(kLinePrefixLength), kSdpDelimiterColonChar, &fields);
		const size_t expected_fields = 2;
		if (fields.size() != expected_fields)
		{
			return false; // ParseFailedExpectFieldNum(line, expected_fields, error);
		}
		std::string role_str = fields[1];
		//if (!cricket::StringToConnectionRole(role_str, role)) {
		//	return false; // ParseFailed(line, "Invalid attribute value.", error);
		//}

		const char* const roles[] = {
			CONNECTIONROLE_ACTIVE_STR, CONNECTIONROLE_PASSIVE_STR,
			CONNECTIONROLE_ACTPASS_STR, CONNECTIONROLE_HOLDCONN_STR };

		for (size_t i = 0; i < 4  ; ++i) {
			if ( roles[i]== role_str)
			{
				role .set_connection_role( static_cast<Connection_Role>(CONNECTIONROLE_ACTIVE + i));
				return true;
			}
		}
		return false;
		return true;
	}

	bool crtc_sdp::_parse_extmap(const std::string& line, Rtp_Extension* extmap)
	{
		// RFC 5285
		// a=extmap:<value>["/"<direction>] <URI> <extensionattributes>
		std::vector<std::string> fields;
		rtc_sdp_util::split(line.substr(kLinePrefixLength), kSdpDelimiterSpaceChar, &fields);
		const size_t expected_min_fields = 2;
		if (fields.size() < expected_min_fields) {
			return false;// ParseFailedExpectMinFieldNum(line, expected_min_fields, error);
		}
		std::string uri = fields[1];

		std::string value_direction;
		if (!GetValue(fields[0], kAttributeExtmap, &value_direction)) {
			return false;
		}
		std::vector<std::string> sub_fields;
		rtc_sdp_util::split(value_direction, kSdpDelimiterSlashChar, &sub_fields);
		int value = 0;
		if (!GetValueFromString(line, sub_fields[0], &value)) {
			return false;
		}

		bool encrypted = false;
		if (uri == RtpExtension_kEncryptHeaderExtensionsUri) {
			// RFC 6904
			// a=extmap:<value["/"<direction>] urn:ietf:params:rtp-hdrext:encrypt <URI>
			//     <extensionattributes>
			const size_t expected_min_fields_encrypted = expected_min_fields + 1;
			if (fields.size() < expected_min_fields_encrypted) {
				return false;// ParseFailedExpectMinFieldNum(line, expected_min_fields_encrypted,
					//error);
			}

			encrypted = true;
			uri = fields[2];
			if (uri == RtpExtension_kEncryptHeaderExtensionsUri) {
				return false;;/// ParseFailed(line, "Recursive encrypted header.", error);
			}
		}
		if (extmap)
		{
			extmap->set_uri(uri);
			extmap->set_id(value);
			extmap->set_encrypt(encrypted);
		}
		//*extmap = Rtp_Extension(uri, value, encrypted);
		return true;
	}

}