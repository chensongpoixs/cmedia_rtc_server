/***********************************************************************************************
created: 		2022-11-10

author:			chensong

purpose:		crtc_player
************************************************************************************************/
#include "cvideo_render.h"
#include "third_party/libyuv/include/libyuv/convert_argb.h"

#include "api/video/i420_buffer.h"

namespace chen {

//#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ | D3DFVF_TEX1)
//	const char kD3DClassName[] = "d3d_renderer";
	cvideo_render::cvideo_render(/*HWND wnd,*/ int width, int height, webrtc::VideoTrackInterface* track_to_render)
	{
		::InitializeCriticalSection(&buffer_lock_);
		rendered_track_ = track_to_render;
		/*::InitializeCriticalSection(&buffer_lock_);
		ZeroMemory(&bmi_, sizeof(bmi_));
		bmi_.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi_.bmiHeader.biPlanes = 1;
		bmi_.bmiHeader.biBitCount = 32;
		bmi_.bmiHeader.biCompression = BI_RGB;
		bmi_.bmiHeader.biWidth = width;
		bmi_.bmiHeader.biHeight = -height;
		bmi_.bmiHeader.biSizeImage =
			width * height * (bmi_.bmiHeader.biBitCount >> 3);*/
		if (!rendered_track_)
		{
			WARNING_EX_LOG("rendered_track_ = NULLptr;");
			return;
		}
		rendered_track_->AddOrUpdateSink(this, rtc::VideoSinkWants());
	}

	cvideo_render::~cvideo_render()
	{
		if (rendered_track_)
		{
			rendered_track_->RemoveSink(this);
		}
		
		::DeleteCriticalSection(&buffer_lock_);
	}

	void cvideo_render::OnFrame(const webrtc::VideoFrame& video_frame)
	{
		{
			AutoLock<cvideo_render> lock(this);

			rtc::scoped_refptr<webrtc::I420BufferInterface> buffer(
				video_frame.video_frame_buffer()->ToI420());
			if (video_frame.rotation() != webrtc::kVideoRotation_0)
			{
				buffer = webrtc::I420Buffer::Rotate(*buffer, video_frame.rotation());
			}
			
			static FILE  *out_video_render_ptr = fopen("./video_render.yuv", "wb+");
			if (out_video_render_ptr)
			{
				NORMAL_EX_LOG("[width = %u][height = %u]", buffer->width(), buffer->height());
				::fwrite(buffer->DataY(), buffer->width() * buffer->height() ,1,out_video_render_ptr);
				::fflush(out_video_render_ptr);
			}
			//SetSize(buffer->width(), buffer->height());




			/*RTC_DCHECK(image_.get() != NULL);
			libyuv::I420ToARGB(buffer->DataY(), buffer->StrideY(), buffer->DataU(),
				buffer->StrideU(), buffer->DataV(), buffer->StrideV(),
				image_.get(),
				bmi_.bmiHeader.biWidth * bmi_.bmiHeader.biBitCount / 8,
				buffer->width(), buffer->height());*/
		}
	//	InvalidateRect(wnd_, NULL, TRUE);
	}

	void cvideo_render::SetSize(int width, int height)
	{
		AutoLock<cvideo_render> lock(this);

		if (width == bmi_.bmiHeader.biWidth && height == bmi_.bmiHeader.biHeight)
		{
			return;
		}

		bmi_.bmiHeader.biWidth = width;
		bmi_.bmiHeader.biHeight = -height;
		bmi_.bmiHeader.biSizeImage =
			width * height * (bmi_.bmiHeader.biBitCount >> 3);
		image_.reset(new uint8_t[bmi_.bmiHeader.biSizeImage]);
	}
	class NullRenderer : public VideoRenderer {
		void OnFrame(const webrtc:: VideoFrame& video_frame) override {}
	};
	VideoRenderer* VideoRenderer::Create(const char* window_title,
		size_t width,
		size_t height, webrtc::VideoTrackInterface* track_to_render) {
		VideoRenderer* renderer = CreatePlatformRenderer(window_title, width, height, track_to_render);
		if (renderer != nullptr)
		{
			return renderer;
		}

		return new NullRenderer();
	}
}