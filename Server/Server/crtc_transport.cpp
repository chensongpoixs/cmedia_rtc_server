/***********************************************************************************************
created: 		2023-02-03

author:			chensong

purpose:		crtc_transport


************************************************************************************************/
#include "crtc_transport.h"
#include "clog.h"


namespace chen {
	crtc_transport::~crtc_transport()
	{
		int32  count = 34;
		NORMAL_EX_LOG("%" PRIu32 "", count);
	}
}