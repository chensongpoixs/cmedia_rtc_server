/***********************************************************************************************
created: 		2022-12-21

author:			chensong

purpose:		media_source


************************************************************************************************/

#ifndef _C_MEDIA_SOURCE_H_
#define _C_MEDIA_SOURCE_H_

#include "cmedia_type.h"
#include "crtp.h"
namespace chen {
	class cmedia_source
	{
	public:
		typedef std::function<bool(MediaChannelId, RtpPacket)>    SendFrameCallback;
	public:
		explicit cmedia_source() {}
		virtual ~cmedia_source(){}

	public:
		virtual EMediaType get_media_type() const
		{
			return m_media_type;
		}

		virtual std::string get_media_description(uint16_t port = 0) = 0;

		virtual std::string get_attribute() = 0;

		virtual bool handler_frame(MediaChannelId channelId, AVFrame frame) = 0;
		 virtual void set_send_frame_callback(const SendFrameCallback callback)
		{
			 m_send_frame_callback_ptr = callback;
		} 

		virtual uint32 GetPayloadType() const
		{
			return m_payload;
		}

		virtual uint32 GetClockRate() const
		{
			return m_clock_rate;
		}
	protected:
	//private:
		EMediaType	m_media_type;
		uint32		m_payload;
		uint32		m_clock_rate;
		SendFrameCallback m_send_frame_callback_ptr;

	};
	 
}

#endif/// _C_MEDIA_SOURCE_H_

