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
#include "cglobal_rtc.h"
namespace chen {

	void cwan_session::handler_msg(uint64_t session_id, const void* p, uint32 size)
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

		if ((msg_id < S2C_WebrtcMessageUpdate) || (msg_id < Msg_Client_Max && msg_id > S2C_LevalRoomUpdate))
		{

			cclient_msg_handler * client_msg_handler = g_client_msg_dispatch.get_msg_handler(msg_id);
			if (!client_msg_handler || !client_msg_handler->handler)
			{
				WARNING_EX_LOG("[session_id = %llu]not find msg_id= %u callback !!!", session_id, msg_id);
				return;
			}
			m_session_id = session_id;
			++client_msg_handler->handle_count;
			(this->*(client_msg_handler->handler))(m_json_response);

		}
		else if (msg_id < C2S_CreateRtc)
		{
			Json::Value reply;
			if (!m_json_response["data"].isMember("room_name") || !m_json_response["data"]["room_name"].isString())
			{
				WARNING_EX_LOG("[session_id = %llu]not find room_name  , [value = %s] !!! ", m_session_id, m_json_response.asCString());
				send_msg(msg_id+1, EShareProtoRoomName, reply);
				return  ;
			}
			cmsg_handler *msg_handler = g_msg_dispatch.get_msg_handler(msg_id);
			if (!msg_handler || !msg_handler->handler)
			{
				
				WARNING_EX_LOG("[session_id = %llu]not find msg_id= %u callback !!!", session_id, msg_id);
				return;
			}
			++msg_handler->handle_count;

			g_room_mgr.join_room(session_id,  m_json_response["data"]["room_name"].asCString(), m_user_name);


			//(this->*(msg_handler->handler))(session_id, m_json_response);
		} 
		else
		{
			WARNING_EX_LOG("not find [msg_id = %u][msg = %s]", msg_id, p);
		}

		 
	}
	bool    cwan_session::handler_login(Json::Value & value)
	{
		Json::Value reply;
		if (!value.isMember("data") || !value["data"].isObject())
		{
			WARNING_EX_LOG("[session_id = %llu]not find cmd type, [value = %s] !!! ", m_session_id, value.asCString());
			send_msg(S2C_Login, EShareProtoData, reply);
			return false;
		}
		if (!value["data"].isMember("user_name") || !value["data"]["user_name"].isString())
		{
			WARNING_EX_LOG("[session_id = %llu]not find user_name      !!! ", m_session_id );
			send_msg(S2C_Login, EShareProtoUserName, reply);
			return false;
		}
		if (!value["data"].isMember("room_name") || !value["data"]["room_name"].isString())
		{
			WARNING_EX_LOG("[session_id = %llu]not find room_name    !!! ", m_session_id );
			send_msg(S2C_Login, EShareProtoRoomName, reply);
			return false;
		}
		m_user_name = value["data"]["user_name"].asCString();
		m_room_name = value["data"]["room_name"].asCString();
		if (!g_room_mgr.join_room(m_session_id, m_room_name, m_user_name))
		{
			//EShareProtoCreateRoomFailed
			WARNING_EX_LOG("[session_id = %llu] create roomname failed   !!! ", m_session_id );
			send_msg(S2C_Login, EShareProtoCreateRoomFailed, reply);
			return false;
		}
		//NORMAL_EX_LOG("[sdp = %s]", value["data"]["sdp"].asCString());
		//if (!m_rtc_sdp.init(value["data"]["sdp"].asCString()))
		//{
		//	WARNING_EX_LOG("rtc session_id = %lu, sdp = %s", value["data"]["sdp"].asCString());
		//	//EShareProtoParseSdpFailed
		//	send_msg(S2C_Login, EShareProtoParseSdpFailed, reply);
		//	return false;
		//}

		m_client_connect_type = EClientConnectSession;

		NORMAL_EX_LOG("%s Login OK!!", m_user_name.c_str());
		send_msg(S2C_Login, EShareProtoOk, reply);
		 send_msg(S2C_RtpCapabilitiesUpdate, 0, g_global_rtc.all_rtp_capabilities());
		return true;
	}



	bool	cwan_session::handler_join_room(Json::Value &value)
	{
		 
		return true;
	}
	bool	cwan_session::handler_destroy_room(Json::Value& value)
	{
		return true;
	}

	bool	cwan_session::handler_webrtc_message(Json::Value& value)
	{
		Json::Value reply;
		if (!value.isMember("data") || !value["data"].isObject())
		{
			WARNING_EX_LOG("[session_id = %llu]not find cmd type, [value = %s] !!! ", m_session_id, value.asCString());
			send_msg(S2C_WebrtcMessage, EShareProtoData, reply);
			return false;
		}
		g_room_mgr.webrtc_message(m_room_name, m_session_id, value["data"]);
		send_msg(S2C_WebrtcMessage, EShareProtoData, reply);
		/*Json::Value reply;
		if (!value.isMember("data") || !value["data"].isObject())
		{
			WARNING_EX_LOG("[session_id = %llu]not find cmd type, [value = %s] !!! ", m_session_id, value.asCString());
			send_msg(S2C_WebrtcMessage, EShareProtoData, reply);
			return false;
		}*/
		/*if (!value["data"].isMember("type") || !value["data"]["type"].isString())
		{
			WARNING_EX_LOG("[session_id = %llu]not find type  , [value = %s] !!! ", m_session_id, value.asCString());
			send_msg(S2C_WebrtcMessage, EShareProtoUserName, reply);
			return false;
		}*/
		//if (!value["data"].isMember("room_name") || !value["data"]["room_name"].isString())
		/*{
			WARNING_EX_LOG("[session_id = %llu]not find room_name  , [value = %s] !!! ", m_session_id, value.asCString());
			send_msg(S2C_Login, EShareProtoRoomName, reply);
			return false;
		}*/


		return true;
	}
	/*bool	cwan_session::handler_create_answer(Json::Value& value)
	{
		return true;
	}

	bool	cwan_session::handler_candidate(Json::Value& value)
	{
		return true;
	}*/
	/*bool cwan_session::handler_create_room(Json::Value & value)
	{

		return true;
	}

	bool cwan_session::handler_destroy_room(Json::Value & value)
	{
		return true;
	}*/



	bool   cwan_session::handler_create_rtc(Json::Value& value)
	{
		g_global_webrtc_mgr.handler_create_webrtc(m_session_id, value);
		return true;
	}

	bool   cwan_session::handler_connect_rtc(Json::Value& value)
	{
		g_global_webrtc_mgr.handler_connect_webrtc(m_session_id, value);
		return true;
	}
	bool   cwan_session::handler_rtc_produce(Json::Value& value)
	{
		return g_global_webrtc_mgr.handler_webrtc_produce(m_session_id, value);;
	}
}