/***********************************************************************************************
created: 		2023-01-15

author:			chensong

purpose:		_C_RTC_TRACK_DESCRIPTION_H_


************************************************************************************************/
#include "crtc_track_description.h"

namespace chen {
	crtc_track_description::~crtc_track_description()
	{
	}

	bool crtc_track_description::has_ssrc(uint32_t ssrc)
	{
		if (!m_is_active) 
		{
			return false;
		}

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

}