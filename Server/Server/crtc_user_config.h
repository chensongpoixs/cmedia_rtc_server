/***********************************************************************************************
created: 		2023-01-15

author:			chensong

purpose:		rtc_user_config


************************************************************************************************/

#ifndef _C_RTC_USER_CONFIG_H_
#define _C_RTC_USER_CONFIG_H_
#include "cnet_type.h"
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include "crtc_sdp.h"


namespace chen {
	// The user config for RTC publish or play.
	class crtc_user_config
	{



	public:
		crtc_user_config()
			: m_remote_sdp_str("")
			, m_remote_sdp()
			, m_eip("")
			, m_codec ("")
			, m_api("")
			, m_local_sdp_str("")
			, m_session_id("")
			, m_publish(false)
			, m_dtls(false)
			, m_srtp(false){}
		 ~crtc_user_config();
	 protected:
	 private:
	public:
		// Original variables from API.
		std::string				m_remote_sdp_str;
		crtc_sdp				m_remote_sdp;
		std::string				m_eip;
		std::string				m_codec;
		std::string				m_api;

		// Session data.
		std::string				m_local_sdp_str;
		std::string				m_session_id;

		// Generated data.
		//Request* req;
		bool					m_publish;
		bool					m_dtls;
		bool					m_srtp;

		// The order of audio and video, or whether audio is before video. Please make sure the order is match for offer and
		// answer, or client might fail at setRemoveDescription(answer). See  
		bool					m_audio_before_video;
	 };



}

#endif // _C_RTC_USER_CONFIG_H_