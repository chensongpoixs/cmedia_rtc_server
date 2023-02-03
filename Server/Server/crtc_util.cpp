/***********************************************************************************************
created: 		2022-08-10

author:			chensong

purpose:		crtc_util


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