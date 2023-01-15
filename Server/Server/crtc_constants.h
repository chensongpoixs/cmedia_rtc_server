/***********************************************************************************************
created: 		2023-01-15

author:			chensong

purpose:		_C_RTC_CONSTANTS_H_


************************************************************************************************/

#ifndef _C_RTC_CONSTANTS_H_
#define _C_RTC_CONSTANTS_H_
#include "cnet_type.h"
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include "crtc_sdp.h"
#include "cmedia_desc.h"
#include "crtx_paylod_des.h"
#include "cred_payload.h"
#include "cvideo_payload.h"
#include "caudio_payload.h"
#include "cred_payload.h"
namespace chen {





	// The RTP packet max size, should never exceed this size.
	const int kRtpPacketSize = 1500;

	const int kRtpHeaderFixedSize = 12;
	const uint8_t kRtpMarker = 0x80;

	// H.264 nalu header type mask.
	const uint8_t kNalTypeMask = 0x1F;

	// @see: https://tools.ietf.org/html/rfc6184#section-5.2
	const uint8_t kStapA = 24;

	// @see: https://tools.ietf.org/html/rfc6184#section-5.2
	const uint8_t kFuA = 28;

	// @see: https://tools.ietf.org/html/rfc6184#section-5.8
	const uint8_t kStart = 0x80; // Fu-header start bit
	const uint8_t kEnd = 0x40; // Fu-header end bit
	//////////////////




	// Firefox defaults as 109, Chrome is 111.
	const int kAudioPayloadType = 111;
	const int kAudioChannel = 2;
	const int kAudioSamplerate = 48000;

	// Firefox defaults as 126, Chrome is 102.
	const int kVideoPayloadType = 102;
	const int kVideoSamplerate = 90000;

	// The RTP payload max size, reserved some paddings for SRTP as such:
	//      kRtpPacketSize = kRtpMaxPayloadSize + paddings
	// For example, if kRtpPacketSize is 1500, recommend to set kRtpMaxPayloadSize to 1400,
	// which reserves 100 bytes for SRTP or paddings.
	// otherwise, the kRtpPacketSize must less than MTU, in webrtc source code, 
	// the rtp max size is assigned by kVideoMtu = 1200.
	// so we set kRtpMaxPayloadSize = 1200.
	// see @doc https://groups.google.com/g/discuss-webrtc/c/gH5ysR3SoZI
	const int kRtpMaxPayloadSize = kRtpPacketSize - 300;

}

#endif // _C_RTC_CONSTANTS_H_