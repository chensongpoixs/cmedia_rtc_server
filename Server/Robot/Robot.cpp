/***********************************************************************************************
created: 		2022-09-11

author:			chensong

purpose:		robot
************************************************************************************************/
#include "Robot.h"
#include "clog.h"
#include "ccfg.h"
#include "cclient.h"
namespace chen {
	Robot g_robot;
	Robot::~Robot()
	{
	}
	bool Robot::init(const char* log_path, const char* config_file)
	{
		printf("LOG init ...");
		if (!LOG::init( log_path, "Robot"))
		{
			return false;
		}
		if (!g_cfg.init(config_file))
		{
			return false;
		}
		return true;;
	}
	bool Robot::Loop()
	{
		//while (!m_stoped)
		//{
		//	//uDelta += time_elapse.get_elapse();

		//	////	g_game_client.update(uDelta);
		//	//g_wan_server.update(uDelta);


		//	//g_room_mgr.update(uDelta);


		//	//uDelta = time_elapse.get_elapse();

		//	//if (uDelta < TICK_TIME)
		//	//{
		//	//	std::this_thread::sleep_for(std::chrono::milliseconds(TICK_TIME - uDelta));
		//	//}
		//}
		// 
		// const std::string & mediasoupIp, uint16_t port, const std::string & roomName, const std::string & clientName
		s_client.Loop();
		SYSTEM_LOG("Leave main loop");

			return true;
	}
	void Robot::destroy()
	{
		s_client.Destory();
		SYSTEM_LOG("client Destroy OK !!!");
		SYSTEM_LOG(" cfg  destroy OK !!!");
		g_cfg.destroy();

		/*g_msg_dispatch.destroy();
		SYSTEM_LOG("msg dispath destroy OK !!!");*/

		//1 log
		LOG::destroy();
		printf("Log Destroy OK!\n");
	}
	void Robot::stop()
	{
		m_stoped = true;
		s_client.stop();
	}
}