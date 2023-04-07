/***********************************************************************************************
				created: 		2019-05-01

				author:			chensong

				purpose:		ccfg
************************************************************************************************/
#include "ccfg.h"
#include "clog.h"
namespace chen {
	ccfg g_cfg;
	
	ccfg::ccfg()
	{
	}


	ccfg::~ccfg()
	{
	}
	bool ccfg::init(const char * file_name)
	{
		if (!cconfig::init(ECI_Max, file_name))
		{
			return false;
		}
		// 默认值
		 
//#if _DEBUG
//		set_uint32(ECI_LogLevel, "log_level", ELogLevel_Num);
//#endif // _DEBUG

		
		set_int32(ECI_TimeAdjust, "time_adjust", 0);
		
		set_uint32(ECI_LogPoolSize, "log_pool_size", 10);
		
		set_string(ECI_WebSocketWanIp, "websocket_wan_ip", "0.0.0.0");
		set_uint32(ECI_WebSocketWanPort, "websocket_wan_port", 9500);

		set_uint32(ECI_RtcMinPort, "rtc_min_port", 50000); // 
		set_uint32(ECI_RtcMaxPort, "rtc_max_port", 60000);
		set_string(ECI_RtcAnnouncedIp, "rtc_announced_ip", "127.0.0.1");

		set_uint32(ECI_LogLevel, "log_level", ELogLevel_Num);

		set_uint32(ECI_StunTimeOut, "stun_time_out", 30000);

		//配置表的值
		load_cfg_file();

		return true;
	}
	void ccfg::destroy()
	{
		cconfig::destroy();
	}
}//namespace chen 
