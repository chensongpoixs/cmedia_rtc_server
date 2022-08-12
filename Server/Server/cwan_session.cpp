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
namespace chen {
	cwan_session::cwan_session()
		: m_session_id(0)
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
	}
	void cwan_session::update(uint32 uDeltaTime)
	{
		//m_client_connect_type = EClientConnectNone;
	}
	void cwan_session::handler_msg(uint64_t session_id , const void* p, uint32 size)
	{
		// 1. 登录状态判断

		if (!m_json_reader.parse((const char *)p, (const char *)p + size, m_json_response))
		{
			ERROR_EX_LOG("parse json failed !!! [session_id = %llu][json = %s]", session_id, p);
			return;
		}
		if (!m_json_response.isMember("cmd"))
		{
			WARNING_EX_LOG("[session_id = %llu]not find cmd type !!!", session_id);
			return;
		}
		// TODO@chensong 20220812 管理的比较漏
		const std::string cmd = m_json_response["cmd"].asString();
		// "create_webrtc"
		if ("create_webrtc" == cmd)
		{

			g_global_webrtc_mgr.handler_create_webrtc(session_id, m_json_response);
		}
		else if ("destroy_webrtc" == cmd)
		{
			g_global_webrtc_mgr.handler_destroy_webrtc(session_id, m_json_response);
		}
		else
		{
			g_global_webrtc_mgr.handler_webrtc(session_id, m_json_response);
			

		}
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



	void    cwan_session::handler_login(const void* ptr, uint32 msg_size)
	{
		 

		
	}

	 
	 
}//namespace chen
