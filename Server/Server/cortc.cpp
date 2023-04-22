/********************************************************************
created:	2022-10-25

author:		chensong

level:		ortc
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
purpose:	一些傻瓜搞WebRTC的问题
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
*********************************************************************/
#include "cortc.h"
#include "clog.h"
#include "h264_profile_level_id.h"
namespace chen {
	namespace ortc {
		/*RTC::RtpCodecParameters*/ int32 match_codecs(const RTC::RtpCodecParameters & media_codec, const std::vector<RTC::RtpCodecParameters>& supported_codec)
		{
			for (const RTC::RtpCodecParameters& rtp_codec_parameter : supported_codec)
			{
				if (rtp_codec_parameter.mimeType.type != media_codec.mimeType.type || rtp_codec_parameter.mimeType.subtype != media_codec.mimeType.subtype)
				{
					continue;
				}
				if (rtp_codec_parameter.clockRate != media_codec.clockRate)
				{
					continue;
				}
				if (rtp_codec_parameter.channels != media_codec.channels)
				{
					continue;
				}
				switch (media_codec.mimeType.subtype)
				{
				case RTC::RtpCodecMimeType::Subtype::H264:
				{
					typedef std::map<std::string, std::string> CODECS_MAP;
					CODECS_MAP::const_iterator media_codec_iter = media_codec.parameters.codecs_map.find("packetization-mode");
					CODECS_MAP::const_iterator rtp_codec_iter = rtp_codec_parameter.parameters.codecs_map.find("packetization-mode");
					if (media_codec_iter == media_codec.parameters.codecs_map.end() || rtp_codec_iter == rtp_codec_parameter.parameters.codecs_map.end())
					{
						continue;
					}
					if (media_codec_iter->second != rtp_codec_iter->second)
					{
						WARNING_EX_LOG("H264 packetization-mode != !!!");
						continue;
					}

					if (!webrtc::H264::IsSameH264Profile(media_codec.parameters.codecs_map, rtp_codec_parameter.parameters.codecs_map))
					{
						WARNING_EX_LOG("IsSameH264Profile");
						continue;
					}
					std::map<std::string, std::string> selectedProfileLevelId;
					webrtc::H264::GenerateProfileLevelIdForAnswer(media_codec.parameters.codecs_map, rtp_codec_parameter.parameters.codecs_map, &selectedProfileLevelId);
					if (selectedProfileLevelId.size() > 0)
					{
						//media_codec.parameters.codecs_map["profile-level-id"]
						// aCodec.parameters['profile-level-id'] = selectedProfileLevelId;
					}
					return rtp_codec_parameter.payloadType;
					break;
				}
				case RTC::RtpCodecMimeType::Subtype::VP9:
				{
				//case 'video/vp9':
				//{
				//	// If strict matching check profile-id.
				//	if (strict) {
				//		const aProfileId = aCodec.parameters['profile-id'] || 0;
				//		const bProfileId = bCodec.parameters['profile-id'] || 0;
				//		if (aProfileId != = bProfileId)
				//			return false;
				//	}
				//	break;
				//}
					break;
				}
				default:
					break;
				}
			}

			return 0;
		}
	}
}
