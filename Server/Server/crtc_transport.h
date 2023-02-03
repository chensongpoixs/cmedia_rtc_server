/***********************************************************************************************
created: 		2023-02-03

author:			chensong

purpose:		crtc_transport


************************************************************************************************/

#ifndef _C_RTC_TRANSPORT_H_
#define _C_RTC_TRANSPORT_H_
#include "cnet_type.h"
#include "crtc_sdp.h"
namespace chen {


	class crtc_transport
	{
	public:
		explicit crtc_transport() {}
		virtual ~crtc_transport();
	protected:
	private:
		crtc_sdp			*			m_local_sdp_ptr;
		crtc_sdp			*			m_remote_sdp_ptr;
	};


}

#endif // _C_RTC_TRANSPORT_H_