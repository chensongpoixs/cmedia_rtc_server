/***********************************************************************************************
created: 		2022-08-24

author:			chensong

purpose:		log
************************************************************************************************/
#include "cclient.h"

#include "clog.h"
#include "ccfg.h"
#include "cwebsocket_mgr.h"
#include <iostream>
#include <io.h>
#include "cmsg_base_id.h"
#include "pc/video_track_source.h"
#include "cmsg_dispatch.h"
#include "json.hpp"
#include "build_version.h"
namespace chen {

#define  WEBSOCKET_CHECK_RESPONSE()  if (msg.find("result") == msg.end())\
	{\
		ERROR_EX_LOG(" [msg = %s] not find 'result' failed !!!", msg.dump().c_str());\
		return false;\
	}\
	uint16  result = msg["result"].get<uint16>();\
	if (0 != result)\
	{\
		ERROR_EX_LOG("  not result  [msg = %s] !!!", msg.dump().c_str());\
		return false;\
	}



	bool cclient::handler_s2c_login(nlohmann::json & msg)
	{
		WEBSOCKET_CHECK_RESPONSE();
		if (EMedia_Loading != m_media_session_stats)
		{
			WARNING_EX_LOG("client status error  status = %u", m_media_session_stats);
		}
		m_media_session_stats = EMedia_CreateVideo;
		NORMAL_EX_LOG("client login ok !!!");
		return true;
	}

	bool cclient::handler_s2c_create_room(nlohmann::json & msg)
	{
		WEBSOCKET_CHECK_RESPONSE();




		return true;
	}

	bool cclient::handler_s2c_destroy_room(nlohmann::json & msg)
	{
		WEBSOCKET_CHECK_RESPONSE();
		return true;
	}


	bool cclient::handler_s2c_rtc_connect(nlohmann::json& msg)
	{
		WEBSOCKET_CHECK_RESPONSE();
		return true;
	}

}