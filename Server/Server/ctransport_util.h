/***********************************************************************************************
created: 		2023-03-08

author:			chensong

purpose:		ctransport_util


************************************************************************************************/

#ifndef _C_TRANSPORT_UTIL_H_
#define _C_TRANSPORT_UTIL_H_
#include "cnet_type.h"
#include "crtc_sdp.h"
namespace chen {
	bool  sdp_has_h264_profile(const cmedia_payload_type& payload_type, const std::string& profile);
	// For example, 42001f 42e01f, see https://blog.csdn.net/epubcn/article/details/102802108
	bool  sdp_has_h264_profile(const crtc_sdp& sdp, const std::string& profile);
}

#endif // _C_TRANSPORT_UTIL_H_