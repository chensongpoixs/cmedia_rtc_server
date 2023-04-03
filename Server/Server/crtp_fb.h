/***********************************************************************************************
created: 		2023-04-03

author:			chensong

purpose:		api_rtc_publish


************************************************************************************************/

#ifndef _C_RTP_FB_H_
#define _C_RTP_FB_H_
#include "cnet_type.h"
#include "clog.h"
#include "cfunction_view.h"
#include "carray_view.h"
#include "buffer.h"
#include "crtcp_packet.h"

namespace chen {


	class crtp_fb : public crtcp_packet
	{
	public:
		static constexpr uint8 kPacketType = 205;

		crtp_fb() : sender_ssrc_(0), media_ssrc_(0) {}
		virtual ~crtp_fb()  {}

		void SetSenderSsrc(uint32 ssrc) { sender_ssrc_ = ssrc; }
		void SetMediaSsrc(uint32 ssrc) { media_ssrc_ = ssrc; }

		uint32 sender_ssrc() const { return sender_ssrc_; }
		uint32 media_ssrc() const { return media_ssrc_; }

	protected:
		static constexpr size_t kCommonFeedbackLength = 8;
		void ParseCommonFeedback(const uint8* payload);
		void CreateCommonFeedback(uint8* payload) const;

	private:
		uint32 sender_ssrc_;
		uint32 media_ssrc_;
	public:
	protected:
	private:
	};


}

#endif //_C_RTP_FB_H_

