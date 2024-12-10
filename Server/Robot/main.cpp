#include <iostream>
#include <csignal> // sigsuspend()
#include <cstdlib>
#include <iostream>
#include <string>
#include "cmediasoup_mgr.h"

#include "crtc_publisher.h"
#include <thread>
#include <chrono>
 
//cmediasoup::cmediasoup_mgr g_mediasoup_mgr;


bool stoped = false;

void signalHandler(int signum)
{
	stoped = true;
	
}



int  main(int argc, char *argv[])
{
	//rtc::scoped_refptr < chen::crtc_publisher> bublisher(new rtc::RefCountedObject<chen::crtc_publisher>());
	//bublisher->create_offer();
	//////chen::crtc_publisher publisher ;
	//////publisher.create_offer();
	//while (true)
	//{
	//	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	//}

	 signal(SIGINT, signalHandler);
	signal(SIGTERM, signalHandler); 
	
	cmediasoup::cmediasoup_mgr::global_init();

	cmediasoup::cmediasoup_mgr g_mediasoup_mgr1;
	//cmediasoup::cmediasoup_mgr g_mediasoup_mgr2;
	g_mediasoup_mgr1.init(5);
	//g_mediasoup_mgr2.init(5);

	//g_mediasoup_mgr.set_mediasoup_status_callback(&mediasoup_callback);
	/*
	const char* mediasoupIp, uint16_t port
		, const char* roomName, const char* clientName
	
	*/
	g_mediasoup_mgr1.startup("127.0.0.1", 9000, "chensong44", "chensong44");
	//g_mediasoup_mgr2.startup("127.0.0.1", 8888, "chensong2", "chensong2");
	while (!stoped)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	g_mediasoup_mgr1.destroy();
	//g_mediasoup_mgr2.destroy();
	cmediasoup::cmediasoup_mgr::global_destroy();
	return 0;
}