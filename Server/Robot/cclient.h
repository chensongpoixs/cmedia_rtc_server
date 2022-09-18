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
 
namespace chen {

	 




	class cclient
	{
	private:
		typedef bool (cclient::*client_protoo_msg)(const  nlohmann::json & msg);
		typedef bool(cclient::*server_protoo_msg)(const  nlohmann::json & msg);
		 
	public:
		cclient();
		~cclient();


	public:
		bool init(uint32 gpu_index);
		void Loop(const std::string& mediasoupIp, uint16_t port, const std::string& roomName, const std::string& clientName
			, uint32_t reconnect_waittime);
		void stop();

		void Destory();


		// 线程不安全的
		bool webrtc_video(unsigned char * rgba, uint32 fmt,  int32_t width, int32_t height);
		bool webrtc_texture(void * texture, uint32 fmt, int32_t width, int32_t height);
		bool webrtc_video(unsigned char * y_ptr, unsigned char * uv_ptr, uint32 fmt, int32_t width, int32_t height);
		bool webrtc_video(const webrtc::VideoFrame& frame);
		

	
		
	//////////////////////////////////////////////////////////////////////////////////////////////
	
	
	
	private:
		uint64			m_id;
		bool			m_loaded;
		bool			m_stoped;
		std::string				m_room_name;
		std::string				m_client_name;
		
	
		
		

		
		cwebsocket_mgr					m_websocket_mgr;
		 
		bool							m_webrtc_connect;
		 
		cpeer_connection		*		m_peer_connection_ptr;


		
	};
#define  s_client chen::csingleton<chen::cclient>::get_instance()
}

#endif // !_C_CLIENT_H_