/***********************************************************************************************
created: 		2023-02-20

author:			chensong

purpose:		api_rtc_publish


************************************************************************************************/

#ifndef _C_RTP_RTCP_H_
#define _C_RTP_RTCP_H_
#include "cnet_type.h"
#include <sstream>
#include "cencoder.h"
#include <vector>
#include "crtp_rtcp_define.h"
namespace chen {

	class crtcp_common : public iccodec
	{
	public:
		explicit crtcp_common()
		: iccodec()
		, m_header()
		, m_ssrc(0)
		, m_payload()
		, m_payload_len(0)
		, m_data(NULL)
		, m_nb_data(0){}
		virtual ~crtcp_common();
	protected:
	protected:
		bool decode_header(cbuffer *buffer);
		bool encode_header(cbuffer *buffer);
	public:
		/*crtcp_common();
		virtual ~crtcp_common();*/
		virtual uint8_t type() const;
		virtual uint8_t get_rc() const;

		uint32_t get_ssrc();
		void set_ssrc(uint32_t ssrc);

		char* data();
		int32 size();
		// interface ISrsCodec
	public:
		virtual bool decode(cbuffer *buffer);
		virtual uint64_t nb_bytes();
		virtual bool encode(cbuffer *buffer);
	private:

		crtcp_header		m_header;
		uint32_t			m_ssrc;
		uint8_t				m_payload[kRtcpPacketSize];
		int32				m_payload_len;

		char*				m_data;
		int32				m_nb_data;
	};

	class crtcp_compound : public iccodec
	{
	public:
		explicit crtcp_compound()
		: m_rtcps()
		, m_nb_bytes()
		, m_data(NULL)
		, m_nb_data(0){}
		virtual ~crtcp_compound();
	public:
		// TODO: FIXME: Should rename it to pop(), because it's not a GET method.
		crtcp_common* get_next_rtcp();
		bool add_rtcp(crtcp_common *rtcp);
		void clear();

		char* data();
		int32 size();

		// interface ISrsCodec
	public:
		virtual bool decode(cbuffer *buffer);
		virtual uint64_t nb_bytes();
		virtual bool encode(cbuffer *buffer);
	protected:

	private:
		std::vector<crtcp_common*> m_rtcps;
		int32			m_nb_bytes;
		char*			m_data;
		int32			m_nb_data;
	};
}

#endif // _C_RTP_RTCP_H_