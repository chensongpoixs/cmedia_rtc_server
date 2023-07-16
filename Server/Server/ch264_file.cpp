/***********************************************************************************************
created: 		2023-07-16

author:			chensong

purpose:		_C_DTLS_ _H_
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
#include "ch264_file.h"
namespace chen {

	ch264_file::ch264_file(int buf_size)
		: m_buf_size(buf_size)
	{
		m_buf = new char[m_buf_size];
	}

	ch264_file::~ch264_file()
	{
		//delete[] m_buf;
	}

	bool ch264_file::Open(const char *path)
	{
		m_file = fopen(path, "rb");
		if (m_file == NULL) {
			return false;
		}

		return true;
	}

	void ch264_file::Close()
	{
		if (m_file) {
			fclose(m_file);
			m_file = NULL;
			m_count = 0;
			m_bytes_used = 0;
		}
	}

	int ch264_file::ReadFrame(char* in_buf, int in_buf_size, bool* end)
	{
		if (m_file == NULL) {
			return -1;
		}

		int bytes_read = (int)fread(m_buf, 1, m_buf_size, m_file);
		if (bytes_read == 0) {
			fseek(m_file, 0, SEEK_SET);
			m_count = 0;
			m_bytes_used = 0;
			bytes_read = (int)fread(m_buf, 1, m_buf_size, m_file);
			if (bytes_read == 0) {
				this->Close();
				return -1;
			}
		}

		bool is_find_start = false, is_find_end = false;
		int i = 0, start_code = 3;
		*end = false;

		for (i = 0; i < bytes_read - 5; i++) {
			if (m_buf[i] == 0 && m_buf[i + 1] == 0 && m_buf[i + 2] == 1) {
				start_code = 3;
			}
			else if (m_buf[i] == 0 && m_buf[i + 1] == 0 && m_buf[i + 2] == 0 && m_buf[i + 3] == 1) {
				start_code = 4;
			}
			else {
				continue;
			}

			if (((m_buf[i + start_code] & 0x1F) == 0x5 || (m_buf[i + start_code] & 0x1F) == 0x1)
				&& ((m_buf[i + start_code + 1] & 0x80) == 0x80)) {
				is_find_start = true;
				i += 4;
				break;
			}
		}

		for (; i < bytes_read - 5; i++) {
			if (m_buf[i] == 0 && m_buf[i + 1] == 0 && m_buf[i + 2] == 1)
			{
				start_code = 3;
			}
			else if (m_buf[i] == 0 && m_buf[i + 1] == 0 && m_buf[i + 2] == 0 && m_buf[i + 3] == 1) {
				start_code = 4;
			}
			else {
				continue;
			}

			if (((m_buf[i + start_code] & 0x1F) == 0x7) || ((m_buf[i + start_code] & 0x1F) == 0x8)
				|| ((m_buf[i + start_code] & 0x1F) == 0x6) || (((m_buf[i + start_code] & 0x1F) == 0x5
					|| (m_buf[i + start_code] & 0x1F) == 0x1) && ((m_buf[i + start_code + 1] & 0x80) == 0x80))) {
				is_find_end = true;
				break;
			}
		}

		bool flag = false;
		if (is_find_start && !is_find_end && m_count > 0) {
			flag = is_find_end = true;
			i = bytes_read;
			*end = true;
		}

		if (!is_find_start || !is_find_end) {
			this->Close();
			return -1;
		}

		int size = (i <= in_buf_size ? i : in_buf_size);
		memcpy(in_buf, m_buf, size);

		if (!flag) {
			m_count += 1;
			m_bytes_used += i;
		}
		else {
			m_count = 0;
			m_bytes_used = 0;
		}

		fseek(m_file, m_bytes_used, SEEK_SET);
		return size;
	}



}