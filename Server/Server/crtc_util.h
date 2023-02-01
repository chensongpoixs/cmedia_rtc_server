/***********************************************************************************************
created: 		2022-08-10

author:			chensong

purpose:		crtc_util


************************************************************************************************/

#ifndef _C_RTC_UTIL_H_
#define _C_RTC_UTIL_H_
#include <iostream>
#include <cstdbool>
#include <cstdint>
#include "cnet_type.h"

namespace chen {
	namespace rtc_byte
	{
		/**
		 * Getters below get value in Host Byte Order.
		 * 
		 */
		uint8_t  get1byte(const uint8_t * data, size_t i);

		uint16 get2bytes(const uint8* data, size_t i);

		uint32_t get3bytes(const uint8_t* data, size_t i);


		uint32_t get4bytes(const uint8_t* data, size_t i);

		uint64_t get8bytes(const uint8_t* data, size_t i);

		/**
		* Setters below set value in Network Byte Order.
		*/
		void set1byte(uint8_t* data, size_t i, uint8_t value);

		void set2bytes(uint8_t* data, size_t i, uint16_t value);


		void set3bytes(uint8_t* data, size_t i, uint32_t value);

		void set4bytes(uint8_t* data, size_t i, uint32_t value);


		void set8bytes(uint8_t* data, size_t i, uint64_t value);


		uint16_t padto4bytes(uint16_t size);

		uint32_t padto4bytes(uint32_t size);
	}

	namespace rtc_bits
	{
		size_t countsetbits(const uint16_t mask);
	}


	namespace rtc_time
	{
		struct Ntp
		{
			uint32_t seconds;
			uint32_t fractions;
		};
		   Ntp TimeMs2Ntp(uint64_t ms);

		  uint64_t Ntp2TimeMs( Ntp ntp);

		  bool IsNewerTimestamp(uint32_t timestamp, uint32_t prevTimestamp);

		    uint32_t LatestTimestamp(uint32_t timestamp1, uint32_t timestamp2);
		uint32_t TimeMsToAbsSendTime(uint64_t ms);
	}
}

#endif //_C_RTC_UTIL_H_