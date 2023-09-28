/***********************************************************************************************
created: 		2022-08-09

author:			chensong

purpose:		cmedia_server
输赢不重要，答案对你们有什么意义才重要。

光阴者，百代之过客也，唯有奋力奔跑，方能生风起时，是时代造英雄，英雄存在于时代。或许世人道你轻狂，可你本就年少啊。 看护好，自己的理想和激情。


我可能会遇到很多的人，听他们讲好2多的故事，我来写成故事或编成歌，用我学来的各种乐器演奏它。
然后还可能在一个国家遇到一个心仪我的姑娘，她可能会被我帅气的外表捕获，又会被我深邃的内涵吸引，在某个下雨的夜晚，她会全身淋透然后要在我狭小的住处换身上的湿衣服。
3小时候后她告诉我她其实是这个国家的公主，她愿意向父皇求婚。我不得已告诉她我是穿越而来的男主角，我始终要回到自己的世界。
然后我的身影慢慢消失，我看到她眼里的泪水，心里却没有任何痛苦，我才知道，原来我的心被丢掉了，我游历全世界的原因，就是要找回自己的本心。
于是我开始有意寻找各种各样失去心的人，我变成一块砖头，一颗树，一滴水，一朵白云，去听大家为什么会失去自己的本心。
我发现，刚出生的宝宝，本心还在，慢慢的，他们的本心就会消失，收到了各种黑暗之光的侵蚀。
从一次争论，到嫉妒和悲愤，还有委屈和痛苦，我看到一只只无形的手，把他们的本心扯碎，蒙蔽，偷走，再也回不到主人都身边。
我叫他本心猎手。他可能是和宇宙同在的级别 但是我并不害怕，我仔细回忆自己平淡的一生 寻找本心猎手的痕迹。
沿着自己的回忆，一个个的场景忽闪而过，最后发现，我的本心，在我写代码的时候，会回来。
安静，淡然，代码就是我的一切，写代码就是我本心回归的最好方式，我还没找到本心猎手，但我相信，顺着这个线索，我一定能顺藤摸瓜，把他揪出来。

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
#include "csctp_association_mgr.h"
#include "cglobal_rtc.h"
#include "cdtls_certificate.h"
#include "cglobal_config.h"
#include "csrtp_session.h"
#include "crtc_transport.h"
#include "ctransport_mgr.h"
#include "cglobal_rtc_port.h"
#include "crtsp_server.h"
#include "cglobal_rtsp.h"
#include "system_wrappers/source/field_trial.h" // webrtc::field_trial
#include "chttp_queue_mgr.h"
#include "cweb_http_api_mgr.h"
namespace chen {
	static std::once_flag globalInitOnce;
	static  const  char FieldTrials[] = "WebRTC-Bwe-AlrLimitedBackoff/Enabled/";

	cmedia_server g_media_server;

	cmedia_server::cmedia_server() 
		:   m_stop(false)
		, m_server_intaval(NULL)
	{
	}

	cmedia_server::~cmedia_server()
	{
	}

	bool cmedia_server::init(const char* log_path, const char* config_file)
	{


		
		if (!g_cfg.init(config_file))
		{
			return false;
		}
		LOG::set_level(static_cast<ELogLevelType>(g_cfg.get_uint32(ECI_LogLevel)));
		ctime_base_api::set_time_zone(g_cfg.get_int32(ECI_TimeZone));
		ctime_base_api::set_time_adjust(g_cfg.get_int32(ECI_TimeAdjust));
		SYSTEM_LOG("dispatch init ...");

		SYSTEM_LOG("global config  init ...");
		if (!g_global_config.init())
		{
			return false;
		}
		SYSTEM_LOG("global config init OK !!!");

		SYSTEM_LOG("global rtc init ...");
		if (!g_global_rtc.init())
		{
			return false;
		}

		SYSTEM_LOG("global rtc init OK !!!");
		if (!g_client_msg_dispatch.init())
		{
			return false;
		}
		SYSTEM_LOG("client_msg dispatch init OK !!!");
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

		if (!g_dtls_certificate.init())
		{
			return false;
		}
		SYSTEM_LOG("srtp init ...");

		if (!csrtp_session::init())
		{
			return false;
		}
		SYSTEM_LOG("srtp init OK !!!");

		std::call_once(globalInitOnce, [] { webrtc::field_trial::InitFieldTrialsFromString(FieldTrials); });

		SYSTEM_LOG("dtls certificate init ");

		if (!g_room_mgr.init())
		{
			return false;
		}
		SYSTEM_LOG("rtc port init ...");
		if (!g_global_rtc_port.init())
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


		SYSTEM_LOG("rtsp init ...");
		//g_rtsp_server.init();
#ifdef AUTH_CONFIG
		g_rtsp_server.SetAuthConfig("-_-", "admin", "12345");
#endif
		//init_rtsp_global();
		//g_rtsp_server.startup();
		SYSTEM_LOG("rtsp start OK !!!");

		SYSTEM_LOG("timer init ...");
		/*if (!ctimer::init())
		{
			return false;
		}*/

		SYSTEM_LOG("Web Http Server API init ...");
		if (!g_web_http_api_mgr.init())
		{
			return false;
		}
		SYSTEM_LOG("Web Http Server API startup ...");
		g_web_http_api_mgr.startup();
		SYSTEM_LOG("Web Http Server API startup OK ...");
		m_server_intaval = new ctimer(this);
		SYSTEM_LOG("timer startup  ...");

		m_server_intaval->Start(100u);

		SYSTEM_LOG(" media rtc server init ok");

		return true;
	}

	bool cmedia_server::Loop()
	{
		SYSTEM_LOG("starting libuv loop");
		g_sctp_association_mgr.CreateChecker();
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
		if (m_server_intaval)
		{
			m_server_intaval->Stop();
			SYSTEM_LOG("g_wan_server timer stop OK !!!");
			//m_server_intaval->destroy();
			delete m_server_intaval;
			m_server_intaval = NULL;
		//	SYSTEM_LOG("g_wan_server timer destroy OK !!!");
		}
		

		g_wan_server.shutdown();
		g_wan_server.destroy();
		SYSTEM_LOG("g_wan_server Destroy OK!!!");


		SYSTEM_LOG("rtsp server Destroy ....");
		g_rtsp_server.shutdown();
		g_rtsp_server.destroy();
		SYSTEM_LOG("rtsp server Destroy OK !!!");


		uv_util::destroy();
		SYSTEM_LOG("uv destroy OK !!!");
		//g_client_collection_mgr.destroy();
		//SYSTEM_LOG("g_client_collection_mgr Destroy OK !!!");

		//g_cloud_render_agent_mgr.destroy();
		SYSTEM_LOG(" cfg  destroy OK !!!");
		g_cfg.destroy();

		g_msg_dispatch.destroy();
		SYSTEM_LOG("msg dispath destroy OK !!!");

		csrtp_session::destroy();
		SYSTEM_LOG("srtp destroy OK !!!");

		g_global_rtc.destory();
		SYSTEM_LOG("global rtc destory OK !!!");

		g_global_config.destroy();
		SYSTEM_LOG("global config destroy OK !!!");
		 uv_util::destroy();

		g_global_rtc_port.destroy();
		g_web_http_api_mgr.destroy();
		SYSTEM_LOG("global rtc port config destroy ok !!!");
		//1 log
		LOG::destroy();
		printf("Log Destroy OK!\n");

		printf(" media rtc  server Destroy End\n");
	}

	void cmedia_server::stop()
	{
		m_server_intaval->Stop();
		m_stop = true;
	}

	void cmedia_server::OnTimer(ctimer * timer)
	{
		//static const uint32 TICK_TIME = 100;
		////启动内网并等待初始化完成
		if (timer == m_server_intaval)
		{
			static int64 prev_time_ms = uv_util::GetTimeMsInt64();

			uint32 tick_time = uv_util::GetTimeMsInt64() - prev_time_ms;

			//DEBUG_EX_LOG("   %u ms", tick_time);
			prev_time_ms = uv_util::GetTimeMsInt64();
			//ctime_elapse time_elapse;
			//uint32 uDelta = 0;
			if (!m_stop)
			{
				//uDelta += time_elapse.get_elapse();

				//	g_game_client.update(uDelta);
				g_wan_server.update(tick_time);


				g_room_mgr.update(tick_time);
				
				g_transport_mgr.update(tick_time);
				//uDelta = time_elapse.get_elapse();
				g_http_queue_mgr.update();
				/*if (uDelta < TICK_TIME)
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(TICK_TIME - uDelta));
				}*/
			}
			m_server_intaval->Start(100u);

		}
		//SYSTEM_LOG("Leave main loop");
	}

}