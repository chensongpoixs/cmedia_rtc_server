/***********************************************************************************************
created: 		2023-02-10

author:			chensong

purpose:		crtc_transport


************************************************************************************************/

#ifndef _C_RTC_TRANSPORT_DEFINE_H_
#define _C_RTC_TRANSPORT_DEFINE_H_
#include "cnet_type.h"
#include "crtc_sdp.h"
#include "crtc_source_description.h"
namespace chen {
	enum ERtcNetworkState
	{
		ERtcNetworkStateInit = -1,
		ERtcNetworkStateWaitingAnswer = 1,
		ERtcNetworkStateWaitingStun = 2,
		ERtcNetworkStateDtls = 3,
		ERtcNetworkStateEstablished = 4,
		ERtcNetworkStateClosed = 5,
	};
}

#endif // _C_RTC_TRANSPORT_DEFINE_H_