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

int test_main(int argc, char *argv[])
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





//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// Copyright (c) 2016-2017 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

//------------------------------------------------------------------------------
//
// Example: WebSocket server, asynchronous
//
//------------------------------------------------------------------------------

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/bind_executor.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <algorithm>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

//using tcp = boost::asio::ip::tcp;               // from <boost/asio/ip/tcp.hpp>
//namespace websocket = boost::beast::websocket;  // from <boost/beast/websocket.hpp>

												//------------------------------------------------------------------------------

												// Report a failure
void fail(boost::system::error_code ec, char const* what)
{
	std::cerr << what << ": " << ec.message() << "\n";
}

// Echoes back all received WebSocket messages
class session : public std::enable_shared_from_this<session>
{
	boost::beast::websocket::stream<boost::asio::ip::tcp::socket> ws_;
	boost::asio::strand< boost::asio::io_context::executor_type> strand_;
	unsigned char *				m_buffer_ptr;
	 

public:
	// Take ownership of the socket
	explicit
		session(boost::asio::ip::tcp::socket socket)
		: ws_(std::move(socket))
		, strand_(ws_.get_executor())
		, m_buffer_ptr(new unsigned char [1024 * 1024])
	{
	}

	// Start the asynchronous operation
	void
		run()
	{
		// Accept the websocket handshake
		ws_.async_accept(
			boost::asio::bind_executor(
				strand_,
				std::bind(
					&session::on_accept,
					shared_from_this(),
					std::placeholders::_1)));
	}

	void
		on_accept(boost::system::error_code ec)
	{
		if (ec)
		{
			return fail(ec, "accept");
		}

		// Read a message
		do_read();
	}

	void
		do_read()
	{
		// Read a message into our buffer
		ws_.async_read_some(
			boost::asio::buffer(m_buffer_ptr, 1024 * 1024),
			boost::asio::bind_executor(
				strand_,
				std::bind(
					&session::on_read,
					shared_from_this(),
					std::placeholders::_1,
					std::placeholders::_2)));
	}

	void
		on_read(
			boost::system::error_code ec,
			std::size_t bytes_transferred)
	{
		boost::ignore_unused(bytes_transferred);

		// This indicates that the session was closed
		if (ec == boost::beast::websocket::error::closed)
		{
			return;
		}

		if (ec)
		{
			fail(ec, "read");
		}
		std::cout << m_buffer_ptr << "][ size = " << bytes_transferred << std::endl;
		// Echo the message
		 ws_.text(ws_.got_text());
		ws_.async_write(
			boost::asio::buffer(m_buffer_ptr, bytes_transferred),
			boost::asio::bind_executor(
				strand_,
				std::bind(
					&session::on_write,
					shared_from_this(),
					std::placeholders::_1,
					std::placeholders::_2))); 
	}

	void on_write(
		boost::system::error_code ec,
		std::size_t bytes_transferred)
	{
		boost::ignore_unused(bytes_transferred);

		if (ec)
		{
			return fail(ec, "write");
		}

		// Clear the buffer
		//read_buffer_.consume(read_buffer_.size());


		 
		do_read();
	}
};

//------------------------------------------------------------------------------

// Accepts incoming connections and launches the sessions
class listener : public std::enable_shared_from_this<listener>
{
	boost::asio::ip::tcp::acceptor acceptor_;
	boost::asio::ip::tcp::socket socket_;

public:
	listener(
		boost::asio::io_context& ioc,
		boost::asio::ip::tcp::endpoint endpoint)
		: acceptor_(ioc)
		, socket_(ioc)
	{
		boost::system::error_code ec;

		// Open the acceptor
		acceptor_.open(endpoint.protocol(), ec);
		if (ec)
		{
			fail(ec, "open");
			return;
		}

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
		if (!acceptor_.is_open())
		{
			return;
		}
		do_accept();
	}

	void
		do_accept()
	{
		acceptor_.async_accept(
			socket_,
			std::bind(
				&listener::on_accept,
				shared_from_this(),
				std::placeholders::_1));
	}

	void
		on_accept(boost::system::error_code ec)
	{
		if (ec)
		{
			fail(ec, "accept");
		}
		else
		{
			// Create the session and run it
			std::make_shared<session>(std::move(socket_))->run();
		}

		// Accept another connection
		do_accept();
	}
};
#include "cmedia_server.h"
#include <signal.h>
//------------------------------------------------------------------------------
void Stop(int i)
{

	chen::g_media_server.stop();
}

void RegisterSignal()
{
	signal(SIGINT, Stop);
	signal(SIGTERM, Stop);

}

int main(int argc, char* argv[])
{

	RegisterSignal();
	bool init = chen::g_media_server.init(NULL, NULL);

	if (init)
	{
		init = chen::g_media_server.Loop();
	}
	else
	{
		return -1;
	}
	chen::g_media_server.destroy();
	if (!init)
	{
		return 1;
	}
	return 0;

	// Check command line arguments.
	/*if (argc != 4)
	{
		std::cerr <<
			"Usage: websocket-server-async <address> <port> <threads>\n" <<
			"Example:\n" <<
			"    websocket-server-async 0.0.0.0 8080 1\n";
		return EXIT_FAILURE;
	}*/
	auto const address = boost::asio::ip::make_address("0.0.0.0");
	const uint16_t port = 9090;
	auto const threads = 1;

	// The io_context is required for all I/O
	boost::asio::io_context ioc{ threads };

	// Create and launch a listening port
	std::make_shared<listener>(ioc, boost::asio::ip::tcp::endpoint{ address, port })->run();

	// Run the I/O service on the requested number of threads
	std::vector<std::thread> v;
	v.reserve(threads - 1);
	for (auto i = threads - 1; i > 0; --i)
		v.emplace_back(
			[&ioc]
	{
		ioc.run();
	});
	ioc.run();

	return EXIT_SUCCESS;
}
