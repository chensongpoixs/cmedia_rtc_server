/***********************************************************************************************
created: 		2023-02-14

author:			chensong

purpose:		api_rtc_publish


************************************************************************************************/

#ifndef _C_CAPTUERER_TRACK_SOURCE_H_
#define _C_CAPTUERER_TRACK_SOURCE_H_
#include "api/media_stream_interface.h"
#include "api/peer_connection_interface.h"
#include "pc/video_track_source.h"
#include <string>
#include "desktop_capture_source.h"
#include "desktop_capture.h"
namespace chen {
	class ccapturer_track_source : public webrtc::VideoTrackSource
	{
	public:
		static rtc::scoped_refptr<ccapturer_track_source> Create() {
			/*const size_t kWidth = 640;
			const size_t kHeight = 480;
			const size_t kFps = 30;
			std::unique_ptr<webrtc::test::VcmCapturer> capturer;
			std::unique_ptr<webrtc::VideoCaptureModule::DeviceInfo> info(
				webrtc::VideoCaptureFactory::CreateDeviceInfo());
			if (!info) {
			  return nullptr;
			}
			int num_devices = info->NumberOfDevices();
			for (int i = 0; i < num_devices; ++i) {
			  capturer = absl::WrapUnique(
				  webrtc::test::VcmCapturer::Create(kWidth, kHeight, kFps, i));
			  if (capturer) {
				return new
					rtc::RefCountedObject<CapturerTrackSource>(std::move(capturer));
			  }
			}*/
			std::unique_ptr<webrtc_demo::DesktopCapture> capturer(webrtc_demo::DesktopCapture::Create(25, 0));
			if (capturer)
			{
				capturer->StartCapture();
				return new
					rtc::RefCountedObject<ccapturer_track_source>(std::move(capturer));
			}
			return nullptr;
		}

	protected:
		explicit ccapturer_track_source(
			std::unique_ptr<webrtc_demo::DesktopCapture> capturer)
			: VideoTrackSource(/*remote=*/false), capturer_(std::move(capturer)) {}

	private:
		rtc::VideoSourceInterface<webrtc::VideoFrame>* source() override {
			return capturer_.get();
		}
		//std::unique_ptr<webrtc::test::VcmCapturer> capturer_;
		std::unique_ptr<webrtc_demo::DesktopCapture> capturer_;
	public:
	protected:
	private:
	};
}// namespace chen 

#endif // ccapturer_track_source