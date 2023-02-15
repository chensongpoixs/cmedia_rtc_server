/***********************************************************************************************
created: 		2019-03-02

author:			chensong

purpose:		log
************************************************************************************************/
#include "cclient.h"
#include "clog.h"

namespace chen {
	cclient::~cclient()
	{
	}
	bool cclient::init(const char * config_file)
	{

		WSADATA wsaData;
		WORD wVersionRequested = MAKEWORD(1, 0);
		WSAStartup(wVersionRequested, &wsaData);
		printf("log init ...\n");
		if (!LOG::init(chen::ELogStorageScreenFile))
		{
			return false;
		}
		SYSTEM_LOG("log init ok !!!");


		if (!g_cfg.init(config_file))
		{
			return false;
		}
		SYSTEM_LOG("config init ok !!!");

		
		LOG::set_level(static_cast<ELogLevelType>(g_cfg.get_uint32(ECI_LogLevel)));
		SYSTEM_LOG("setting log level  (%u) ok !!!", g_cfg.get_uint32(ECI_LogLevel));
		ctime_base_api::set_time_zone(g_cfg.get_int32(ECI_TimeZone));
		ctime_base_api::set_time_adjust(g_cfg.get_int32(ECI_TimeAdjust));
		SYSTEM_LOG("setting time zone (%u)ok !!!", g_cfg.get_int32(ECI_TimeZone));



		m_stoped = false;
		////////////////////////////////////////////////////
		SYSTEM_LOG("client init ok !!!");

		return true;
	}
	void cclient::Loop()
	{
		//ws://127.0.0.1:9500/?roomId=20220927&peerId=chensong
		std::string ws_url = "ws://" + g_cfg.get_string(ECI_WanIp) + ":" + g_cfg.get_int32(ECI_WanPort) + "/?roomId=" + g_cfg.get_string(ECI_Room_Name) + "&peerId=" + g_cfg.get_string(ECI_PeerId);
		SYSTEM_LOG("Loop run stoped = %d [ws_url = %s]", m_stoped, ws_url.c_str());
		while (!m_stoped)
		{
			switch (m_session_status)
			{
				case ESessionNone:
				{
					if (!m_websocket_mgr.init(ws_url))
					{
						WARNING_EX_LOG("websocket connect [url = %s] failed !!!", ws_url.c_str());
						break;
					}
					if (!m_websocket_mgr.startup())
					{
						m_session_status = ESessionDestroy;
						WARNING_EX_LOG("weboskcet startup failed !!!");
						break;
					}
					m_session_status = ESessionConnected;

					break;
				}
				case ESessionConnecting:
				{
					break;
				}
				case ESessionConnected:
				{
					/*
	rtc::scoped_refptr < chen::crtc_publisher> bublisher(new rtc::RefCountedObject<chen::crtc_publisher>( ));
	bublisher->create_offer();
					*/
					if (m_rtc_publisher)
					{
						//m_rtc_publisher->destroy();
						//m_rtc_publisher = nullptr;
					}
					m_rtc_publisher = new rtc::RefCountedObject<chen::crtc_publisher>();
					if (!m_rtc_publisher)
					{
						WARNING_EX_LOG("alloc rtc publisher failed !!!");
						m_session_status = ESessionDestroy;
						break;
					}
					m_session_status = ESessionCreateOffering;
					m_rtc_publisher->create_offer();
					
					break;
				}
				case ESessionCreateOffering:
				{
					break;
				}
				case ESessionCreateOffered:
				{
					break;
				}
				case ESessionSetAnswered:
				{
					break;
				}
				case ESessionDestroy:
				{
					//m_rtc_publisher == nullptr;
					m_websocket_mgr.destroy();
					m_session_status = ESessionNone;
					break;
				}
				default:
					break;
				}
		}
		SYSTEM_LOG("Loop exit !!! stoped = %d", m_stoped);
	}
	void cclient::stop()
	{

		SYSTEM_LOG(" stop server !!! stoped = %d", m_stoped);
		m_stoped = true;
	}
	void cclient::destroy()
	{
		SYSTEM_LOG("websocket destroy ...");
		m_websocket_mgr.destroy();
		SYSTEM_LOG("websocket destroy ok !!!");



		LOG::destroy();
		SYSTEM_LOG("log destroy ok !!!");
	}
}