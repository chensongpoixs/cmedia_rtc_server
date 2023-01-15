/***********************************************************************************************
created: 		2023-01-15

author:			chensong

purpose:		_C_VIDEO_PAYLOAD_H_


************************************************************************************************/

#ifndef _C_VIDEO_PAYLOAD_H_
#define _C_VIDEO_PAYLOAD_H_
#include "cnet_type.h"
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include "crtc_sdp.h"
#include "cmedia_desc.h"
#include "ccodec_payload.h"


namespace chen {

	class cvideo_payload :public ccodec_payload
	{
	public:
		cvideo_payload()
			: ccodec_payload()
			, m_h264_param() 
		{
			m_type = "video";
		}


		cvideo_payload(uint8 pt, std::string encode_name, int32 sample)
			: ccodec_payload(pt, encode_name, sample)
			, m_h264_param()
		{
			m_type = "video";
			m_h264_param.m_profile_level_id = "";
			m_h264_param.m_packetization_mode = "";
			m_h264_param.m_level_asymmerty_allow = "";
		}

		virtual ~cvideo_payload();


	public:
		virtual cmedia_payload_type generate_media_payload_type();
	public:

		int32  set_h264_param_desc(std::string fmtp);
	protected:
	private:


	public:
		ch264_specific_param		m_h264_param;
	};

}


#endif // _C_VIDEO_PAYLOAD_H_