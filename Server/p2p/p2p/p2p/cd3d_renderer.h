/***********************************************************************************************
created: 		2022-11-10

author:			chensong

purpose:		crtc_player
************************************************************************************************/
#ifndef _C_D3D_RENDERER_H_
#define _C_D3D_RENDERER_H_
#include "api/create_peerconnection_factory.h"
#include "clog.h"
#include <Windows.h>
#include <d3d9.h>
#pragma comment(lib, "d3d9.lib")  // located in DirectX SDK

#include "api/scoped_refptr.h"
#include "test/video_renderer.h"
#include "cvideo_render.h"
namespace chen {

	class D3dRenderer : public VideoRenderer {
	public:
		static D3dRenderer* Create(const char* window_title,
			size_t width,
			size_t height, webrtc::VideoTrackInterface* track_to_render);
		virtual ~D3dRenderer();

		void OnFrame(const webrtc::VideoFrame& frame) override;

	private:
		D3dRenderer(size_t width, size_t height, webrtc::VideoTrackInterface* track_to_render);

		static LRESULT WINAPI WindowProc(HWND hwnd,
			UINT msg,
			WPARAM wparam,
			LPARAM lparam);
		bool Init(const char* window_title);
		void Resize(size_t width, size_t height);
		void Destroy();

		size_t width_, height_;

		HWND hwnd_;
		rtc::scoped_refptr<IDirect3D9> d3d_;
		rtc::scoped_refptr<IDirect3DDevice9> d3d_device_;

		rtc::scoped_refptr<IDirect3DTexture9> texture_;
		rtc::scoped_refptr<IDirect3DVertexBuffer9> vertex_buffer_;
		rtc::scoped_refptr<webrtc::VideoTrackInterface> rendered_track_;
	};
}

#endif // 