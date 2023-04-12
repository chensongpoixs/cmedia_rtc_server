/***********************************************************************************************
created: 		2023-02-18

author:			chensong

purpose:		_C_DTLS_ _H_


************************************************************************************************/
#include "cglobal_rtc_port.h"
#include "ccfg.h"
#include "clog.h"
namespace chen {
	cglobal_rtc_port g_global_rtc_port;
	cglobal_rtc_port::~cglobal_rtc_port()
	{
	}
	bool cglobal_rtc_port::init()
	{
		if (g_cfg.get_uint32(ECI_RtcMinPort) >= g_cfg.get_uint32(ECI_RtcMaxPort))
		{
			WARNING_EX_LOG("rtc min port = %u, max port = %u", g_cfg.get_uint32(ECI_RtcMinPort), g_cfg.get_uint32(ECI_RtcMaxPort));
			return false;
		}

		m_min_port = g_cfg.get_uint32(ECI_RtcMinPort);
		m_max_port = g_cfg.get_uint32(ECI_RtcMaxPort);
		m_cur_use_port = m_min_port - 1;
		return true;
	}
	void cglobal_rtc_port::update()
	{
	}
	void cglobal_rtc_port::destroy()
	{
	}
	uint32 cglobal_rtc_port::get_new_port()
	{
		uint32 port = 0;
		//if (m_unuse_port.empty())
		{
			//if ()
			port = ++m_cur_use_port;
			
			return port;
		}

		return port;
	}
	void cglobal_rtc_port::brack_port(uint32 port)
	{
	}
}