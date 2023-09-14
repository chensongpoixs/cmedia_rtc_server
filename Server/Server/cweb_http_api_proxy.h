/********************************************************************
created:	2021-11-24

author:		chensong

purpose:	Location

*********************************************************************/

#ifndef _C_WEB_HTTP_API_PROXY_H_
#define _C_WEB_HTTP_API_PROXY_H_

#include <memory>
#include <string>
#include <utility>
#include "cweb_http_api_interface.h"
#include "cproxy.h"
#include "cweb_http_api_mgr.h"
#include "cweb_http_api_proxy.h"
#include "chttp_queue_mgr.h"
#include "croom.h"
#include <vector>
namespace chen {
	BEGIN_PROXY_MAP(cweb_http_api) 
		PROXY_WORKER_METHOD0(std::vector< croom_info>, get_all_room);

		PROXY_WORKER_METHOD1(std::vector< chen::cuser_info>, get_room_info, const std::string &  )
	END_PROXY_MAP()


		
}
extern chen::cweb_http_api_proxy g_web_http_api_proxy;
#endif //_C_WEB_API_PROXY_H_