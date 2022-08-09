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

int  main(int argc, char* argv[])
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
