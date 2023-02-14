#include <iostream>
#include "crtc_publisher.h"
#include <thread>
#include <chrono>
#include <deque>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "clog.h"
int main(int argc, char *argv[])
{
	chen::LOG::init(chen::ELogStorageScreenFile);
	WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD(1, 0);
	WSAStartup(wVersionRequested, &wsaData);
	//chen::crtc_publisher publisher(1);
	//chen::int32 ww = 5;
	rtc::scoped_refptr < chen::crtc_publisher> bublisher(new rtc::RefCountedObject<chen::crtc_publisher>( ));
	bublisher->create_offer();
	//chen::crtc_publisher publisher ;
	//publisher.create_offer();
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	return EXIT_SUCCESS;
}