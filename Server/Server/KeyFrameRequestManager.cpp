
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
//#define MS_CLASS "KeyFrameRequestManager"
// #define MS_LOG_DEV_LEVEL 3

#include "KeyFrameRequestManager.hpp"
//#include "Logger.hpp"
#include "clog.h"
static constexpr uint32_t KeyFrameRetransmissionWaitTime{ 1000 };

/* PendingKeyFrameInfo methods. */

RTC::PendingKeyFrameInfo::PendingKeyFrameInfo(PendingKeyFrameInfo::Listener* listener, uint32_t ssrc)
  : listener(listener)
  , ssrc(ssrc)
{
	//MS_TRACE();

	this->timer = new chen::ctimer(this);
	//chen::ctimer::init();
	this->timer->Start(KeyFrameRetransmissionWaitTime);
}

RTC::PendingKeyFrameInfo::~PendingKeyFrameInfo()
{
	//MS_TRACE();

	 this->timer-> Stop();
	 delete this->timer;
}

inline void RTC::PendingKeyFrameInfo::OnTimer(chen::ctimer * timer)
{
//	MS_TRACE();

	 if (timer ==  this->timer)
		this->listener->OnKeyFrameRequestTimeout(this);
}

/* KeyFrameRequestDelayer methods. */

RTC::KeyFrameRequestDelayer::KeyFrameRequestDelayer(
  KeyFrameRequestDelayer::Listener* listener, uint32_t ssrc, uint32_t delay)
  :  listener(listener)
  , ssrc(ssrc)
{
	//MS_TRACE();
	//chen::ctimer::init();
	this->timer = new chen::ctimer(this);
	this->timer->Start(delay);
}

RTC::KeyFrameRequestDelayer::~KeyFrameRequestDelayer()
{
//	MS_TRACE();

	 this->timer-> Stop();
	 delete this->timer;
}

inline void RTC::KeyFrameRequestDelayer::OnTimer(chen::ctimer * timer)
{
	//MS_TRACE();

	 if (timer == this->timer )
		this->listener->OnKeyFrameDelayTimeout(this);
}

/* KeyFrameRequestManager methods. */

RTC::KeyFrameRequestManager::KeyFrameRequestManager(
  KeyFrameRequestManager::Listener* listener, uint32_t keyFrameRequestDelay)
  : listener(listener), keyFrameRequestDelay(keyFrameRequestDelay)
{
	//MS_TRACE();
}

RTC::KeyFrameRequestManager::~KeyFrameRequestManager()
{
	//MS_TRACE();

	for (auto& kv : this->mapSsrcPendingKeyFrameInfo)
	{
		auto* pendingKeyFrameInfo = kv.second;

		delete pendingKeyFrameInfo;
	}
	this->mapSsrcPendingKeyFrameInfo.clear();

	for (auto& kv : this->mapSsrcKeyFrameRequestDelayer)
	{
		auto* keyFrameRequestDelayer = kv.second;

		delete keyFrameRequestDelayer;
	}
	this->mapSsrcKeyFrameRequestDelayer.clear();
}
using namespace chen;
void RTC::KeyFrameRequestManager::KeyFrameNeeded(uint32_t ssrc)
{
	//MS_TRACE();

	if (this->keyFrameRequestDelay > 0u)
	{
		auto it = this->mapSsrcKeyFrameRequestDelayer.find(ssrc);
		
		// There is a delayer for the given ssrc, so enable it and return.
		if (it != this->mapSsrcKeyFrameRequestDelayer.end())
		{
			
			DEBUG_EX_LOG("there is a delayer for the given ssrc, enabling it and returning");

			auto* keyFrameRequestDelayer = it->second;

			keyFrameRequestDelayer->SetKeyFrameRequested(true);

			return;
		}
		// Otherwise create a delayer (not yet enabled) and continue.
		else
		{
			DEBUG_EX_LOG("creating a delayer for the given ssrc");

			this->mapSsrcKeyFrameRequestDelayer[ssrc] =
			  new KeyFrameRequestDelayer(this, ssrc, this->keyFrameRequestDelay);
		}
	}

	auto it = this->mapSsrcPendingKeyFrameInfo.find(ssrc);

	// There is a pending key frame for the given ssrc.
	if (it != this->mapSsrcPendingKeyFrameInfo.end())
	{
		auto* pendingKeyFrameInfo = it->second;

		// Re-request the key frame if not received on time.
		pendingKeyFrameInfo->SetRetryOnTimeout(true);

		return;
	}

	this->mapSsrcPendingKeyFrameInfo[ssrc] = new PendingKeyFrameInfo(this, ssrc);

	this->listener->OnKeyFrameNeeded(this, ssrc);
}

