/***********************************************************************************************
created: 		2022-08-09

author:			chensong

purpose:		TLSv1.3 协议的学习

原因是WebRTC中有DTSTransport --> 
************************************************************************************************/
#define _CRT_SECURE_NO_WARNINGS
//#define _WIN32_WINNT  (0x0601)
#include <stdio.h>
#include <stdlib.h>
#include <openssl/crypto.h>
#include <openssl/rand.h>
#include <iostream>
#include <mutex>
#include <openssl/asn1.h>
#include <openssl/bn.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>

#include <vector>
#include <map>
#include <string>
#include "clog.h"
#include "dtlsv1.x.h"
/* Static. */

static std::once_flag globalInitOnce;

int main(int argc, char *argv[])
{
	std::call_once(globalInitOnce, [] {
		DEBUG_EX_LOG( "openssl version: %s" ,  OpenSSL_version(OPENSSL_VERSION));
		// Initialize some crypto stuff.
		RAND_poll();
	});
	
	/////////////
	// global ssl
	////////////
	//chen::dtlsv1x::ReadCertificateAndPrivateKeyFromFiles();
	chen::dtlsv1x::GenerateCertificateAndPrivateKey();

	// Create a global SSL_CTX.
	chen::dtlsv1x::CreateSslCtx();

	// Generate certificate fingerprints.
	chen::dtlsv1x::GenerateFingerprints();

	 


	chen::dtlsv1x client("client");
	chen::dtlsv1x server("server");
	client.SetListener(&server);
	server.SetListener(&client);

	client.startup();
	server.startup();
	//server 准备handhasb
	server.Run(chen::Role::SERVER);

	client.Run(chen::Role::CLIENT);

	while (true)
	{
		DEBUG_EX_LOG("main sleep 1 seconds ...");
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	return EXIT_SUCCESS;
}


#include <iostream>
 
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/strand.hpp>
#include <algorithm>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include "clog.h"
#include <thread>
#include <vector>

 
//------------------------------------------------------------------------------

// Report a failure
void fail(boost::beast::error_code ec, char const* what)
{
    std::cerr << what << ": " << ec.message() << "\n";
}

// Echoes back all received WebSocket messages
class session : public std::enable_shared_from_this<session>
{
    boost::beast::websocket::stream<boost::beast::tcp_stream> ws_;
    boost::beast::flat_buffer buffer_;

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
            return fail(ec, "accept");
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
            buffer_,
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
            fail(ec, "read");

        // Echo the message
        ws_.text(ws_.got_text());
        ws_.async_write(
            buffer_.data(),
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
            return fail(ec, "write");

        // Clear the buffer
        buffer_.consume(buffer_.size());

        // Do another read
        do_read();
    }
};

//------------------------------------------------------------------------------

// Accepts incoming connections and launches the sessions
class listener : public std::enable_shared_from_this<listener>
{
    boost::asio::io_context& ioc_;
    boost::asio::ip::tcp::acceptor acceptor_;

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
            std::make_shared<session>(std::move(socket))->run();
        }

        // Accept another connection
        do_accept();
    }
};

//------------------------------------------------------------------------------

int test_websocket_server_main(int argc, char* argv[])
{
    // Check command line arguments.
   /* if (argc != 4)
    {
        std::cerr <<
            "Usage: websocket-server-async <address> <port> <threads>\n" <<
            "Example:\n" <<
            "    websocket-server-async 0.0.0.0 8080 1\n";
        return EXIT_FAILURE;
    }*/


    auto const address = boost::asio::ip::make_address("0.0.0.0");
    static const uint16_t port = 9999;
    static const uint32_t io_threads = 1;
    

    // The io_context is required for all I/O
    boost::asio::io_context ioc{ io_threads };

    // Create and launch a listening port
    std::make_shared<listener>(ioc, boost::asio::ip::tcp::endpoint{ address, port })->run();

    // Run the I/O service on the requested number of threads
    std::vector<std::thread> v;
    v.reserve(io_threads - 1);
    for (auto i = io_threads - 1; i > 0; --i)
        v.emplace_back(
            [&ioc]
            {
                ioc.run();
            });
    ioc.run();

    return EXIT_SUCCESS;
}
