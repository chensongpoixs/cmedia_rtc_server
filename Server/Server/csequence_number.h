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
#ifndef _C_SEQUENCE_NUMBER_H_
#define _C_SEQUENCE_NUMBER_H_

#include "cnet_type.h"
#include "cnoncopyable.h"
#include "csingleton.h"
#include <stdint.h>
#include <limits>
#include <type_traits>

#include "cmod_ops.h"

namespace chen
{

	// Test if the sequence number |a| is ahead or at sequence number |b|.
	//
	// If |M| is an even number and the two sequence numbers are at max distance
	// from each other, then the sequence number with the highest value is
	// considered to be ahead.
	template <typename T, T M>
	inline typename std::enable_if<(M > 0), bool>::type AheadOrAt(T a, T b) {
		static_assert(std::is_unsigned<T>::value,
			"Type must be an unsigned integer.");
		const T maxDist = M / 2;
		if (!(M & 1) && MinDiff<T, M>(a, b) == maxDist)
			return b < a;
		return ForwardDiff<T, M>(b, a) <= maxDist;
	}

	template <typename T, T M>
	inline typename std::enable_if<(M == 0), bool>::type AheadOrAt(T a, T b)
	{
		static_assert(std::is_unsigned<T>::value,
			"Type must be an unsigned integer.");
		const T maxDist = std::numeric_limits<T>::max() / 2 + T(1);
		if (a - b == maxDist)
		{
			return b < a;
		}
		return ForwardDiff(b, a) < maxDist;
	}

	template <typename T>
	inline bool AheadOrAt(T a, T b) {
		return AheadOrAt<T, 0>(a, b);
	}

	// Test if the sequence number |a| is ahead of sequence number |b|.
	//
	// If |M| is an even number and the two sequence numbers are at max distance
	// from each other, then the sequence number with the highest value is
	// considered to be ahead.
	template <typename T, T M = 0>
	inline bool AheadOf(T a, T b) {
		static_assert(std::is_unsigned<T>::value,
			"Type must be an unsigned integer.");
		return a != b && AheadOrAt<T, M>(a, b);
	}

	// Comparator used to compare sequence numbers in a continuous fashion.
	//
	// WARNING! If used to sort sequence numbers of length M then the interval
	//          covered by the sequence numbers may not be larger than floor(M/2).
	template <typename T, T M = 0>
	struct AscendingSeqNumComp {
		bool operator()(T a, T b) const { return AheadOf<T, M>(a, b); }
	};

	// Comparator used to compare sequence numbers in a continuous fashion.
	//
	// WARNING! If used to sort sequence numbers of length M then the interval
	//          covered by the sequence numbers may not be larger than floor(M/2).
	template <typename T, T M = 0>
	struct DescendingSeqNumComp {
		bool operator()(T a, T b) const { return AheadOf<T, M>(b, a); }
	};

	// A sequence number unwrapper where the first unwrapped value equals the
	// first value being unwrapped.
	template <typename T, T M = 0>
	class SeqNumUnwrapper {
		static_assert(
			std::is_unsigned<T>::value &&
			std::numeric_limits<T>::max() < std::numeric_limits<int64_t>::max(),
			"Type unwrapped must be an unsigned integer smaller than int64_t.");

	public:
		int64_t Unwrap(T value) {
			if (!use/*!last_value_*/) 
			{
				use = true;
				last_unwrapped_ = { value };
			}
			else {
				last_unwrapped_ += ForwardDiff<T, M>(last_value_, value);

				if (!AheadOrAt<T, M>(value, last_value_)) {
					constexpr int64_t kBackwardAdjustment =
						M == 0 ? int64_t{ std::numeric_limits<T>::max() } +1 : M;
					last_unwrapped_ -= kBackwardAdjustment;
				}
			}

			last_value_ = value;
			return last_unwrapped_;
		}

	private:
		int64_t last_unwrapped_ = 0;
		bool	use = false;
		/*absl::optional<T>*/ T last_value_;
	};

}
#endif // _C_SEQUENCE_NUMBER_H_