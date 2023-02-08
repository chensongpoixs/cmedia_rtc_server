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

	chen::ccodec_payload * cred_paylod::copy()
	{
		cred_paylod* cp = new cred_paylod();

		cp->m_type = m_type;
		cp->m_pt = m_pt;
		cp->m_pt_of_publisher = m_pt_of_publisher;
		cp->m_name = m_name;
		cp->m_sample = m_sample;
		cp->m_rtcp_fbs = m_rtcp_fbs;
		cp->m_channel = m_channel;
		//cp->m_opus_param = m_opus_param;

		return cp;
	}

}