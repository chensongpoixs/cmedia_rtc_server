/***********************************************************************************************
created: 		2019-04-30

author:			chensong

purpose:		wan_server

************************************************************************************************/

#ifndef _C_WAN_SERVER_H_
#define _C_WAN_SERVER_H_
#include "cnoncopyable.h"
#include "cnet_type.h"
//#include "cnet_mgr.h"
#include "cwebsocket_server_mgr.h"
#include "cwan_session.h"
 
namespace chen {
	class cwan_server : private cnoncopyable
	{
	public:
		cwan_server();
		~cwan_server();

	public:
		bool init();

		void destroy();
	public:
		bool startup();
	public:
		void update(uint32 uDeltaTime);
		void shutdown();
	public:
		void on_connect(uint64_t session_id,  const char* buf);
		void on_msg_receive(uint64_t session_id,   const void* p, uint32 size);
		void on_disconnect(uint64_t session_id);
	public:
		void send_msg(uint32 session_id, uint16 msg_id, const void *p, uint32 size);
	public:

		cwan_session* get_session(uint64 index);
		//cwan_session*  get_session(uint32 index, uint32 session_id);
	public:
		/**
		* 关闭指定连接
		* @param session_id	连接id
		*/
		void close(uint32 session_id);
	private:
		bool index_valid(uint32 index) { return index < m_max_session_num; }
		uint32 get_session_index(uint32 session_id) { return session_id & 0X0000FFFF; }
	private:
		cwebsocket_server_mgr	*				m_websocket_server_ptr;
		bool						m_stoped;
		uint32						m_max_session_num;
		 std::map<uint64,  cwan_session*>				m_session_map;
		uint32						m_update_timer;

	};
	extern cwan_server   g_wan_server;
}//namespace chen 
#endif // !#define _C_WAN_SERVER_H_