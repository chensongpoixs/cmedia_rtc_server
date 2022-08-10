/***********************************************************************************************
created: 		2022-08-09

author:			chensong

purpose:		websocket_session
************************************************************************************************/

#ifndef _C_WEBSOCKET_SESSION_H
#define _C_WEBSOCKET_SESSION_H

#include <boost/beast/core.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/bind_executor.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <mutex>
//#include "cwebsocket_server_mgr.h"
namespace chen {
	class cwebsocket_server_mgr;

	class cwebsocket_session 
	{
	private:
		typedef boost::beast::websocket::stream<boost::asio::ip::tcp::socket>			cws;
		typedef boost::asio::strand< boost::asio::io_context::executor_type>			strand_;
		typedef std::mutex															    clock_type;
		typedef std::lock_guard<clock_type>											    clock_guard;
		typedef boost::beast::multi_buffer												     recv_buffer_;
		typedef std::list<std::string>													send_buffer_list;
		typedef std::atomic_bool 														catomic_bool;
	public:
		cwebsocket_session( cwebsocket_server_mgr * websocket_server,  uint64_t session_id, boost::asio::ip::tcp::socket  socket);
			
		~cwebsocket_session();


	public:
		void run();

		uint64_t get_session_id() { return m_session_id; }
		/**
		* 发送数据包缓存到发送缓冲区中
		* @param   msg_ptr	: 发送数据包
		* @param   size		: 发送数据包的大小
		**/
		bool send_msg(uint64_t session_id,  const void * msg_ptr, uint32_t msg_size);
		//void do_read();


		bool close(uint64_t session_id);
	private:
		void _handle_accept(boost::system::error_code ec);


		void _handle_read( boost::system::error_code ec, std::size_t bytes_transferred);

		void _handle_write( boost::system::error_code ec, std::size_t bytes_transferred);

		void _release();

		void _handle_close(boost::system::error_code ec);
	private:
		cwebsocket_server_mgr *								m_websocket_server_mgr_ptr;
		uint64_t											m_session_id;		// 网络层的会话id
		cws													m_ws;
		strand_												m_strand;
		unsigned char *										m_recv_buffer;
		uint64_t											m_recv_buffer_size;
		catomic_bool										m_writeing_atomic;
		clock_type											m_mutex;
		send_buffer_list									m_send_buffer_list;
		send_buffer_list									m_sending_buffer_list;

	};
}

#endif // _C_WEBSOCKET_SESSION_H