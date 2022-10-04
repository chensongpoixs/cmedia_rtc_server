/***********************************************************************************************
created: 		2022-08-10

author:			chensong

purpose:		room

Copyright boost
************************************************************************************************/


#include "croom.h"
#include "clog.h"
#include "cwan_server.h"
#include "cmsg_base_id.h"
#include "ccrypto_random.h"
namespace chen {
	croom::croom()
		: m_room_name("")
	{
	}
	croom::~croom()
	{
	}
	bool croom::init(const std::string & room_name)
	{
		m_room_name = room_name;
		return true;
	}
	bool croom::join_userinfo(uint64 session_id, const std::string & username)
	{
		Json::Value reply;
		reply["username"] = username;
		_broadcast_message(session_id, S2C_JoinRoomUpdate, reply);
		CUSERINFO_MAP::iterator iter =   m_userinfo_map.find(session_id);
		if (iter != m_userinfo_map.end())
		{
			WARNING_EX_LOG("[room_name = %s][session_id = %u][old user_name = %s][new username = %s]", m_room_name.c_str(), session_id, iter->second.username.c_str(), username.c_str()  );
			iter->second.username = username;
			return true;
		}

		cuser_info userinfo;
		userinfo.session_id = session_id;
		userinfo.username = username;
		if (!m_userinfo_map.insert(std::make_pair(session_id, userinfo)).second)
		{
			ERROR_EX_LOG("[room_name = %s][session_id = %u][ user_name = %s] insert failed !!!", m_room_name.c_str(), session_id, username.c_str());
			return false;
		}
			return true;
	}
	bool croom::leave_userinfo(uint64 session_id)
	{
		Json::Value reply;
		
		CUSERINFO_MAP::iterator iter = m_userinfo_map.find(session_id);
		if (iter != m_userinfo_map.end())
		{
			reply["username"] = iter->second.username;
			_broadcast_message(session_id, S2C_LevalRoomUpdate, reply);
			//WARNING_EX_LOG("[room_name = %s][session_id = %u][old user_name = %s][new username = %s]", m_room_name.c_str(), session_id, iter->second.username.c_str(), username.c_str());
			m_userinfo_map.erase(iter);
			return true;
		}
		WARNING_EX_LOG("[room_name = %s][session_id = %u] not find  failed !!!", m_room_name.c_str(), session_id );
		reply["username"] = "";
		_broadcast_message(session_id, S2C_LevalRoomUpdate, reply);
		return true;
	}
	bool croom::create_webrtc(uint64 session_id, Json::Value & value)
	{
		cuser_info* user_info_ptr = _get_user_info(session_id);
		if (!user_info_ptr)
		{
			ERROR_EX_LOG("not find session_id = %u", session_id);
			return false;
		}


		std::string transportId = s_crypto_random.GetRandomString(20);
		cwebrtc_transport* transport_ptr = new cwebrtc_transport();
		if (!transport_ptr)
		{
			ERROR_EX_LOG("webrtc transport alloc  failed !!!");
			return false;
		}
		if (!transport_ptr->init(transportId))
		{
			delete transport_ptr;
			transport_ptr = NULL;
			ERROR_EX_LOG("webrtc transport init failed !!!");
			return false;
		}

		user_info_ptr->producers[transportId] = transport_ptr;

		return true;
	}
	bool croom::webrtc_message(uint64 session_id, Json::Value & value)
	{
		return _broadcast_message(session_id, S2C_WebrtcMessageUpdate , value);
	}
	bool croom::_broadcast_message(uint64 session_id, uint32 msg_id, Json::Value & value)
	{
		for (const std::pair<uint64, const cuser_info> &pi : m_userinfo_map)
		{
			if (pi.first != session_id)
			{
				// 
				Json::Value reply;
				reply["msg_id"] = msg_id;
				reply["data"] = value;
				Json::StyledWriter swriter;
				std::string str = swriter.write(reply);
				NORMAL_EX_LOG("[join room = %s][username = %s][message = %s]", m_room_name.c_str(), pi.second.username.c_str(), str.c_str());
				//g_wan_server.send_msg(m_session_id, msg_id, str.c_str(), str.length());
				g_wan_server.send_msg(pi.first, msg_id, str.c_str(), str.length());
			}
		}
		return true;
	}
	cuser_info * croom::_get_user_info(uint64 session_id)
	{
		CUSERINFO_MAP::iterator iter = m_userinfo_map.find(session_id);
		if (iter != m_userinfo_map.end())
		{
			return NULL;
		}
		return &iter->second;
	}
}