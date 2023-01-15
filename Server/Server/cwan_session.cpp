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
#include "cshare_proto_error.h"
namespace chen {
	cwan_session::cwan_session()
		: m_session_id(0)
		, m_room_name("")
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
		m_room_name.clear();
		m_user_name.clear();
		m_session_id = 0;
		//m_rtc_sdp.destroy();
	}
	void cwan_session::update(uint32 uDeltaTime)
	{
		//m_client_connect_type = EClientConnectNone;
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

		g_room_mgr.leave_room(m_session_id, m_room_name);
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

	bool cwan_session::send_msg(uint16 msg_id, int32 result,  Json::Value  data)
	{
		Json::Value value;
		value["msg_id"] = msg_id;
		value["result"] = result;
		value["data"] = data;
		Json::StyledWriter swriter;
		std::string str = swriter.write(value);
		 g_wan_server.send_msg(m_session_id, msg_id, str.c_str(), str.length() );
		 return true;
	}




	 
	 
}//namespace chen
