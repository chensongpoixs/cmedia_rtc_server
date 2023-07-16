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


#ifndef _C_BUFFER_READER_H_
#define _C_BUFFER_READER_H_
#include "cnet_type.h"
#include <vector>


namespace chen {

	uint32_t ReadUint32BE(char* data);
	uint32_t ReadUint32LE(char* data);
	uint32_t ReadUint24BE(char* data);
	uint32_t ReadUint24LE(char* data);
	uint16_t ReadUint16BE(char* data);
	uint16_t ReadUint16LE(char* data);

	class cbuffer_reader
	{
	public:
		cbuffer_reader(uint32_t initial_size = 2048);
		virtual ~cbuffer_reader();

		uint32_t ReadableBytes() const
		{
			return (uint32_t)(writer_index_ - reader_index_);
		}

		uint32_t WritableBytes() const
		{
			return (uint32_t)(buffer_.size() - writer_index_);
		}

		char* Peek()
		{
			return Begin() + reader_index_;
		}

		const char* Peek() const
		{
			return Begin() + reader_index_;
		}

		const char* FindFirstCrlf() const {
			const char* crlf = std::search(Peek(), BeginWrite(), kCRLF, kCRLF + 2);
			return crlf == BeginWrite() ? nullptr : crlf;
		}

		const char* FindLastCrlf() const {
			const char* crlf = std::find_end(Peek(), BeginWrite(), kCRLF, kCRLF + 2);
			return crlf == BeginWrite() ? nullptr : crlf;
		}

		const char* FindLastCrlfCrlf() const {
			char crlfCrlf[] = "\r\n\r\n";
			const char* crlf = std::find_end(Peek(), BeginWrite(), crlfCrlf, crlfCrlf + 4);
			return crlf == BeginWrite() ? nullptr : crlf;
		}

		void RetrieveAll() {
			writer_index_ = 0;
			reader_index_ = 0;
		}

		void Retrieve(size_t len) {
			if (len <= ReadableBytes()) {
				reader_index_ += len;
				if (reader_index_ == writer_index_) {
					reader_index_ = 0;
					writer_index_ = 0;
				}
			}
			else {
				RetrieveAll();
			}
		}

		void RetrieveUntil(const char* end)
		{
			Retrieve(end - Peek());
		}

		int Read(uint8 *data,  size_t len);
		uint32_t ReadAll(std::string& data);
		uint32_t ReadUntilCrlf(std::string& data);

		uint32_t Size() const
		{
			return (uint32_t)buffer_.size();
		}

	private:
		char* Begin()
		{
			return &*buffer_.begin();
		}

		const char* Begin() const
		{
			return &*buffer_.begin();
		}

		char* beginWrite()
		{
			return Begin() + writer_index_;
		}

		const char* BeginWrite() const
		{
			return Begin() + writer_index_;
		}

		std::vector<char> buffer_;
		size_t reader_index_ = 0;
		size_t writer_index_ = 0;

		static const char kCRLF[];
		static const uint32_t MAX_BYTES_PER_READ = 4096;
		static const uint32_t MAX_BUFFER_SIZE = 1024 * 100000;
	protected:
	private:
	};
}

#endif //_C_BUFFER_READER_H_