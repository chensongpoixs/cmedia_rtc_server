/***********************************************************************************************
created: 		2023-01-15

author:			chensong

purpose:		_C_RTC_SOURCE_MGR_H_


************************************************************************************************/
#include "crtc_source_mgr.h"
#include "crtc_source.h"
#include <cassert>
namespace chen {
	crtc_source_mgr g_rtc_source_mgr;
	crtc_source_mgr::~crtc_source_mgr()
	{
	}
	int32 crtc_source_mgr::fetch_or_create(crtc_user_config * ruc, crtc_source** pps)
	{
		int32 err = 0;

		crtc_source * source = NULL;
		if ((source == fetch(ruc)) != NULL)
		{
			// we always update the request of resource,
			// for origin auth is on, the token in request maybe invalid,
			// and we only need to update the token of request, it's simple.
			//source->update_auth(r);
			*pps = source;
			return err;
		}

		// TODO@chensong 2022-01-15
		std::string stream_url = "";;// ruc->m_api;

		// should always not exists for create a source.
		assert(m_pool.find(stream_url) == m_pool.end());


		NORMAL_EX_LOG("new rtc source, stream_url=%s", stream_url.c_str());

		source = new crtc_source();
		if ((err = source->init( )) != 0)
		{
			WARNING_EX_LOG("init source %s", stream_url.c_str());
			return err;
			//return  error_wrap(err, "init source %s", r->get_stream_url().c_str());
		}

		m_pool[stream_url] = source;

		*pps = source;

		return err;
		return err;
	}

	chen::crtc_source* crtc_source_mgr::fetch(crtc_user_config * ruc)
	{
		crtc_source* source = NULL;

		//TODO@chensong 2022-01-15 
		std::string stream_url = "";//r->get_stream_url();
		if (m_pool.find(stream_url) == m_pool.end())
		{
			return NULL;
		}

		source = m_pool[stream_url];

		return source;
	}

}