/***********************************************************************************************
created: 		2022-08-09

author:			chensong

purpose:		cwebsocket_session
************************************************************************************************/

#include "cwebsocket_session.h"
#include "cwebsocket_msg_queue.h"
#include "cwebsocket_server_mgr.h"

#include <string>
#include "clog.h"
namespace chen {
	cwebsocket_session::cwebsocket_session(cwebsocket_server_mgr *  websocket_server, uint64_t session_id, boost::asio::ip::tcp::socket  socket)
		: m_websocket_server_mgr_ptr(websocket_server)
		, m_session_id(session_id)
		, m_ws(std::move(socket))
		, m_strand(m_ws.get_executor())
		, m_recv_buffer((unsigned char *)::malloc(sizeof(unsigned char *) * (1024 * 1024)))
		, m_recv_buffer_size(1024 * 1024)
		, m_writeing_atomic(false)
		, m_send_buffer_list()
		, m_sending_buffer_list()
	{}
	/*cwebsocket_session::cwebsocket_session(boost::asio::ip::tcp::socket socket)
	{
	}*/
	cwebsocket_session::~cwebsocket_session()
	{
		m_websocket_server_mgr_ptr = NULL;
		if (m_recv_buffer)
		{
			::free(   m_recv_buffer);
			m_recv_buffer = NULL;
		}
		m_sending_buffer_list.clear();
		m_send_buffer_list.clear();
	}
	void cwebsocket_session::run()
	{
		// Accept the websocket handshake
		m_ws.async_accept(
			boost::asio::bind_executor( m_strand, std::bind( &cwebsocket_session::_handle_accept, this, std::placeholders::_1)));
	}
	bool cwebsocket_session::send_msg(uint64_t session_id, const void * msg_ptr, uint32_t msg_size)
	{
		
		if (m_session_id != session_id/* || 0 == msg_id || ENSS_Open != m_status*/)
		{
			return false;
		}
		
		{
			//可能在异步发送数据的包
			clock_guard lock(m_mutex);
			//boost::asio::buffer bufferw(msg_ptr, msg_size);
			//std::string data(msg_ptr, msg_size);
 			m_send_buffer_list.emplace_back(std::string((char *)msg_ptr, msg_size));
		}

		bool write_bool = false;

		if (!m_writeing_atomic.compare_exchange_strong(write_bool, true))
		{
			return true;
		}
		{
			clock_guard lock(m_mutex);
			m_sending_buffer_list.swap(m_send_buffer_list);
		}
		m_ws.text(m_ws.got_text());
		m_ws.async_write(
			boost::asio::buffer( m_sending_buffer_list.front(), m_sending_buffer_list.front().size()),
			boost::asio::bind_executor(
				m_strand,
				std::bind(
					&cwebsocket_session::_handle_write,
					this,
					std::placeholders::_1,
					std::placeholders::_2)));
		return true;
	}
	bool cwebsocket_session::close(uint64_t session_id)
	{

		boost::system::error_code ec;
		//m_ws.async_close(boost::asio::ip::tcp::socket::shutdown_both, ec);
		//m_ws.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
		if (m_ws.is_open())
		{
			boost::beast::websocket::close_reason reason;
			m_ws.async_close(reason, std::bind(&cwebsocket_session::_handle_close, this, std::placeholders::_1));
			return true;
		}
		_release();
		return true;
	}
	void cwebsocket_session::_handle_accept(boost::system::error_code ec)
	{
		if (ec)
		{
			
			ERROR_EX_LOG("exit accept failed ec !!!z");
			_release();
			return;
		}

		// Read a message
		//boost::beast::multi_buffer buffer;
	
		m_ws.async_read(
			m_buffer, 
			boost::asio::bind_executor(
				m_strand,
				std::bind(
					&cwebsocket_session::_handle_read,
					this,
					std::placeholders::_1,
					std::placeholders::_2)));
	}
	 
