/***********************************************************************************************
created: 		2023-03-10

author:			chensong

purpose:		ctransport_util


************************************************************************************************/
#include "csdp_util.h"

namespace chen {
	bool  sdp_has_h264_profile(const cmedia_payload_type& payload_type, const std::string& profile)
	{
		int32 err = 0;

		if (payload_type.m_format_specific_param.empty())
		{
			return false;
		}

		ch264_specific_param h264_param;
		if ((err = parse_h264_fmtp(payload_type.m_format_specific_param, h264_param)) != 0)
		{
			WARNING_EX_LOG(" parse h264 fmtp failed !!! ");
			return false;
		}

		if (h264_param.m_profile_level_id == profile)
		{
			return true;
		}

		return false;
	}

	// For example, 42001f 42e01f, see https://blog.csdn.net/epubcn/article/details/102802108
	bool  sdp_has_h264_profile(const crtc_sdp& sdp, const std::string& profile)
	{
		for (size_t i = 0; i < sdp.m_media_descs.size(); ++i)
		{
			const cmedia_desc& desc = sdp.m_media_descs[i];
			if (!desc.is_video())
			{
				continue;
			}

			std::vector<cmedia_payload_type> payloads = desc.find_media_with_encoding_name("H264");
			if (payloads.empty())
			{
				continue;
			}

			for (std::vector<cmedia_payload_type>::iterator it = payloads.begin(); it != payloads.end(); ++it)
			{
				const cmedia_payload_type& payload_type = *it;
				if (sdp_has_h264_profile(payload_type, profile))
				{
					return true;
				}
			}
		}

		return false;
	}

}