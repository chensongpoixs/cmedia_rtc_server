/***********************************************************************************************
created: 		2022-08-24

author:			chensong

purpose:		log
************************************************************************************************/
#include "cclient.h"
//#include "mediasoupclient.hpp"
//#include "ortc.hpp"
#include "clog.h"
#include "ccfg.h"
#include "cwebsocket_mgr.h"
#include <iostream>
#include <io.h>
//#define _CRT_SECURE_NO_WARNINGS
//#include "cdevice.h"
#include "pc/video_track_source.h"
//#include "crecv_transport.h"
//#include "csend_transport.h"
//#include "cinput_device.h"
//#include "NvCodec/nvenc.h"
#include "build_version.h"
namespace chen {




	static const uint32_t TICK_TIME = 200;

	cclient::cclient()
		:m_id(100000)
		,m_loaded(false)
		, m_stoped(false)
		{}
	cclient::~cclient(){}


	
	 
	bool cclient::init(uint32 gpu_index)
	{
		
		
		return true;
	}
	void cclient::stop()
	{
		
	}
	void cclient::Loop(const std::string & mediasoupIp, uint16_t port, const std::string & roomName, const std::string & clientName
	,	uint32_t websocket_reconnect_waittime)
	{
		
	}
	 
	void cclient::Destory()
	{
		
	  
	}
	 
	
	bool cclient::webrtc_video(unsigned char * rgba, uint32 fmt, int32_t width, int32_t height)
	{
		
		return true;
	}
	bool cclient::webrtc_texture(void * texture, uint32 fmt, int32_t width, int32_t height)
	{
		return true;
	}
	bool cclient::webrtc_video(unsigned char * y_ptr, unsigned char * uv_ptr, uint32 fmt, int32_t width, int32_t height)
	{
		

		return true;
	}
	bool cclient::webrtc_video(const webrtc::VideoFrame& frame)
	{
		 
		return true;
	}
	
	

	
}