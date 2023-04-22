/********************************************************************
created:	2019-03-24

author:		chensong

purpose:	×Ö·û´®´¦Àí¹¤¾ßÀà
//__pragma(warning(suppress:6334))(sizeof ((_Val) + (float)0) == sizeof (float) ? 'f' : sizeof ((_Val) + (double)0) == sizeof (double) ? 'd' : 'l')
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
*********************************************************************/
#include "cdigit2str.h"
#include <algorithm>
#include <cstdio>
#include "crtc_sdp_util.h"
#ifdef _MSC_VER
#pragma warning (disable:4996)
#endif
namespace chen
{
	template<>
	int32 digit2str_dec<int8>(char* buf, int32 buf_size, int8 value)
	{
		int8 i = value;
		char* p = buf;
		int num = 0;
		do {
			i /= 10;
			++num;
		} while (i != 0);
		if (num > buf_size)
		{
			return 0;
		}
		i = value;
		do
		{
			*p++ = '0' + (i % 10);
			i /= 10;
		} while (i != 0);

		if (value < 0)
		{
			*p++ = '-';
		}
		*p = '\0';
		std::reverse(buf, p);

		return static_cast<int>(p - buf);
	}
	template<>
	int32 digit2str_dec<uint8>(char* buf, int32 buf_size, uint8 value)
	{
		uint8 i = value;
		char* p = buf;
		int num = 0;
		do {
			i /= 10;
			++num;
		} while (i != 0);
		if (num > buf_size)
		{
			return 0;
		}
		i = value;
		do
		{
			*p++ = '0' + (i % 10);
			i /= 10;
		} while (i != 0);

		/*if (value < 0)
		{
		*p++ = '-';
		}*/
		*p = '\0';
		std::reverse(buf, p);

		return static_cast<int>(p - buf);
	}
	template<>
	int32 digit2str_dec<int16>(char* buf, int32 buf_size, int16 value)
	{
		int16 i = value;
		char* p = buf;
		int num = 0;
		do {
			i /= 10;
			++num;
		} while (i != 0);
		if (num > buf_size)
		{
			return 0;
		}
		i = value;
		do
		{
			*p++ = '0' + (i % 10);
			i /= 10;
		} while (i != 0);

		if (value < 0)
		{
			*p++ = '-';
		}
		*p = '\0';
		std::reverse(buf, p);

		return static_cast<int>(p - buf);
	}
	template<>
	int32 digit2str_dec<uint16>(char* buf, int32 buf_size, uint16 value)
	{
		uint16 i = value;
		char* p = buf;
		int num = 0;
		do {
			i /= 10;
			++num;
		} while (i != 0);
		if (num > buf_size)
		{
			return 0;
		}
		i = value;
		do
		{
			*p++ = '0' + (i % 10);
			i /= 10;
		} while (i != 0);

		/*if (value < 0)
		{
		*p++ = '-';
		}*/
		*p = '\0';
		std::reverse(buf, p);

		return static_cast<int>(p - buf);
	}
	template<>
	int32 digit2str_dec<int32>(char* buf, int32 buf_size, int32 value)
	{
		int32 i = value;
		char* p = buf;
		int num = 0;
		do {
			i /= 10;
			++num;
		} while (i != 0);
		if (num > buf_size)
		{
			return 0;
		}
		i = value;
		do
		{
			*p++ = '0' + (i % 10);
			i /= 10;
		} while (i != 0);

		if (value < 0)
		{
			*p++ = '-';
		}
		*p = '\0';
		std::reverse(buf, p);

		return static_cast<int>(p - buf);
	}

	template<>
	int32 digit2str_dec<uint32>(char* buf, int32 buf_size, uint32 value)
	{
		uint32 i = value;
		char* p = buf;
		int num = 0;
		do {
			i /= 10;
			++num;
		} while (i != 0);
		if (num > buf_size)
		{
			return 0;
		}
		i = value;
		do
		{
			*p++ = '0' + (i % 10);
			i /= 10;
		} while (i != 0);

		/*if (value < 0)
		{
		*p++ = '-';
		}*/
		*p = '\0';
		std::reverse(buf, p);

		return static_cast<int>(p - buf);
	}

