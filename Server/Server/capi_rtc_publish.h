/***********************************************************************************************
created: 		2023-01-15

author:			chensong

purpose:		api_rtc_publish


************************************************************************************************/

#ifndef _C_API_RTC_PUBLISH_H_
#define _C_API_RTC_PUBLISH_H_
#include "cnet_type.h"
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include "crtc_sdp.h"
#include "crtc_source_description.h"
namespace chen {

	class crtc_user_config;
	class capi_rtc_publish
	{
	public:
		capi_rtc_publish() {}
		~capi_rtc_publish();


	public:



		bool do_serve_client(const std::string &remote_sdp, const std::string & roomname, const std::string & peerid,   std::string & local_sdp);
	public:
		// publish -> remote sdp 
		bool _negotiate_publish_capability(crtc_sdp& remote_sdp, crtc_source_description * stream_desc);
		bool _generate_publish_local_sdp(const std::string & roomname, const std::string & peerid, crtc_sdp& local_sdp, crtc_source_description* stream_desc, bool unified_plan, bool audio_before_video);

		bool _generate_publish_local_sdp_for_audio(crtc_sdp& local_sdp, crtc_source_description* stream_desc);
		bool _generate_publish_local_sdp_for_video(crtc_sdp& local_sdp, crtc_source_description* stream_desc, bool unified_plan);

	protected:
	private:

		int32 _serve_client(crtc_user_config * ruc);

		int32 _check_remote_sdp(const crtc_sdp & remote_sdp);
	};
}

#endif // _C_API_RTC_PUBLISH_H_