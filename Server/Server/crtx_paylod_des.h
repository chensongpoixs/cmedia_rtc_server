/***********************************************************************************************
created: 		2023-01-15

author:			chensong

purpose:		_C_VIDEO_PAYLOAD_H_


************************************************************************************************/

#ifndef _C_RTX_PAYLOAD_DES_H_
#define _C_RTX_PAYLOAD_DES_H_
#include "cnet_type.h"
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include "crtc_sdp.h"
#include "cmedia_desc.h"
#include "ccodec_payload.h"


namespace chen {

	class crtx_payload_des : public ccodec_payload
	{
	public:
		crtx_payload_des()
		: ccodec_payload()
		, m_apt(0){}


		crtx_payload_des(uint8 pt, uint8 apt)
			: ccodec_payload(pt, "rtx", 8000)
			, m_apt(apt)
		{}

		virtual ~crtx_payload_des();
	public:
		virtual cmedia_payload_type generate_media_payload_type();

		virtual ccodec_payload * copy();

	protected:
	private:

	public:
		uint8					m_apt;
	};
}


#endif // _C_RTX_PAYLOAD_DES_H_