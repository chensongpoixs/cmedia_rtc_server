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



	static const uint32_t TICK_TIME = 200;

	cclient::cclient()
		:m_id(100000)
		,m_loaded(false)
		, m_stoped(false)
		, m_peer_connection_ptr(NULL)
		, m_media_session_stats(EMedia_Session_Node)
		, m_offer_sdp("")
		{}
	cclient::~cclient(){}


	
	 
	bool cclient::init(uint32 gpu_index)
	{
		//m_peer_connection_ptr->init();
		
		return true;
	}
	void cclient::stop()
	{
		m_stoped = true;
	}
	void cclient::Loop(/*const std::string & mediasoupIp, uint16_t port, const std::string & roomName, const std::string & clientName
	,	uint32_t websocket_reconnect_waittime*/)
	{
		//new rtc::RefCountedObject<Conductor>(&client, &wnd));
		m_peer_connection_ptr = new rtc::RefCountedObject<cpeer_connection>(this);
		if (!m_peer_connection_ptr)
		{
			ERROR_EX_LOG("create peer connect ptr == null !!!");
			return;
		}

		if (!m_peer_connection_ptr->init())
		{
			WARNING_EX_LOG("peer connection object init failed !!!");
			return;
		}
		std::string ws_url = "ws://" + g_cfg.get_string(ECI_WebSocketHost)+":" + std::to_string(g_cfg.get_uint32(ECI_WebSocketPort));
		std::list<std::string> msgs;
		while (!m_stoped)
		{
			switch (m_media_session_stats)
			{
			case chen::EMedia_Session_Node: 
			case chen::EMedia_Session_connecting: 
			case chen::EMedia_Session_Connected: 
			{
				if (!m_websocket_mgr.init(ws_url))
				{
					WARNING_EX_LOG("weboscket connect url = %s failed !!!   ", ws_url.c_str());
					/*m_status = EMediasoup_Reset;
					++m_websocket_timer;*/
					/*if (m_websocket_timer > g_cfg.get_uint32(ECI_WebSocketTimers))
					{
					_mediasoup_status_callback(EMediasoup_WebSocket_Init, 1);
					}*/
					continue;;
				}

				if (!m_websocket_mgr.startup())
				{
					//	m_status = EMediasoup_Reset;
					continue;
				}
				m_media_session_stats = EMedia_Login;
				break;
			}
			case chen::EMedia_Login:
			{
				//if (!m_offer_sdp.empty())
				{
					// send fonsg 
					nlohmann::json data =
					{
						//{"sdp",m_offer_sdp},
						{"room_name", g_cfg.get_string(ECI_Room_Name)},
						{"user_name", g_cfg.get_string(ECI_Client_Name)}
					};
					if (!_send_request_media(C2S_Login, data))
					{
						m_media_session_stats = EMedia_Session_Node;
						WARNING_EX_LOG("send login failed !!!");
						continue;
					}
					m_media_session_stats = EMedia_Loading;
				}
				
				break;
			}
			case chen::EMedia_Loading:
			{
				break;
			}
			case chen::EMedia_Gameing:
			{
				break;
			}
			//case EMedia_CreateRoom:
			//{
			//	// send fonsg 
			//	nlohmann::json data =
			//	{
			//		//{"sdp",m_offer_sdp},
			//		//{"room_name", g_cfg.get_string(ECI_Room_Name)},
			//		{"room_name", g_cfg.get_string(ECI_Room_Name)}
			//	};
			//	if (!_send_request_media(C2S_CreateRoom, data))
			//	{
			//		m_media_session_stats = EMedia_Session_Node;
			//		WARNING_EX_LOG("send login failed !!!");
			//		continue;
			//	}
			//	NORMAL_EX_LOG("create room send  room_name = %s", g_cfg.get_string(ECI_Room_Name).c_str());
			//	m_media_session_stats = EMedia_CreateRooming;
			//	break;
			//}
			//case	EMedia_CreateRooming:
			//{
			//	break;
			//}
			case	EMedia_CreateVideo:
			{
				if (!m_offer_sdp.empty())
				{
					// send fonsg 
					nlohmann::json data =
					{
						{"sdp",m_offer_sdp}/*,
						{"room_name", g_cfg.get_string(ECI_Room_Name)},
						{"user_name", g_cfg.get_string(ECI_Client_Name)}*/
					};
					if (!_send_request_media(C2S_RtcConnect, data))
					{
						m_media_session_stats = EMedia_Session_Node;
						WARNING_EX_LOG("send login failed !!!");
						continue;
					}
					m_media_session_stats = EMedia_CreateVideoing;
				} 
				break;
			}
			case	EMedia_CreateVideoing:
			{
				break;
			}
			default:
			{
				break;
			}
			}
			m_websocket_mgr.presssmsg(msgs);


			if (!msgs.empty() && m_websocket_mgr.get_status() == CWEBSOCKET_MESSAGE)
			{
				_presssmsg(msgs);
			}

			if (m_websocket_mgr.get_status() != CWEBSOCKET_MESSAGE)
			{
				m_media_session_stats = EMedia_Session_Node;
				msgs.clear();
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			
		}
	}
	 
	void cclient::Destory()
	{
		if (m_peer_connection_ptr)
		{
			m_peer_connection_ptr = NULL;
		}
	  
	}
	 
	
	 

	bool cclient::_send_request_media(uint32 msg_id, const nlohmann::json & data)
	{
		if (m_websocket_mgr.get_status() != CWEBSOCKET_MESSAGE)
		{
			WARNING_EX_LOG("websocket mgr status = %d !!!", m_websocket_mgr.get_status());
			return false;
		}
		nlohmann::json request_data =
		{
			//{""  ,true},
			{"msg_id" , msg_id}, //  
			{"data" , data}, //方法
		 
		};
		m_websocket_mgr.send(request_data.dump());
		return true;
	}

	void cclient::_presssmsg(std::list<std::string>& msgs)
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
				NORMAL_EX_LOG("[data = %s]", msg.c_str());
				uint16 msg_id = response["msg_id"].get<uint16>();
				cmsg_handler* msg_handler =  g_msg_dispatch.get_msg_handler(msg_id);
				if (!msg_handler || !msg_handler->handler)
				{
					WARNING_EX_LOG("not find msg_id = %u  or callback !!!", msg_id);
					continue;
				}

				++msg_handler->handle_count;
				(this->*(msg_handler->handler))(response);
			}
			else
			{
				WARNING_EX_LOG("not find msg_id [data = %s]", msg.c_str());
			}
			 
		}
	}

	
	

	
}