	template<>
	int32 digit2str_dec<signed long >(char* buf, int32 buf_size, signed long value)
	{
		signed long i = value;
		char* p = buf;
		int num = 0;
		do {
			i /= 10;
			++num;
		} while (i != 0);
		if (num > buf_size)
		{
			return 0;
		}
		i = value;
		do
		{
			*p++ = '0' + (i % 10);
			i /= 10;
		} while (i != 0);

		if (value < 0)
		{
			*p++ = '-';
		}
		*p = '\0';
		std::reverse(buf, p);

		return static_cast<int>(p - buf);
	}
	template<>
	int32 digit2str_dec<unsigned long>(char* buf, int32 buf_size, unsigned long value)
	{
		unsigned long i = value;
		char* p = buf;
		int num = 0;
		do {
			i /= 10;
			++num;
		} while (i != 0);
		if (num > buf_size)
		{
			return 0;
		}
		i = value;
		do
		{
			*p++ = '0' + (i % 10);
			i /= 10;
		} while (i != 0);

		/*if (value < 0)
		{
		*p++ = '-';
		}*/
		*p = '\0';
		std::reverse(buf, p);

		return static_cast<int>(p - buf);
	}
	template<>
	int32 digit2str_dec<signed long  long>(char* buf, int32 buf_size, signed long long value)
	{
		signed long long i = value;
		char* p = buf;
		int num = 0;
		do {
			i /= 10;
			++num;
		} while (i != 0);
		if (num > buf_size)
		{
			return 0;
		}
		i = value;
		do
		{
			*p++ = '0' + (i % 10);
			i /= 10;
		} while (i != 0);

		if (value < 0)
		{
			*p++ = '-';
		}
		*p = '\0';
		std::reverse(buf, p);

		return static_cast<int>(p - buf);
	}
	template<>
	int32 digit2str_dec<unsigned long long>(char* buf, int32 buf_size, unsigned long long value)
	{
		unsigned long long i = value;
		char* p = buf;
		int num = 0;
		do {
			i /= 10;
			++num;
		} while (i != 0);
		if (num > buf_size)
		{
			return 0;
		}
		i = value;
		do
		{
			*p++ = '0' + (i % 10);
			i /= 10;
		} while (i != 0);

		/*if (value < 0)
		{
		*p++ = '-';
		}*/
		*p = '\0';
		std::reverse(buf, p);

		return static_cast<int>(p - buf);
	}
	template<>
	int digit2str_dec<float>(char* buf, int buf_size, float value)
	{
		if (6 > buf_size)
		{
			return 0;
		}

		return sprintf(buf, "%.2g", value);
	}
	/**
	* ¿ÆÑ§¼ÆÊý·¨
	* stata ÖÐ Êý¾ÝÊÇ 1.333e+09 ÕâÊÇÊ²Ã´ÒâË¼
	* ÄúµÄÊý¾ÝÌ«´óÁË£¬ÓÃ¿ÆÑ§¼ÆÊý·¨±íÊ¾ÁË
	* Õý½â  ´ú±íÃÝ´Î·½£¬Èç¹ûÊÇ1.333e-09£¬¾ÍËµÃ÷Ç÷½üÓÚ0ÁË £¬1.33*10µÄ¸º9´Î·½
	* ÊÇ1330000000£¬Ò²¾ÍÊÇ 1.33 * (10^9)
	*/
	template<>
	int digit2str_dec<double>(char* buf, int buf_size, double value)
	{
		if (6 > buf_size)
		{
			return 0;
		}
		return sprintf(buf, "%.2g", value);
	}
	std::string ToHex(const int i) 
	{
		char buffer[50];
		snprintf(buffer, sizeof(buffer), "%x", i);

		return std::string(buffer);
	}

	std::string digit2str_dec(int64 value)
	{
		return rtc_sdp_util::fmt("%" PRId64, value);;
	}

} // namespace chen