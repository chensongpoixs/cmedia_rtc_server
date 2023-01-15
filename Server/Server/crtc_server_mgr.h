/***********************************************************************************************
created: 		2023-01-15

author:			chensong

purpose:		rtc_servr_mgr


************************************************************************************************/

#ifndef _C_RTC_SERVR_MGR_H_
#define _C_RTC_SERVR_MGR_H_
#include "cnet_type.h"
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include "crtc_sdp.h"

namespace chen {
	class crtc_connection;
	class crtc_user_config;
	class crtc_server_mgr
	{
	public:
		crtc_server_mgr() {}
		~crtc_server_mgr();
	public:

		int32 create_session(crtc_user_config* ruc, crtc_sdp& local_sdp, crtc_connection** psession);
	protected:
	private:
	};
}

#endif // _C_RTC_SERVR_MGR_H_