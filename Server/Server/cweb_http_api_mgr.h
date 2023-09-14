/********************************************************************
created:	2021-11-24

author:		chensong

purpose:	Location

*********************************************************************/

#ifndef _C_WEB_API_PROXY_H_
#define _C_WEB_API_PROXY_H_

#include <memory>
#include <string>
#include <utility>
#include "cweb_http_api_interface.h"
#include "server_http.hpp"
#include <future>

// Added for the json-example
#define BOOST_SPIRIT_THREADSAFE
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

// Added for the default_resource example
#include <algorithm>
#include <boost/filesystem.hpp>
#include <fstream>
#include <vector>
#ifdef HAVE_OPENSSL
#include "crypto.hpp"
#endif
#include <vector>
#include "croom.h"

namespace chen {

	class cweb_http_api_mgr : public cweb_http_api_interface
	{
	public:
		cweb_http_api_mgr();
		virtual ~cweb_http_api_mgr();


	public:
		bool init();
		void update();
		void destroy();



		void startup();
	public:
		virtual std::vector< croom_info>   get_all_room();
		virtual std::vector< chen::cuser_info>   get_room_info(const std::string& room_name );
	private:
		void _pthread_work();
	private:
		SimpleWeb::Server<SimpleWeb::HTTP>	m_server;

	std::thread							m_thread;
	};
	extern cweb_http_api_mgr g_web_http_api_mgr;
}



#endif // _C_WEB_API_PROXY_H_