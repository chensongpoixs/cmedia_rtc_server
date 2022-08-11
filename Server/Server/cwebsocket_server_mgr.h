/***********************************************************************************************
created: 		2022-08-09

author:			chensong

purpose:		websocket_server_mgr
************************************************************************************************/
#ifndef _C_WEBSOCKET_SERVER_MGR_H
#define _C_WEBSOCKET_SERVER_MGR_H

#include "cnet_define.h"
#include <mutex>
//#include "cwebsocket_session.h"
#include "cwebsocket_msg_queue.h"
#include <unordered_map>
#include <memory>
namespace chen {
	class cwebsocket_session;
	class cwebsocket_server_mgr
	{

	public:
		//连接回调
		typedef std::function<void(uint64_t session_id,   const char* buf)>				cconnect_cb;
		//断开连接回调
		typedef std::function<void(uint64_t session_id)> 								cdisconnect_cb;
		//新消息回调
		typedef std::function<void(uint64_t session_id,  const void* p, uint32_t size)> cmsg_cb;
	private:
		typedef boost::asio::io_context														cservice;
		typedef boost::asio::ip::tcp::acceptor												cacceptor;
		typedef	boost::asio::ip::tcp::socket												csocket_type;
		typedef boost::asio::ip::tcp::endpoint												cendpoint;
		typedef boost::system::error_code													cerror_code;


	private:
		typedef std::atomic_bool 															catomic_bool;
		typedef std::vector<std::thread>													cthreads;
		typedef std::unordered_map<uint64_t, std::shared_ptr<cwebsocket_session>>			CWEBSOCKET_SESSION_MAP;

		typedef std::mutex															clock_type;
		typedef std::lock_guard<clock_type>											clock_guard;
	public:
		cwebsocket_server_mgr()
			: m_io_service()
			, m_acceptor_ptr(NULL)
			, m_socket(m_io_service)
			, m_shuting(false)
			, m_session_map() 
			, m_client_seeesion(100)
			, m_msgs(NULL)
		{}
		~cwebsocket_server_mgr();

	public:

		bool 		init();

		/**
		*  @param  thread_num io线程数
		*  @param  ip
		*  @param  port
		**/
		bool 		startup(uint32_t thread_num, const char *ip, uint16_t port);


		/**
		* 关服操作
		**/
		void 		shutdown();
		void 		destroy();
	public:

		void		msg_push(cwebsocket_msg * msg_ptr);

		void		post_disconnect(cwebsocket_session*  p);

	public:
		//连接回调
		void set_connect_callback(cconnect_cb callback) { m_connect_callback = callback; }

		//断开连接回调
		void set_disconnect_callback(cdisconnect_cb callback) { m_disconnect_callback = callback; }

		//消息回调
		void set_msg_callback(cmsg_cb callback) { m_msg_callback = callback; }


	public:
		void 		process_msg();

		//bool		transfer_msg(uint32 sessionId, cnet_msg & msg);

		/**
		*  @param  sessionId : 回话id
		*  @param  msg_id    : 消息id
		*  @param  msg_ptr   : 消息的数据
		*  @param  msg_size  : 消息的大小
		**/
		bool 		send_msg(uint64_t sessionId,   const void* msg_ptr, uint32_t msg_size);

		 /**
		*  @param sessionId  : 关闭指定的回话id
		**/
		void 		close(uint64_t sessionId);
		// 连接客户端的数量和数据 信息  
		void		show_info();
	private:
		bool _start_listen(const std::string & ip, uint16_t port, uint32_t thread_num);


		void _post_accept();

		void _handle_accept(const boost::system::error_code& ec);

		void _handle_close(cwebsocket_session*   session_ptr);
		void _worker_thread();
	private:
		cservice													m_io_service;
		cacceptor		*											m_acceptor_ptr;
		csocket_type													m_socket;


		// callback
		cconnect_cb													m_connect_callback;
		cmsg_cb														m_msg_callback;
		cdisconnect_cb												m_disconnect_callback;

		////
		catomic_bool												m_shuting;
		clock_type													m_session_mutex;
		CWEBSOCKET_SESSION_MAP										m_session_map;
		
		cthreads													m_threads;
		uint64_t													m_client_seeesion;
		cwebsocket_msg_queue *										m_msgs;
	};
}


#endif // _C_WEBSOCKET_SERVER_MGR_H