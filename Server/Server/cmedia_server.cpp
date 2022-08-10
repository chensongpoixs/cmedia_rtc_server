/***********************************************************************************************
created: 		2022-08-09

author:			chensong

purpose:		cmedia_server


************************************************************************************************/

#include "cmedia_server.h"
#include "cwan_server.h"
#include "ctime_elapse.h"
namespace chen {
	cmedia_server g_media_server;

	cmedia_server::cmedia_server() : m_stop(false)
	{
	}

	cmedia_server::~cmedia_server()
	{
	}

	bool cmedia_server::init(const char* log_path, const char* config_file)
	{
		//SYSTEM_LOG("wan server  init ...");
		if (!g_wan_server.init())
		{
			return false;
		}
		//SYSTEM_LOG("wan server  startup ...");
		if (!g_wan_server.startup())
		{
			return false;
		}
		//SYSTEM_LOG(" agent server init ok");

		return true;
	}

	bool cmedia_server::Loop()
	{
		static const uint32 TICK_TIME = 100;
		//启动内网并等待初始化完成

		ctime_elapse time_elapse;
		uint32 uDelta = 0;
		while (!m_stop)
		{
			uDelta += time_elapse.get_elapse();

			//	g_game_client.update(uDelta);
			g_wan_server.update(uDelta);


			//g_client_collection_mgr.update(uDelta);




			uDelta = time_elapse.get_elapse();

			if (uDelta < TICK_TIME)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(TICK_TIME - uDelta));
			}
		}

		//SYSTEM_LOG("Leave main loop");

		return true;
		return true;
	}

	void cmedia_server::destroy()
	{
		g_wan_server.shutdown();
		g_wan_server.destroy();
		//SYSTEM_LOG("g_wan_server Destroy OK!");

		//g_client_collection_mgr.destroy();
		//SYSTEM_LOG("g_client_collection_mgr Destroy OK !!!");

		//g_cloud_render_agent_mgr.destroy();
		//SYSTEM_LOG(" thrift agent destroy OK !!!");
		//g_cfg.destroy();

		//1 log
		//LOG::destroy();
		//printf("Log Destroy OK!\n");

		//printf(" agent server Destroy End\n");
	}

	void cmedia_server::stop()
	{
		m_stop = true;
	}

}