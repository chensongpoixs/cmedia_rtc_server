/***********************************************************************************************
created: 		2022-08-24

author:			chensong

purpose:		log
************************************************************************************************/

#include "desktop_capture_source.h"

#include "api/video/i420_buffer.h"
#include "api/video/video_rotation.h"
#include "rtc_base/logging.h"
#include <chrono>
#include "third_party/libyuv/include/libyuv.h"
#include "clog.h"

namespace chen {

	void VideoCaptureSource::AddOrUpdateSink(
		rtc::VideoSinkInterface<webrtc::VideoFrame>* sink,
		const rtc::VideoSinkWants& wants) {
	  broadcaster_.AddOrUpdateSink(sink, wants);
	  UpdateVideoAdapter();
	}
	VideoCaptureSource* VideoCaptureSource::Create()
	{
		 std::unique_ptr<VideoCaptureSource> Source_ptr(new VideoCaptureSource());
		 if (Source_ptr)
		 {
			 return Source_ptr.release();
		 }
		 return nullptr;
	}
	void VideoCaptureSource::RemoveSink(
		rtc::VideoSinkInterface<webrtc::VideoFrame>* sink) {
	  broadcaster_.RemoveSink(sink);
	  UpdateVideoAdapter();
	}

	void VideoCaptureSource::UpdateVideoAdapter() {
	  //video_adapter_.OnSinkWants(broadcaster_.wants());
		rtc::VideoSinkWants wants = broadcaster_.wants();
		video_adapter_.OnResolutionFramerateRequest(
			wants.target_pixel_count, wants.max_pixel_count, wants.max_framerate_fps);
	}
	void VideoCaptureSource::VideoOnFrame(const webrtc::VideoFrame& frame)
	{
		OnFrame(frame);
	}
	void VideoCaptureSource::OnFrame(const webrtc::VideoFrame& frame) 
	{
		broadcaster_.OnFrame(frame);
		return;


	
	}


}