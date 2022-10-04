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
#include <json/json.h>
#include "cwebrtc_transport.h"
namespace chen
{
	struct cuser_info
	{
		uint64      session_id;
		std::string username;
		std::unordered_map<std::string, cwebrtc_transport*>     producers;
		std::unordered_map<std::string, cwebrtc_transport*>     consumers;
		cuser_info()
			: session_id(0)
			, username("")
			, producers()
			, consumers() {}
	};
	class croom
	{
	public:
		


		typedef std::map<uint64, cuser_info>						CUSERINFO_MAP;
	public:
		 croom();
		~croom();

	public:

		bool init(const std::string & room_name);

		bool join_userinfo(uint64 sesssion_id, const std::string & username);



		bool leave_userinfo(uint64 session_id );

		bool create_webrtc(uint64 session_id, Json::Value& value);


		bool webrtc_message(uint64 session_id, Json::Value& value);

	private:

		// 房间中广播消息
		bool _broadcast_message(uint64 session_id,  uint32 msg_id, Json::Value & value);
	private:
		//cnoncopyable(cnoncopyable&&);
		//croom(const croom&);
		//cnoncopyable &operator =(cnoncopyable &&);
		//croom& operator=(const croom&);
		
		cuser_info* _get_user_info(uint64 session_id);

	private:
		std::string					m_room_name;
		//uint64						m_room_id;
		CUSERINFO_MAP               m_userinfo_map;
	};
}

#endif // _C_ROOM_H_
