/***********************************************************************************************
			created: 		2022-09-19
			author:			chensong
			purpose:		client_msg_dipatch
************************************************************************************************/

#ifndef _C_CLIENT_MSG_DIPATCH_H_
#define _C_CLIENT_MSG_DIPATCH_H_

#include "cnet_type.h"
#include <atomic>
#include "cwebrtc_mgr.h"
#include "cwebrtc_transport.h"
#include <json/json.h>
#include <unordered_map>
#include "cmsg_base_id.h"
namespace chen {
	class  cwan_session;
	//typedef void (cwebrtc_mgr::*handler_webrtc_type)(Json::Value & value);
	typedef bool (cwan_session::*handler_client_type)(  Json::Value & value);
#pragma pack(push, 4)

	struct cclient_msg_handler
	{
		std::string							msg_name;
		std::atomic<long>					handle_count;
		handler_client_type						handler;

		cclient_msg_handler() : msg_name(""), handle_count(0), handler(NULL) {}
	};

	//handler_type

	class cclient_msg_dispatch :private cnoncopyable
	{
	public:

	public:
		explicit cclient_msg_dispatch();
		~cclient_msg_dispatch();
	public:

		bool init();
		void destroy();

		cclient_msg_handler* get_msg_handler(uint16 msg_id);

	private:
		void _register_msg_handler(uint16 msg_id, const std::string& msg_name, handler_client_type handler);
		//void _register_webrtc_transport_msg_handler(uint16 msg_id, const std::string& msg_name, handler_type handler);
	private:
		cclient_msg_handler		m_msg_handler[Msg_Client_Max];

	};

#pragma pack(pop)
	//extern cmsg_dispatch<handler_webrtc_type> g_webrtc_mgr_msg_dispatch;
	extern cclient_msg_dispatch g_client_msg_dispatch;
}

#endif // _C_CLIENT_MSG_DIPATCH_H_