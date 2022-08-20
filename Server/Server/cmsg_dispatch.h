/***********************************************************************************************
			created: 		2019-05-13
			author:			chensong
			purpose:		msg_dipatch
************************************************************************************************/

#ifndef _C_MSG_DIPATCH_H_
#define _C_MSG_DIPATCH_H_
 
#include "cnet_type.h"
#include <atomic>
#include "cwebrtc_mgr.h"
#include "cwebrtc_transport.h"
#include <json/json.h>
#include <unordered_map>
#include "cmsg_base_id.h"
namespace chen {
	//typedef void (cwebrtc_mgr::*handler_webrtc_type)(Json::Value & value);
	typedef bool (cwebrtc_transport::*handler_type)(uint64 session_id, Json::Value & value);
#pragma pack(push, 4)

	struct cmsg_handler
	{
		std::string							msg_name;
		std::atomic<long>					handle_count;
		handler_type						handler;

		cmsg_handler() : msg_name(""), handle_count(0), handler(NULL) {}
	};
	
	//handler_type
	 
	class cmsg_dispatch :private cnoncopyable
	{
	public:
		
	public:
		explicit cmsg_dispatch();
		~cmsg_dispatch();
	public:

		bool init();
		void destroy();

		cmsg_handler* get_msg_handler(uint16 msg_id);

	private:
		void _register_msg_handler(uint16 msg_id, const std::string& msg_name, handler_type handler);
		//void _register_webrtc_transport_msg_handler(uint16 msg_id, const std::string& msg_name, handler_type handler);
	private:
		cmsg_handler		m_msg_handler[Msg_Client_Max];
		
	};
 
#pragma pack(pop)
	//extern cmsg_dispatch<handler_webrtc_type> g_webrtc_mgr_msg_dispatch;
	extern cmsg_dispatch g_msg_dispatch;
} //namespace chen

#endif //!#define _C_MSG_DIPATCH_H_