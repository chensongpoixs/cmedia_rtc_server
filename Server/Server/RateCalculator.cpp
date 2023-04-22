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
		// æ¯”ç‰¹æµ / æ€»çš„æ»‘åŠ¨çª—å£æ¯«ç§’æ•°
		float scale = this->scale / this->windowSizeMs;

		this->lastTime = nowMs;
		// å…·ä½“å°±æ˜¯æ»‘åŠ¨çª—å£æ¯”ç‰¹æµçš„å…¬å¼ =  æ€»æ»‘åŠ¨çª—å£ä¸­æœ‰çš„æ•°æ®çš„æ€»å’Œ * 8000 / windowsizems  ;
		// åŽŸæ¥æˆ‘ä»¥ä¸ºæœ‰ä»€ä¹ˆç‰¹æ®Šå…¬å¼å‘¢ï¼ï¼ï¼
		this->lastRate = static_cast<uint32_t>(std::trunc(this->totalCount * scale + 0.5f));
		//trunc()å‡½æ•°æ˜¯cmathæ ‡å¤´çš„åº“å‡½æ•°ï¼Œç”¨äºŽå°†å€¼å››èˆäº”å…¥(æˆªæ–­)ä¸ºé›¶ï¼Œå®ƒæŽ¥å—ä¸€ä¸ªæ•°å­—å¹¶è¿”å›žå…¶å¤§å°ä¸å¤§äºŽç»™å®šæ•°å­—çš„æœ€è¿‘æ•´æ•°å€¼ã€‚
		return this->lastRate;
	}

	inline void RateCalculator::RemoveOldData(uint64_t nowMs)
	{
		 
		// No item set.
		if (this->newestItemIndex < 0 || this->oldestItemIndex < 0)
		{
			return;
		}
		// 1. è¿™ä¸ªå€¼æ˜¯æ ¹æ® å½“å‰æ¯«ç§’æ•°å‡åŽ»å…¨éƒ¨çš„æ»‘åŠ¨çª—å£æ—¶é—´æ¥è®¡ç®—çš„
		uint64_t newoldestTime = nowMs - this->windowSizeMs;

		// 2. åˆ¤æ–­æ˜¯å¦å°äºŽæ»‘åŠ¨çª—å£ä¸­æœ€å°çš„æ—¶é—´æ¯«ç§’æ•° ï¼Œ å¦‚æžœå°äºŽå°±ä¸éœ€è¦å¤„ç†å“ˆï¼ˆæ»‘åŠ¨çª—å£ä¸­æ²¡æœ‰è®¡ç®—è¯¥å€¼å“ˆï¼‰
		// Oldest item already removed.
		if (newoldestTime <= this->oldestItemStartTime)
		{
			return;
		}
		// è¯´æ˜Žä¸Šæ¬¡å‘é€æ•°æ®æ—¶é—´é•¿å¤ªé•¿ï¼Œ è¶…å‡ºæ»‘åŠ¨çª—å£çš„æ—¶é—´é•¿ éœ€è¦é‡ç½®æ»‘åŠ¨çª—å£
		// A whole window size time has elapsed since last entry. Reset the buffer.
		if (newoldestTime > this->newestItemStartTime)
		{
			Reset();

			return;
		}
		// ç§»é™¤è¶…å‡ºæ»‘åŠ¨çª—å£æ—¶é—´çš„æ•°æ® count
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
			// æ€»æ˜¯è®°å½•æ»‘åŠ¨çª—å£ä¸­æœ€å°æ¯«ç§’æ•°
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
