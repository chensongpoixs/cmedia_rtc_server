

#include "cweb_http_api_mgr.h"
#include <iostream>
#include "ccfg.h"
#include <thread>
#include "cweb_http_api_proxy.h"
//#include <json.h>
#include <json/json.h>
#include "croom_mgr.h"

namespace chen {

	using namespace std;


	cweb_http_api_mgr g_web_http_api_mgr;
	cweb_http_api_mgr::cweb_http_api_mgr()
	: m_server(){}


	cweb_http_api_mgr::~cweb_http_api_mgr(){}
	bool cweb_http_api_mgr::init()
	{
		m_server.config.port = g_cfg.get_uint32(ECI_WebHttpWanPort);
		m_server.resource["^/rooms$"]["GET"] = [](std::shared_ptr<SimpleWeb::Server<SimpleWeb::HTTP>::Response> response, std::shared_ptr<SimpleWeb::Server<SimpleWeb::HTTP>::Request> request)
		{
			std::cout << "http thread id  :" << std::this_thread::get_id() << std::endl;
			std::vector<croom_info> room_infos = g_web_http_api_proxy.get_all_room();

			//if (infos.)
			Json::Value value;
			//Json::arrayValue Array_Value;
			//Json::Value Array_Value;
			//value["room_name"] = request->path_match[1].str();
			for (size_t i = 0; i < room_infos.size(); ++i)
			{

				Json::Value room_item;
				room_item["room_name"] = room_infos[i].room_name;

				for (size_t j = 0; j < room_infos [i].infos.size(); ++j)
				{
					Json::Value  item;
					item["user_name"] = room_infos[i].infos[j].username;
					item["type"] = room_infos [i].infos[j].m_type;
					item["ip"] = room_infos[i].infos[j].m_remote_ip;
					item["port"] = room_infos[i].infos[j].m_remote_port;
					room_item["user_infos"].append(item);
				}
				
				Json::Value  while_item;
				for (const std::string & user_ : room_infos[i].m_while_list)
				{
					while_item.append(user_);
					/*item[""] = infos[i].username;

					item["type"] = infos[i].m_type;
					item["ip"] = infos[i].m_remote_ip;*/
				}
				room_item["while_user"].append(while_item);
				value["room_infos"].append(room_item);
			}
			// = Array_Value;
			Json::StyledWriter swriter;
			std::string str = swriter.write(value);
			//g_wan_server.send_msg(m_session_id, msg_id, str.c_str(), str.length());
			response->write(str);
			//g_web_http_api_proxy.g(request->path_match[1].str());
			//response->write(request->path_match[1].str());
		};
		// GET-example for the path /match/[number], responds with the matched string in path (number)
	// For instance a request GET /match/123 will receive: 123
		m_server.resource["^/roomId/([a-zA-Z0-9_-]+)$"]["GET"] = [](std::shared_ptr<SimpleWeb::Server<SimpleWeb::HTTP>::Response> response, std::shared_ptr<SimpleWeb::Server<SimpleWeb::HTTP>::Request> request)
		{
			std::cout << "http thread id  :" << std::this_thread::get_id() << std::endl;
			std::vector< chen::cuser_info> infos =  g_web_http_api_proxy.get_room_info(request->path_match[1].str());

			//if (infos.)
			Json::Value value;
			//Json::arrayValue Array_Value;
			//Json::Value Array_Value;
			value["room_name"] = request->path_match[1].str();
			for (size_t i = 0; i < infos.size(); ++i)
			{
				Json::Value  item;
				item["user_name"] = infos[i].username;

				item["type"] = infos[i].m_type ;
				item["ip"] = infos[i].m_remote_ip;
				item["port"] = infos[i].m_remote_port;
				value["user_infos"].append(item);
				//Json::Value  while_item;
				//for (const std::string & user_ :  infos.m_while_list)
				//{
				//	while_item.append(user_);
				//	/*item[""] = infos[i].username;

				//	item["type"] = infos[i].m_type;
				//	item["ip"] = infos[i].m_remote_ip;*/
				//}
				//value["while_user"].append(while_item);
			}
			 
			// = Array_Value;
			Json::StyledWriter swriter;
			std::string str = swriter.write(value);
			//g_wan_server.send_msg(m_session_id, msg_id, str.c_str(), str.length());
			response->write(str);
		};
		// Kick out
		m_server.resource["^/kickout/roomId/([a-zA-Z0-9_.@-]+)/username/([a-zA-Z0-9_.@-]+)$"]["GET"] = [](std::shared_ptr<SimpleWeb::Server<SimpleWeb::HTTP>::Response> response, std::shared_ptr<SimpleWeb::Server<SimpleWeb::HTTP>::Request> request)
		{
			std::cout << "http thread id  :" << std::this_thread::get_id() << std::endl;
			//std::vector< chen::cuser_info> infos = g_web_http_api_proxy.get_room_info(request->path_match[1].str());
			std::cout << "room_name = " << request->path_match[1] << ", username = " << request->path_match[2];
			//if (infos.)
			Json::Value value;
			value["result"] = g_web_http_api_proxy.kick_room_username(request->path_match[1], request->path_match[2]);
			//Json::arrayValue Array_Value;
			//Json::Value Array_Value;
			//value["room_name"] = request->path_match[1].str();
			/*for (size_t i = 0; i < infos.size(); ++i)
			{
				Json::Value  item;
				item["user_name"] = infos[i].username;
				item["type"] = infos[i].m_type;
				item["ip"] = infos[i].m_remote_ip;
				item["port"] = infos[i].m_remote_port;
				value["user_infos"].append(item);
			}*/
			// = Array_Value;
			Json::StyledWriter swriter;
			std::string str = swriter.write(value);
			//g_wan_server.send_msg(m_session_id, msg_id, str.c_str(), str.length());
			response->write(str);
		};


		m_server.resource["^/add_while/roomId/([a-zA-Z0-9_.@-]+)/username/([a-zA-Z0-9_.@-]+)$"]["GET"] = [](std::shared_ptr<SimpleWeb::Server<SimpleWeb::HTTP>::Response> response, std::shared_ptr<SimpleWeb::Server<SimpleWeb::HTTP>::Request> request)
		{
			std::cout << "http thread id  :" << std::this_thread::get_id() << std::endl;
			//std::vector< chen::cuser_info> infos = g_web_http_api_proxy.get_room_info(request->path_match[1].str());
			std::cout << "room_name = " << request->path_match[1] << ", username = " << request->path_match[2];
			//if (infos.)
			Json::Value value;
			value["result"] = g_web_http_api_proxy.add_while_room_username(request->path_match[1], request->path_match[2]);
			//Json::arrayValue Array_Value;
			//Json::Value Array_Value;
			//value["room_name"] = request->path_match[1].str();
			/*for (size_t i = 0; i < infos.size(); ++i)
			{
				Json::Value  item;
				item["user_name"] = infos[i].username;
				item["type"] = infos[i].m_type;
				item["ip"] = infos[i].m_remote_ip;
				item["port"] = infos[i].m_remote_port;
				value["user_infos"].append(item);
			}*/
			// = Array_Value;
			Json::StyledWriter swriter;
			std::string str = swriter.write(value);
			//g_wan_server.send_msg(m_session_id, msg_id, str.c_str(), str.length());
			response->write(str);
		};

		m_server.resource["^/delete_while/roomId/([a-zA-Z0-9_.@-]+)/username/([a-zA-Z0-9_.@-]+)$"]["GET"] = [](std::shared_ptr<SimpleWeb::Server<SimpleWeb::HTTP>::Response> response, std::shared_ptr<SimpleWeb::Server<SimpleWeb::HTTP>::Request> request)
		{
			std::cout << "http thread id  :" << std::this_thread::get_id() << std::endl;
			//std::vector< chen::cuser_info> infos = g_web_http_api_proxy.get_room_info(request->path_match[1].str());
			std::cout << "room_name = " << request->path_match[1] << ", username = " << request->path_match[2];
			//if (infos.)
			Json::Value value;
			value["result"] = g_web_http_api_proxy.delete_while_room_username(request->path_match[1], request->path_match[2]);
			//Json::arrayValue Array_Value;
			//Json::Value Array_Value;
			//value["room_name"] = request->path_match[1].str();
			/*for (size_t i = 0; i < infos.size(); ++i)
			{
				Json::Value  item;
				item["user_name"] = infos[i].username;
				item["type"] = infos[i].m_type;
				item["ip"] = infos[i].m_remote_ip;
				item["port"] = infos[i].m_remote_port;
				value["user_infos"].append(item);
			}*/
			// = Array_Value;
			Json::StyledWriter swriter;
			std::string str = swriter.write(value);
			//g_wan_server.send_msg(m_session_id, msg_id, str.c_str(), str.length());
			response->write(str);
		};
		// Default GET-example. If no other matches, this anonymous function will be called.
	// Will respond with content in the web/-directory, and its subdirectories.
	// Default file: index.html
	// Can for instance be used to retrieve an HTML 5 client that uses REST-resources on this server
		m_server.default_resource["GET"] = [](std::shared_ptr<SimpleWeb::Server<SimpleWeb::HTTP>::Response> response, std::shared_ptr<SimpleWeb::Server<SimpleWeb::HTTP>::Request> request) {
			try {
				auto web_root_path = boost::filesystem::canonical("web");
				auto path = boost::filesystem::canonical(web_root_path / request->path);
				// Check if path is within web_root_path
				if (distance(web_root_path.begin(), web_root_path.end()) > distance(path.begin(), path.end()) ||
					!equal(web_root_path.begin(), web_root_path.end(), path.begin()))
					throw invalid_argument("path must be within root path");
				if (boost::filesystem::is_directory(path))
					path /= "index.html";

				SimpleWeb::CaseInsensitiveMultimap header;

				// Uncomment the following line to enable Cache-Control
				// header.emplace("Cache-Control", "max-age=86400");

#ifdef HAVE_OPENSSL
//    Uncomment the following lines to enable ETag
//    {
//      ifstream ifs(path.string(), ifstream::in | ios::binary);
//      if(ifs) {
//        auto hash = SimpleWeb::Crypto::to_hex_string(SimpleWeb::Crypto::md5(ifs));
//        header.emplace("ETag", "\"" + hash + "\"");
//        auto it = request->header.find("If-None-Match");
//        if(it != request->header.end()) {
//          if(!it->second.empty() && it->second.compare(1, hash.size(), hash) == 0) {
//            response->write(SimpleWeb::StatusCode::redirection_not_modified, header);
//            return;
//          }
//        }
//      }
//      else
//        throw invalid_argument("could not read file");
//    }
#endif

				auto ifs = make_shared<ifstream>();
				ifs->open(path.string(), ifstream::in | ios::binary | ios::ate);

				if (*ifs) {
					auto length = ifs->tellg();
					ifs->seekg(0, ios::beg);

					header.emplace("Content-Length", to_string(length));
					response->write(header);

					// Trick to define a recursive function within this scope (for example purposes)
					class FileServer {
					public:
						static void read_and_send(const shared_ptr<SimpleWeb::Server<SimpleWeb::HTTP>::Response> &response, const std::shared_ptr<ifstream> &ifs) {
							// Read and send 128 KB at a time
							static vector<char> buffer(131072); // Safe when server is running on one thread
							streamsize read_length;
							if ((read_length = ifs->read(&buffer[0], static_cast<streamsize>(buffer.size())).gcount()) > 0) {
								response->write(&buffer[0], read_length);
								if (read_length == static_cast<streamsize>(buffer.size())) {
									response->send([response, ifs](const SimpleWeb::error_code &ec) {
										if (!ec)
											read_and_send(response, ifs);
										else
											std::cerr << "Connection interrupted" << endl;
									});
								}
							}
						}
					};
					FileServer::read_and_send(response, ifs);
				}
				else
					throw std::invalid_argument("could not read file");
			}
			catch (const std::exception &e) {
				response->write(SimpleWeb::StatusCode::client_error_bad_request, "Could not open path " + request->path + ": " + e.what());
			}
		};

		m_server.on_error = [](std::shared_ptr<SimpleWeb::Server<SimpleWeb::HTTP>::Request> /*request*/, const SimpleWeb::error_code & /*ec*/) {
			// Handle errors here
			// Note that connection timeouts will also call this handle with ec set to SimpleWeb::errc::operation_canceled
		};

		// Start server and receive assigned port when server is listening for requests
		//std::promise<unsigned short> server_port;
		//std::thread server_thread([&m_server, &server_port]() {
		//	// Start server
		//	m_server.start([&server_port](unsigned short port) {
		//		server_port.set_value(port);
		//	});
		//});
		//std::cout << "Server listening on port " << server_port.get_future().get() << std::endl << std::endl;



		return true;
	}
	void cweb_http_api_mgr::update()
	{}
	void cweb_http_api_mgr::destroy()
	{
		m_server.stop();
		if (m_thread.joinable())
		{
			m_thread.join();
		}
	}

