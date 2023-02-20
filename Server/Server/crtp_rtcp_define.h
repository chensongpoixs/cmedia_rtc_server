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


}

#endif // 