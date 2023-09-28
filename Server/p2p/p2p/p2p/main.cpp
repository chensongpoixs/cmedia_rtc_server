/***********************************************************************************************
					created: 		2022-11-10

					author:			chensong

					purpose:		async_log
************************************************************************************************/

#include <iostream>

#include "crtc_player.h"
#include "cmain_wnd.h"
int main(int argc, char *argv[])
{
	using namespace chen;
	g_rtc_player.Init("./client.cfg");
	g_rtc_player.Loop();
	g_rtc_player.Destroy();

	//chen::cmain_wnd main_wnd;

	//main_wnd.init();
	//MSG msg;

	//BOOL gm;
	//while ((gm = ::GetMessage(&msg, NULL, 0, 0)) != 0 && gm != -1)
	//{
	//	//if (!wnd.PreTranslateMessage(&msg))
	//	{
	//		::TranslateMessage(&msg);
	//		::DispatchMessage(&msg);
	//	}
	//}

	 while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	} 

	return EXIT_SUCCESS;
}
