/***********************************************************************************************
created: 		2022-08-10

author:			chensong

purpose:		cwebrtc_mgr

Copyright boost
************************************************************************************************/

#ifndef _C_WEBRTC_MGR_H_
#define _C_WEBRTC_MGR_H_
#include <string>
#include "cnet_type.h"

#include <json/json.h>
#include "cwebrtc_transport.h"
#include <unordered_map>
namespace chen
{ 
	class cwebrtc_mgr
	{
	public:
		cwebrtc_mgr();
		~cwebrtc_mgr();

	public:
		bool handler_info(uint64 session_id,  Json::Value & value);
		bool handler_create_webrtc(uint64 session_id, Json::Value & value);
		bool handler_destroy_webrtc(uint64 session_id, Json::Value& value);
		/////////////////////////////////
		bool handler_webrtc(uint64 session_id, Json::Value& value);
	private:
		//cnoncopyable(cnoncopyable&&);
		cwebrtc_mgr(const cwebrtc_mgr&);
		//cnoncopyable &operator =(cnoncopyable &&);
		cwebrtc_mgr& operator=(const cwebrtc_mgr&);
	private:
		std::unordered_map<std::string, cwebrtc_transport*>     m_webrtc_transport_map;
	};


	extern cwebrtc_mgr g_global_webrtc_mgr;
}

#endif // _C_WEBRTC_MGR_H_