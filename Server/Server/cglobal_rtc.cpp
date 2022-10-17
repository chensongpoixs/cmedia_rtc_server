/***********************************************************************************************
created: 		2022-10-17

author:			chensong

purpose:		cglobal_rtc


************************************************************************************************/
#include "cglobal_rtc.h"
#include "DtlsTransport.hpp"
#include "csctp_association_mgr.h"
#include "clog.h"
namespace chen {
	cglobal_rtc g_global_rtc;
	cglobal_rtc::cglobal_rtc()
	{
	}
	cglobal_rtc::~cglobal_rtc()
	{
	}
	bool cglobal_rtc::init()
	{
		if (!g_sctp_association_mgr.init())
		{
			return false;
		}
		SYSTEM_LOG("sctp_association_mgr init OK !!!");
		RTC::DtlsTransport::ClassInit();
		SYSTEM_LOG("DtlsTransport Init OK !!!");
		return true;
	}
	void cglobal_rtc::destory()
	{
		RTC::DtlsTransport::ClassDestroy();
		SYSTEM_LOG("DtlsTransport destroy OK !!!");
		g_sctp_association_mgr.destroy();
		SYSTEM_LOG("sctp_association_mrg destory OK !!!");
		 
	}
}