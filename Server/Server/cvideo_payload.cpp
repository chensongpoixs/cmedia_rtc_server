/***********************************************************************************************
created: 		2023-01-15

author:			chensong

purpose:		_C_VIDEO_PAYLOAD_H_
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
#include "cvideo_payload.h"
#include "crtc_sdp_util.h"
#include "cshare_proto_error.h"
namespace chen {
	cvideo_payload::~cvideo_payload()
	{
	}
	cmedia_payload_type cvideo_payload::generate_media_payload_type()
	{
		cmedia_payload_type media_payload_type(m_pt);

		media_payload_type.m_encoding_name = m_name;
		media_payload_type.m_clock_rate = m_sample;
		media_payload_type.m_rtcp_fb = m_rtcp_fbs;

		std::ostringstream format_specific_param;
		if (!m_h264_param.m_level_asymmerty_allow.empty())
		{
			format_specific_param << "level-asymmetry-allowed=" << m_h264_param.m_level_asymmerty_allow;
		}
		if (!m_h264_param.m_packetization_mode.empty()) 
		{
			format_specific_param << ";packetization-mode=" << m_h264_param.m_packetization_mode;
		}
		if (!m_h264_param.m_profile_level_id.empty()) 
		{
			format_specific_param << ";profile-level-id=" << m_h264_param.m_profile_level_id;
		}

		media_payload_type.m_format_specific_param = format_specific_param.str().c_str();



		std::ostringstream codec_bitrate_param;
		if (0!= m_bitrate_param.m_min_bitrate)
		{
			codec_bitrate_param << "x-google-min-bitrate=" << m_bitrate_param.m_min_bitrate;
		}
		if (0!= m_bitrate_param.m_start_bitrate)
		{
			codec_bitrate_param << ";x-google-start-bitrate=" << m_bitrate_param.m_start_bitrate;
		}
		if (0!=m_bitrate_param.m_max_bitrate)
		{
			codec_bitrate_param << ";x-google-max-bitrate=" << m_bitrate_param.m_max_bitrate;
		}
		media_payload_type.m_codec_bitrate_param = codec_bitrate_param.str().c_str();
		return media_payload_type;

		//return cmedia_payload_type();
	}
	ccodec_payload * cvideo_payload::copy()
	{
		cvideo_payload* cp = new cvideo_payload();

		cp->m_type = m_type;
		cp->m_pt = m_pt;
		cp->m_pt_of_publisher = m_pt_of_publisher;
		cp->m_name = m_name;
		cp->m_sample = m_sample;
		cp->m_rtcp_fbs = m_rtcp_fbs;
		cp->m_h264_param = m_h264_param;
		cp->m_bitrate_param = m_bitrate_param;
		return cp;
	}
	int32 cvideo_payload::set_h264_param_desc(std::string fmtp)
	{
		int32 err = 0;

		// For example: level-asymmetry-allowed=1;packetization-mode=1;profile-level-id=42e01f
		std::vector<std::string> attributes = rtc_sdp_util:: split_str(fmtp, ";");

		for (size_t i = 0; i < attributes.size(); ++i) 
		{
			std::string attribute = attributes.at(i);

			std::vector<std::string> kv = rtc_sdp_util::  split_str(attribute, "=");
			if (kv.size() != 2) 
			{
				ERROR_EX_LOG("invalid h264 param=%s", attribute.c_str());
				return EShareProtoSdp;
				//return srs_error_new(ERROR_RTC_SDP_DECODE, "invalid h264 param=%s", attribute.c_str());
			}

			if (kv[0] == "profile-level-id") 
			{
				m_h264_param.m_profile_level_id = kv[1];
			}
			else if (kv[0] == "packetization-mode")
			{
				// 6.3.  Non-Interleaved Mode
				// This mode is in use when the value of the OPTIONAL packetization-mode
				// media type parameter is equal to 1.  This mode SHOULD be supported.
				// It is primarily intended for low-delay applications.  Only single NAL
				// unit packets, STAP-As, and FU-As MAY be used in this mode.  STAP-Bs,
				// MTAPs, and FU-Bs MUST NOT be used.  The transmission order of NAL
				// units MUST comply with the NAL unit decoding order.
				// @see https://tools.ietf.org/html/rfc6184#section-6.3
				m_h264_param.m_packetization_mode = kv[1];
			}
			else if (kv[0] == "level-asymmetry-allowed") 
			{
				m_h264_param.m_level_asymmerty_allow = kv[1];
			}
			else 
			{
				ERROR_EX_LOG("invalid h264 param=%s", kv[0].c_str());
				return EShareProtoSdp;
			//	return srs_error_new(ERROR_RTC_SDP_DECODE, "invalid h264 param=%s", kv[0].c_str());
			}
		}


		return err;
		//return int32();
	}
	int32 cvideo_payload::set_bitrate_param_desc(std::string fmtp)
	{
		return parse_codec_bitrate_fmtp(fmtp, m_bitrate_param);
	}
}