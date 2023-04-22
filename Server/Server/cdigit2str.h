/********************************************************************
created:	2019-03-24

author:		chensong

purpose:	×Ö·û´®´¦Àí¹¤¾ßÀà
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
#ifndef _C_DIGIT2STR_H_
#define _C_DIGIT2STR_H_
#include <algorithm>
#include <cstdio>
#include "cnet_type.h"
#ifdef _MSC_VER
#pragma warning (disable:4996)
#include <malloc.h>
#include <wchar.h>
#include <windows.h>
#define alloca _alloca

#define STACK_ARRAY(TYPE, LEN) \
  static_cast<TYPE*>(::alloca((LEN) * sizeof(TYPE)))
#endif
namespace chen
{
	//convert int to decimal string, add '\0' at end
	template<typename T>
	int32 digit2str_dec(char* buf, int32 buf_size, T value);
	// TODO(jonasolsson): replace with absl::Hex when that becomes available.
	std::string ToHex(const int i);



	std::string  digit2str_dec(int64 value);

#ifdef _MSC_VER

	inline std::wstring ToUtf16(const char* utf8, size_t len) {
		int len16 = ::MultiByteToWideChar(CP_UTF8, 0, utf8, static_cast<int>(len), nullptr, 0);
		wchar_t* ws = STACK_ARRAY(wchar_t, len16);
		::MultiByteToWideChar(CP_UTF8, 0, utf8, static_cast<int>(len), ws, len16);
		return std::wstring(ws, len16);
	}

	inline std::wstring ToUtf16(const std::string& str) {
		return ToUtf16(str.data(), str.length());
	}

	inline std::string ToUtf8(const wchar_t* wide, size_t len) {
		int len8 = ::WideCharToMultiByte(CP_UTF8, 0, wide, static_cast<int>(len),
			nullptr, 0, nullptr, nullptr);
		char* ns = STACK_ARRAY(char, len8);
		::WideCharToMultiByte(CP_UTF8, 0, wide, static_cast<int>(len), ns, len8,
			nullptr, nullptr);
		return std::string(ns, len8);
	}

	inline std::string ToUtf8(const wchar_t* wide) {
		return ToUtf8(wide, wcslen(wide));
	}

	inline std::string ToUtf8(const std::wstring& wstr) {
		return ToUtf8(wstr.data(), wstr.length());
	}

#endif

}// namespace chen

#endif //_C_DIGIT2STR_H_