/***********************************************************************************************
created: 		2023-07-23

author:			chensong

purpose:		vmc packet

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

#include "cdav1d_decoder.h"
#include "clog.h"

#include "cvideo_renderer.h"
namespace chen {
	class ScopedDav1dData {
	public:
		~ScopedDav1dData() { dav1d_data_unref(&data_); }

		Dav1dData& Data() { return data_; }

	private:
		Dav1dData data_ = {};
	};

	class ScopedDav1dPicture  {
	public:
		~ScopedDav1dPicture() { dav1d_picture_unref(&picture_); }

		Dav1dPicture& Picture() { return picture_; }
		//using rtc::RefCountedNonVirtual<ScopedDav1dPicture>::HasOneRef;

	private:
		Dav1dPicture picture_ = {};
	};

	// Calling `dav1d_data_wrap` requires a `free_callback` to be registered.
	void NullFreeCallback(const uint8_t* buffer, void* opaque) {}



	bool cdav1d_decoder::init()
	{
		Dav1dSettings s;
		dav1d_default_settings(&s);

		s.n_threads = 3;// std::max(2, settings.number_of_cores());
		s.max_frame_delay = 1;   // For low latency decoding.
		s.all_layers = 0;        // Don't output a frame for every spatial layer.
		s.operating_point = 31;  // Decode all operating points.

		return dav1d_open(&context_, &s) == 0;
	}

	void cdav1d_decoder::destroy()
	{
		dav1d_close(&context_);
	}

	int32_t cdav1d_decoder::Decode(const cvcm_encoded_frame & encoded_image, bool missing_frames, int64_t render_time_ms)
	{

#if 0
		static FILE * out_file_ptr = ::fopen("./av1/test_webrtc.mp4", "wb+");;
		if (out_file_ptr)
		{
			//static bool ret = true;
			//if (ret)
			{
				uint8 p[2] = { 0X12u, 0u };
				::fwrite(&p[0], 1, 2, out_file_ptr);
				::fflush(out_file_ptr);
				//ret = false;
			}
			::fwrite(encoded_image.data(), 1, encoded_image.data_size(), out_file_ptr);
			//	NORMAL_EX_LOG("[rtp_packet][rtc_count = %u][packet_size = %u]", rtc_count, frame_data.data_size());
			::fflush(out_file_ptr);

		}
#endif
		if (!context_)
		{
			WARNING_EX_LOG("context not init ...");
			return 0;
		}

		ScopedDav1dData scoped_dav1d_data;
		Dav1dData& dav1d_data = scoped_dav1d_data.Data();
		dav1d_data_wrap(&dav1d_data, encoded_image.data(), encoded_image.data_size(),
			/*free_callback=*/&NullFreeCallback,
			/*user_data=*/nullptr);
		if (int32 decode_res = dav1d_send_data(context_, &dav1d_data)) {
			/*RTC_LOG(LS_WARNING)
				<< "Dav1dDecoder::Decode decoding failed with error code "
				<< decode_res;*/
			WARNING_EX_LOG("Dav1dDecoder::Decode decoding failed with error code %u", decode_res);
			return 0;
		}

		ScopedDav1dPicture scoped_dav1d_picture;

		Dav1dPicture& dav1d_picture = scoped_dav1d_picture.Picture();
		if (int32 get_picture_res = dav1d_get_picture(context_, &dav1d_picture)) {
			/*RTC_LOG(LS_WARNING)
				<< "Dav1dDecoder::Decode getting picture failed with error code "
				<< get_picture_res;*/
			WARNING_EX_LOG("Dav1dDecoder::Decode getting picture failed with error code %u", get_picture_res);
			return 0;
		}

		if (dav1d_picture.p.bpc != 8) 
		{
			// Only accept 8 bit depth.
			//RTC_LOG(LS_ERROR) << "Dav1dDecoder::Decode unhandled bit depth: "
		//		<< dav1d_picture.p.bpc;
			ERROR_EX_LOG("Dav1dDecoder::Decode unhandled bit depth: %u", dav1d_picture.p.bpc);
			return 0;
		}

		//rtc::scoped_refptr<VideoFrameBuffer> wrapped_buffer;
		if (dav1d_picture.p.layout == DAV1D_PIXEL_LAYOUT_I420) 
		{
			//wrapped_buffer = WrapI420Buffer(
			//	dav1d_picture.p.w, dav1d_picture.p.h,
			//	static_cast<uint8_t*>(dav1d_picture.data[0]), dav1d_picture.stride[0],
			//	static_cast<uint8_t*>(dav1d_picture.data[1]), dav1d_picture.stride[1],
			//	static_cast<uint8_t*>(dav1d_picture.data[2]), dav1d_picture.stride[1],
			//	// To keep |scoped_dav1d_picture.Picture()| alive
			//	[scoped_dav1d_picture] {});
			NORMAL_EX_LOG("AV1 --> I420");
			static bool init_render = false;
			if (!init_render)
			{
				init_render = true;
				g_video_renderer.init("AV1", dav1d_picture.p.w, dav1d_picture.p.h);
			}
			g_video_renderer.OnFrame(dav1d_picture, dav1d_picture.p.w, dav1d_picture.p.h);
		}
		else if (dav1d_picture.p.layout == DAV1D_PIXEL_LAYOUT_I444) 
		{
			//wrapped_buffer = WrapI444Buffer(
			//	dav1d_picture.p.w, dav1d_picture.p.h,
			//	static_cast<uint8_t*>(dav1d_picture.data[0]), dav1d_picture.stride[0],
			//	static_cast<uint8_t*>(dav1d_picture.data[1]), dav1d_picture.stride[1],
			//	static_cast<uint8_t*>(dav1d_picture.data[2]), dav1d_picture.stride[1],
			//	// To keep |scoped_dav1d_picture.Picture()| alive
			//	[scoped_dav1d_picture] {});
			NORMAL_EX_LOG("AV1 --> I444");
		}
		else {
			// Only accept I420 or I444 pixel format.
			//RTC_LOG(LS_ERROR) << "Dav1dDecoder::Decode unhandled pixel layout: "
			//	<< dav1d_picture.p.layout;
			ERROR_EX_LOG("Dav1dDecoder::Decode unhandled pixel layout: %u", dav1d_picture.p.layout);
			return 0;
		}

		



		return int32_t();
	}

}