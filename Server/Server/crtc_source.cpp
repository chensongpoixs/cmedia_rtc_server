/***********************************************************************************************
created: 		2023-01-15

author:			chensong

purpose:		rtc_source
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