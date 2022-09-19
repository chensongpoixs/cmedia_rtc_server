/***********************************************************************************************
created: 		2022-08-24

author:			chensong

purpose:		log
************************************************************************************************/
#ifndef _C_CCLIENT_H_
#define _C_CCLIENT_H_
 
#include "cnet_types.h"
#include <string>
#include "json.hpp"
 
#include "cwebsocket_mgr.h"

#include "csingleton.h"
#include "cdesktop_capture.h"
#include "cpeer_connnection.h"

namespace chen {

	 
	enum EMedia_Session_Status
	{
		EMedia_Session_Node = 0,
		EMedia_Session_connecting,
		EMedia_Session_Connected,
		EMedia_Login,
		EMedia_Loading,
		EMedia_Gameing,
	};



	class cclient
	{
	
	public:
		cclient();
		~cclient();

	public:

		void set_load_offer(const std::string & sdp) { m_offer_sdp = sdp; }
	public:
		bool init(uint32 gpu_index);
		void Loop(/*const std::string& mediasoupIp, uint16_t port, const std::string& roomName, const std::string& clientName
			, uint32_t reconnect_waittime*/);
		void stop();

		void Destory();

 

	
		
	//////////////////////////////////////////////////////////////////////////////////////////////
	
	
	private:
		bool _send_request_media(uint32 msg_id, const nlohmann::json & data);


	private:
		void  _presssmsg(std::list<std::string> & msgs);

	public:
		bool handler_s2c_login(nlohmann::json& data);
		bool handler_s2c_create_room(nlohmann::json&data);
		bool handler_s2c_destroy_room(nlohmann::json& data);
	private:
		uint64			m_id;
		bool			m_loaded;
		bool			m_stoped;
		std::string				m_room_name;
		std::string				m_client_name;
		
	
		
		

		
		cwebsocket_mgr					m_websocket_mgr;
		 
		bool							m_webrtc_connect;
		 
		rtc::scoped_refptr<cpeer_connection>		 		m_peer_connection_ptr;
		EMedia_Session_Status						m_media_session_stats;
		std::string										m_offer_sdp;

		
	};
#define  s_client chen::csingleton<chen::cclient>::get_instance()
}

#endif // !_C_CLIENT_H_