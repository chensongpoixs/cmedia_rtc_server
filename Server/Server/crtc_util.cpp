/***********************************************************************************************
created: 		2022-08-10

author:			chensong

purpose:		crtc_util


************************************************************************************************/

#include "crtc_util.h"
#ifdef _WIN32
#include <ws2ipdef.h>
// https://stackoverflow.com/a/24550632/2085408
#include <intrin.h>
#define __builtin_popcount __popcnt
#endif

namespace chen {
	namespace rtc_byte
	{
		uint8_t get1byte(const uint8_t * data, size_t i)
		{
			return data[i];
		}
		uint16_t get2bytes(const uint8_t * data, size_t i)
		{
			return uint16_t{ data[i + 1] } | uint16_t{ data[i] } << 8;;
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
}