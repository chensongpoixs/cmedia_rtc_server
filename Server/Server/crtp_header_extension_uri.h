/***********************************************************************************************
created: 		2023-04-08

author:			chensong

purpose:		ccfg
************************************************************************************************/
#ifndef _C_RTP_HEADER_EXTENSION_URI_H_
#define _C_RTP_HEADER_EXTENSION_URI_H_
#include "cnet_type.h"
#include "crtp_rtcp_define.h"
#include "clog.h"
#include "crtp_header_extensions.h"

namespace chen {
	   ERtpHeaderExtensionUri get_rtp_header_extension_uri_type(const std::string& uri);
}

#endif // _C_RTP_HEADER_EXTENSION_URI_H_