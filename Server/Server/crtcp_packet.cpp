/***********************************************************************************************
created: 		2023-04-03

author:			chensong

purpose:		api_rtc_publish
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