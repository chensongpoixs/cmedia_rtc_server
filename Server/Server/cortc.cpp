/********************************************************************
created:	2022-10-25

author:		chensong

level:		ortc

purpose:	一些傻瓜搞WebRTC的问题
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

					if (webrtc::H264::IsSameH264Profile(media_codec.parameters.codecs_map, rtp_codec_parameter.parameters.codecs_map))
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
