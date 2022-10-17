﻿/***********************************************************************************************
created: 		2022-10-17

author:			chensong

purpose:		cglobal_rtc


************************************************************************************************/

#ifndef _C_GLOBAL_RTC_H_
#define _C_GLOBAL_RTC_H_
#include "cnoncopyable.h"
namespace chen {
	class cglobal_rtc : public cnoncopyable
	{
	public :
		explicit cglobal_rtc();
		virtual ~cglobal_rtc();

		
	public:
		  	bool init();
		  void destory();
	};
	extern cglobal_rtc g_global_rtc;
}

#endif // _C_GLOBAL_RTC_H_