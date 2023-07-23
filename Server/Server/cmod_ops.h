/********************************************************************
created:	2014/11/21
author:		chensong

purpose:	随机数管理类 crandom
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
#ifndef _C_MOD_OPS_H_
#define _C_MOD_OPS_H_

#include "cnet_type.h"
#include "cnoncopyable.h"
#include "csingleton.h"
#include <stdint.h>
#include <limits>
#include <type_traits>
#include <algorithm>
#include <type_traits>

 
namespace chen
{

	template <unsigned long M>                                    // NOLINT
	inline unsigned long Add(unsigned long a, unsigned long b) {  // NOLINT
		RTC_DCHECK_LT(a, M);
		unsigned long t = M - b % M;  // NOLINT
		unsigned long res = a - t;    // NOLINT
		if (t > a)
		{
			return res + M;
		}
		return res;
	}

	template <unsigned long M>                                         // NOLINT
	inline unsigned long Subtract(unsigned long a, unsigned long b) {  // NOLINT
		RTC_DCHECK_LT(a, M);
		unsigned long sub = b % M;  // NOLINT
		if (a < sub)
			return M - (sub - a);
		return a - sub;
	}

	// Calculates the forward difference between two wrapping numbers.
	//
	// Example:
	// uint8_t x = 253;
	// uint8_t y = 2;
	//
	// ForwardDiff(x, y) == 5
	//
	//   252   253   254   255    0     1     2     3
	// #################################################
	// |     |  x  |     |     |     |     |  y  |     |
	// #################################################
	//          |----->----->----->----->----->
	//
	// ForwardDiff(y, x) == 251
	//
	//   252   253   254   255    0     1     2     3
	// #################################################
	// |     |  x  |     |     |     |     |  y  |     |
	// #################################################
	// -->----->                              |----->---
	//
	// If M > 0 then wrapping occurs at M, if M == 0 then wrapping occurs at the
	// largest value representable by T.
	template <typename T, T M>
	inline typename std::enable_if<(M > 0), T>::type ForwardDiff(T a, T b) {
		static_assert(std::is_unsigned<T>::value,
			"Type must be an unsigned integer.");
		RTC_DCHECK_LT(a, M);
		RTC_DCHECK_LT(b, M);
		return a <= b ? b - a : M - (a - b);
	}

	template <typename T, T M>
	inline typename std::enable_if<(M == 0), T>::type ForwardDiff(T a, T b) {
		static_assert(std::is_unsigned<T>::value,
			"Type must be an unsigned integer.");
		return b - a;
	}

	template <typename T>
	inline T ForwardDiff(T a, T b) {
		return ForwardDiff<T, 0>(a, b);
	}

	// Calculates the reverse difference between two wrapping numbers.
	//
	// Example:
	// uint8_t x = 253;
	// uint8_t y = 2;
	//
	// ReverseDiff(y, x) == 5
	//
	//   252   253   254   255    0     1     2     3
	// #################################################
	// |     |  x  |     |     |     |     |  y  |     |
	// #################################################
	//          <-----<-----<-----<-----<-----|
	//
	// ReverseDiff(x, y) == 251
	//
	//   252   253   254   255    0     1     2     3
	// #################################################
	// |     |  x  |     |     |     |     |  y  |     |
	// #################################################
	// ---<-----|                             |<-----<--
	//
	// If M > 0 then wrapping occurs at M, if M == 0 then wrapping occurs at the
	// largest value representable by T.
	template <typename T, T M>
	inline typename std::enable_if<(M > 0), T>::type ReverseDiff(T a, T b) {
		static_assert(std::is_unsigned<T>::value,
			"Type must be an unsigned integer.");
		RTC_DCHECK_LT(a, M);
		RTC_DCHECK_LT(b, M);
		return b <= a ? a - b : M - (b - a);
	}

	template <typename T, T M>
	inline typename std::enable_if<(M == 0), T>::type ReverseDiff(T a, T b) {
		static_assert(std::is_unsigned<T>::value,
			"Type must be an unsigned integer.");
		return a - b;
	}

	template <typename T>
	inline T ReverseDiff(T a, T b) {
		return ReverseDiff<T, 0>(a, b);
	}

	// Calculates the minimum distance between to wrapping numbers.
	//
	// The minimum distance is defined as min(ForwardDiff(a, b), ReverseDiff(a, b))
	template <typename T, T M = 0>
	inline T MinDiff(T a, T b) {
		static_assert(std::is_unsigned<T>::value,
			"Type must be an unsigned integer.");
		return std::min(ForwardDiff<T, M>(a, b), ReverseDiff<T, M>(a, b));
	}

}

#endif // _C_MOD_OPS_H_