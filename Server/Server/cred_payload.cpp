/***********************************************************************************************
created: 		2023-01-15

author:			chensong

purpose:		_C_RED_PAYLOAD_H_


************************************************************************************************/
#include "cred_payload.h"

namespace chen {
	cred_paylod::~cred_paylod()
	{
	}
	cmedia_payload_type cred_paylod::generate_media_payload_type()
	{
		cmedia_payload_type media_payload_type(m_pt);

		media_payload_type.m_encoding_name = m_name;
		media_payload_type.m_clock_rate = m_sample;
		if (m_channel != 0) 
		{
			media_payload_type.m_encoding_param = std::to_string(m_channel);
		}
		media_payload_type.m_rtcp_fb = m_rtcp_fbs;

		return media_payload_type;
		//return cmedia_payload_type();
	}
}