/***********************************************************************************************
created: 		2019-03-02

author:			chensong

purpose:		log
************************************************************************************************/
#ifndef _C_CLIENT_H
#define _C_CLIENT_H
#include <iostream>
#include "cwebsocket_mgr.h"
#include "crtc_publisher.h"
namespace chen {




	// websocket 的连接状态 
	enum ESessionType
	{
		ESessionNone = 0,
		ESessionConnecting,
		ESessionConnected,
		ESessionCreateOffering,
		ESessionCreateOffered,
		ESessionSetAnswered,
		ESessionDestroy,

	};

	class cclient
	{
	public:
		explicit cclient()
		: m_stoped(true)
		, m_session_status(ESessionNone)
		, m_websocket_mgr()
		, m_rtc_publisher(nullptr){}
		
		virtual ~cclient();
		 

	public:
		bool init(const char * config_file);
		void Loop();
		void destroy();

		void stop();

	protected:
	private:
		bool											m_stoped;
		ESessionType									m_session_status;
		cwebsocket_mgr									m_websocket_mgr;
		rtc::scoped_refptr < chen::crtc_publisher>		m_rtc_publisher;
	};
}

#endif // 