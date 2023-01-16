/***********************************************************************************************
created: 		2023-01-15

author:			chensong

purpose:		_C_RTC_TRACK_DESCRIPTION_H_


************************************************************************************************/

#ifndef _C_RTC_TRACK_DESCRIPTION_H_
#define _C_RTC_TRACK_DESCRIPTION_H_
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

	class crtc_track_description
	{
	public:
		crtc_track_description()
		: m_type("")
		, m_id("")
		, m_ssrc(0)
		, m_fec_ssrc(0)
		, m_rtx_ssrc(0)
		, m_extmaps()
		, m_is_active(false)
		, m_direction("")
		, m_mid("")
		, m_msid("")
		, m_media_ptr(NULL)
		, m_red_ptr(NULL)
		, m_rtx_ptr(NULL)
		, m_ulpfec_ptr(NULL){}
		~crtc_track_description();


	public:
		// whether or not the track has ssrc.
		// for example:
		//    we need check track has the ssrc in the ssrc_group, then add ssrc_group to the track,
		bool has_ssrc(uint32_t ssrc);

 
	public:
		void add_rtp_extension_desc(int32 id, std::string uri);
		void del_rtp_extension_desc(std::string uri);
		void set_direction(std::string direction);
		void set_codec_payload(ccodec_payload* payload);
		// auxiliary paylod include red, rtx, ulpfec.
		void create_auxiliary_payload(const std::vector<cmedia_payload_type> payload_types);
		void set_rtx_ssrc(uint32 ssrc);
		void set_fec_ssrc(uint32 ssrc);
		void set_mid(std::string mid);
		int32 get_rtp_extension_id(std::string uri);
	protected:
	private:


	public:

		// type: audio, video
		std::string											m_type;
		// track_id
		std::string											m_id;
		// ssrc is the primary ssrc for this track,
		// if sdp has ssrc-group, it is the first ssrc of the ssrc-group
		uint32												m_ssrc;
		// rtx ssrc is the second ssrc of "FEC" src-group,
		// if no rtx ssrc, rtx_ssrc_ = 0.
		uint32												m_fec_ssrc;
		// rtx ssrc is the second ssrc of "FID" src-group,
		// if no rtx ssrc, rtx_ssrc_ = 0.
		uint32												m_rtx_ssrc;
		// key: rtp header extension id, value: rtp header extension uri.
		std::map<int, std::string>							m_extmaps;
		// Whether this track active. default: active.
		bool												m_is_active;
		// direction
		std::string											m_direction;
		// mid is used in BOUNDLE
		std::string											m_mid;
		// msid_: track stream id
		std::string											m_msid;

		// meida payload, such as opus, h264. 
		ccodec_payload *									m_media_ptr;
		ccodec_payload *									m_red_ptr;
		ccodec_payload *									m_rtx_ptr;
		ccodec_payload *									m_ulpfec_ptr;
		 


	};

}


#endif // _C_RTC_TRACK_DESCRIPTION_H_