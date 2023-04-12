/***********************************************************************************************
created: 		2023-01-15

author:			chensong

purpose:		rtc_source


************************************************************************************************/
#include "crtc_source.h"
#include "crtc_track_description.h"
#include "crandom.h"
#include "crtc_ssrc_generator.h"
#include "crtc_constants.h"
#include <cassert>
namespace chen {
	crtc_source::~crtc_source()
	{
	}
	int32 crtc_source::init()
	{
		// Create default relations to allow play before publishing.
		_init_for_play_before_publishing();
		return 0;
	}
	void crtc_source::_init_for_play_before_publishing()
	{
		// If the stream description has already been setup by RTC publisher,
		// we should ignore and it's ok, because we only need to setup it for bridge.
		if (m_stream_desc_ptr) 
		{
			return;
		}

		crtc_source_description* stream_desc = new crtc_source_description();
		

		// audio track description
		if (true) 
		{
			crtc_track_description* audio_track_desc = new crtc_track_description();
			stream_desc->m_audio_track_desc_ptr = audio_track_desc;

			audio_track_desc->m_type = "audio";
			audio_track_desc->m_id = "audio-" + c_rand.rand_str(8);

			uint32_t audio_ssrc = c_rtc_ssrc_generator.generate_ssrc();
			audio_track_desc->m_ssrc = audio_ssrc;
			audio_track_desc->m_direction = "recvonly";

			audio_track_desc->m_media_ptr = new caudio_payload(kAudioPayloadType, "opus", kAudioSamplerate, kAudioChannel);
		}

		// video track description
		if (true) 
		{
			crtc_track_description* video_track_desc = new crtc_track_description();
			stream_desc->m_video_track_descs.push_back(video_track_desc);

			video_track_desc->m_type = "video";
			video_track_desc->m_id = "video-" + c_rand.rand_str(8);

			uint32_t video_ssrc = c_rtc_ssrc_generator.generate_ssrc();
			video_track_desc->m_ssrc = video_ssrc;
			video_track_desc->m_direction = "recvonly";

			cvideo_payload* video_payload = new cvideo_payload(kVideoPayloadType, "H264", kVideoSamplerate);
			video_track_desc->m_media_ptr = video_payload;

			video_payload->set_h264_param_desc("level-asymmetry-allowed=1;packetization-mode=1;profile-level-id=42e01f");
		}

		set_stream_desc(stream_desc);
	}
	int32 crtc_source::create_consumer(crtc_consumer *& consumer)
	{
		int32 err = 0;

		//consumer = new crtc_consumer();
		//m_consumers.push_back(consumer);

		// TODO: FIXME: Implements edge cluster.

		return err;
	}
	int32 crtc_source::consumer_dumps(crtc_consumer * consumer, bool ds, bool dm, bool dg)
	{
		NORMAL_EX_LOG("create consumer, no gop cache");
		return 0;
	}
	void crtc_source::on_consumer_destroy(crtc_consumer * consumer)
	{
		std::vector<crtc_consumer*>::iterator it;
		it = std::find(m_consumers.begin(), m_consumers.end(), consumer);
		if (it != m_consumers.end()) {
			it = m_consumers.erase(it);
		}

		// When all consumers finished, notify publisher to handle it.
		//if (/*m_publish_stream &&*/ consumers.empty()) {
		//	for (size_t i = 0; i < event_handlers_.size(); i++) {
		//		ISrsRtcSourceEventHandler* h = event_handlers_.at(i);
		//		h->on_consumers_finished();
		//	}
		//}
	}
	bool crtc_source::can_publish()
	{
		// TODO: FIXME: Should check the status of bridge.

		return !m_is_created;
	}
	void crtc_source::set_stream_created()
	{
		assert(!m_is_created && !m_is_delivering_packets);
		m_is_created = true;
	}
	bool crtc_source::has_stream_desc()
	{
		return m_stream_desc_ptr;
	}

	void crtc_source::set_stream_desc(crtc_source_description* stream_desc)
	{
		/*if (m_stream_desc_ptr)
		{
			delete m_stream_desc_ptr;
			m_stream_desc_ptr = NULL;
		}*/

		if (stream_desc)
		{
			m_stream_desc_ptr = stream_desc;
		}
	}

	std::vector<crtc_track_description*> crtc_source::get_track_desc(std::string type, std::string media_name)
	{
		std::vector<crtc_track_description*> track_descs;
		if (!m_stream_desc_ptr)
		{
			return track_descs;
		}

		if (type == "audio") 
		{
			if (!m_stream_desc_ptr->m_audio_track_desc_ptr) 
			{
				return track_descs;
			}
			if (m_stream_desc_ptr->m_audio_track_desc_ptr->m_media_ptr->m_name == media_name) 
			{
				track_descs.push_back(m_stream_desc_ptr->m_audio_track_desc_ptr);
			}
		}

		if (type == "video") 
		{
			std::vector<crtc_track_description*>::iterator it = m_stream_desc_ptr->m_video_track_descs.begin();
			while (it != m_stream_desc_ptr->m_video_track_descs.end())
			{
				track_descs.push_back(*it);
				++it;
			}
		}

		return track_descs;
	}

}