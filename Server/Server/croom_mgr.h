/***********************************************************************************************
created: 		2022-08-10

author:			chensong

purpose:		room_mgr

Copyright boost
************************************************************************************************/

#ifndef _C_ROOM_MGR_H_
#define _C_ROOM_MGR_H_
#include "cnet_type.h"
#include <string>
#include <unordered_map>
#include <json/json.h>
namespace chen {
	class croom;
	class croom_mgr 
	{
	private:
		typedef		std::unordered_map<std::string, croom>		CROOM_MAP;
	public:
		croom_mgr();
		~croom_mgr();


	public:



		std::unordered_map<std::string, uint32>   m_master;



		bool init();

		void update(uint32 uDeltaTime);

		void destory();

	public:
		// join room
		bool join_room(uint64 session_id, const std::string & room_name , const std::string & user_name);


		// leve room

		bool leave_room(uint64 session_id, const std::string & room_name);

		bool webrtc_message(const std::string &room_name, uint64  session_id, Json::Value & value);



	private:
		//cnoncopyable(cnoncopyable&&);
		croom_mgr(const croom_mgr&);
		//cnoncopyable &operator =(cnoncopyable &&);
		croom_mgr& operator=(const croom_mgr&);
	private:

		CROOM_MAP							m_room_map;
	};


	extern croom_mgr g_room_mgr;
}
#endif // _C_ROOM_MGR_H_