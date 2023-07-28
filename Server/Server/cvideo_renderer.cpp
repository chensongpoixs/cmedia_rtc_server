/***********************************************************************************************
					created: 		2019-05-13

					author:			chensong

					purpose:		msg_base_id
					输赢不重要，答案对你们有什么意义才重要。

					光阴者，百代之过客也，唯有奋力奔跑，方能生风起时，是时代造英雄，英雄存在于时代。或许世人道你轻狂，可你本就年少啊。 看护好，自己的理想和激情。


					我可能会遇到很多的人，听他们讲好2多的故事，我来写成故事或编成歌，用我学来的各种乐器演奏它。
					然后还可能在一个国家遇到一个心仪我的姑娘，她可能会被我帅气的外表捕获，又会被我深邃的内涵吸引，在某个下雨的夜晚，她会全身淋透然后要在我狭小的住处换身上的湿衣服。
					3小时候后她告诉我她其实是这个国家的公主，她愿意向父皇求婚。我不得已告诉她我是穿越而来的男主角，我始终要回到自己的世界。
					然后我的身影慢慢消失，我看到她眼里的泪水，心里却没有任何痛苦，我才知道，原来我的心被丢掉了，我游历全世界的原因，就是要找回自己的本心。
					于是我开始有意寻找各种各样失去心的人，我变成一块砖头，一颗树，一滴水，一朵白云，去听大家为什么会失去自己的本心。
					我发现，刚出生的宝宝，本心还在，慢慢的，他们的本心就会消失，收到了各种黑暗之光的侵蚀。
					从一次争论，到嫉妒和悲愤，还有委屈和痛苦，我看到一只只无形的手，把他们的本心扯碎，蒙蔽，偷走，再也回不到主人都身边。
					我叫他本心猎手。他可能是和宇宙同在的级别 但是我并不害怕，我仔细回忆自己平淡的一生 寻找本心猎手的痕迹。
					沿着自己的回忆，一个个的场景忽闪而过，最后发现，我的本心，在我写代码的时候，会回来。
					安静，淡然，代码就是我的一切，写代码就是我本心回归的最好方式，我还没找到本心猎手，但我相信，顺着这个线索，我一定能顺藤摸瓜，把他揪出来。
************************************************************************************************/
#include "cvideo_renderer.h"
#include "libyuv.h"
#include "clog.h"
namespace chen {


#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ | D3DFVF_TEX1)

	struct D3dCustomVertex {
		float x, y, z;
		float u, v;
	};

	const char kD3DClassName[] = "rtc_renderer";




	cvideo_renderer g_video_renderer;
	bool cvideo_renderer::init(const char * window_title, uint32 width, uint32 height)
	{


		static ATOM wc_atom = 0;
		if (wc_atom == 0) {
			WNDCLASSA wc = {};

			wc.style = CS_HREDRAW | CS_VREDRAW;
			wc.lpfnWndProc = WindowProc;
			wc.hCursor = LoadCursor(NULL, IDC_ARROW);
			wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW);
			wc.lpszClassName = kD3DClassName;

			wc_atom = RegisterClassA(&wc);
			if (wc_atom == 0)
			{
				WARNING_EX_LOG("");
				return false;
			}
		}

		hwnd_ = CreateWindowA(kD3DClassName, window_title, WS_OVERLAPPEDWINDOW, 0, 0,
			static_cast<int>(width_), static_cast<int>(height_),
			NULL, NULL, NULL, NULL);

		if (hwnd_ == NULL) {
			Destroy();
			return false;
		}

		d3d_ = Direct3DCreate9(D3D_SDK_VERSION);
		if (d3d_ == nullptr) {
			Destroy();
			return false;
		}

		D3DPRESENT_PARAMETERS d3d_params = {};

