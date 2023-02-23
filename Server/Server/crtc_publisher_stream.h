/***********************************************************************************************
created: 		2023-02-23

author:			chensong

purpose:		rtc_connection


************************************************************************************************/

#ifndef _C_RTC_PUBLISHER_STREAM_H_
#define _C_RTC_PUBLISHER_STREAM_H_
#include "cnet_type.h"
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include "crtc_sdp.h"


namespace chen {

	class crtc_publisher_stream
	{
	public:
		explicit crtc_publisher_stream(){}
		virtual ~crtc_publisher_stream();
	protected:
	private:
	};
}

#endif // _C_RTC_PLAYER_STREAM_H_