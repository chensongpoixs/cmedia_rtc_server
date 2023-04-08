/***********************************************************************************************
created: 		2023-02-20

author:			chensong

purpose:		api_rtc_publish


************************************************************************************************/

#ifndef _C_RTP_RTCP_DEFINE_H_
#define _C_RTP_RTCP_DEFINE_H_
#include "cnet_type.h"
namespace chen {






const int kRtcpPacketSize = 1500;
const uint8_t kRtcpVersion = 0x2;

// 1500 - 20(ip_header) - 8(udp_header)
const int kMaxUDPDataSize = 1472;

// RTCP Packet Types, @see http://www.networksorcery.com/enp/protocol/rtcp.htm
enum ERtcpType
{
	ERtcpType_fir = 192,
	ERtcpType_sr = 200,
	ERtcpType_rr = 201,
	ERtcpType_sdes = 202,
	ERtcpType_bye = 203,
	ERtcpType_app = 204,
	ERtcpType_rtpfb = 205,
	ERtcpType_psfb = 206,
	ERtcpType_xr = 207,
};


enum EFeedbackPs
{
	EPSPLI = 1,
	EPSSLI = 2,
	EPSRPSI = 3,
	EPSFIR = 4,
	EPSTSTR = 5,
	EPSTSTN = 6,
	EPSVBCM = 7,
	EPSPSLEI = 8,
	EPSROI = 9,
	EPSAFB = 15,
	EPSEXT = 31
};

enum EFeedbackRtp
{
	ERtpfbNACK = 1,
	ERtpfbTMMBR = 3,
	ERtpfbTMMBN = 4,
	ERtpfbSR_REQ = 5,
	ERtpfbRAMS = 6,
	ERtpfbTLLEI = 7,
	ERtpfbECN = 8,
	ERtpfbPS = 9,
	ERtpfbTCC = 15,
	ERtpfbEXT = 31
};

// webrtc 中rtcp中xr扩展类型
enum EExtendedReportBlock
{
	EExtendedLRLE = 1,
	EExtendedDRLE = 2,
	EExtendedPRT = 3,
	EExtendedRRT = 4,
	EExtendedDLRR = 5,
	EExtendedSS = 6,
	EExtendedVM = 7
};

// @see: https://tools.ietf.org/html/rfc4585#section-6.3
const uint8_t kPLI  = 1;
const uint8_t kSLI  = 2;
const uint8_t kRPSI = 3;
const uint8_t kAFB  = 15;

// RTCP Header, @see http://tools.ietf.org/html/rfc3550#section-6.1
// @remark The header must be 4 bytes, which align with the max field size 2B.
struct crtcp_header
{
	uint16_t rc:5;
	uint16_t padding:1;
	uint16_t version:2;
	uint16_t type:8;

	uint16_t length:16;

	crtcp_header() {
        rc = 0;
        padding = 0;
        version = 0;
        type = 0;
        length = 0;
    }
};
// The Message format of TWCC, @see https://tools.ietf.org/html/draft-holmer-rmcat-transport-wide-cc-extensions-01#section-3.1
//       0                   1                   2                   3
//       0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//      |V=2|P|  FMT=15 |    PT=205     |           length              |
//      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//      |                     SSRC of packet sender                     |
//      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//      |                      SSRC of media source                     |
//      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//      |      base sequence number     |      packet status count      |
//      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//      |                 reference time                | fb pkt. count |
//      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//      |          packet chunk         |         packet chunk          |
//      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//      .                                                               .
//      .                                                               .
//      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//      |         packet chunk          |  recv delta   |  recv delta   |
//      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//      .                                                               .
//      .                                                               .
//      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//      |           recv delta          |  recv delta   | zero padding  |
//      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#define kTwccFbPktHeaderSize (4 + 8 + 8)
#define kTwccFbChunkBytes (2)
#define kTwccFbPktFormat (15)
#define kTwccFbPayloadType (205)
#define kTwccFbMaxPktStatusCount (0xffff)
#define kTwccFbDeltaUnit (250)	 // multiple of 250us
#define kTwccFbTimeMultiplier (kTwccFbDeltaUnit * (1 << 8)) // multiplicand multiplier/* 250us -> 64ms  (1 << 8) */
#define kTwccFbReferenceTimeDivisor ((1ll<<24) * kTwccFbTimeMultiplier) // dividend divisor

#define kTwccFbMaxRunLength 		0x1fff
#define kTwccFbOneBitElements 		14
#define kTwccFbTwoBitElements 		7
#define kTwccFbLargeRecvDeltaBytes	2
#define kTwccFbMaxBitElements 		kTwccFbOneBitElements



// This enum must not have any gaps, i.e., all integers between
// kRtpExtensionNone and kRtpExtensionNumberOfExtensions must be valid enum
// entries.
enum RTPExtensionType  {
	kRtpExtensionNone,
	kRtpExtensionTransmissionTimeOffset,
	kRtpExtensionAudioLevel,
	kRtpExtensionAbsoluteSendTime,
	kRtpExtensionVideoRotation,
	kRtpExtensionTransportSequenceNumber,
	kRtpExtensionTransportSequenceNumber02,
	kRtpExtensionPlayoutDelay,
	kRtpExtensionVideoContentType,
	kRtpExtensionVideoTiming,
	kRtpExtensionFrameMarking,
	kRtpExtensionRtpStreamId,
	kRtpExtensionRepairedRtpStreamId,
	kRtpExtensionMid,
	kRtpExtensionGenericFrameDescriptor00,
	kRtpExtensionGenericFrameDescriptor = kRtpExtensionGenericFrameDescriptor00,
	kRtpExtensionGenericFrameDescriptor01,
	kRtpExtensionColorSpace,
	kRtpExtensionNumberOfExtensions  // Must be the last entity in the enum.
};

}

#endif // 