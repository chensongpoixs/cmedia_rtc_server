/***********************************************************************************************
created: 		2022-11-10

author:			chensong

purpose:		crtc_player
************************************************************************************************/
 
#include "crtc_player.h"
#include "clog.h"
#include "ccfg.h"
#include "json.hpp"

namespace chen 
{
	crtc_player		g_rtc_player;



	crtc_player::crtc_player()
		: m_stoped(false)
	{
	}
	crtc_player::~crtc_player()
	{
	}
	bool crtc_player::Init(const char * config_file)
	{

		if (!LOG::init(ELogStorageScreenFile))
		{
			std::cerr << " log init failed !!!";
			return false;
		}
		SYSTEM_LOG("Log init ...\n");

		bool init = g_cfg.init(config_file);
		if (!init)
		{
			//	//RTC_LOG(LS_ERROR) << "config init failed !!!" << config_name;
			ERROR_EX_LOG("config init failed !!! config_name = %s", config_file);
			return false;
		}
		SYSTEM_LOG("config init ok !!!");
		// set log level
		LOG::set_level(static_cast<ELogLevelType>(g_cfg.get_uint32(ECI_LogLevel)));

		SYSTEM_LOG("set level = %u", g_cfg.get_uint32(ECI_LogLevel));


		return true;
	}
	bool crtc_player::Loop()
	{
		static const uint32_t TICK_TIME = 100;

		m_stoped = false;
		std::string ws_url = "ws://" + g_cfg.get_string(ECI_RtcMediaIp) + ":" + std::to_string(g_cfg.get_uint32(ECI_RtcMediaPort)) + "/?roomId=" + g_cfg.get_string(ECI_RoomName) + "&peerId=" + g_cfg.get_string(ECI_ClientName);//ws://127.0.0.1:8888/?roomId=chensong&peerId=xiqhlyrn", "http://127.0.0.1:8888")
		std::chrono::steady_clock::time_point cur_time_ms;
		std::chrono::steady_clock::time_point pre_time = std::chrono::steady_clock::now();
		std::chrono::steady_clock::duration dur;
		std::chrono::milliseconds ms;
		uint32_t elapse = 0;
		m_player = new rtc::RefCountedObject<crtc_transport>( );
		std::list<std::string> msgs;
		while (!m_stoped)
		{
			pre_time = std::chrono::steady_clock::now();
			if (m_websocket_mgr.get_status() != CWEBSOCKET_MESSAGE)
			{
				if (!m_websocket_mgr.init(ws_url))
				{
					//RTC_LOG(LS_ERROR) << "weboscket connect failed !!! url = " << ws_url;
					WARNING_EX_LOG("weboscket connect url = %s failed !!!   ", ws_url.c_str());

					//continue;;
				}
				else
				{
					if (!m_websocket_mgr.startup())
					{

					}
					else
					{
						m_player->init(this);
					}
				}
				
			}
			if (m_websocket_mgr.get_status() == CWEBSOCKET_MESSAGE)
			{
				m_websocket_mgr.presssmsg(msgs);


				if (!msgs.empty() && m_websocket_mgr.get_status() == CWEBSOCKET_MESSAGE)
				{
					_presssmsg(msgs);
				}

			}

			if (!m_stoped)
			{
				cur_time_ms = std::chrono::steady_clock::now();
				dur = cur_time_ms - pre_time;
				ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur);
				elapse = static_cast<uint32_t>(ms.count());
				if (elapse < TICK_TIME)
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(TICK_TIME - elapse));
				}

			}

		}

		return true;
	}
	void crtc_player::Destroy()
	{
		g_cfg.destroy();

		SYSTEM_LOG("rtc player destroy ok !!!");
		LOG::destroy();
	}
	void crtc_player::stop()
	{
		m_stoped = true;
	}
	bool crtc_player::send_offer(const std::string & offer)
	{
		if (m_websocket_mgr.get_status() != CWEBSOCKET_MESSAGE)
		{
			WARNING_EX_LOG("websocket mgr status = %d !!!", m_websocket_mgr.get_status());
			return false;
		}
		nlohmann::json data = {
			{"offer", offer},
			{"roomname", g_cfg.get_string(ECI_RoomName)},
			{"video_peerid", g_cfg.get_string(ECI_ClientName)},
			{"peerid", std::to_string(rand()* 100000)}
		};
		nlohmann::json request_data =
		{
			{"msg_id", 1074},
			{"data" , data}
		};
		m_websocket_mgr.send(request_data.dump());
		return true;
	}
	void crtc_player::_presssmsg(std::list<std::string>& msgs)
	{
		nlohmann::json response;
		while (!msgs.empty() && m_websocket_mgr.get_status() == CWEBSOCKET_MESSAGE)
		{
			std::string msg = std::move(msgs.front());
			msgs.pop_front();
			NORMAL_EX_LOG("server ====> msg = %s", msg.c_str());

			try
			{
				response = nlohmann::json::parse(msg);
			}
			catch (const std::exception&)
			{
				ERROR_EX_LOG("websocket protoo [msg = %s] json parse failed !!!", msg.c_str());
				continue;
			}

			if (response.find("msg_id") != response.end())
			{
				auto msg_id_iter = response.find("msg_id");
				if (msg_id_iter == response.end())
				{
					WARNING_EX_LOG("notification websocket protoo not find msg_id  msg = %s", response.dump().c_str());
					continue;
				}

				if (response["result"] == 0)
				{
					m_player->set_remote_description(response["data"]["sdp"]);
					//m_rtc_publisher_ptr->set_remoter_description(response["data"]["sdp"]);
				}
				 

			}
			else
			{
				ERROR_EX_LOG(" not find msg_id !!! msg = %s", msg.c_str());
			}

			 
		}
	}
}

 