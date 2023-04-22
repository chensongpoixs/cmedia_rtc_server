/***********************************************************************************************
created: 		2022-08-10

author:			chensong

purpose:		crtc_util

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

#include "crtc_util.h"
#ifdef _WIN32
//#include <ws2ipdef.h>
// https://stackoverflow.com/a/24550632/2085408
#include <intrin.h>
#define __builtin_popcount __popcnt
#endif
#include <string>
#include "clog.h"
namespace chen {
	namespace rtc_byte
	{
		uint8_t get1byte(const uint8_t * data, size_t i)
		{
			return data[i];
		}
		uint16 get2bytes(const uint8  * data, size_t i)
		{
			return uint16 { data[i + 1] } | uint16 { data[i] } << 8;;
		}
		uint32_t get3bytes(const uint8_t * data, size_t i)
		{
			return  uint32_t{ data[i + 2] } | uint32_t{ data[i + 1] } << 8 | uint32_t{ data[i] } << 16;
		}
		uint32_t get4bytes(const uint8_t * data, size_t i)
		{
			return uint32_t{ data[i + 3] } | uint32_t{ data[i + 2] } << 8 | uint32_t{ data[i + 1] } << 16 | uint32_t{ data[i] } << 24;
		}
		uint64_t get8bytes(const uint8_t * data, size_t i)
		{
			return uint64_t{  get4bytes(data, i) } << 32 |  get4bytes(data, i + 4);
		}
		std::string getbytes(const uint8 * data, size_t i, size_t len)
		{
			assert(data);
			std::string value;
			value.append((char *)(data + i), len);
			return value;
		}
		//std::string get12bytes(const uint8 * data, size_t i)
		//{
		//	//char buffer[12] = {0};
		//	std::string value;
		//	value.append(( char * )(data + i), 12);


		//	return value;
		//}
		 
		void set1byte(uint8_t * data, size_t i, uint8_t value)
		{
			data[i] = value;
		}
		void set2bytes(uint8_t * data, size_t i, uint16_t value)
		{
			data[i + 1] = static_cast<uint8_t>(value);
			data[i] = static_cast<uint8_t>(value >> 8);
		}
		void set3bytes(uint8_t * data, size_t i, uint32_t value)
		{
			data[i + 2] = static_cast<uint8_t>(value);
			data[i + 1] = static_cast<uint8_t>(value >> 8);
			data[i] = static_cast<uint8_t>(value >> 16);
		}
		void set4bytes(uint8_t * data, size_t i, uint32_t value)
		{
			data[i + 3] = static_cast<uint8_t>(value);
			data[i + 2] = static_cast<uint8_t>(value >> 8);
			data[i + 1] = static_cast<uint8_t>(value >> 16);
			data[i] = static_cast<uint8_t>(value >> 24);
		}
		void set8bytes(uint8_t * data, size_t i, uint64_t value)
		{
			data[i + 7] = static_cast<uint8_t>(value);
			data[i + 6] = static_cast<uint8_t>(value >> 8);
			data[i + 5] = static_cast<uint8_t>(value >> 16);
			data[i + 4] = static_cast<uint8_t>(value >> 24);
			data[i + 3] = static_cast<uint8_t>(value >> 32);
			data[i + 2] = static_cast<uint8_t>(value >> 40);
			data[i + 1] = static_cast<uint8_t>(value >> 48);
			data[i] = static_cast<uint8_t>(value >> 56);
		}
		void setbytes(uint8 * data, size_t i, uint8 * p, int32 len)
		{
			cassert(data);
			cassert( p);
			cassert(len);
			memcpy(data + i, p, len);
		}
		uint16_t padto4bytes(uint16_t size)
		{
			// If size is not multiple of 32 bits then pad it.
			if (size & 0x03)
			{
				return (size & 0xFFFC) + 4;
			}
			else
			{
				return size;
			}
			return size;
		}
		uint32_t padto4bytes(uint32_t size)
		{
			// If size is not multiple of 32 bits then pad it.
			if (size & 0x03)
			{
				return (size & 0xFFFFFFFC) + 4;
			}
			else
			{
				return size;
			}
			return size;
		}
	}



	namespace rtc_bits
	{
		size_t countsetbits(const uint16_t mask)
		{
			return static_cast<size_t>(__builtin_popcount(mask));
		}
	}

	namespace rtc_time
	{
		// Seconds from Jan 1, 1900 to Jan 1, 1970.
		static const  uint32_t UnixNtpOffset{ 0x83AA7E80 };
		// NTP fractional unit.
		static const uint64_t NtpFractionalUnit{ 1LL << 32 };
		   Ntp TimeMs2Ntp(uint64_t ms)
		{
			 Ntp ntp; // NOLINT(cppcoreguidelines-pro-type-member-init)

			ntp.seconds = ms / 1000;
			ntp.fractions =
				static_cast<uint32_t>((static_cast<double>(ms % 1000) / 1000) * NtpFractionalUnit);

			return ntp;
		}

		  uint64_t Ntp2TimeMs(Ntp ntp)
		{
			// clang-format off
			return (
				static_cast<uint64_t>(ntp.seconds) * 1000 +
				static_cast<uint64_t>(std::round((static_cast<double>(ntp.fractions) * 1000) / NtpFractionalUnit))
				);
			// clang-format on
		}

		  bool IsNewerTimestamp(uint32_t timestamp, uint32_t prevTimestamp)
		{
			// Distinguish between elements that are exactly 0x80000000 apart.
			// If t1>t2 and |t1-t2| = 0x80000000: IsNewer(t1,t2)=true,
			// IsNewer(t2,t1)=false
			// rather than having IsNewer(t1,t2) = IsNewer(t2,t1) = false.
			if (static_cast<uint32_t>(timestamp - prevTimestamp) == 0x80000000)
				return timestamp > prevTimestamp;

			return timestamp != prevTimestamp &&
				static_cast<uint32_t>(timestamp - prevTimestamp) < 0x80000000;
		}

		  uint32_t LatestTimestamp(uint32_t timestamp1, uint32_t timestamp2)
		{
			return IsNewerTimestamp(timestamp1, timestamp2) ? timestamp1 : timestamp2;
		}
		uint32_t TimeMsToAbsSendTime(uint64_t ms)
		{
			return static_cast<uint32_t>(((ms << 18) + 500) / 1000) & 0x00FFFFFF;
		}
	}
}