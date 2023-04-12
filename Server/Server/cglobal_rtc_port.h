/***********************************************************************************************
created: 		2023-02-18

author:			chensong

purpose:		_C_DTLS_ _H_


************************************************************************************************/

#ifndef _C_GLOBAL_RTC_PORT_H_
#define _C_GLOBAL_RTC_PORT_H_
#include "cnet_type.h"
#include <string>
#include <vector>
#include <set>
#include <set>

namespace chen {
	class cglobal_rtc_port
	{
	public:
		cglobal_rtc_port() 
		: m_min_port(0)
		, m_max_port(0)
		, m_cur_use_port(0)
		, m_unuse_port(){}
		~cglobal_rtc_port();


	public:
		bool init();
		void update();
		void destroy();
	public:
		uint32 get_new_port();
		void   brack_port(uint32 port);
	protected:
	private:


		uint32				m_min_port;
		uint32				m_max_port;
		uint32				m_cur_use_port;
		std::set<uint32>    m_unuse_port;

	};
	extern cglobal_rtc_port g_global_rtc_port;
}

#endif //_C_GLOBAL_RTC_PORT_H_