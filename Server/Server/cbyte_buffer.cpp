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
#include "cbyte_buffer.h"

#include <stdint.h>

#include <cstring>
#include "cbyte_order.h"
 
namespace chen {
	bool cbyte_buffer_reader::ReadUInt8(uint8_t * val)
	{
		if (!val)
		{
			return false;
		}

		return ReadBytes(reinterpret_cast<char*>(val), 1);
	}
	bool cbyte_buffer_reader::ReadUInt16(uint16_t * val)
	{
		if (!val)
		{
			return false;
		}

		uint16_t v;
		if (!ReadBytes(reinterpret_cast<char*>(&v), 2))
		{
			return false;
		}
		else 
		{
			*val = NetworkToHost16(v);
			return true;
		}
	}
	bool cbyte_buffer_reader::ReadUInt24(uint32_t * val)
	{
		if (!val)
			return false;

		uint32_t v = 0;
		char* read_into = reinterpret_cast<char*>(&v);
		++read_into;

		if (!ReadBytes(read_into, 3)) {
			return false;
		}
		else {
			*val = NetworkToHost32(v);
			return true;
		}
	}
	bool cbyte_buffer_reader::ReadUInt32(uint32_t * val)
	{
		if (!val)
			return false;

		uint32_t v;
		if (!ReadBytes(reinterpret_cast<char*>(&v), 4)) {
			return false;
		}
		else {
			*val = NetworkToHost32(v);
			return true;
		}
	}
	bool cbyte_buffer_reader::ReadUInt64(uint64_t * val)
	{
		if (!val)
			return false;

		uint64_t v;
		if (!ReadBytes(reinterpret_cast<char*>(&v), 8)) {
			return false;
		}
		else {
			*val = NetworkToHost64(v);
			return true;
		}
	}
	bool cbyte_buffer_reader::ReadUVarint(uint64_t * val)
	{
		if (!val) {
			return false;
		}
		// Integers are deserialized 7 bits at a time, with each byte having a
		// continuation byte (msb=1) if there are more bytes to be read.
		uint64_t v = 0;
		for (int i = 0; i < 64; i += 7) {
			char byte;
			if (!ReadBytes(&byte, 1)) {
				return false;
			}
			// Read the first 7 bits of the byte, then offset by bits read so far.
			// 读取字节的前7位，然后按迄今为止读取的位进行偏移。
			/* TODO@chensong 2023-07-24  AV1文件格式 读取
			>>:右移，将一个数的二进制位全部右移N位，高位舍弃，低位补零。
		>>1:相当于将一个数的二进制全部右移1位，高位舍弃，低位补零。
		整数中：8>>1,的含意与8/2，相同。
		<<:左移，用来将一个数的二进制位全部左移N位，高位舍弃，低位补零。
			*/
			v |= (static_cast<uint64_t>(byte) & 0x7F) << i;
			// True if the msb is not a continuation byte.
			if (static_cast<uint64_t>(byte) < 0x80) {
				*val = v;
				return true;
			}
		}
		return false;
	}
	bool cbyte_buffer_reader::ReadBytes(char * val, size_t len)
	{
		if (len > Length()) {
			return false;
		}
		else {
			memcpy(val, m_bytes+ m_start, len);
			m_start += len;
			return true;
		}
	}
	bool cbyte_buffer_reader::ReadString(std::string * val, size_t len)
	{
		if (!val)
			return false;

		if (len > Length()) {
			return false;
		}
		else {
			val->append(m_bytes + m_start, len);
			m_start += len;
			return true;
		}
		return false;
	}
	bool cbyte_buffer_reader::Consume(size_t size)
	{
		if (size > Length())
		{
			return false;
		}
		m_start += size;
		return true;
	}
}