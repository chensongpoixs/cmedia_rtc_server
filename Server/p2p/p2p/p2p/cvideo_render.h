/***********************************************************************************************
created: 		2022-11-10

author:			chensong

purpose:		crtc_player
************************************************************************************************/
#ifndef _C_VIDEO_RENDER_H_
#define _C_VIDEO_RENDER_H_
#include "api/create_peerconnection_factory.h"
#include "clog.h"
#include <map>
#include <memory>
#include <string>
#include <stddef.h>

#include "api/video/video_sink_interface.h"
#include "api/media_stream_interface.h"
#include "api/video/video_frame.h"
 
#include "media/base/media_channel.h"
#include "media/base/video_common.h"
#if defined(WEBRTC_WIN)
#include "rtc_base/win32.h"
#endif  // WEBRTC_WIN
namespace chen {
	// A little helper class to make sure we always to proper locking and
  // unlocking when working with VideoRenderer buffers.
	template <typename T>
	class AutoLock {
	public:
		explicit AutoLock(T* obj) : obj_(obj) { obj_->Lock(); }
		~AutoLock() { obj_->Unlock(); }

	protected:
		T* obj_;
	};
	class cvideo_render : public rtc::VideoSinkInterface<webrtc::VideoFrame>
	{
	public:
		cvideo_render(/*HWND wnd,*/ int width, int height, 	webrtc::VideoTrackInterface* track_to_render);
		virtual ~cvideo_render();
	public:
		void Lock() { ::EnterCriticalSection(&buffer_lock_); }

		void Unlock() { ::LeaveCriticalSection(&buffer_lock_); }

		// VideoSinkInterface implementation
		void OnFrame(const webrtc::VideoFrame& frame) override;

		const BITMAPINFO& bmi() const { return bmi_; }
		const uint8_t* image() const { return image_.get(); }

	protected:
		void SetSize(int width, int height);

		enum {
			SET_SIZE,
			RENDER_FRAME,
		};

		HWND hwnd_;
		BITMAPINFO bmi_;
		std::unique_ptr<uint8_t[]> image_;
		CRITICAL_SECTION buffer_lock_;
		rtc::scoped_refptr<webrtc::VideoTrackInterface> rendered_track_;
	protected:
	private:
	};

	class webrtc::VideoFrame;
	class VideoRenderer : public rtc::VideoSinkInterface<webrtc::VideoFrame> {
	public:
		// Creates a platform-specific renderer if possible, or a null implementation
		// if failing.
		static VideoRenderer* Create(const char* window_title,
			size_t width,
			size_t height, webrtc::VideoTrackInterface* track_to_render);
		// Returns a renderer rendering to a platform specific window if possible,
		// NULL if none can be created.
		// Creates a platform-specific renderer if possible, returns NULL if a
		// platform renderer could not be created. This occurs, for instance, when
		// running without an X environment on Linux.
		static VideoRenderer* CreatePlatformRenderer(const char* window_title,
			size_t width,
			size_t height, webrtc::VideoTrackInterface* track_to_render);
		virtual ~VideoRenderer() {}

	protected:
		VideoRenderer() {}
	};
}


#endif // _C_VIDEO_RENDER_H_
