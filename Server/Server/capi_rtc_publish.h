/***********************************************************************************************
created: 		2023-01-15

author:			chensong

purpose:		api_rtc_publish


************************************************************************************************/

#ifndef _C_API_RTC_PUBLISH_H_
#define _C_API_RTC_PUBLISH_H_
#include "cnet_type.h"
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include "crtc_sdp.h"

namespace chen {

	class crtc_user_config;
	class capi_rtc_publish
	{
	public:
		capi_rtc_publish() {}
		~capi_rtc_publish();


	public:



		int32 do_serve_client(const std::string &remote_sdp);

	protected:
	private:

		int32 _serve_client(crtc_user_config * ruc);

		int32 _check_remote_sdp(const crtc_sdp & remote_sdp);
	};
}

#endif // _C_API_RTC_PUBLISH_H_