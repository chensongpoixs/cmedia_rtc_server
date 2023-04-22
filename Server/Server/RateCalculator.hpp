/*
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
*/

#ifndef MS_RTC_RATE_CALCULATOR_HPP
#define MS_RTC_RATE_CALCULATOR_HPP

//#include "common.hpp"
//#include "DepLibUV.hpp"
#include "RtpPacket.hpp"
#include "cuv_util.h"
namespace RTC
{
	// It is considered that the time source increases monotonically.
	// ie: the current timestamp can never be minor than a timestamp in the past.
	class RateCalculator
	{
	public:
		static constexpr size_t DefaultWindowSize{ 1000u };   // 默认滑动窗口占有毫秒数
		static constexpr float DefaultBpsScale{ 8000.0f };    // 比特流
		static constexpr uint16_t DefaultWindowItems{ 100u }; // 滑动窗口有多少个

	public:
		RateCalculator(
		  size_t windowSizeMs  = DefaultWindowSize,
		  float scale          = DefaultBpsScale,
		  uint16_t windowItems = DefaultWindowItems)
		  : windowSizeMs(windowSizeMs), scale(scale), windowItems(windowItems)
		{
			this->itemSizeMs = std::max(windowSizeMs / windowItems, static_cast<size_t>(1));

			Reset();
		}
		void Update(size_t size, uint64_t nowMs);
		uint32_t GetRate(uint64_t nowMs);
		size_t GetBytes() const
		{
			return this->bytes;
		}

	private:
		void RemoveOldData(uint64_t nowMs);
		void Reset()
		{
			this->buffer.reset(new BufferItem[this->windowItems]);
			this->newestItemStartTime = 0u;
			this->newestItemIndex     = -1;
			this->oldestItemStartTime = 0u;
			this->oldestItemIndex     = -1;
			this->totalCount          = 0u;
			this->lastRate            = 0u;
			this->lastTime            = 0u;
		}

	private:
		struct BufferItem
		{
			size_t count{ 0u };
			uint64_t time{ 0u };
		};

	private:
		// Window Size (in milliseconds).
		size_t windowSizeMs{ DefaultWindowSize }; // 滑动窗口的大小毫秒数
		// Scale in which the rate is represented.
		float scale{ DefaultBpsScale }; // 表示比率的刻度。
		// Window Size (number of items).
		uint16_t windowItems{ DefaultWindowItems }; // 滑动窗口的数组个数
		// Item Size (in milliseconds), calculated as: windowSizeMs / windowItems.
		size_t itemSizeMs{ 0u }; // 每个滑动窗口占有毫秒数
		// Buffer to keep data.
		std::unique_ptr<BufferItem[]> buffer; // 滑动窗口的数组记录数据集
		// Time (in milliseconds) for last item in the time window.
		uint64_t newestItemStartTime{ 0u }; // 当前最新窗口的时间毫秒数
		// Index for the last item in the time window.
		int32_t newestItemIndex{ -1 }; // 最新的窗口的下标
		// Time (in milliseconds) for oldest item in the time window.
		uint64_t oldestItemStartTime{ 0u }; // 总是记录滑动窗口中最小毫秒数
		// Index for the oldest item in the time window.
		int32_t oldestItemIndex{ -1 }; // 最老的下标
		// Total count in the time window.
		size_t totalCount{ 0u }; // 数据同和当前
		// Total bytes transmitted.
		size_t bytes{ 0u };
		// Last value calculated by GetRate().
		uint32_t lastRate{ 0u };
		// Last time GetRate() was called.
		uint64_t lastTime{ 0u };
	};

	class RtpDataCounter
	{
	public:
		explicit RtpDataCounter(size_t windowSizeMs = 2500) : rate(windowSizeMs)
		{
		}

	public:
		void Update(RTC::RtpPacket* packet);
		uint32_t GetBitrate(uint64_t nowMs)
		{
			return this->rate.GetRate(nowMs);
		}
		size_t GetPacketCount() const
		{
			return this->packets;
		}
		size_t GetBytes() const
		{
			return this->rate.GetBytes();
		}

	private:
		RateCalculator rate;
		size_t packets{ 0u };
	};
} // namespace RTC

#endif
