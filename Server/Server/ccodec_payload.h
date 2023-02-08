/***********************************************************************************************
created: 		2023-01-15

author:			chensong

purpose:		_C_CODEC_PAYLOAD_H_


************************************************************************************************/

#ifndef _C_CODEC_PAYLOAD_H_
#define _C_CODEC_PAYLOAD_H_
#include "cnet_type.h"
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include "crtc_sdp.h"
#include "cmedia_desc.h"
namespace chen {
	// TODO: FIXME: Rename it.
	class ccodec_payload
	{
	public:
		ccodec_payload()
			: m_type("")
			, m_pt(0)
			, m_pt_of_publisher(0)
			, m_name("")
			, m_sample(0)
			, m_rtcp_fbs() {}

		ccodec_payload(uint8 pt, std::string encode_name, int32 sample)
			: m_type("")
			, m_pt(pt)
			, m_pt_of_publisher(pt)
			, m_name(encode_name)
			, m_sample(sample)
			, m_rtcp_fbs() {}

		virtual ~ccodec_payload();
	public:

		virtual cmedia_payload_type  generate_media_payload_type();

		virtual ccodec_payload * copy();
	protected:
	private:


	public:
		std::string					m_type;
		uint8						m_pt;
		// for publish, equals to PT of itself;
		// for subscribe, is the PT of publisher;
		uint8						m_pt_of_publisher;
		std::string					m_name;
		int32						m_sample;

		std::vector<std::string>	m_rtcp_fbs;
	};
}


#endif // _C_CODEC_PAYLOAD_H_