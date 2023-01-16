/***********************************************************************************************
created: 		2023-01-15

author:			chensong

purpose:		_C_RTC_TRACK_DESCRIPTION_H_


************************************************************************************************/

#ifndef _C_RTC_SOURCE_DESCRIPTION_H_
#define _C_RTC_SOURCE_DESCRIPTION_H_
#include "cnet_type.h"
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include "crtc_sdp.h"
#include "cmedia_desc.h"
#include "crtx_paylod_des.h"
#include "cred_payload.h"
#include "cvideo_payload.h"
#include "caudio_payload.h"
#include "cred_payload.h"

namespace chen {
	class crtc_track_description;
	class crtc_source_description
	{
	public:
		crtc_source_description()
		: m_id("")
		, m_audio_track_desc_ptr(NULL)
		, m_video_track_descs(){}
		~crtc_source_description();

	public:
		crtc_track_description * find_track_description_by_ssrc(uint32 ssrc);
	protected:
	private:

	public:
		// the id for this stream;
		std::string								m_id;


		crtc_track_description*					m_audio_track_desc_ptr;


		std::vector<crtc_track_description*>		m_video_track_descs;

	};
}


#endif //_C_RTC_SOURCE_DESCRIPTION_H_