/***********************************************************************************************
created: 		2023-01-15

author:			chensong

purpose:		_C_CODEC_PAYLOAD_H_


************************************************************************************************/
#include "ccodec_payload.h"
namespace chen {
	 
	ccodec_payload::~ccodec_payload()
	{
	}
	cmedia_payload_type ccodec_payload::generate_media_payload_type()
	{

		cmedia_payload_type media_payload_type(m_pt);


		media_payload_type.m_encoding_name = m_name;
		media_payload_type.m_clock_rate = m_sample;
		media_payload_type.m_rtcp_fb = m_rtcp_fbs;
		return media_payload_type;
		//return cmedia_payload_type();
	}
}