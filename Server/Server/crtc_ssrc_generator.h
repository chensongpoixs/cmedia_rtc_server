/***********************************************************************************************
created: 		2023-01-15

author:			chensong

purpose:		_C_RTC_SSRC_GENERATOR_H_


************************************************************************************************/

#ifndef _C_RTC_SSRC_GENERATOR_H_
#define _C_RTC_SSRC_GENERATOR_H_
#include "cnet_type.h"
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include "crtc_sdp.h"
#include "cmedia_desc.h"
#include "csingleton.h"
namespace chen {
	class crtc_ssrc_generator
	{
	public:
		crtc_ssrc_generator()
			: m_ssrc_num(0) {}
		~crtc_ssrc_generator();
	public:
		uint32  generate_ssrc();
	protected:
	private:
		uint32				m_ssrc_num;
	};

}
#define c_rtc_ssrc_generator chen::csingleton<chen::crtc_ssrc_generator>::get_instance()
#endif // _C_RTC_SSRC_GENERATOR_H_