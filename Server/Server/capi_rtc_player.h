/***********************************************************************************************
created: 		2023-01-15

author:			chensong

purpose:		api_rtc_publish


************************************************************************************************/
#ifndef _C_API_RTC_PLAYER_H_
#define _C_API_RTC_PLAYER_H_
#include "cnet_type.h"
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include "crtc_source_description.h"
#include "crtc_track_description.h"
namespace chen {
	class capi_rtc_player
	{
	public:
		explicit capi_rtc_player() {}
		virtual ~capi_rtc_player();

		bool do_serve_client(const std::string &remote_sdp, const std::string & roomname, const std::string & peerid, const std::string & video_peerid, std::string & local_sdp);

	private:
		//TODO: Use StreamDescription to negotiate and remove first negotiate_play_capability function
		bool _negotiate_play_capability(crtc_sdp& remote_sdp, crtc_source_description * stream_desc, std::map<uint32_t, crtc_track_description*>& sub_relations);
		bool _generate_play_local_sdp(const std::string & roomname, const std::string & peerid, crtc_sdp& local_sdp, crtc_source_description* stream_desc, bool unified_plan, bool audio_before_video);
		bool _generate_play_local_sdp_for_audio(crtc_sdp& local_sdp, crtc_source_description* stream_desc, std::string cname);
		bool _generate_play_local_sdp_for_video(crtc_sdp& local_sdp, crtc_source_description* stream_desc, bool unified_plan, std::string cname);
		bool _create_player(  std::map<uint32, crtc_track_description*> sub_relations);
		//bool _create_publisher(crtc_source_description* stream_desc);

		std::vector<crtc_track_description*> _get_track_desc(crtc_source_description * stream_desc, std::string type, std::string media_name);
	protected:
	private:
	};

}

#endif // _C_API_RTC_PLAYER_H_