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

		set_string(ECI_WebSocketHost, "websocket_ip", "127.0.0.1");
		set_uint32(ECI_WebSocketPort, "websocket_port", 9500);
		set_uint32(ECI_WebSocketReconnect, "websocket_reconnect", 30);
		set_string(ECI_Room_Name, "room_name", "chensong");
		set_string(ECI_Client_Name, "client_name", "chensong");

		load_cfg_file();

		return true;
	}
	void ccfg::destroy()
	{
		cconfig::destroy();
	}
}//namespace chen 
