/***********************************************************************************************
created: 		2023-01-15

author:			chensong

purpose:		_C_VIDEO_PAYLOAD_H_


************************************************************************************************/

#include "crtx_paylod_des.h"
namespace chen {
	crtx_payload_des::~crtx_payload_des()
	{
	}
	cmedia_payload_type crtx_payload_des::generate_media_payload_type()
	{
		cmedia_payload_type media_payload_type(m_pt);

		media_payload_type.m_encoding_name = m_name;
		media_payload_type.m_clock_rate = m_sample;
		std::ostringstream format_specific_param;
		format_specific_param << "fmtp:" << m_pt << " apt=" << m_apt;

		media_payload_type.m_format_specific_param = format_specific_param.str();

		return media_payload_type;
		//return cmedia_payload_type();
	}

	chen::ccodec_payload * crtx_payload_des::copy()
	{
		crtx_payload_des* cp = new crtx_payload_des();

		cp->m_type = m_type;
		cp->m_pt = m_pt;
		cp->m_pt_of_publisher = m_pt_of_publisher;
		cp->m_name = m_name;
		cp->m_sample = m_sample;
		cp->m_rtcp_fbs = m_rtcp_fbs;
		cp->m_apt = m_apt;
		//cp->m_opus_param = m_opus_param;

		return cp;
	}

}
