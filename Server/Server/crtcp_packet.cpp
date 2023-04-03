/***********************************************************************************************
created: 		2023-04-03

author:			chensong

purpose:		api_rtc_publish


************************************************************************************************/
#include "crtcp_packet.h"
#include "clog.h"
#include "cfunction_view.h"
#include "buffer.h"
namespace chen {
	void ExplicitZeroMemory(void* ptr, size_t len) {
		RTC_DCHECK(ptr || !len);
#if _MSC_VER
		SecureZeroMemory(ptr, len);
#else
		memset(ptr, 0, len);
#if !defined(__pnacl__)
		/* As best as we can tell, this is sufficient to break any optimisations that
		   might try to eliminate "superfluous" memsets. If there's an easy way to
		   detect memset_s, it would be better to use that. */
		__asm__ __volatile__("" : : "r"(ptr) : "memory");  // NOLINT
#endif
#endif  // !_MSC_VER
	}
	 Buffer crtcp_packet::Build() const {
		 Buffer packet(BlockLength());

		size_t length = 0;
		bool created = Create(packet.data(), &length, packet.capacity(), nullptr);
		cassert_desc(created,  "Invalid packet is not supported.");
		cassert_desc(length <= packet.size() , "BlockLength mispredicted size used by Create");

		return packet;
	}

	bool crtcp_packet::Build(size_t max_length, PacketReadyCallback callback) const
	{
		RTC_CHECK_LE(max_length , IP_PACKET_SIZE);
		uint8 buffer[IP_PACKET_SIZE];
		size_t index = 0;
		if (!Create(buffer, &index, max_length, callback))
		{
			return false;
		}
		return OnBufferFull(buffer, &index, callback);
	}

	bool crtcp_packet::OnBufferFull(uint8* packet, size_t* index, PacketReadyCallback callback) const 
	{
		if (*index == 0)
		{
			return false;
		}
		cassert_desc(callback,  "Fragmentation not supported.");
		callback( ArrayView<const uint8>(packet, *index));
		*index = 0;
		return true;
	}

	size_t crtcp_packet::HeaderLength() const {
		size_t length_in_bytes = BlockLength();
		cassert(length_in_bytes >= 0);
		cassert_desc((length_in_bytes % 4) >= 0 ,  "Padding must be handled by each subclass.");
		// Length in 32-bit words without common header.
		return (length_in_bytes - kHeaderLength) / 4;
	}

	// From RFC 3550, RTP: A Transport Protocol for Real-Time Applications.
	//
	// RTP header format.
	//   0                   1                   2                   3
	//   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//  |V=2|P| RC/FMT  |      PT       |             length            |
	//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	void crtcp_packet::CreateHeader(size_t count_or_format,  // Depends on packet type. 
		uint8 packet_type, size_t length, uint8* buffer, size_t* pos)
	{
		CreateHeader(count_or_format, packet_type, length, /*padding=*/false, buffer,
			pos);
	}

	void crtcp_packet::CreateHeader(size_t count_or_format,  // Depends on packet type.
		uint8 packet_type, size_t length, bool padding, uint8* buffer, size_t* pos)
	{
		RTC_DCHECK_LE(length, 0xffffU);
		RTC_DCHECK_LE(count_or_format, 0x1f);
		constexpr uint8_t kVersionBits = 2 << 6;
		uint8_t padding_bit = padding ? 1 << 5 : 0;
		buffer[*pos + 0] = kVersionBits | padding_bit | static_cast<uint8_t>(count_or_format);
		buffer[*pos + 1] = packet_type;
		buffer[*pos + 2] = (length >> 8) & 0xff;
		buffer[*pos + 3] = length & 0xff;
		*pos += kHeaderLength;
	}

}