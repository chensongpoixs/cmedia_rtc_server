/***********************************************************************************************
created: 		2022-08-09

author:			chensong

purpose:		cwebsocket_session


************************************************************************************************/
#ifndef _C_WEBSOCKET_SESSION_H_
#define _C_WEBSOCKET_SESSION_H_
#include "cnet_type.h"
#include "cnoncopyable.h"

#include "cnet_define.h"
#include <iostream>
#include "clog.h"

namespace chen {


	// Report a failure
	void fail(boost::beast::error_code ec, char const* what)
	{
		std::cerr << what << ": " << ec.message() << "\n";
	}
	class cwebsocket_session : public cnoncopyable
	{
	public:
		explicit cwebsocket_session(uint32 session_id);
		virtual ~cwebsocket_session();
	public:
		uint32			m_session_id;
	};


    // Echoes back all received WebSocket messages
    class session : public std::enable_shared_from_this<session>
    {
        boost::beast::websocket::stream<boost::beast::tcp_stream> ws_;
        boost::beast::flat_buffer read_buffer_;
        boost::beast::flat_buffer write_buffer;


    public:
        // Take ownership of the socket
        explicit
            session(boost::asio::ip::tcp::socket&& socket)
            : ws_(std::move(socket))
        {
        }

        // Get on the correct executor
        void
            run()
        {
            // We need to be executing within a strand to perform async operations
            // on the I/O objects in this session. Although not strictly necessary
            // for single-threaded contexts, this example code is written to be
            // thread-safe by default.
            boost::asio::dispatch(ws_.get_executor(),
                boost::beast::bind_front_handler(
                    &session::on_run,
                    shared_from_this()));
        }

        // Start the asynchronous operation
        void
            on_run()
        {
            // Set suggested timeout settings for the websocket
            ws_.set_option(
                boost::beast::websocket::stream_base::timeout::suggested(
                    boost::beast::role_type::server));

            // Set a decorator to change the Server of the handshake
            ws_.set_option(boost::beast::websocket::stream_base::decorator(
                [](boost::beast::websocket::response_type& res)
                {
                    res.set(boost::beast::http::field::server,
                        std::string(BOOST_BEAST_VERSION_STRING) +
                        " websocket-server-async");
                }));
            // Accept the websocket handshake
            ws_.async_accept(
                boost::beast::bind_front_handler(
                    &session::on_accept,
                    shared_from_this()));
        }

        void
            on_accept(boost::beast::error_code ec)
        {
            if (ec)
            {
                return fail(ec, "accept");
            }
            DEBUG_EX_LOG("");
            // Read a message
            do_read();
        }

        void
            do_read()
        {
            DEBUG_EX_LOG("");
            // Read a message into our buffer
            ws_.async_read(
                read_buffer_,
                boost::beast::bind_front_handler(
                    &session::on_read,
                    shared_from_this()));
        }

        void
            on_read(
                boost::beast::error_code ec,
                std::size_t bytes_transferred)
        {
            boost::ignore_unused(bytes_transferred);

            // This indicates that the session was closed
            if (ec == boost::beast::websocket::error::closed)
                return;
            DEBUG_EX_LOG("");
            if (ec)
            {
                fail(ec, "read");
            }

            // 处理业务逻辑
            ////
            // Echo the message
           // ws_.text(ws_.got_text());
           // //buffer_.cdata();
           //// boost::asio::mutable_buffer test_data =  buffer_.data();
           //// std::cout << buffer_.cdata() << std::endl;
           // DEBUG_EX_LOG("[ws_.got_text() = %d][text = %s][len = %lu]", ws_.got_text(), buffer_.data().data(), buffer_.size());
           // ws_.async_write(
           //     buffer_.data(),
           //     boost::beast::bind_front_handler(
           //         &session::on_write,
           //         shared_from_this()));
            read_buffer_.consume(read_buffer_.size());
            do_read();
        }



        void write(const char* buffer, uint32 size)
        {
            ws_.text(ws_.got_text());
             
            write_buffer.commit(size);
             
           //  DEBUG_EX_LOG("[ws_.got_text() = %d][text = %s][len = %lu]", ws_.got_text(), buffer_.data().data(), buffer_.size());
             ws_.async_write(
                 write_buffer.data(),
                 boost::beast::bind_front_handler(
                     &session::on_write,
                     shared_from_this()));
        }
        void
            on_write(
                boost::beast::error_code ec,
                std::size_t bytes_transferred)
        {
            boost::ignore_unused(bytes_transferred);

            if (ec)
            {
                return fail(ec, "write");
            }

            // Clear the buffer
            write_buffer.consume(write_buffer.size());

            // Do another read
            do_read();
        }
    };
}

#endif // _C_WEBSOCKET_SESSION_H_