/***********************************************************************************************
created: 		2022-08-10

author:			chensong

purpose:		room

Copyright boost
************************************************************************************************/


#include "croom.h"
#include "clog.h"
namespace chen {
	croom::croom(const std::string &room_name)
		: m_room_name(room_name)
	{
	}
	croom::~croom()
	{
	}
	bool croom::add_userinfo(uint64 sesssion_id, const std::string & username)
	{
		CUserInfo_MAP::iterator iter =   m_userinfo_map.find(sesssion_id);
		if (iter != m_userinfo_map.end())
		{
			
			iter->second.username = username;
			return true;
		}
		return true;
	}
	bool croom::remove_userinfo(uint64 session_id)
	{
		return false;
	}
}