void RTC::KeyFrameRequestManager::ForceKeyFrameNeeded(uint32_t ssrc)
{
	//MS_TRACE();

	if (this->keyFrameRequestDelay > 0u)
	{
		// Create/replace a delayer for this ssrc.
		auto it = this->mapSsrcKeyFrameRequestDelayer.find(ssrc);

		// There is a delayer for the given ssrc, so enable it and return.
		if (it != this->mapSsrcKeyFrameRequestDelayer.end())
		{
			auto* keyFrameRequestDelayer = it->second;

			delete keyFrameRequestDelayer;
		}

		this->mapSsrcKeyFrameRequestDelayer[ssrc] =
		  new KeyFrameRequestDelayer(this, ssrc, this->keyFrameRequestDelay);
	}

	auto it = this->mapSsrcPendingKeyFrameInfo.find(ssrc);

	// There is a pending key frame for the given ssrc.
	if (it != this->mapSsrcPendingKeyFrameInfo.end())
	{
		auto* pendingKeyFrameInfo = it->second;

		pendingKeyFrameInfo->SetRetryOnTimeout(true);
		pendingKeyFrameInfo->Restart();
	}
	else
	{
		this->mapSsrcPendingKeyFrameInfo[ssrc] = new PendingKeyFrameInfo(this, ssrc);
	}

	this->listener->OnKeyFrameNeeded(this, ssrc);
}

void RTC::KeyFrameRequestManager::KeyFrameReceived(uint32_t ssrc)
{
	//MS_TRACE();

	auto it = this->mapSsrcPendingKeyFrameInfo.find(ssrc);

	// There is no pending key frame for the given ssrc.
	if (it == this->mapSsrcPendingKeyFrameInfo.end())
		return;

	auto* pendingKeyFrameInfo = it->second;

	delete pendingKeyFrameInfo;

	this->mapSsrcPendingKeyFrameInfo.erase(it);
}

inline void RTC::KeyFrameRequestManager::OnKeyFrameRequestTimeout(PendingKeyFrameInfo* pendingKeyFrameInfo)
{
	//MS_TRACE();

	auto it = this->mapSsrcPendingKeyFrameInfo.find(pendingKeyFrameInfo->GetSsrc());

	//MS_ASSERT(
	//  it != this->mapSsrcPendingKeyFrameInfo.end(), "PendingKeyFrameInfo not present in the map");

	if (!pendingKeyFrameInfo->GetRetryOnTimeout())
	{
		delete pendingKeyFrameInfo;

		this->mapSsrcPendingKeyFrameInfo.erase(it);

		return;
	}

	// Best effort in case the PLI/FIR was lost. Do not retry on timeout.
	pendingKeyFrameInfo->SetRetryOnTimeout(false);
	pendingKeyFrameInfo->Restart();

	DEBUG_EX_LOG("requesting key frame on timeout");

	this->listener->OnKeyFrameNeeded(this, pendingKeyFrameInfo->GetSsrc());
}

inline void RTC::KeyFrameRequestManager::OnKeyFrameDelayTimeout(
  KeyFrameRequestDelayer* keyFrameRequestDelayer)
{
	//MS_TRACE();

	auto it = this->mapSsrcKeyFrameRequestDelayer.find(keyFrameRequestDelayer->GetSsrc());

//	MS_ASSERT(
//	  it != this->mapSsrcKeyFrameRequestDelayer.end(), "KeyFrameRequestDelayer not present in the map");

	auto ssrc              = keyFrameRequestDelayer->GetSsrc();
	auto keyFrameRequested = keyFrameRequestDelayer->GetKeyFrameRequested();

	delete keyFrameRequestDelayer;

	this->mapSsrcKeyFrameRequestDelayer.erase(it);

	// Ask for a new key frame as normal if needed.
	if (keyFrameRequested)
	{
		DEBUG_EX_LOG("requesting key frame after delay timeout");

		KeyFrameNeeded(ssrc);
	}
}
