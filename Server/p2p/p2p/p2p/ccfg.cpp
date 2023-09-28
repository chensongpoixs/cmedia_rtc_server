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
		 
		set_string(ECI_RoomName, "room_name", "chensong");
		set_string(ECI_ClientName, "client_name", "chensong");
		set_uint32(ECI_VideoFps, "video_fps", 30);
		set_string(ECI_RtcMediaIp, "rtc_media_ip", "127.0.0.1");
		set_uint32(ECI_RtcMediaPort, "rtc_media_port", 9500);
		
		 
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
