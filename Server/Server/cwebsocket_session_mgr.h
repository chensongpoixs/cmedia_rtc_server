/***********************************************************************************************
created: 		2022-08-09

author:			chensong

purpose:		cwebsocket_session_mgr


************************************************************************************************/
#ifndef _C_WEBSOCKET_SESSION_MGR_H_
#define _C_WEBSOCKET_SESSION_MGR_H_
#include "cnoncopyable.h"
#include "cwebsocket_session.h"
#include "cnet_type.h"
#include <unordered_map>
namespace chen {
	class cwebsocket_session_mgr : public cnoncopyable
	{
	private:
		typedef std::unordered_map<uint64, std::shared_ptr<session>>    WEBSOCKET_SESSION_MAP;
	public:
		explicit cwebsocket_session_mgr();
		virtual ~cwebsocket_session_mgr();
	
	public:
		bool	init(uint32 session_size);

		void    destroy();
	
		//cwebsocket_session* get_wait_connect_session();

		//cwebsocket_session* get_connecting_session(uint64 session_id);

		std::shared_ptr<session> get_connected_session(uint64 session_id);

		 bool	add_connected_session(uint64 session_id, std::shared_ptr<session> session_ptr);

		 bool delete_connected_session(uint64 session_id);
		
	private:
		
	/*	WEBSOCKET_SESSION_MAP							m_wait_connect_map;

		WEBSOCKET_SESSION_MAP							m_connecting_map;*/
		
		WEBSOCKET_SESSION_MAP							m_connected_map;
		

		//void* m_mem_ptr;



	};
	extern cwebsocket_session_mgr g_websocket_sesson_mgr;
}

#endif // _C_WEBSOCKET_SESSION_H_