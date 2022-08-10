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
		struct cuser_info
		{
			uint64      session_id;
			std::string username;
			cuser_info()
				: session_id(0)
				, username(""){}
		};


		typedef std::map<uint64, cuser_info>						CUSERINFO_MAP;
	public:
		 croom();
		~croom();

	public:

		bool init(const std::string & room_name);

		bool join_userinfo(uint64 sesssion_id, const std::string & username);



		bool leave_userinfo(uint64 session_id);




	private:

		// 房间中广播消息

	private:
		//cnoncopyable(cnoncopyable&&);
		//croom(const croom&);
		//cnoncopyable &operator =(cnoncopyable &&);
		//croom& operator=(const croom&);
		

	private:
		std::string					m_room_name;
		//uint64						m_room_id;
		CUSERINFO_MAP               m_userinfo_map;
	};
}

#endif // _C_ROOM_H_
