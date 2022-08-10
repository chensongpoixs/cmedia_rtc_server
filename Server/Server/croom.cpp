/***********************************************************************************************
created: 		2022-08-10

author:			chensong

purpose:		room

Copyright boost
************************************************************************************************/


#include "croom.h"
#include "clog.h"
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
		CUSERINFO_MAP::iterator iter = m_userinfo_map.find(session_id);
		if (iter != m_userinfo_map.end())
		{
			//WARNING_EX_LOG("[room_name = %s][session_id = %u][old user_name = %s][new username = %s]", m_room_name.c_str(), session_id, iter->second.username.c_str(), username.c_str());
			m_userinfo_map.erase(iter);
			return true;
		}
		WARNING_EX_LOG("[room_name = %s][session_id = %u] not find  failed !!!", m_room_name.c_str(), session_id );

		return true;
	}
}