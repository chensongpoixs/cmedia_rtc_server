/***********************************************************************************************
created: 		2023-01-15

author:			chensong

purpose:		rtc_user_config


************************************************************************************************/

#ifndef _C_GLOBAL_RTC_CONFIG_H_
#define _C_GLOBAL_RTC_CONFIG_H_
#include "cnet_type.h"
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include "crtc_sdp.h"


namespace chen {

	struct crtc_ssrc_info
	{
		uint32		m_audio_ssrc;
		uint32		m_video_ssrc;
		uint32		m_rtx_video_ssrc;
		crtc_ssrc_info() 
			: m_audio_ssrc(0)
			, m_video_ssrc(0)
			, m_rtx_video_ssrc(0)
		{}
	};
	class cglobal_rtc_config
	{
	public:
		cglobal_rtc_config()
			: m_global_rtc_ssrc_infos(){}
		~cglobal_rtc_config();

	public:
		bool init();
		void update();
		void destroy();


	public:
		crtc_ssrc_info * get_stream_uri(const std::string& uri);


	protected:
	private:

		std::map<std::string, crtc_ssrc_info*>			m_global_rtc_ssrc_infos;


	};

	extern  cglobal_rtc_config g_global_rtc_config;
}

#endif //_C_GLOBAL_RTC_CONFIG_H_