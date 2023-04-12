/***********************************************************************************************
created: 		2023-01-15

author:			chensong

purpose:		rtc_user_config


************************************************************************************************/
#include "cglobal_rtc_config.h"
#include "crtc_ssrc_generator.h"
#include "clog.h"
namespace chen {
	cglobal_rtc_config g_global_rtc_config;
	cglobal_rtc_config::~cglobal_rtc_config()
	{
	}
	bool cglobal_rtc_config::init()
	{
		return true;
	}
	void cglobal_rtc_config::update()
	{
	}
	void cglobal_rtc_config::destroy()
	{
		for (std::map<std::string, crtc_ssrc_info*>::iterator iter = m_global_rtc_ssrc_infos.begin(); 
			iter != m_global_rtc_ssrc_infos.end(); ++iter)
		{
			if (iter->second)
			{
				delete iter->second;
			}
		}
		m_global_rtc_ssrc_infos.clear();
	}
	crtc_ssrc_info * cglobal_rtc_config::get_stream_uri(const std::string & uri)
	{
		std::map<std::string, crtc_ssrc_info*>::iterator iter =  m_global_rtc_ssrc_infos.find(uri);
		if (iter != m_global_rtc_ssrc_infos.end())
		{
			return iter->second;
		}
		crtc_ssrc_info * ssrc_ptr = new crtc_ssrc_info();
		ssrc_ptr->m_audio_ssrc = c_rtc_ssrc_generator.generate_ssrc();
		ssrc_ptr->m_video_ssrc = c_rtc_ssrc_generator.generate_ssrc();
		ssrc_ptr->m_rtx_video_ssrc = c_rtc_ssrc_generator.generate_ssrc();
		if (!m_global_rtc_ssrc_infos.insert(std::make_pair(uri, ssrc_ptr)).second)
		{
			WARNING_EX_LOG("global rtc ssrc insert failed !!! (uri = %s)", uri.c_str());
			return NULL;
		}
		return ssrc_ptr;
	//c_rtc_ssrc_generator.generate_ssrc()
		return nullptr;
	}
}