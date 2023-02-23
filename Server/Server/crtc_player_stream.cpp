/***********************************************************************************************
created: 		2023-02-23

author:			chensong

purpose:		rtc_connection


************************************************************************************************/
#include "crtc_player_stream.h"

namespace chen {
	crtc_player_stream::~crtc_player_stream()
	{
	}
	bool crtc_player_stream::init(const std::map<uint32_t, crtc_track_description*>& sub_relations)
	{
		for (std::map<uint32 , crtc_track_description*>::const_iterator it = sub_relations.begin(); it != sub_relations.end(); ++it) {
			uint32_t ssrc = it->first;
			crtc_track_description* desc = it->second;

			if (desc->m_type == "audio") {
			//	SrsRtcAudioSendTrack* track = new SrsRtcAudioSendTrack(session_, desc);
				//audio_tracks_.insert(make_pair(ssrc, track));
				m_audio_tracks.insert(std::make_pair(ssrc, desc->copy()));
			}

			if (desc->m_type == "video") {
				//SrsRtcVideoSendTrack* track = new SrsRtcVideoSendTrack(session_, desc);
				//video_tracks_.insert(make_pair(ssrc, track));
				m_video_tracks.insert(std::make_pair(ssrc, desc->copy()));
			}
		}

		return true;
	}
}