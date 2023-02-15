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
		 



		set_int32(ECI_TimeZone, "time_zone", 8);
		set_int32(ECI_TimeAdjust, "time_adjust", 0);
		set_uint32(ECI_LogLevel, "log_level", ELogLevel_Info);
		set_string(ECI_WanIp, "wan_ip", "127.0.0.1");
		set_int32(ECI_WanPort, "wan_port", 9999);
		set_string(ECI_RoomName, "room_name", "chensong");
		set_string(ECI_PeerId, "peer_id", "chensong");

		  
#if _DEBUG
		set_uint32(ECI_LogLevel, "log_level", ELogLevel_Num);
#endif // _DEBUG
		 
		//配置表的值
		load_cfg_file();

		return true;
	}
	void ccfg::destroy()
	{
		cconfig::destroy();
	}
}//namespace chen 
