/***********************************************************************************************
created: 		2022-09-04

author:			chensong

purpose:		crtc_sdp_util
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
	// TODO@chensong 2023-07-19 M85版本中已经剔除该rtcp的扩展协议该协议是针对H264和H265的rtp的包的封装
	const char RtpExtension_kFrameMarkingUri[] = "urn:ietf:params:rtp-hdrext:framemarking";

	const char RtpExtension_kFrameMarking07Uri[] =
		"http://tools.ietf.org/html/draft-ietf-avtext-framemarking-07";

	const char RtpExtension_kGenericFrameDescriptorUri00[] =
		"http://www.webrtc.org/experiments/rtp-hdrext/generic-frame-descriptor-00";
	const char RtpExtension_kGenericFrameDescriptorUri01[] =
		"http://www.webrtc.org/experiments/rtp-hdrext/generic-frame-descriptor-01";
	const char RtpExtension_kGenericFrameDescriptorUri[] =
		"http://www.webrtc.org/experiments/rtp-hdrext/generic-frame-descriptor-00";



	const std::string kTWCCExt = "http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01";

	const char RtpExtension_kEncryptHeaderExtensionsUri[] =
		"urn:ietf:params:rtp-hdrext:encrypt";

	const char RtpExtension_kColorSpaceUri[] =
		"http://www.webrtc.org/experiments/rtp-hdrext/color-space";

	const char RtpExtension_kRidUri[] =
		"urn:ietf:params:rtp-hdrext:sdes:rtp-stream-id";

	const char RtpExtension_kRepairedRidUri[] =
		"urn:ietf:params:rtp-hdrext:sdes:repaired-rtp-stream-id";



	enum MediaType { MEDIA_TYPE_AUDIO, MEDIA_TYPE_VIDEO, MEDIA_TYPE_DATA };
	//////////////////////////////////////media constants/////////////////////////////////////////////

	const int kVideoCodecClockrate = 90000;
	const int kDataCodecClockrate = 90000;
	const int kDataMaxBandwidth = 30720;  // bps

	const float kHighSystemCpuThreshold = 0.85f;
	const float kLowSystemCpuThreshold = 0.65f;
	const float kProcessCpuThreshold = 0.10f;

	const char kRtxCodecName[] = "rtx";
	const char kRedCodecName[] = "red";
	const char kUlpfecCodecName[] = "ulpfec";
	const char kMultiplexCodecName[] = "multiplex";

	// TODO(brandtr): Change this to 'flexfec' when we are confident that the
	// header format is not changing anymore.
	const char kFlexfecCodecName[] = "flexfec-03";

	// draft-ietf-payload-flexible-fec-scheme-02.txt
	const char kFlexfecFmtpRepairWindow[] = "repair-window";

	const char kCodecParamAssociatedPayloadType[] = "apt";
	const char kCodecParamAssociatedCodecName[] = "acn";

	const char kOpusCodecName[] = "opus";
	const char kIsacCodecName[] = "ISAC";
	const char kL16CodecName[] = "L16";
	const char kG722CodecName[] = "G722";
	const char kIlbcCodecName[] = "ILBC";
	const char kPcmuCodecName[] = "PCMU";
	const char kPcmaCodecName[] = "PCMA";
	const char kCnCodecName[] = "CN";
	const char kDtmfCodecName[] = "telephone-event";

	// draft-spittka-payload-rtp-opus-03.txt
	const char kCodecParamPTime[] = "ptime";
	const char kCodecParamMaxPTime[] = "maxptime";
	const char kCodecParamMinPTime[] = "minptime";
	const char kCodecParamSPropStereo[] = "sprop-stereo";
	const char kCodecParamStereo[] = "stereo";
	const char kCodecParamUseInbandFec[] = "useinbandfec";
	const char kCodecParamUseDtx[] = "usedtx";
	const char kCodecParamMaxAverageBitrate[] = "maxaveragebitrate";
	const char kCodecParamMaxPlaybackRate[] = "maxplaybackrate";

	const char kCodecParamSctpProtocol[] = "protocol";
	const char kCodecParamSctpStreams[] = "streams";

	const char kParamValueTrue[] = "1";
	const char kParamValueEmpty[] = "";

	const int kOpusDefaultMaxPTime = 120;
	const int kOpusDefaultPTime = 20;
	const int kOpusDefaultMinPTime = 3;
	const int kOpusDefaultSPropStereo = 0;
	const int kOpusDefaultStereo = 0;
	const int kOpusDefaultUseInbandFec = 0;
	const int kOpusDefaultUseDtx = 0;
	const int kOpusDefaultMaxPlaybackRate = 48000;

	const int kPreferredMaxPTime = 120;
	const int kPreferredMinPTime = 10;
	const int kPreferredSPropStereo = 0;
	const int kPreferredStereo = 0;
	const int kPreferredUseInbandFec = 0;

	const char kRtcpFbParamNack[] = "nack";
	const char kRtcpFbNackParamPli[] = "pli";
	const char kRtcpFbParamRemb[] = "goog-remb";
	const char kRtcpFbParamTransportCc[] = "transport-cc";

	const char kRtcpFbParamCcm[] = "ccm";
	const char kRtcpFbCcmParamFir[] = "fir";
	const char kRtcpFbParamRrtr[] = "rrtr";
	const char kCodecParamMaxBitrate[] = "x-google-max-bitrate";
	const char kCodecParamMinBitrate[] = "x-google-min-bitrate";
	const char kCodecParamStartBitrate[] = "x-google-start-bitrate";
	const char kCodecParamMaxQuantization[] = "x-google-max-quantization";
	const char kCodecParamPort[] = "x-google-port";
	const char kCodecParamMaxMessageSize[] = "x-google-max-message-size";

	const int kGoogleRtpDataCodecPlType = 109;
	const char kGoogleRtpDataCodecName[] = "google-data";

	const int kGoogleSctpDataCodecPlType = 108;
	const char kGoogleSctpDataCodecName[] = "google-sctp-data";

	const char kComfortNoiseCodecName[] = "CN";

	const char kVp8CodecName[] = "VP8";
	const char kVp9CodecName[] = "VP9";
	const char kH264CodecName[] = "H264";

	// RFC 6184 RTP Payload Format for H.264 video
	const char kH264FmtpProfileLevelId[] = "profile-level-id";
	const char kH264FmtpLevelAsymmetryAllowed[] = "level-asymmetry-allowed";
	const char kH264FmtpPacketizationMode[] = "packetization-mode";
	const char kH264FmtpSpropParameterSets[] = "sprop-parameter-sets";
	const char kH264ProfileLevelConstrainedBaseline[] = "42e01f";

	const int kDefaultVideoMaxFramerate = 60;

	const size_t kConferenceMaxNumSpatialLayers = 3;
	const size_t kConferenceMaxNumTemporalLayers = 3;
	const size_t kConferenceDefaultNumTemporalLayers = 3;


	///////////////////////////////////////////////////////////////////////////////



	// TODO: FIXME: Maybe we should use json.encode to escape it?
	const char kCRLF[] = "\r\n";













}

#endif //_C_RTC_SDP_UTIL_DEFINE_H_