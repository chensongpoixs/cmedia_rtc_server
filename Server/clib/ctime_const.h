/********************************************************************
created:	2019-03-24

author:		chensong

purpose:	time const

*********************************************************************/
#ifndef _C_TIME_CONST_H
#define _C_TIME_CONST_H

namespace chen
{
	// ʱ�䳣��
	enum ETimeConstant
	{
		ETC_Minute = 60,					// ����, ��λ1s
		ETC_QuarterHour = 15 * ETC_Minute,		// һ����, ��λ1s
		ETC_HalfHour = 30 * ETC_Minute,		// ��Сʱ, ��λ1s
		ETC_Hour = 60 * ETC_Minute,		// Сʱ, ��λ1s
		ETC_Day = 24 * ETC_Hour,		// ��, ��λ1s
		ETC_Week = 7 * ETC_Day,			// ��, ��λs
	};

	static const int ASCII_DATETIME_LEN = 20; //2016-04-24 14:06:59
}  // namespace chen

#endif //!#define _C_TIME_CONST_H

