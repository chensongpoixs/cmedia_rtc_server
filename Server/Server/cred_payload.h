/***********************************************************************************************
created: 		2023-01-15

author:			chensong

purpose:		_C_RED_PAYLOAD_H_


************************************************************************************************/

#ifndef _C_RED_PAYLOAD_H_
#define _C_RED_PAYLOAD_H_
#include "cnet_type.h"
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include "crtc_sdp.h"
#include "cmedia_desc.h"
#include "ccodec_payload.h"



namespace chen {
	class cred_paylod : public ccodec_payload
	{
	public:
		cred_paylod()
		: ccodec_payload()
		, m_channel(0){}


		cred_paylod(uint8 pt, std::string encode_name, int32 sample, int32 channel)
		: ccodec_payload(pt, encode_name, sample)
		, m_channel(channel){}

		virtual ~cred_paylod();


	public:
		virtual cmedia_payload_type generate_media_payload_type();
	protected:
	private:


	public:
		int32  m_channel;
	};
}


#endif // _C_RED_PAYLOAD_H_