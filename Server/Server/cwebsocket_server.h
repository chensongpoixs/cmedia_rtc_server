/***********************************************************************************************
created: 		2022-08-09

author:			chensong

purpose:		cwebsocket_server


************************************************************************************************/
#ifndef _C_WEBSOCKET_SERVER_H_
#define _C_WEBSOCKET_SERVER_H_
#include "cnoncopyable.h"
#include "cnet_type.h"
#include "cwebsocket_session.h"
#include "cnet_define.h"
#include "cwebsocket_session_mgr.h"
namespace chen {
	class cwebsocket_server : public cnoncopyable
	{
	public:
		typedef boost::asio::io_context					cserver;
		typedef boost::asio::ip::tcp::acceptor			cacceptor;

	public:
		cwebsocket_server();
	  	~cwebsocket_server();

	public:
		bool init();

		void destroy();
	public:
		bool startup();
	public:
		void update(uint32 uDeltaTime);
		void shutdown();
	public:
		void on_connect(uint32 session_id, uint32 para, const char* buf);
		void on_msg_receive(uint32 session_id, uint16 msg_id, const void* p, uint32 size);
		void on_disconnect(uint32 session_id);
	public:
		void send_msg(uint32 session_id, uint16 msg_id, const void* p, uint32 size);
	public:

		cwebsocket_session* get_session(uint32 index);
		cwebsocket_session* get_session(uint32 index, uint32 session_id);
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
		cserver						m_server;
		//cacceptor					m_acceptor;
		bool						m_stoped;
		uint32						m_max_session_num;
		cwebsocket_session*			m_session_ptr;
		uint32						m_update_timer;

	};
	extern cwebsocket_server   g_websocket_server;




    // Accepts incoming connections and launches the sessions
    class listener : public std::enable_shared_from_this<listener>
    {
        boost::asio::io_context& ioc_;
        boost::asio::ip::tcp::acceptor acceptor_;
        uint64  m_client_sessoin = 100;
    public:
        listener(
            boost::asio::io_context& ioc,
            boost::asio::ip::tcp::endpoint endpoint)
            : ioc_(ioc)
            , acceptor_(ioc)
        {
            boost::beast::error_code ec;

            // Open the acceptor
            acceptor_.open(endpoint.protocol(), ec);
            if (ec)
            {
                fail(ec, "open");
                return;
            }

            // Allow address reuse
            acceptor_.set_option(boost::asio::socket_base::reuse_address(true), ec);
            if (ec)
            {
                fail(ec, "set_option");
                return;
            }
            DEBUG_EX_LOG("");
            // Bind to the server address
            acceptor_.bind(endpoint, ec);
            if (ec)
            {
                fail(ec, "bind");
                return;
            }

            // Start listening for connections
            acceptor_.listen(
                boost::asio::socket_base::max_listen_connections, ec);
            if (ec)
            {
                fail(ec, "listen");
                return;
            }
        }

        // Start accepting incoming connections
        void
            run()
        {
            do_accept();
        }

    private:
        void
            do_accept()
        {
            // The new connection gets its own strand
            acceptor_.async_accept(
                boost::asio::make_strand(ioc_),
                boost::beast::bind_front_handler(
                    &listener::on_accept,
                    shared_from_this()));
        }

        void
            on_accept(boost::beast::error_code ec, boost::asio::ip::tcp::socket socket)
        {
            if (ec)
            {
                fail(ec, "accept");
            }
            else
            {
                // Create the session and run it
                std::shared_ptr<session> session_ptr = std::make_shared<session>(std::move(socket));
                if (!session_ptr)
                {
                    ERROR_EX_LOG("->run();");
                }
                else
                {
                    session_ptr->run();
                    std::shared_ptr<session> old_session_ptr =  g_websocket_sesson_mgr.get_connected_session(m_client_sessoin);
                    if (old_session_ptr)
                    {
                        // TODO@chensong 这边需要处理哈
                        WARNING_EX_LOG("find old session id ");
                    }
                    else
                    {
                        g_websocket_sesson_mgr.add_connected_session(m_client_sessoin++, session_ptr);
                    }
                }
                
            }

            // Accept another connection
            do_accept();
        }
    };

}

#endif // _C_WEBSOCKET_SERVER_H_