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


#ifndef _C_BUFFER_H_
#define _C_BUFFER_H_
#include "cnet_type.h"

namespace chen {

	class cbuffer
	{
	public:
		explicit cbuffer(char * p, int32 size)
		: m_position_ptr(p)
		, m_bytes_ptr(p)
		, m_nb_bytes(size){}
		 ~cbuffer();
	public:
		 
		// Get the data and head of buffer.
		//      current-bytes = head() = data() + pos()
		char* data();
		char* head();
		// Get the total size of buffer.
		//      left-bytes = size() - pos()
		int32 size();
		void set_size(int32 v);
		// Get the current buffer position.
		int32 pos();
		// Left bytes in buffer, total size() minus the current pos().
		int32 left();
		// Whether buffer is empty.
		bool empty();
		// Whether buffer is able to supply required size of bytes.
		// @remark User should check buffer by require then do read/write.
		// @remark Assert the required_size is not negative.
		bool require(int32 required_size);
	public:
		// Skip some size.
		// @param size can be any value. positive to forward; negative to backward.
		// @remark to skip(pos()) to reset buffer.
		// @remark assert initialized, the data() not NULL.
		void skip(int32 size);
	public:
		// Read 1bytes char from buffer.
		int8 read_1bytes();
		// Read 2bytes int from buffer.
		int16  read_2bytes();
		int16  read_le2bytes();
		// Read 3bytes int from buffer.
		int32  read_3bytes();
		int32  read_le3bytes();
		// Read 4bytes int from buffer.
		int32  read_4bytes();
		int32  read_le4bytes();
		// Read 8bytes int from buffer.
		int64  read_8bytes();
		int64  read_le8bytes();
		// Read string from buffer, length specifies by param len.
		std::string read_string(int32 len);
		// Read bytes from buffer, length specifies by param len.
		void read_bytes(char* data, int32 size);
	public:
		// Write 1bytes char to buffer.
		void write_1bytes(int8  value);
		// Write 2bytes int to buffer.
		void write_2bytes(int16  value);
		void write_le2bytes(int16  value);
		// Write 4bytes int to buffer.
		void write_4bytes(int32  value);
		void write_le4bytes(int32  value);
		// Write 3bytes int to buffer.
		void write_3bytes(int32  value);
		void write_le3bytes(int32  value);
		// Write 8bytes int to buffer.
		void write_8bytes(int64  value);
		void write_le8bytes(int64  value);
		// Write string to buffer
		void write_string(std::string value);
		// Write bytes to buffer
		void write_bytes(char* data, int32 size);
	protected:
	private: 
		// current position at bytes.
		char*							m_position_ptr;
		// the bytes data for buffer to read or write.
		char*							m_bytes_ptr;
		// the total number of bytes.
		int32							m_nb_bytes;

	};

}
#endif // _C_BUFFER_H_