
/********************************************************************
created:	2021-11-24

author:		chensong

purpose:	Location

*********************************************************************/

#ifndef _C_WEB_HTTP_API_INTERFACE_H_
#define _C_WEB_HTTP_API_INTERFACE_H_

#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "croom.h"
#include "croom_mgr.h"
namespace chen {
	class cweb_http_api_interface
	{
	public:
		virtual std::vector< croom_info>   get_all_room() = 0;
		virtual std::vector< chen::cuser_info>   get_room_info(const std::string& room_name ) = 0;


		virtual ~cweb_http_api_interface() {}
	};
}

#endif // _C_WEB_API_INTERFACE_H_
