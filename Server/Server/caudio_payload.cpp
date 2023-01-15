/***********************************************************************************************
created: 		2023-01-15

author:			chensong

purpose:		_C_AUDIO_PAYLOAD_H_


************************************************************************************************/
#include "caudio_payload.h"
#include "crtc_sdp_util.h"
#include "cshare_proto_error.h"
namespace chen {
	caudio_payload::~caudio_payload()
	{
	}
	cmedia_payload_type caudio_payload::generate_media_payload_type()
	{

		cmedia_payload_type media_payload_type(m_pt);

		media_payload_type.m_encoding_name = m_name;
		media_payload_type.m_clock_rate = m_sample;
		if (0 != m_channel)
		{
			media_payload_type.m_encoding_param = std::to_string(m_channel);
		}

		media_payload_type.m_rtcp_fb = m_rtcp_fbs;
		std::ostringstream format_specific_param;
		if (m_opus_param.m_min_time) 
		{
			format_specific_param << "minptime=" << m_opus_param.m_min_time;
		}
		if (m_opus_param.m_use_inband_fec) 
		{
			format_specific_param << ";useinbandfec=1";
		}
		if (m_opus_param.m_usedtx)
		{
			format_specific_param << ";usedtx=1";
		}
		media_payload_type.m_format_specific_param = format_specific_param.str();

		return media_payload_type;
		//return cmedia_payload_type();
	}
	int32 caudio_payload::set_opus_param_desc(std::string fmtp)
	{
		int32 err = 0;

		std::vector<std::string>   vec = rtc_sdp_util::split_str(fmtp, ";");

		for (size_t i = 0; i < vec.size(); ++i)
		{
			std::vector<std::string> kv = rtc_sdp_util::split_str(vec[i], "=");

			if (2 == kv.size())
			{
				if ("minptime" == kv[0])
				{
					m_opus_param.m_min_time = std::stoi(kv[1].c_str());
				}
				else if ("useinbandfec" == kv[0])
				{
					m_opus_param.m_use_inband_fec = (kv[1] == "1") ? true : false;
				}
				else if ("usedtx" == kv[0])
				{
					m_opus_param.m_usedtx = (kv[1] == "1") ? true : false;
				}
			}
			else
			{
				ERROR_EX_LOG("invalid opus param=%s", vec[i].c_str());
				return EShareProtoSdp;
			}
		}

		return err;
		//return int32();
	}
}