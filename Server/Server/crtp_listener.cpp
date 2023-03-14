/***********************************************************************************************
created: 		2023-03-12

author:			chensong

purpose:		rtc_listener


************************************************************************************************/
#include "crtp_listener.h"

namespace chen {
	crtp_listener::~crtp_listener()
	{
	}
	void crtp_listener::add_producer(crtc_producer * producer)
	{
	}

	void crtp_listener::remove_producer(crtc_producer * producer)
	{

	}

	chen::crtc_producer * crtp_listener::get_producer(const RTC::RtpPacket * packet)
	{
		return NULL;
	}

	crtc_producer * crtp_listener::get_producer(uint32 ssrc) const
	{
		return nullptr;
	}

}