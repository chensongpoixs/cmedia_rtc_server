/***********************************************************************************************
					created: 		2022-11-10

					author:			chensong

					purpose:		async_log
************************************************************************************************/

#include <iostream>

#include "crtc_player.h"

int main(int argc, char *argv[])
{
	using namespace chen;
	g_rtc_player.Init("./client.cfg");
	g_rtc_player.Loop();
	g_rtc_player.Destroy();
	return EXIT_SUCCESS;
}
