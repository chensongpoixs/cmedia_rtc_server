/***********************************************************************************************
created: 		2023-04-08

author:			chensong

purpose:		ccfg
************************************************************************************************/
#ifndef _C_RTP_PACKET_H_
#define _C_RTP_PACKET_H_
#include "cnet_type.h"
#include "crtp_headers.h"
#include <map>
namespace chen {


	class crtp_packet
	{
	public:
		/* Struct for RTP header. */
		struct crtp_header
		{
#if defined(MS_LITTLE_ENDIAN)
			uint8  csrcCount : 4;
			uint8  extension : 1;
			uint8  padding : 1;
			uint8  version : 2;
			uint8  payloadType : 7;
			uint8  marker : 1;
#elif defined(MS_BIG_ENDIAN)
			uint8 version : 2;
			uint8 padding : 1;
			uint8 extension : 1;
			uint8 csrcCount : 4;
			uint8 marker : 1;
			uint8 payloadType : 7;
#endif
			uint16  sequenceNumber;
			uint32  timestamp;
			uint32  ssrc;
		};
		/* Struct for RTP header extension. */
		struct crtp_header_extension
		{
			uint16  id;
			uint16  length; // Size of value in multiples of 4 bytes.
			uint8 value[1];
		};
		/* Struct for One-Byte extension. */
		struct cone_byte_extension
		{
#if defined(MS_LITTLE_ENDIAN)
			uint8  len : 4;
			uint8  id : 4;
#elif defined(MS_BIG_ENDIAN)
			uint8  id : 4;
			uint8  len : 4;
#endif
			uint8  value[1];
		};
		/* Struct for Two-Bytes extension. */
		struct ctwo_bytes_extension
		{
			uint8  id : 8;
			uint8  len : 8;
			uint8  value[1];
		};
		/* Struct with frame-marking information. */

// Frame Marking.
//
// Meta-information about an RTP stream outside the encrypted media payload,
// useful for an RTP switch to do codec-agnostic selective forwarding
// without decrypting the payload.
//
// For non-scalable streams:
//    0                   1
//    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |  ID   | L = 0 |S|E|I|D|0 0 0 0|
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// For scalable streams:
//    0                   1                   2                   3
//    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |  ID   | L = 2 |S|E|I|D|B| TID |      LID      |   TL0PICIDX   |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
///////////////////////////////////////////////////////////////////////////////////////
//	各个字段解释如下：
//	
//		S				: Start of Frame(1 bit) ，一帧的开始。 
//		E				: End of Frame(1 bit) ，一帧的结束。 
//		I				: Independent Frame(1 bit) ，是否是独立可解码的一帧，如264的IDR。 
//		D				: Discardable Frame(1 bit) ，可以被丢弃的帧，如时域分层中high layer，丢弃后其他帧仍然能正常解码。 
//		B				: Base Layer Sync(1 bit) ，当TID不为0，发送端知道该帧只依赖base layer，则填1，否则填0
//	    TID				: Temporal ID(3 bits) ，时域分层中的层ID，0表示base layer。短的extension格式中，填0.
//		LID				: Layer ID(8 bits) ，空域或者质量分层的层ID，0标识base layer。 
//		TL0PICIDX		: Temporal Layer 0 Picture Index(8 bits) ，时域0层picture index。
//						  因为只有8个bit，所以是循环计数的。TID = 0 LID = 0标识该帧ID，TID != 0或者LID != 0标识该帧依赖哪一个picture index。
//						  如果不存在时域分层或者计数未知，这个字段可以省略。
//	
///////////////////////////////////////////////////////////////////////////////////////

		struct cframe_marking
		{
#if defined(MS_LITTLE_ENDIAN)
			uint8  tid : 3;
			uint8  base : 1;
			uint8  discardable : 1; //可以被丢弃的帧，如时域分层中high layer，丢弃后其他帧仍然能正常解码。 
			uint8  independent : 1; // 是否是独立可解码的一帧，如264的IDR。 
			uint8  end : 1;
			uint8  start : 1;
#elif defined(MS_BIG_ENDIAN)
			uint8  start : 1;
			uint8  end : 1;
			uint8  independent : 1;
			uint8  discardable : 1;
			uint8  base : 1;
			uint8  tid : 3;
#endif
			uint8  lid;
			uint8  tl0picidx;
		};
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
		/*RtpHeader *				m_header_ptr;
		uint8 *					m_csrc_list_ptr;*/
		//RtpHeaderExtension*     m_rtp_header_extension_ptr;

		crtp_header			 *							m_header_ptr{ nullptr };
		uint8 *											m_csrcList{ nullptr };
		crtp_header_extension *							 m_headerExtension{ nullptr };   // rtp中扩展头 
		std::map<uint8 , cone_byte_extension*>			m_mapOneByteExtensions;
		std::map<uint8 , ctwo_bytes_extension *>		m_mapTwoBytesExtensions;
		uint8											m_midExtensionId{ 0u };
		uint8											m_ridExtensionId{ 0u };
		uint8											m_rridExtensionId{ 0u };
		uint8											m_absSendTimeExtensionId{ 0u };
		uint8											m_transportWideCc01ExtensionId{ 0u };
		uint8											m_frameMarking07ExtensionId{ 0u }; // NOTE: Remove once RFC.
		uint8											m_frameMarkingExtensionId{ 0u };
		uint8											m_ssrcAudioLevelExtensionId{ 0u };
		uint8											m_videoOrientationExtensionId{ 0u };
		uint8 *											m_payload{ nullptr };
		size_t											m_payloadLength{ 0u };
		uint8											m_payloadPadding{ 0u };
		size_t											m_size{ 0u }; // Full size of the packet in bytes.
		// Codecs
		//std::unique_ptr<Codecs::PayloadDescriptorHandler> payloadDescriptorHandler;
	};

}

#endif // _C_RTP_PACKET_H_