		d3d_params.Windowed = TRUE;
		d3d_params.SwapEffect = D3DSWAPEFFECT_COPY;
		d3d_params.BackBufferWidth = width;
		d3d_params.BackBufferHeight = height;
		d3d_params.BackBufferFormat = D3DFMT_X8R8G8B8;
		d3d_params.BackBufferCount = 1;
		d3d_params.MultiSampleType = D3DMULTISAMPLE_NONE;
		d3d_params.MultiSampleQuality = D3DMULTISAMPLE_NONE;
		d3d_params.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3d_params.hDeviceWindow = hwnd_;
		d3d_params.Windowed = hwnd_ == NULL;
		d3d_params.EnableAutoDepthStencil = true;
		d3d_params.AutoDepthStencilFormat = D3DFMT_D24S8;
		d3d_params.Flags = 0;
		d3d_params.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
		d3d_params.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;	// Locks fps to 60!
		//IDirect3DDevice9* d3d_device;
		HRESULT ret = d3d_->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd_,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3d_params,
			&d3d_device_);
		if ( ret != D3D_OK) 
		{
			Destroy();
			return false;
		}
		//d3d_device_ = d3d_device;
		//d3d_device->Release();

		//IDirect3DVertexBuffer9* vertex_buffer;
		const int kRectVertices = 4;
		if (d3d_device_->CreateVertexBuffer(kRectVertices * sizeof(D3dCustomVertex),
			0, D3DFVF_CUSTOMVERTEX, D3DPOOL_MANAGED,
			&vertex_buffer_, NULL) != D3D_OK) {
			Destroy();
			return false;
		}
		//vertex_buffer_ = vertex_buffer;
		//vertex_buffer->Release();

		d3d_device_->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		d3d_device_->SetRenderState(D3DRS_LIGHTING, FALSE);
		Resize(width_, height_);

		ShowWindow(hwnd_, SW_SHOW);
		d3d_device_->Present(NULL, NULL, NULL, NULL);

		return true;
	}
	void cvideo_renderer::OnFrame(const Dav1dPicture& picture, uint32 width, uint32 height)
	{
		if (static_cast<size_t>(width) != width_ ||
			static_cast<size_t>(height) != height_) {
			Resize(static_cast<size_t>(width),
				static_cast<size_t>(height));
		}
		/*static FILE *out_file_ptr = ::fopen("./test_render.mp4", "wb+");
		if (out_file_ptr)
		{
			::fwrite(picture.data[0], 1, width * height, out_file_ptr);
			::fflush(out_file_ptr);
		}*/
		D3DLOCKED_RECT lock_rect;
		if (texture_->LockRect(0, &lock_rect, NULL, 0) != D3D_OK)
			return;

		//ConvertFromI420(frame, VideoType::kARGB, 0,
		//	static_cast<uint8_t*>(lock_rect.pBits));
		libyuv::ConvertFromI420(
			(const uint8_t*)picture.data[0], picture.stride[0],
			(const uint8_t*)picture.data[1], picture.stride[1],
			(const uint8_t*)picture.data[2], picture.stride[1],
			static_cast<uint8_t*>(lock_rect.pBits), 0, width, height,
			libyuv::FOURCC_ARGB);
		texture_->UnlockRect(0);

		d3d_device_->BeginScene();
		d3d_device_->SetFVF(D3DFVF_CUSTOMVERTEX);
		d3d_device_->SetStreamSource(0, vertex_buffer_, 0,
			sizeof(D3dCustomVertex));
		d3d_device_->SetTexture(0, texture_);
		d3d_device_->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
		d3d_device_->EndScene();
		ShowWindow(hwnd_, SW_SHOW);
		d3d_device_->Present(NULL, NULL, NULL, NULL);
	}
	void cvideo_renderer::Destroy()
	{
		texture_ = nullptr;
		vertex_buffer_ = nullptr;
		d3d_device_ = nullptr;
		d3d_ = nullptr;

		if (hwnd_ != NULL) {
			DestroyWindow(hwnd_);
			//RTC_DCHECK(!IsWindow(hwnd_));
			hwnd_ = NULL;
		}
	}
	LRESULT __stdcall cvideo_renderer::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		if (msg == WM_DESTROY || (msg == WM_CHAR && wparam == VK_RETURN)) {
			PostQuitMessage(0);
			return 0;
		}

		return DefWindowProcA(hwnd, msg, wparam, lparam);
	}
	void cvideo_renderer::Resize(size_t width, size_t height)
	{
		width_ = width;
		height_ = height;
		//IDirect3DTexture9* texture;

		d3d_device_->CreateTexture(static_cast<UINT>(width_),
			static_cast<UINT>(height_), 1, 0, D3DFMT_A8R8G8B8,
			D3DPOOL_MANAGED, &texture_, NULL);
		//texture_ = texture;
		//texture->Release();

		// Vertices for the video frame to be rendered to.
		static const D3dCustomVertex rect[] = {
			{-1.0f, -1.0f, 0.0f, 0.0f, 1.0f},
			{-1.0f, 1.0f, 0.0f, 0.0f, 0.0f},
			{1.0f, -1.0f, 0.0f, 1.0f, 1.0f},
			{1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
		};

		void* buf_data;
		if (vertex_buffer_->Lock(0, 0, &buf_data, 0) != D3D_OK)
			return;

		memcpy(buf_data, &rect, sizeof(rect));
		vertex_buffer_->Unlock();
	}
}