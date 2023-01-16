/***********************************************************************************************
created: 		2023-01-15

author:			chensong

purpose:		_C_RTC_TRACK_DESCRIPTION_H_


************************************************************************************************/

#ifndef _C_RTC_CONSUMER_H_
#define _C_RTC_CONSUMER_H_
#include "cnet_type.h"
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include "crtc_sdp.h"
#include "cmedia_desc.h"
#include "crtx_paylod_des.h"
#include "cred_payload.h"
#include "cvideo_payload.h"
#include "caudio_payload.h"
#include "cred_payload.h"
namespace chen {
	class crtc_consumer
	{
	public:
		crtc_consumer(){}
		~crtc_consumer();
	protected:
	private:
	};
}

#endif // _C_RTC_CONSUMER_H_