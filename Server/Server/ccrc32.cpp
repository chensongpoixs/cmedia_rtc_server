﻿/***********************************************************************************************
created: 		2023-02-02

author:			chensong

purpose:		 buffer
************************************************************************************************/

#include "ccrc32.h"

namespace chen {
	// @see pycrc reflect at https://github.com/winlinvip/pycrc/blob/master/pycrc/algorithms.py#L107
	uint64_t __crc32_reflect(uint64_t data, int width)
	{
		uint64_t res = data & 0x01;

		for (int i = 0; i < (int)width - 1; i++) {
			data >>= 1;
			res = (res << 1) | (data & 0x01);
		}

		return res;
	}
	// @see pycrc gen_table at https://github.com/winlinvip/pycrc/blob/master/pycrc/algorithms.py#L178
	void __crc32_make_table(uint32_t t[256], uint32_t poly, bool reflect_in)
	{
		int width = 32; // 32bits checksum.
		uint64_t msb_mask = (uint32_t)(0x01 << (width - 1));
		uint64_t mask = (uint32_t)(((msb_mask - 1) << 1) | 1);

		int tbl_idx_width = 8; // table index size.
		int tbl_width = 0x01 << tbl_idx_width; // table size: 256

		for (int i = 0; i < (int)tbl_width; i++) {
			uint64_t reg = uint64_t(i);

			if (reflect_in) {
				reg = __crc32_reflect(reg, tbl_idx_width);
			}

			reg = reg << (width - tbl_idx_width);
			for (int j = 0; j < tbl_idx_width; j++) {
				if ((reg&msb_mask) != 0) {
					reg = (reg << 1) ^ poly;
				}
				else {
					reg = reg << 1;
				}
			}

			if (reflect_in) {
				reg = __crc32_reflect(reg, width);
			}

			t[i] = (uint32_t)(reg & mask);
		}
	}


	// @see pycrc table_driven at https://github.com/winlinvip/pycrc/blob/master/pycrc/algorithms.py#L207
	uint32_t __crc32_table_driven(uint32_t* t, const void* buf, int size, uint32_t previous, bool reflect_in, uint32_t xor_in, bool reflect_out, uint32_t xor_out)
	{
		int width = 32; // 32bits checksum.
		uint64_t msb_mask = (uint32_t)(0x01 << (width - 1));
		uint64_t mask = (uint32_t)(((msb_mask - 1) << 1) | 1);

		int tbl_idx_width = 8; // table index size.

		uint8_t* p = (uint8_t*)buf;
		uint64_t reg = 0;

		if (!reflect_in) {
			reg = xor_in;

			for (int i = 0; i < (int)size; i++) {
				uint8_t tblidx = (uint8_t)((reg >> (width - tbl_idx_width)) ^ p[i]);
				reg = t[tblidx] ^ (reg << tbl_idx_width);
			}
		}
		else {
			reg = previous ^ __crc32_reflect(xor_in, width);

			for (int i = 0; i < (int)size; i++) {
				uint8_t tblidx = (uint8_t)(reg ^ p[i]);
				reg = t[tblidx] ^ (reg >> tbl_idx_width);
			}

			reg = __crc32_reflect(reg, width);
		}

		if (reflect_out) {
			reg = __crc32_reflect(reg, width);
		}

		reg ^= xor_out;
		return (uint32_t)(reg & mask);
	}
	// @see pycrc https://github.com/winlinvip/pycrc/blob/master/pycrc/algorithms.py#L207
// IEEETable is the table for the IEEE polynomial.
	static uint32  __crc32_IEEE_table[256];
	static bool __crc32_IEEE_table_initialized = false;

	// @see pycrc https://github.com/winlinvip/pycrc/blob/master/pycrc/models.py#L220
	//      crc32('123456789') = 0xcbf43926
	// where it's defined as model:
	//      'name':         'crc-32',
	//      'width':         32,
	//      'poly':          0x4c11db7,
	//      'reflect_in':    True,
	//      'xor_in':        0xffffffff,
	//      'reflect_out':   True,
	//      'xor_out':       0xffffffff,
	//      'check':         0xcbf43926,
	uint32  srs_crc32_ieee(const void* buf, int32 size, uint32  previous)
	{
		// @see golang IEEE of hash/crc32/crc32.go
		// IEEE is by far and away the most common CRC-32 polynomial.
		// Used by ethernet (IEEE 802.3), v.42, fddi, gzip, zip, png, ...
		// @remark The poly of CRC32 IEEE is 0x04C11DB7, its reverse is 0xEDB88320,
		//      please read https://en.wikipedia.org/wiki/Cyclic_redundancy_check
		uint32_t poly = 0x04C11DB7;

		bool reflect_in = true;
		uint32_t xor_in = 0xffffffff;
		bool reflect_out = true;
		uint32_t xor_out = 0xffffffff;

		if (!__crc32_IEEE_table_initialized) {
			__crc32_make_table(__crc32_IEEE_table, poly, reflect_in);
			__crc32_IEEE_table_initialized = true;
		}

		return __crc32_table_driven(__crc32_IEEE_table, buf, size, previous, reflect_in, xor_in, reflect_out, xor_out);
	}

}