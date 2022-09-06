/***********************************************************************************************
			created: 		2019-05-01

			author:			chensong

			purpose:		ccfg
************************************************************************************************/
#ifndef _C_CFG_H_
#define _C_CFG_H_
//#include "cconfig.h"
#include "cconfig.h"
//#include "csingleton.h"
namespace chen {
	enum ECNGIndex
	{

		ECI_TimeZone,
		ECI_TimeAdjust,
		ECI_LogPoolSize,
		//
		ECI_WebSocketWanIp,
		ECI_WebSocketWanPort,

		// RTC config
		ECI_RtcMinPort,
		ECI_RtcMaxPort,

		ECI_RtcAnnouncedIp,

		ECI_Max,
	};
	class ccfg : public cconfig
	{
	public:
	    explicit	ccfg();
		virtual	~ccfg();
	public:
		bool init(const char *file_name);
		void destroy();
	};

	extern 	ccfg g_cfg;
} //namespace chen

#endif //!#define _C_CFG_H_

