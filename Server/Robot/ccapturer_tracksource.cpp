/***********************************************************************************************
created: 		2022-08-24

author:			chensong

purpose:		log
************************************************************************************************/
#include "ccapturer_tracksource.h"
#include "cdesktop_capture.h"

namespace chen {
	rtc::scoped_refptr<ProxyVideoTrackSource> ProxyVideoTrackSource::Create() 
	{
		std::unique_ptr<  DesktopCapture> capturer(DesktopCapture::Create(30, 0));
		if (capturer)
		{
			capturer->StartCapture();
			/*return new
				rtc::RefCountedObject<CapturerTrackSource>(std::move(capturer));*/
			return new rtc::RefCountedObject<ProxyVideoTrackSource>(std::move(capturer));
		}
		return nullptr;
	}

	

}