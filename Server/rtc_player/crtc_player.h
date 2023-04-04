/***********************************************************************************************
created: 		2022-11-10

author:			chensong

purpose:		crtc_player
************************************************************************************************/
#ifndef _C_RTC_PLAYER_H_
#define _C_RTC_PLAYER_H_
#include "cwebsocket_mgr.h"

#include "crtc_transport.h"
namespace chen 
{
	class crtc_player
	{
	public:
		crtc_player();
		~crtc_player();
		


	public:
		bool Init(const char * config_file);
		bool Loop();
		void Destroy();
		void stop();
	public:

		bool send_offer(const std::string & offer);
	private:

		void _presssmsg(std::list<std::string> & msgs);

		 
	private:


		bool m_stoped;
		cwebsocket_mgr					m_websocket_mgr;
		rtc::scoped_refptr<crtc_transport> m_player;
	};
	extern crtc_player		g_rtc_player;
}

#endif // _C_SDP_UTIL_H_