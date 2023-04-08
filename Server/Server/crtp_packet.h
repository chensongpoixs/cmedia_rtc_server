/***********************************************************************************************
created: 		2023-04-08

author:			chensong

purpose:		ccfg
************************************************************************************************/
#ifndef _C_RTP_PACKET_H_
#define _C_RTP_PACKET_H_
#include "cnet_type.h"
#include "crtp_headers.h"
namespace chen {


	class crtp_packet
	{
	public:
		crtp_packet();
		~crtp_packet();


	public:
		bool parse(const uint8* buffer, size_t size);

	protected:
	private:

		//bool				m_marker;
		//uint8				m_payload_type;
		//uint8				m_padding_size;
		//uint16				m_sequence_number;
		//uint32				m_timestamp;
		//uint32				m_ssrc;
		//size_t				m_payload_offset; // Match header size with csrcs and extensions
		//size_t				m_payload_size;

		/////////////////
		RtpHeader *				m_header_ptr;
		uint8 *					m_csrc_list_ptr;
		//RtpHeaderExtension*     m_rtp_header_extension_ptr;
	};

}

#endif // _C_RTP_PACKET_H_