/***********************************************************************************************
created: 		2023-01-15

author:			chensong

purpose:		_C_RTC_TRACK_DESCRIPTION_H_

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
#include "crtc_track_description.h"

namespace chen {
	crtc_track_description::~crtc_track_description()
	{
	}

	bool crtc_track_description::has_ssrc(uint32_t ssrc)
	{
		/*if (!m_is_active) 
		{
			return false;
		}*/

		if (ssrc == m_ssrc || ssrc == m_rtx_ssrc || ssrc == m_fec_ssrc) 
		{
			return true;
		}

		return false;
	}

	void crtc_track_description::add_rtp_extension_desc(int32 id, std::string uri)
	{
		m_extmaps[id] = uri;
	}

	void crtc_track_description::del_rtp_extension_desc(std::string uri)
	{
		for (std::map<int, std::string>::iterator it = m_extmaps.begin(); it != m_extmaps.end(); ++it) 
		{
			if (uri == it->second)
			{
				m_extmaps.erase(it++);
				break;
			}
		}
	}

	void crtc_track_description::set_direction(std::string direction)
	{
		m_direction = direction;
	}

	void crtc_track_description::set_codec_payload(ccodec_payload * payload)
	{
		m_media_ptr = payload;
	}

	void crtc_track_description::create_auxiliary_payload(const std::vector<cmedia_payload_type> payloads)
	{
		if (!payloads.size()) 
		{
			return;
		}

		cmedia_payload_type payload = payloads.at(0);
		if (payload.m_encoding_name == "red")
		{
			if (m_red_ptr)
			{
				delete m_red_ptr;
				m_red_ptr = NULL;
			}
			
			m_red_ptr = new cred_paylod(payload.m_payload_type, "red", payload.m_clock_rate, ::atol(payload.m_encoding_param.c_str()));
		}
		else if (payload.m_encoding_name == "rtx") 
		{
			if (m_rtx_ptr)
			{
				delete m_rtx_ptr;
				m_rtx_ptr = NULL;
			}
			// TODO: FIXME: Rtx clock_rate should be payload.clock_rate_
			m_rtx_ptr = new crtx_payload_des(payload.m_payload_type, ::atol(payload.m_encoding_param.c_str()));
		}
		else if (payload.m_encoding_name == "ulpfec") 
		{
			if (m_ulpfec_ptr)
			{
				delete m_ulpfec_ptr;
				m_ulpfec_ptr = NULL;
			}
			m_ulpfec_ptr = new ccodec_payload(payload.m_payload_type, "ulpfec", payload.m_clock_rate);
		}
	}

	void crtc_track_description::set_rtx_ssrc(uint32 ssrc)
	{
		m_rtx_ssrc = ssrc;
	}

	void crtc_track_description::set_fec_ssrc(uint32 ssrc)
	{
		m_fec_ssrc = ssrc;
	}

	void crtc_track_description::set_mid(std::string mid)
	{
		m_mid = mid;
	}

	int32 crtc_track_description::get_rtp_extension_id(std::string uri)
	{
		for (std::map<int, std::string>::iterator it = m_extmaps.begin(); it != m_extmaps.end(); ++it) 
		{
			if (uri == it->second) 
			{
				return it->first;
			}
		}

		return 0;
	}

	crtc_track_description * crtc_track_description::copy()
	{
		crtc_track_description* cp = new crtc_track_description();

		cp->m_type = m_type;
		cp->m_id = m_id;
		cp->m_ssrc = m_ssrc;
		cp->m_fec_ssrc = m_fec_ssrc;
		cp->m_rtx_ssrc = m_rtx_ssrc;
		cp->m_extmaps = m_extmaps;
		cp->m_direction = m_direction;
		cp->m_mid = m_mid;
		cp->m_msid = m_msid;
		cp->m_is_active = m_is_active;
		cp->m_media_ptr = m_media_ptr ? m_media_ptr->copy() : NULL;
		cp->m_red_ptr = m_red_ptr ? m_red_ptr->copy() : NULL;
		cp->m_rtx_ptr = m_rtx_ptr ? m_rtx_ptr->copy() : NULL;
		cp->m_ulpfec_ptr = m_ulpfec_ptr ? m_ulpfec_ptr->copy() : NULL;

		return cp;
	}

}