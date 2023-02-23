/***********************************************************************************************
created: 		2023-02-23

author:			chensong

purpose:		rtc_connection


************************************************************************************************/

#ifndef _C_RTC_PLAYER_STREAM_H_
#define _C_RTC_PLAYER_STREAM_H_
#include "cnet_type.h"
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include "crtc_sdp.h"

#include "crtc_track_description.h"
namespace chen {
	class crtc_player_stream
	{
	public:
		explicit crtc_player_stream() {}
		virtual ~crtc_player_stream();
	public:
		bool init(const std::map<uint32_t, crtc_track_description*>& sub_relations);

	protected:
	public:
		// key: publish_ssrc, value: send track to process rtp/rtcp
		std::map<uint32, crtc_track_description*>   m_audio_tracks;
		std::map<uint32, crtc_track_description*>	m_video_tracks;
	};

}

#endif // _C_RTC_PLAYER_STREAM_H_