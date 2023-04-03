/***********************************************************************************************
created: 		2023-04-03

author:			chensong

purpose:		api_rtc_publish


************************************************************************************************/

#ifndef _C_RTCP_PACKET_H_
#define _C_RTCP_PACKET_H_
#include "cnet_type.h"
#include "clog.h"
#include "cfunction_view.h"
#include "carray_view.h"
#include "buffer.h"

#define RTCP_CNAME_SIZE 256  // RFC 3550 page 44, including null termination
#define IP_PACKET_SIZE 1500  // we assume ethernet
namespace chen {
	// Class for building RTCP packets.
//
//  Example:
//  ReportBlock report_block;
//  report_block.SetMediaSsrc(234);
//  report_block.SetFractionLost(10);
//
//  ReceiverReport rr;
//  rr.SetSenderSsrc(123);
//  rr.AddReportBlock(report_block);
//
//  Fir fir;
//  fir.SetSenderSsrc(123);
//  fir.AddRequestTo(234, 56);
//
//  size_t length = 0;                     // Builds an intra frame request
//  uint8_t packet[kPacketSize];           // with sequence number 56.
//  fir.Build(packet, &length, kPacketSize);
//
//  rtc::Buffer packet = fir.Build();      // Returns a RawPacket holding
//                                         // the built rtcp packet.
//
//  CompoundPacket compound;               // Builds a compound RTCP packet with
//  compound.Append(&rr);                  // a receiver report, report block
//  compound.Append(&fir);                 // and fir message.
//  rtc::Buffer packet = compound.Build();
	class crtcp_packet
	{
	public:
		// Callback used to signal that an RTCP packet is ready. Note that this may
  // not contain all data in this RtcpPacket; if a packet cannot fit in
  // max_length bytes, it will be fragmented and multiple calls to this
  // callback will be made.
	using PacketReadyCallback = cfunction_view<void(ArrayView<const uint8> packet)>;
	public:
		virtual ~crtcp_packet() {}


		// Convenience method mostly used for test. Creates packet without
  // fragmentation using BlockLength() to allocate big enough buffer.
		 Buffer Build() const;

		// Returns true if call to Create succeeded.
		bool Build(size_t max_length, PacketReadyCallback callback) const;

		// Size of this packet in bytes (including headers).
		virtual size_t BlockLength() const = 0;

		// Creates packet in the given buffer at the given position.
		// Calls PacketReadyCallback::OnPacketReady if remaining buffer is too small
		// and assume buffer can be reused after OnPacketReady returns.
		virtual bool Create(uint8* packet, size_t* index, size_t max_length, PacketReadyCallback callback) const = 0;
	protected:
		// Size of the rtcp common header.
		static const  size_t kHeaderLength = 4;
		crtcp_packet() {}

		static void CreateHeader(size_t count_or_format,
			uint8 packet_type,
			size_t block_length,  // Payload size in 32bit words.
			uint8* buffer,
			size_t* pos);

		static void CreateHeader(size_t count_or_format,
			uint8 packet_type,
			size_t block_length,  // Payload size in 32bit words.
			bool padding,  // True if there are padding bytes.
			uint8* buffer,
			size_t* pos);

		bool OnBufferFull(uint8* packet,
			size_t* index,
			PacketReadyCallback callback) const;
		// Size of the rtcp packet as written in header.
		size_t HeaderLength() const;

	public:

	protected:
	private:
	};
}
#endif // _C_RTCP_PACKET_H_