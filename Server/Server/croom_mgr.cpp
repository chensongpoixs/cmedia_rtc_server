#include "croom_mgr.h"
/***********************************************************************************************
created: 		2022-08-10

author:			chensong

purpose:		room_mgr

Copyright boost
************************************************************************************************/
#include "croom_mgr.h"
#include "clog.h"
#include "croom.h"
namespace chen {
	croom_mgr g_room_mgr;
	croom_mgr::croom_mgr()
	{
	}
	croom_mgr::~croom_mgr()
	{
	}
	bool croom_mgr::init()
	{
		return true;
	}
	void croom_mgr::update(uint32 uDeltaTime)
	{
	}
	void croom_mgr::destory()
	{
	}

	bool croom_mgr::join_room(uint64 session_id, const std::string & room_name, const std::string & user_name)
	{
		CROOM_MAP::iterator iter =  m_room_map.find(room_name);
		if (iter == m_room_map.end()) 
		{
			croom room;
			room.init(room_name);
			if (!m_room_map.insert(std::make_pair(room_name, room)).second)
			{
				WARNING_EX_LOG("insert [room name = %s] failed !!!", room_name.c_str());
			}
			iter = m_room_map.find(room_name);
		}
		if (iter == m_room_map.end())
		{
			WARNING_EX_LOG("not find [room name = %s] [user_name  = %s]failed !!!", room_name.c_str(), user_name.c_str());
			return false;
		}
		
		return iter->second.join_userinfo(session_id, user_name);
	}

	bool croom_mgr::leave_room(uint64 session_id, const std::string & room_name)
	{
		CROOM_MAP::iterator iter = m_room_map.find(room_name);
		if (iter == m_room_map.end())
		{
			croom room;
			room.init(room_name);
			if (!m_room_map.insert(std::make_pair(room_name, room)).second)
			{
				WARNING_EX_LOG("insert [room name = %s] failed !!!", room_name.c_str());
			}
			iter = m_room_map.find(room_name);
		}
		if (iter == m_room_map.end())
		{
			WARNING_EX_LOG("not find [room name = %s]  failed !!!", room_name.c_str() );
			return false;
		}
		return iter->second.leave_userinfo(session_id);
	}

}