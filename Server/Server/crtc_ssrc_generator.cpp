/***********************************************************************************************
created: 		2023-01-15

author:			chensong

purpose:		_C_RTC_SSRC_GENERATOR_H_


************************************************************************************************/
#include "crtc_ssrc_generator.h"
#include "crandom.h"
namespace chen {
	crtc_ssrc_generator::~crtc_ssrc_generator()
	{
	}
	uint32 crtc_ssrc_generator::generate_ssrc()
	{
		if (!m_ssrc_num) 
		{
			m_ssrc_num = c_rand.rand(100100, 9999999);//::getpid() * 10000 + ::getpid() * 100 + ::getpid();
		}
		return ++m_ssrc_num;
	}
}