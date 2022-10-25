/********************************************************************
created:	2022-10-25

author:		chensong

level:		ortc

purpose:	一些傻瓜搞WebRTC的问题
*********************************************************************/


#ifndef _C_ORTC_H_
#define _C_ORTC_H_
#include "cglobal_rtc.h"
#include "cnet_type.h"
namespace chen 
{
	namespace ortc
	{
		/*RTC::RtpCodecParameters*/ int32  match_codecs(const RTC::RtpCodecParameters & media_codec, const std::vector<RTC::RtpCodecParameters>& supported_codec);
	}
}

#endif // _C_ORTC_H_