	void cwebsocket_session::_handle_read(boost::system::error_code ec, std::size_t bytes_transferred)
	{

		boost::ignore_unused(bytes_transferred);

		// This indicates that the session was closed
		if (ec == boost::beast::websocket::error::closed)
		{
			_release();
			ERROR_EX_LOG("exit closed !!!");
			return;
		}

		if (ec)
		{
			ERROR_EX_LOG("exit read closed !!!");
			//fail(ec, "read")
			_release();
			return;
		}

		/*
		if (error || bytes_transferred == 0)
		{
			
			m_reading_atomic.store(false);
			//LOG_ERROR << "_handle_read =" << error.value();
			_close();
			return;
		}

		if (m_status != ENSS_Open)
		{
			m_reading_atomic.store(false);
			_close();
			return;
		}
		*/


		// 业务逻辑
		cwebsocket_msg * websocket_msg_ptr = new cwebsocket_msg();
		if (!websocket_msg_ptr)
		{
			// wrate
			//return;
		}
		else
		{
			//std::ostringstream cmd;
			//cmd << m_buffer;
			//boost::asio::mutable_buffer::mutable_buffer p = m_buffer.data();
			/*boost::beast::const_buffers_type<char> p = */// m_buffer.data();
			std::stringstream ss;
			ss << boost::beast::buffers(m_buffer.data());
			websocket_msg_ptr->set_buffer((unsigned char *)ss.str().c_str(), ss.str().length());
			websocket_msg_ptr->set_session_id(m_session_id);
			m_websocket_server_mgr_ptr->msg_push(websocket_msg_ptr);
		}
		
		//std::cout << m_buffer.member() << std::endl;;
		m_buffer.consume(m_buffer.size());
	//	m_ws.text(m_ws.got_text());
		/*m_ws.async_read_some(
			boost::asio::buffer(m_recv_buffer, m_recv_buffer_size),
			boost::asio::bind_executor(
				m_strand,
				std::bind(
					&cwebsocket_session::_handle_read,
					this,
					std::placeholders::_1,
					std::placeholders::_2)));*/
		m_ws.async_read (
			m_buffer,
			boost::asio::bind_executor(
				m_strand,
				std::bind(
					&cwebsocket_session::_handle_read,
					this,
					std::placeholders::_1,
					std::placeholders::_2)));
			// Echo the message
	/*	ws_.text(ws_.got_text());
		ws_.async_write(
			buffer_.data(),
			boost::asio::bind_executor(
				strand_,
				std::bind(
					&session::on_write,
					shared_from_this(),
					std::placeholders::_1,
					std::placeholders::_2)));*/

	}
	void cwebsocket_session::_handle_write(boost::system::error_code ec, std::size_t bytes_transferred)
	{
		boost::ignore_unused(bytes_transferred);

		if (ec)
		{
			//return fail(ec, "write");
			m_sending_buffer_list.clear();
			//m_send_buffer_list.clear();
			_release();
			return;
		}

		/*if (error || bytes_transferred == 0)
		{
			m_writeing_atomic.store(false);
			_close();
			return;
		}

		if (m_status != ENSS_Open)
		{
			m_writeing_atomic.store(false);
			_close();
			return;
		}*/
		if (m_sending_buffer_list.size() > 1)
		{
			m_sending_buffer_list.pop_front();
			m_ws.text(m_ws.got_text());
			m_ws.async_write(
				boost::asio::buffer(m_sending_buffer_list.front(), m_sending_buffer_list.front().size()),
				boost::asio::bind_executor(
					m_strand,
					std::bind(
						&cwebsocket_session::_handle_write,
						this,
						std::placeholders::_1,
						std::placeholders::_2)));
		}
		else if (!m_sending_buffer_list.empty() && !m_send_buffer_list.empty())
		{
			m_sending_buffer_list.pop_front();
			
			{
				clock_guard lock(m_mutex);
				m_sending_buffer_list.swap(m_send_buffer_list);
			}
			m_ws.text(m_ws.got_text());
			m_ws.async_write(
				boost::asio::buffer(m_sending_buffer_list.front(), m_sending_buffer_list.front().size()),
				boost::asio::bind_executor(
					m_strand,
					std::bind(
						&cwebsocket_session::_handle_write,
						this,
						std::placeholders::_1,
						std::placeholders::_2)));

		}
		else if (!m_sending_buffer_list.empty())
		{
			m_sending_buffer_list.pop_front();
			m_writeing_atomic.store(false);
		}
		else
		{
			m_writeing_atomic.store(false);
		}
		// Clear the buffer
		//buffer_.consume(buffer_.size());

		// Do another read
		//do_read();
	}
	void cwebsocket_session::_release()
	{
		m_session_id = 0;
		//boost::asio::ip::tcp::socket::shutdown_both
		if (m_ws.is_open())
		{
			boost::system::error_code ec;
			//m_ws.async_close(  ec);
			boost::beast::websocket::close_reason reason;
			m_ws.async_close(reason, std::bind( &cwebsocket_session::_handle_close, this, std::placeholders::_1 ) );
				return;
		}
		 m_websocket_server_mgr_ptr->post_disconnect(this);;
	}
	void cwebsocket_session::_handle_close(boost::system::error_code ec)
	{
		m_session_id = 0;
		m_websocket_server_mgr_ptr->post_disconnect(this);;
	}
}

 