/***********************************************************************************************
created: 		2023-04-08

author:			chensong

purpose:		ccfg

输赢不重要，答案对你们有什么意义才重要。

光阴者，百代之过客也，唯有奋力奔跑，方能生风起时，是时代造英雄，英雄存在于时代。或许世人道你轻狂，可你本就年少啊。 看护好，自己的理想和激情。


我可能会遇到很多的人，听他们讲好2多的故事，我来写成故事或编成歌，用我学来的各种乐器演奏它。
然后还可能在一个国家遇到一个心仪我的姑娘，她可能会被我帅气的外表捕获，又会被我深邃的内涵吸引，在某个下雨的夜晚，她会全身淋透然后要在我狭小的住处换身上的湿衣服。
3小时候后她告诉我她其实是这个国家的公主，她愿意向父皇求婚。我不得已告诉她我是穿越而来的男主角，我始终要回到自己的世界。
然后我的身影慢慢消失，我看到她眼里的泪水，心里却没有任何痛苦，我才知道，原来我的心被丢掉了，我游历全世界的原因，就是要找回自己的本心。
于是我开始有意寻找各种各样失去心的人，我变成一块砖头，一颗树，一滴水，一朵白云，去听大家为什么会失去自己的本心。
我发现，刚出生的宝宝，本心还在，慢慢的，他们的本心就会消失，收到了各种黑暗之光的侵蚀。
从一次争论，到嫉妒和悲愤，还有委屈和痛苦，我看到一只只无形的手，把他们的本心扯碎，蒙蔽，偷走，再也回不到主人都身边。
我叫他本心猎手。他可能是和宇宙同在的级别 但是我并不害怕，我仔细回忆自己平淡的一生 寻找本心猎手的痕迹。
沿着自己的回忆，一个个的场景忽闪而过，最后发现，我的本心，在我写代码的时候，会回来。
安静，淡然，代码就是我的一切，写代码就是我本心回归的最好方式，我还没找到本心猎手，但我相信，顺着这个线索，我一定能顺藤摸瓜，把他揪出来。
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