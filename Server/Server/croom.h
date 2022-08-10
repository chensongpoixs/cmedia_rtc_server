/***********************************************************************************************
created: 		2022-08-10

author:			chensong

purpose:		room

Copyright boost
************************************************************************************************/

#ifndef _C_ROOM_H_
#define _C_ROOM_H_
#include <string>
#include "cnet_type.h"
#include <map>
namespace chen
{
	class croom
	{
	private:
		struct UserInfo
		{
			uint64      sesssion_id;
			std::string username;
			UserInfo()
				: sesssion_id(0)
				, username(""){}
		};


		typedef std::map<uint64, UserInfo>						CUserInfo_MAP;
	public:
		explicit croom(const std::string &room_name);
		~croom();

	public:



		bool add_userinfo(uint64 sesssion_id, const std::string & username);



		bool remove_userinfo(uint64 session_id);




	private:



		

	private:
		std::string					m_room_name;
		//uint64						m_room_id;
		CUserInfo_MAP               m_userinfo_map;
	};
}

#endif // _C_ROOM_H_
