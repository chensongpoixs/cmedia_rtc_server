/***********************************************************************************************
created: 		2022-08-24

author:			chensong

purpose:		log
************************************************************************************************/
#include <iostream>
#include <csignal> // sigsuspend()
#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include "Robot.h"
void Stop(int i)
{
	chen::g_robot.stop();
}

void RegisterSignal()
{
	signal(SIGINT, Stop);
	signal(SIGTERM, Stop);

}

int main(int argc, char* argv[])
{
	 
	RegisterSignal();

	const char* config_filename = "server.cfg";
	if (argc > 1)
	{
		config_filename = argv[1];
	}
	const char* log_path = "./log";
	if (argc > 2)
	{
		log_path = argv[2];
	}
	bool init = chen::g_robot.init(log_path, config_filename);

	if (init)
	{
		init = chen::g_robot.Loop();
	}


	chen::g_robot.destroy();
	if (!init)
	{
		return 1;
	}
	return 0;


}
