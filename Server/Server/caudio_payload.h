/***********************************************************************************************
created: 		2023-01-15

author:			chensong

purpose:		_C_AUDIO_PAYLOAD_H_


************************************************************************************************/

#ifndef _C_AUDIO_PAYLOAD_H_
#define _C_AUDIO_PAYLOAD_H_
#include "cnet_type.h"
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include "crtc_sdp.h"
#include "cmedia_desc.h"
#include "ccodec_payload.h"
namespace chen {
	// TODO: FIXME: Rename it.
	class caudio_payload : public ccodec_payload
	{
	public:
		struct copus_parameter
		{
			int32  m_min_time;
			bool   m_use_inband_fec;
			bool   m_usedtx;

			copus_parameter()
				: m_min_time(0)
				, m_use_inband_fec(false)
				, m_usedtx(false){}
		};
	public:
		caudio_payload()
			: ccodec_payload()
			, m_channel(0)
			, m_opus_param()
		{
			m_type = "audio";
		}

		caudio_payload(uint8_t pt, std::string encode_name, int sample, int channel)
			: ccodec_payload(pt, encode_name, sample)
			, m_channel(channel)
			, m_opus_param()
		{
			m_type = "audio";
		
			m_opus_param.m_min_time = 0;
			m_opus_param.m_use_inband_fec = false;
			m_opus_param.m_usedtx = false;
		}

	   virtual  ~caudio_payload();
	public:
		virtual cmedia_payload_type  generate_media_payload_type();

		virtual ccodec_payload * copy();
	public:
		int32 set_opus_param_desc(std::string fmtp);
	protected:
	private:


	public:
		int32						m_channel;
		copus_parameter				m_opus_param;
	};
}

#endif // _C_AUDIO_PAYLOAD_H_