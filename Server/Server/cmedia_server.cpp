/***********************************************************************************************
created: 		2022-08-09

author:			chensong

purpose:		cmedia_server


************************************************************************************************/

#include "cmedia_server.h"
#include "cwan_server.h"
#include "ctime_elapse.h"
#include "clog.h"
#include "croom_mgr.h"
#include "cmsg_dispatch.h"
#include "cuv_util.h"
#include "ccfg.h"
#include "crtc_sdp.h"
#include "cclient_msg_dispatch.h"
namespace chen {
	cmedia_server g_media_server;

	cmedia_server::cmedia_server() 
		: ctimer()
		, m_stop(false)
	{
	}

	cmedia_server::~cmedia_server()
	{
	}

	bool cmedia_server::init(const char* log_path, const char* config_file)
	{


		printf("LOG init ...");
		if (!LOG::init(log_path, "media_rtc_server"))
		{
			return false;
		}
		if (!g_cfg.init(config_file))
		{
			return false;
		}
		
		//ctime_base_api::set_time_zone(g_cfg.get_int32(ECI_TimeZone));
		//ctime_base_api::set_time_adjust(g_cfg.get_int32(ECI_TimeAdjust));
		SYSTEM_LOG("dispatch init ...");

		if (!g_client_msg_dispatch.init())
		{
			return false;
		}

		if (!g_msg_dispatch.init())
		{
			return false;
		}

		SYSTEM_LOG("uv init ...");

		if (!uv_util::init())
		{
			return false;
		}
		uv_util::print_version();


		SYSTEM_LOG("room mgr init ...");
		if (!g_room_mgr.init())
		{
			return false;
		}

		SYSTEM_LOG("websocket wan server  init ...");
		if (!g_wan_server.init())
		{
			return false;
		}
		SYSTEM_LOG("websocket wan server  startup ...");
		if (!g_wan_server.startup())
		{
			return false;
		}
		SYSTEM_LOG("timer init ...");
		if (!ctimer::init())
		{
			return false;
		}
		SYSTEM_LOG("timer startup  ...");

		ctimer::Start(1u, 100u);

		SYSTEM_LOG(" media rtc server init ok");

		return true;
	}

	bool cmedia_server::Loop()
	{
		SYSTEM_LOG("starting libuv loop");
		//DepLibUV::RunLoop();
		uv_util::run_loop();
		SYSTEM_LOG("libuv loop ended");
		//static const uint32 TICK_TIME = 100;
		////启动内网并等待初始化完成

		//ctime_elapse time_elapse;
		//uint32 uDelta = 0;
		//while (!m_stop)
		//{
		//	uDelta += time_elapse.get_elapse();

		//	//	g_game_client.update(uDelta);
		//	g_wan_server.update(uDelta);

 
		//	g_room_mgr.update(uDelta);


		//	uDelta = time_elapse.get_elapse();

		//	if (uDelta < TICK_TIME)
		//	{
		//		std::this_thread::sleep_for(std::chrono::milliseconds(TICK_TIME - uDelta));
		//	}
		//}

		//SYSTEM_LOG("Leave main loop");

		return true;
		 
	}

	void cmedia_server::destroy()
	{
		g_wan_server.shutdown();
		g_wan_server.destroy();
		SYSTEM_LOG("g_wan_server Destroy OK!");

		uv_util::destroy();
		SYSTEM_LOG("uv destroy OK !!!");
		//g_client_collection_mgr.destroy();
		//SYSTEM_LOG("g_client_collection_mgr Destroy OK !!!");

		//g_cloud_render_agent_mgr.destroy();
		SYSTEM_LOG(" cfg  destroy OK !!!");
		g_cfg.destroy();

		g_msg_dispatch.destroy();
		SYSTEM_LOG("msg dispath destroy OK !!!");

		//1 log
		LOG::destroy();
		printf("Log Destroy OK!\n");

		printf(" media rtc  server Destroy End\n");
	}

	void cmedia_server::stop()
	{
		Stop();
		m_stop = true;
	}

	void cmedia_server::OnTimer()
	{
		static const uint32 TICK_TIME = 100;
		////启动内网并等待初始化完成
		//DEBUG_EX_LOG("   %llu ms", uv_util::GetTimeMsInt64());
		//ctime_elapse time_elapse;
		//uint32 uDelta = 0;
		if (!m_stop)
		{
			//uDelta += time_elapse.get_elapse();

			//	g_game_client.update(uDelta);
			g_wan_server.update(TICK_TIME);


			g_room_mgr.update(TICK_TIME);


			//uDelta = time_elapse.get_elapse();

			/*if (uDelta < TICK_TIME)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(TICK_TIME - uDelta));
			}*/
		}

		//SYSTEM_LOG("Leave main loop");
	}

}