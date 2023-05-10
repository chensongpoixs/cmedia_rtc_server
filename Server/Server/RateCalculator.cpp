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

 
#include "RateCalculator.hpp"
//#include "Logger.hpp"
#include <cmath> // std::trunc()
#include "clog.h"
namespace RTC
{
	void RateCalculator::Update(size_t size, uint64_t nowMs)
	{
	//	MS_TRACE();

		// Ignore too old data. Should never happen.
		if (nowMs < this->oldestItemStartTime)
		{
			return;
		}

		// Increase bytes.
		this->bytes += size;

		RemoveOldData(nowMs);

		// If the elapsed time from the newest item start time is greater than the
		// item size (in milliseconds), increase the item index.
		if (this->newestItemIndex < 0 || nowMs - this->newestItemStartTime >= this->itemSizeMs)
		{
			this->newestItemIndex++;
			this->newestItemStartTime = nowMs;
			if (this->newestItemIndex >= this->windowItems)
			{
				this->newestItemIndex = 0;
			}

			// Newest index overlaps with the oldest one, remove it.
			if (this->newestItemIndex == this->oldestItemIndex && this->oldestItemIndex != -1)
			{
				using namespace chen;
				WARNING_EX_LOG("calculation buffer full, windowSizeMs:%zu ms windowItems:%" PRIu16,
				  this->windowSizeMs,
				  this->windowItems);

				BufferItem& oldestItem = buffer[this->oldestItemIndex];
				this->totalCount -= oldestItem.count;
				oldestItem.count = 0u;
				oldestItem.time  = 0u;
				if (++this->oldestItemIndex >= this->windowItems)
				{
					this->oldestItemIndex = 0;
				}
			}

			// Set the newest item.
			BufferItem& item = buffer[this->newestItemIndex];
			item.count       = size;
			item.time        = nowMs;
		}
		else
		{
			// Update the newest item.
			BufferItem& item = buffer[this->newestItemIndex];
			item.count += size;
		}

		// Set the oldest item index and time, if not set.
		if (this->oldestItemIndex < 0)
		{
			this->oldestItemIndex     = this->newestItemIndex;
			this->oldestItemStartTime = nowMs;
		}

		this->totalCount += size;

		// Reset lastRate and lastTime so GetRate() will calculate rate again even
		// if called with same now in the same loop iteration.
		this->lastRate = 0;
		this->lastTime = 0;
	}

	uint32_t RateCalculator::GetRate(uint64_t nowMs)
	{
		 
		if (nowMs == this->lastTime)
		{
			return this->lastRate;
		}

		RemoveOldData(nowMs);
		// 比特流 / 总的滑动窗口毫秒数
		float scale = this->scale / this->windowSizeMs;

		this->lastTime = nowMs;
		// 具体就是滑动窗口比特流的公式 =  总滑动窗口中有的数据的总和 * 8000 / windowsizems  ;
		// 原来我以为有什么特殊公式呢！！！
		/*
		ceil函数：向上取整
		floor函数：向下取整
		round函数：四舍五入取整
		trunc函数：舍尾取整
		*/
		this->lastRate = static_cast<uint32_t>(std::trunc(this->totalCount * scale + 0.5f));
		//trunc()函数是cmath标头的库函数，用于将值四舍五入(截断)为零，它接受一个数字并返回其大小不大于给定数字的最近整数值
		return this->lastRate;
	}

	inline void RateCalculator::RemoveOldData(uint64_t nowMs)
	{
		 
		// No item set.
		if (this->newestItemIndex < 0 || this->oldestItemIndex < 0)
		{
			return;
		}
		// 1. 这个值是根据 当前毫秒数减去全部的滑动窗口时间来计算的
		uint64_t newoldestTime = nowMs - this->windowSizeMs;

		// 2. 判断是否小于滑动窗口中最小的时间毫秒数 ， 如果小于就不需要处理哈（滑动窗口中没有计算该值哈）
		// Oldest item already removed.
		if (newoldestTime <= this->oldestItemStartTime)
		{
			return;
		}
		// 说明上次发送数据时间长太长， 超出滑动窗口的时间长 需要重置滑动窗口
		// A whole window size time has elapsed since last entry. Reset the buffer.
		if (newoldestTime > this->newestItemStartTime)
		{
			Reset();

			return;
		}
		// 移除超出滑动窗口时间的数据 count
		while (this->oldestItemStartTime < newoldestTime)
		{
			BufferItem& oldestItem = buffer[this->oldestItemIndex];
			this->totalCount -= oldestItem.count;
			oldestItem.count = 0u;
			oldestItem.time  = 0u;

			if (++this->oldestItemIndex >= this->windowItems)
			{
				this->oldestItemIndex = 0;
			}

			const BufferItem& newOldestItem = buffer[this->oldestItemIndex];
			// 总是记录滑动窗口中最小毫秒数
			this->oldestItemStartTime       = newOldestItem.time;
		}
	}

	void RtpDataCounter::Update(RTC::RtpPacket* packet)
	{
		uint64_t nowMs = chen::uv_util::GetTimeMs();

		this->packets++;
		this->rate.Update(packet->GetSize(), nowMs);
	}
} // namespace RTC