	void cweb_http_api_mgr::startup()
	{
		std::promise<unsigned short> server_port;
		std::thread t_([this, &server_port]() 
		{
			// Start server
			m_server.start([&server_port](unsigned short port) 
			{
				server_port.set_value(port);
			});
		});
		std::cout << "Server listening on port " << server_port.get_future().get() << std::endl;
		m_thread.swap(t_);
	}
	void cweb_http_api_mgr::_pthread_work()
	{
		


	}
	std::vector< croom_info>   cweb_http_api_mgr::get_all_room()
	{

		std::cout << "work thread id :" << std::this_thread::get_id() <<std::endl;
		std::vector< croom_info>  room_infos;
			g_room_mgr.build_all_room_info(room_infos);
		return room_infos;
	}
	std::vector< chen::cuser_info>   cweb_http_api_mgr::get_room_info(const std::string& room_name )
	{
		std::vector< chen::cuser_info> infos;

		g_room_mgr.get_room_info(room_name, infos);

		std::cout << room_name << std::endl;
		return infos;
	}
	uint32_t cweb_http_api_mgr::kick_room_username(const std::string & room_name, const std::string & user_name)
	{
		return g_room_mgr.kick_room_username(room_name, user_name);
	}

	  uint32_t  cweb_http_api_mgr::add_while_room_username(const std::string& room_name, const std::string & user_name)
	  {
		  return g_room_mgr.add_white_room_username(room_name, user_name);
	  }
	  uint32_t  cweb_http_api_mgr::delete_while_room_username(const std::string& room_name, const std::string & user_name)
	  {
		  return g_room_mgr.delete_white_room_username(room_name, user_name);
	  }
}