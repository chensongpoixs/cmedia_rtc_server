/***********************************************************************************************
created: 		2022-09-04

author:			chensong

purpose:		crtc_sdp_util


************************************************************************************************/

#ifndef _C_RTC_SDP_UTIL_DEFINE_H_
#define _C_RTC_SDP_UTIL_DEFINE_H_
#include <iostream>

namespace chen {

	const char CN_AUDIO[] = "audio";
	const char CN_VIDEO[] = "video";
	const char CN_DATA[] = "data";
	const char CN_OTHER[] = "main";

	const char GROUP_TYPE_BUNDLE[] = "BUNDLE";

	// Minimum ufrag length is 4 characters as per RFC5245.
	const int ICE_UFRAG_LENGTH = 4;
	// Minimum password length of 22 characters as per RFC5245. We chose 24 because
	// some internal systems expect password to be multiple of 4.
	const int ICE_PWD_LENGTH = 24;
	const size_t ICE_UFRAG_MIN_LENGTH = 4;
	const size_t ICE_PWD_MIN_LENGTH = 22;
	const size_t ICE_UFRAG_MAX_LENGTH = 256;
	const size_t ICE_PWD_MAX_LENGTH = 256;

	// This is media-specific, so might belong
	// somewhere like media/base/mediaconstants.h
	const int ICE_CANDIDATE_COMPONENT_RTP = 1;
	const int ICE_CANDIDATE_COMPONENT_RTCP = 2;
	const int ICE_CANDIDATE_COMPONENT_DEFAULT = 1;

	// From RFC 4145, SDP setup attribute values.
	const char CONNECTIONROLE_ACTIVE_STR[] = "active";
	const char CONNECTIONROLE_PASSIVE_STR[] = "passive";
	const char CONNECTIONROLE_ACTPASS_STR[] = "actpass";
	const char CONNECTIONROLE_HOLDCONN_STR[] = "holdconn";

	const int MIN_CHECK_RECEIVING_INTERVAL = 50;
	const int RECEIVING_TIMEOUT = MIN_CHECK_RECEIVING_INTERVAL * 50;
	const int RECEIVING_SWITCHING_DELAY = 1000;
	const int BACKUP_CONNECTION_PING_INTERVAL = 25 * 1000;
	const int REGATHER_ON_FAILED_NETWORKS_INTERVAL = 5 * 60 * 1000;

	// When the socket is unwritable, we will use 10 Kbps (ignoring IP+UDP headers)
	// for pinging. When the socket is writable, we will use only 1 Kbps because we
	// don't want to degrade the quality on a modem.  These numbers should work well
	// on a 28.8K modem, which is the slowest connection on which the voice quality
	// is reasonable at all.
	const int STUN_PING_PACKET_SIZE = 60 * 8;
	const int STRONG_PING_INTERVAL = 1000 * STUN_PING_PACKET_SIZE / 1000;  // 480ms.
	const int WEAK_PING_INTERVAL = 1000 * STUN_PING_PACKET_SIZE / 10000;   // 48ms.
	const int WEAK_OR_STABILIZING_WRITABLE_CONNECTION_PING_INTERVAL = 900;
	const int STRONG_AND_STABLE_WRITABLE_CONNECTION_PING_INTERVAL = 2500;
	const int CONNECTION_WRITE_CONNECT_TIMEOUT = 5 * 1000;  // 5 seconds
	const uint32_t CONNECTION_WRITE_CONNECT_FAILURES = 5;   // 5 pings

	const int STUN_KEEPALIVE_INTERVAL = 10 * 1000;  // 10 seconds

	const int MIN_CONNECTION_LIFETIME = 10 * 1000;          // 10 seconds.
	const int DEAD_CONNECTION_RECEIVE_TIMEOUT = 30 * 1000;  // 30 seconds.
	const int WEAK_CONNECTION_RECEIVE_TIMEOUT = 2500;       // 2.5 seconds
	const int CONNECTION_WRITE_TIMEOUT = 15 * 1000;         // 15 seconds
															// There is no harm to keep this value high other than a small amount
															// of increased memory, but in some networks (2G), we observe up to 60s RTTs.
	const int CONNECTION_RESPONSE_TIMEOUT = 60 * 1000;  // 60 seconds




	const char RtpExtension_kAudioLevelUri[] =
		"urn:ietf:params:rtp-hdrext:ssrc-audio-level";

	const char RtpExtension_kTimestampOffsetUri[] =
		"urn:ietf:params:rtp-hdrext:toffset";

	const char RtpExtension_kAbsSendTimeUri[] =
		"http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time";

	const char RtpExtension_kVideoRotationUri[] = "urn:3gpp:video-orientation";

	const char RtpExtension_kTransportSequenceNumberUri[] =
		"http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01";
	const char RtpExtension_kTransportSequenceNumberV2Uri[] =
		"http://www.webrtc.org/experiments/rtp-hdrext/transport-wide-cc-02";

	// This extension allows applications to adaptively limit the playout delay
	// on frames as per the current needs. For example, a gaming application
	// has very different needs on end-to-end delay compared to a video-conference
	// application.
	const char RtpExtension_kPlayoutDelayUri[] =
		"http://www.webrtc.org/experiments/rtp-hdrext/playout-delay";

	const char RtpExtension_kVideoContentTypeUri[] =
		"http://www.webrtc.org/experiments/rtp-hdrext/video-content-type";

	const char RtpExtension_kVideoTimingUri[] =
		"http://www.webrtc.org/experiments/rtp-hdrext/video-timing";

	const char RtpExtension_kMidUri[] = "urn:ietf:params:rtp-hdrext:sdes:mid";

	const char RtpExtension_kFrameMarkingUri[] =
		"http://tools.ietf.org/html/draft-ietf-avtext-framemarking-07";

	const char RtpExtension_kGenericFrameDescriptorUri00[] =
		"http://www.webrtc.org/experiments/rtp-hdrext/generic-frame-descriptor-00";
	const char RtpExtension_kGenericFrameDescriptorUri01[] =
		"http://www.webrtc.org/experiments/rtp-hdrext/generic-frame-descriptor-01";
	const char RtpExtension_kGenericFrameDescriptorUri[] =
		"http://www.webrtc.org/experiments/rtp-hdrext/generic-frame-descriptor-00";

	const char RtpExtension_kEncryptHeaderExtensionsUri[] =
		"urn:ietf:params:rtp-hdrext:encrypt";

	const char RtpExtension_kColorSpaceUri[] =
		"http://www.webrtc.org/experiments/rtp-hdrext/color-space";

	const char RtpExtension_kRidUri[] =
		"urn:ietf:params:rtp-hdrext:sdes:rtp-stream-id";

	const char RtpExtension_kRepairedRidUri[] =
		"urn:ietf:params:rtp-hdrext:sdes:repaired-rtp-stream-id";
}

#endif //_C_RTC_SDP_UTIL_DEFINE_H_