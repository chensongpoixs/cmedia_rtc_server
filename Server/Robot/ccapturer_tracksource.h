﻿/***********************************************************************************************
created: 		2022-08-24

author:			chensong

purpose:		log
************************************************************************************************/
#ifndef _C_CAPTURER_TRACKSOURCE_H_
#define _C_CAPTURER_TRACKSOURCE_H_
 
#include "api/video/video_frame.h"
#include "api/video/video_sink_interface.h"
#include "desktop_capture_source.h"
#include "desktop_capture_source.h"
#include "modules/desktop_capture/desktop_capturer.h"
#include "modules/desktop_capture/desktop_frame.h"
#include "api/video/i420_buffer.h"
#include "csend_transport.h"
#include "clog.h"
#include "sdptransform.hpp"
#include "sdp/Utils.hpp"
#include "ortc.hpp"
#include "csession_description.h"
#include <api/audio_codecs/builtin_audio_decoder_factory.h>
#include <api/audio_codecs/builtin_audio_encoder_factory.h>
#include <api/create_peerconnection_factory.h>
#include <api/video_codecs/builtin_video_decoder_factory.h>
#include <api/video_codecs/builtin_video_encoder_factory.h>
#include <rtc_base/ssl_adapter.h>
#include "sdptransform.hpp"
#include "sdp/Utils.hpp"
#include "ortc.hpp"
#include "cproducer.h"
#include <memory>
#include "cclient.h"
 
#include "cdataconsumer.h"
#include "csession_description.h"
#include "pc/video_track_source.h"
#include "third_party/libyuv/include/libyuv.h"
#include <thread>
#include <atomic>
#include "ccfg.h"
//#include "NvCodec/nvenc.h"
 
namespace chen {
	 

	class ProxyVideoTrackSource : public webrtc::VideoTrackSource {
	public:
		static rtc::scoped_refptr<ProxyVideoTrackSource> Create()
		{ 
			std::unique_ptr<  VideoCaptureSource> video_source_ptr(VideoCaptureSource::Create( ));
			if (video_source_ptr)
			{
				 
				return new rtc::RefCountedObject<ProxyVideoTrackSource>(std::move(video_source_ptr));
			}
			return nullptr;
		}
		
		
		
		// YUV 
		bool OnFrame(const webrtc::VideoFrame & frame)
		{
			video_source_ptr->VideoOnFrame(frame);
			return true;
		}
	protected:
		explicit ProxyVideoTrackSource(
			std::unique_ptr< VideoCaptureSource> video_source)
			: VideoTrackSource(/*remote=*/false), video_source_ptr(std::move(video_source)) {}

	private:
		rtc::VideoSourceInterface<webrtc::VideoFrame>* source() override {
			return video_source_ptr.get();
		}
		std::atomic_bool start_flag_  = false;

		rtc::scoped_refptr<webrtc::I420Buffer> i420_buffer_;
		//std::unique_ptr<webrtc::test::VcmCapturer> capturer_;
		std::unique_ptr< VideoCaptureSource> video_source_ptr;
	};
}


#endif // !_C_CAPTURER_TRACKSOURCE_H_