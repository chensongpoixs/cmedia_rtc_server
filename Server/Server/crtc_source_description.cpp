/***********************************************************************************************
created: 		2023-01-15

author:			chensong

purpose:		_C_RTC_TRACK_DESCRIPTION_H_


************************************************************************************************/
#include "crtc_source_description.h"
#include "crtc_track_description.h"

namespace chen {
	crtc_source_description::~crtc_source_description()
	{
	}
	crtc_track_description * crtc_source_description::find_track_description_by_ssrc(uint32 ssrc)
	{
		if (m_audio_track_desc_ptr && m_audio_track_desc_ptr->has_ssrc(ssrc))
		{
			return m_audio_track_desc_ptr;
		}

		for (size_t i = 0; i <  m_video_track_descs.size(); ++i) 
		{
			if (m_video_track_descs.at(i)->has_ssrc(ssrc)) 
			{
				return m_video_track_descs.at(i);
			}
		}

		return NULL;
	}
}