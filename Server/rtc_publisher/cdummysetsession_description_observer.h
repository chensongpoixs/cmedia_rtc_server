/***********************************************************************************************
created: 		2023-02-13

author:			chensong

purpose:		api_rtc_publish


************************************************************************************************/

#ifndef _C_DUMMYSETSESSION_DESCRTION_OBSERVER_H_
#define _C_DUMMYSETSESSION_DESCRTION_OBSERVER_H_
#include "api/media_stream_interface.h"
#include "api/peer_connection_interface.h"
#include <string>
#include "cnet_types.h"
#include "rtc_base/logging.h"
namespace chen {
	class cdummysetsession_description_observer
		: public webrtc::SetSessionDescriptionObserver {
	public:
		static cdummysetsession_description_observer* Create() {
			return new rtc::RefCountedObject<cdummysetsession_description_observer>();
		}
		virtual void OnSuccess() { RTC_LOG(INFO) << __FUNCTION__; }
		virtual void OnFailure(webrtc::RTCError error) {
			RTC_LOG(INFO) << __FUNCTION__ << " " << ToString(error.type()) << ": "
				<< error.message();
		}
	};
}

#endif//_C_DUMMYSETSESSION_DESCRTION_OBSERVER_H_