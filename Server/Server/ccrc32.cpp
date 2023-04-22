/***********************************************************************************************
created: 		2023-02-02

author:			chensong

purpose:		 buffer
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