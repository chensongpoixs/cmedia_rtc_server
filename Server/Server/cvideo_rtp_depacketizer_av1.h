/***********************************************************************************************
created: 		2023-02-20

author:			chensong

purpose:		api_rtc_publish


************************************************************************************************/

#ifndef _C_VIDEO_RTP_DEPACKETIZER_AV1_H_
#define _C_VIDEO_RTP_DEPACKETIZER_AV1_H_
#include "cnet_type.h"
#include <sstream>
#include "cencoder.h"
#include <vector>
#include "crtp_rtcp_define.h"
#include <map>
#include <set>
#include "cvcm_encoded_frame.h"

namespace chen {
	class cvideo_rtp_depacketizer_av1
	{
	public:

		explicit cvideo_rtp_depacketizer_av1() {}
		virtual ~cvideo_rtp_depacketizer_av1() {}


	public:
		std::vector<cvcm_encoded_frame> AssembleFrame(const std::vector<cvcm_packet> & data);
	protected:
	private:
	};
}

#endif // _C_VIDEO_RTP_DEPACKETIZER_AV1_H_