/********************************************************************
created:	2019-05-07

author:		chensong

level:		网络层

purpose:	网络数据的收发
*********************************************************************/
#include "cwan_session.h"

#include "clog.h"
#include "cmsg_dispatch.h"
#include <json/json.h>
#include "cwan_server.h"
#include "croom_mgr.h"
#include "cclient_msg_dispatch.h"
#include "cmsg_dispatch.h"
namespace chen {
	cwan_session::cwan_session()
		: m_session_id(0)
		, m_user_name("")
		, m_client_connect_type(EClientConnectNone)
	{
		
	}
	cwan_session::~cwan_session()
	{
		//WARNING_EX_LOG("");
		 
	}
	bool cwan_session::init()
	{
		//WARNING_EX_LOG("");
			m_client_connect_type = EClientConnectNone;
			return true;
	}
	void cwan_session::destroy()
	{
		//WARNING_EX_LOG("");
		m_client_connect_type = EClientConnectNone;
		m_user_name.clear();
		m_session_id = 0;
		m_rtc_sdp.destroy();
	}
	void cwan_session::update(uint32 uDeltaTime)
	{
		//m_client_connect_type = EClientConnectNone;
	}
	void cwan_session::handler_msg(uint64_t session_id , const void* p, uint32 size)
	{
		// 1. 登录状态判断
		NORMAL_EX_LOG("[session_id = %u][p = %s]", session_id, p);
		if (!m_json_reader.parse((const char *)p, (const char *)p + size, m_json_response))
		{
			ERROR_EX_LOG("parse json failed !!! [session_id = %llu][json = %s]", session_id, p);
			return;
		}
		if (!m_json_response.isMember("msg_id"))
		{
			WARNING_EX_LOG("[session_id = %llu]not find cmd type !!!", session_id);
			return;
		}
		// TODO@chensong 20220812 管理的比较漏
		const uint32 msg_id = m_json_response["msg_id"].asUInt();

		if (msg_id < S2S_DestroyRoom)
		{
			cclient_msg_handler * client_msg_handler = g_client_msg_dispatch.get_msg_handler(msg_id);
			if (!client_msg_handler)
			{
				WARNING_EX_LOG("[session_id = %llu]not find msg_id= %u callback !!!", session_id, msg_id);
				return;
			}
			++client_msg_handler->handle_count;
			(this->*(client_msg_handler->handler))(m_json_response);

		}
		else if (msg_id < Msg_Client_Max)
		{
			//cmsg_handler *msg_handler = g_msg_dispatch.get_msg_handler(msg_id);
			//++msg_handler->handle_count;
			//(this->*(msg_handler->handler))(session_id, m_json_response);
		}
		else
		{
			WARNING_EX_LOG("not find [msg_id = %u][msg = %s]", msg_id, p);
		}
		
		// "create_webrtc"
		//if ( C2S_Login == msg_id)
		//{
		//	m_session_id = session_id;
		//	//g_global_webrtc_mgr.handler_create_webrtc(session_id, m_json_response);
		//}
		//else if (C2S_destroy_room == msg_id)
		//{
		//	//g_global_webrtc_mgr.handler_destroy_webrtc(session_id, m_json_response);
		//}
		//else
		//{
		////	g_global_webrtc_mgr.handler_webrtc(session_id, m_json_response);
		//	

		//}
		// "destroy_webrtc"


		//cmsg_handler* handler =  g_msg_dispatch.get_msg_handler();
		/*M_MSG_MAP::iterator iter = m_msg_map.find(msg_id);
		if (iter == m_msg_map.end())
		{
			g_wan_server.send_msg(m_session_id, msg_id, p, size);

			WARNING_EX_LOG("not find msg_id = %u", msg_id);
			return;
		}
		(this->*(iter->second))(p, size);*/
	}
	void cwan_session::close()
	{
		//WARNING_EX_LOG("");
		m_client_connect_type = EClientConnectNone;
		m_room_list.clear();
	}
	bool cwan_session::is_used()
	{
		//DEBUG_LOG("m_session_id = %u, m_client_session = %lu, m_client_connect_type = %u", m_session_id, m_client_session, m_client_connect_type);
		return m_client_connect_type > EClientConnectNone;
	}

	void cwan_session::set_used()
	{
		m_client_connect_type = EClientConnected;
	}

	void cwan_session::disconnect()
	{
		m_client_connect_type = EClientConnectNone;



		// 退出房间
		for (const std::string & room_name : m_room_list)
		{
			g_room_mgr.leave_room(m_session_id, room_name);
		}

		/*if (!g_client_collection_mgr.client_remove_session_collection(m_client_session))
		{
			ERROR_EX_LOG("client collection remove session_id = %lu failed !!!", m_client_session);
		}
		else
		{
			NORMAL_LOG("client collection remove session_id = %lu ok ", m_client_session);
		}*/


		/*collection* collection_ptr = g_client_collection_mgr.get_client_session_collection(m_client_session);
		if (!collection_ptr)
		{
			 return;
		}
		collection_ptr->set_status(ECollection_Init);*/
	}

	bool cwan_session::send_msg(uint16 msg_id, const std::string & data)
	{
		 g_wan_server.send_msg(m_session_id, msg_id, data.c_str(), data.length() );
		 return true;
	}



	bool    cwan_session::handler_login(  Json::Value & value)
	{
		if (!value.isMember("data") || !value["data"].isObject())
		{
			WARNING_EX_LOG("[session_id = %llu]not find cmd type, [value = %s] !!! ", m_session_id, value.asCString());
			return false;
		}  
		if (!value["data"].isMember("user_name") || !value["data"]["user_name"].isString())
		{
			WARNING_EX_LOG("[session_id = %llu]not find user_name  , [value = %s] !!! ", m_session_id, value.asCString());
			return false;
		}
		if (!value["data"].isMember("sdp") || !value["data"]["sdp"].isString())
		{
			WARNING_EX_LOG("[session_id = %llu]not find sdp  , [value = %s] !!! ", m_session_id, value.asCString());
			return false;
		}
		m_user_name = value["data"]["user_name"].asCString();
		NORMAL_EX_LOG("[sdp = %s]", value["data"]["sdp"].asCString());
		if (!m_rtc_sdp.init(value["data"]["sdp"].asCString()))
		{
			WARNING_EX_LOG("rtc session_id = %lu, sdp = %s", value["data"]["sdp"].asCString());
			return false ;
		}
		
		m_client_connect_type = EClientConnectSession;

		NORMAL_EX_LOG("sdp = %s", m_rtc_sdp.get_webrtc_sdp().c_str());


		return true;
	}

	bool cwan_session::handler_create_room(Json::Value & value)
	{
		return true;
	}

	bool cwan_session::handler_destroy_room(Json::Value & value)
	{
		return true;
	}

	 
	 
}//namespace chen
