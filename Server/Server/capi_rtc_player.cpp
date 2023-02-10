/***********************************************************************************************
created: 		2023-01-15

author:			chensong

purpose:		api_rtc_publish


************************************************************************************************/
#include "capi_rtc_player.h"

namespace chen {
	capi_rtc_player::~capi_rtc_player()
	{
	}
	bool capi_rtc_player::_negotiate_play_capability()
	{
		return true;
	}
	bool capi_rtc_player::_generate_play_local_sdp(crtc_sdp & local_sdp, crtc_source_description * stream_desc, bool unified_plan, bool audio_before_video)
	{
		return true;
	}
	bool capi_rtc_player::_generate_play_local_sdp_for_audio(crtc_sdp & local_sdp, crtc_source_description * stream_desc, std::string cname)
	{
		return true;
	}
	bool capi_rtc_player::_generate_play_local_sdp_for_video(crtc_sdp & local_sdp, crtc_source_description * stream_desc, bool unified_plan, std::string cname)
	{
		return true;
	}
	bool capi_rtc_player::_create_player(std::map<uint32, crtc_track_description*> sub_relations)
	{
		return true;
	}
}