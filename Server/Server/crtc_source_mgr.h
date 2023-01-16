/***********************************************************************************************
created: 		2023-01-15

author:			chensong

purpose:		_C_RTC_SOURCE_MGR_H_


************************************************************************************************/

#ifndef _C_RTC_SOURCE_MGR_H_
#define _C_RTC_SOURCE_MGR_H_
#include "cnet_type.h"
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include "crtc_sdp.h"
#include "cmedia_desc.h"
#include "csingleton.h"

namespace chen {
	class crtc_source;
	class crtc_user_config;
	class crtc_source_mgr
	{
	public:
		crtc_source_mgr()
			: m_pool()
		{}
		~crtc_source_mgr();

	public:
		//  create source when fetch from cache failed.
		// @param r the client request.
		// @param pps the matched source, if success never be NULL.
		virtual int32 fetch_or_create(crtc_user_config * ruc,  crtc_source** pps);
	public:
		// Get the exists source, NULL when not exists.
		virtual crtc_source* fetch( crtc_user_config * ruc);
	private:


		std::map<std::string, crtc_source*> m_pool;
	};
	extern crtc_source_mgr g_rtc_source_mgr;

}


#endif // _C_RTC_SOURCE_MGR_H_