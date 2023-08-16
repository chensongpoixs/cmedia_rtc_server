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
//#define MS_CLASS "RTC::NackGenerator"
// #define MS_LOG_DEV_LEVEL 3

#include "NackGenerator.hpp"
//#include "DepLibUV.hpp"
//#include "Logger.hpp"
#include <iterator> // std::ostream_iterator
#include <sstream>  // std::ostringstream
#include <utility>  // std::make_pair()
#include "cuv_util.h"
#include <cassert>
namespace RTC
{
	/* Static. */
	using namespace chen;
	constexpr size_t MaxPacketAge{ 10000u };
	constexpr size_t MaxNackPackets{ 1000u };
	constexpr uint32_t DefaultRtt{ 100u };
	constexpr uint8_t MaxNackRetries{ 10u };
	constexpr uint64_t TimerInterval{ 40u };

	/* Instance methods. */

	NackGenerator::NackGenerator(Listener* listener) 
		:   listener(listener)
		, rtt(DefaultRtt)
	{
		//MS_TRACE();

		// Set the timer.
		 this->timer = new chen::ctimer(this);
		 
	}

	NackGenerator::~NackGenerator()
	{
		//MS_TRACE();

		// Close the timer.
		if (this->timer)
		{
			//this->timer->destroy();
			delete this->timer;
			timer = NULL;
		}
	}

	void NackGenerator::destroy()
	{
		// Close the timer.
		if (this->timer)
		{
			//this->timer->destroy();
			delete this->timer;
			timer = NULL;
		}
	}

	// Returns true if this is a found nacked packet. False otherwise.
	bool NackGenerator::ReceivePacket(RTC::RtpPacket* packet, bool isRecovered)
	{
		//MS_TRACE();

		uint16_t seq    = packet->GetSequenceNumber();
		bool isKeyFrame = packet->IsKeyFrame();

		if (!this->started)
		{
			this->started = true;
			this->lastSeq = seq;

			if (isKeyFrame)
			{
				this->keyFrameList.insert(seq);
			}

			return false;
		}

		// Obviously never nacked, so ignore.
		if (seq == this->lastSeq)
		{
			return false;
		}

		// May be an out of order packet, or already handled retransmitted packet,
		// or a retransmitted packet.
		if (SeqManager<uint16_t>::IsSeqLowerThan(seq, this->lastSeq))
		{
			auto it = this->nackList.find(seq);

			// It was a nacked packet.
			if (it != this->nackList.end())
			{
				DEBUG_EX_LOG(
				  "NACKed packet received [ssrc:%u, seq:%hu, recovered:%s]",
				  packet->GetSsrc(),
				  packet->GetSequenceNumber(),
				  isRecovered ? "true" : "false");

				this->nackList.erase(it);

				return true;
			}

			// Out of order packet or already handled NACKed packet.
			if (!isRecovered)
			{
				WARNING_EX_LOG(
				  "ignoring older packet not present in the NACK list [ssrc:%u, seq:%hu]",
				  packet->GetSsrc(),
				  packet->GetSequenceNumber());
			}

			return false;
		}

		// If we are here it means that we may have lost some packets so seq is
		// newer than the latest seq seen.

		if (isKeyFrame)
		{
			this->keyFrameList.insert(seq);
		}

		// Remove old keyframes.
		{
			auto it = this->keyFrameList.lower_bound(seq - MaxPacketAge);

			if (it != this->keyFrameList.begin())
			{
				this->keyFrameList.erase(this->keyFrameList.begin(), it);
			}
		}

		if (isRecovered)
		{
			this->recoveredList.insert(seq);

			// Remove old ones so we don't accumulate recovered packets.
			auto it = this->recoveredList.lower_bound(seq - MaxPacketAge);

			if (it != this->recoveredList.begin())
			{
				this->recoveredList.erase(this->recoveredList.begin(), it);
			}

			// Do not let a packet pass if it's newer than last seen seq and came via
			// RTX.
			return false;
		}

		AddPacketsToNackList(this->lastSeq + 1, seq);

		this->lastSeq = seq;

		// Check if there are any nacks that are waiting for this seq number.
		std::vector<uint16_t> nackBatch = GetNackBatch(NackFilter::SEQ);

		if (!nackBatch.empty())
		{
			this->listener->OnNackGeneratorNackRequired(nackBatch);
		}

		// This is important. Otherwise the running timer (filter:TIME) would be
		// interrupted and NACKs would never been sent more than once for each seq.
		if (! this->timer-> IsActive())
		{
			MayRunTimer();
		}

		return false;
	}

	void NackGenerator::AddPacketsToNackList(uint16_t seqStart, uint16_t seqEnd)
	{
		//MS_TRACE();
		if (seqStart != seqEnd)
		{
			WARNING_EX_LOG("[seqStart =%u][seqEnd = %u]", seqStart, seqEnd);
		}
		// Remove old packets.
		auto it = this->nackList.lower_bound(seqEnd - MaxPacketAge);

		this->nackList.erase(this->nackList.begin(), it);

		// If the nack list is too large, remove packets from the nack list until
		// the latest first packet of a keyframe. If the list is still too large,
		// clear it and request a keyframe.
		uint16_t numNewNacks = seqEnd - seqStart;

		if (this->nackList.size() + numNewNacks > MaxNackPackets)
		{
			// clang-format off
			while (RemoveNackItemsUntilKeyFrame() && this->nackList.size() + numNewNacks > MaxNackPackets )
			// clang-format on
			{
			}

			//TODO@chensong 2022-11-03 丢包太多了[1000] 就全部删除了， 重新请求关键帧
			if (this->nackList.size() + numNewNacks > MaxNackPackets)
			{
				WARNING_EX_LOG("rtx, NACK list full, clearing it and requesting a key frame [seqEnd:%hu]", seqEnd);

				this->nackList.clear();
				this->listener->OnNackGeneratorKeyFrameRequired();

				return;
			}
		}
//<<<<<<< HEAD
		// TODO@chensong 2022-11-03 查看[seqstart, seqend] 中间哪些包是在rtx中已经接受到了， 接受到就不要在nacklist中添加了
//=======


		//TODO@chensong 2022-11-18  统计没有接受的的包的seqnumber
//>>>>>>> e7c53c323244c384996ff48dc36fc15109b527f0
		//uint64_t nowMs = uv_util::GetTimeMs();
		for (uint16_t seq = seqStart; seq != seqEnd; ++seq)
		{
			cassert_desc(this->nackList.find(seq) == this->nackList.end(), "packet already in the NACK list");

			// Do not send NACK for packets that are already recovered by RTX.
			if (this->recoveredList.find(seq) != this->recoveredList.end())
			{
				continue;
			}

			this->nackList.emplace(std::make_pair(seq, NackInfo{ seq, seq /*, nowMs*/ }));
		}
	}

	bool NackGenerator::RemoveNackItemsUntilKeyFrame()
	{
		//MS_TRACE();

		while (!this->keyFrameList.empty())
		{
			auto it = this->nackList.lower_bound(*this->keyFrameList.begin());

			if (it != this->nackList.begin())
			{
				// We have found a keyframe that actually is newer than at least one
				// packet in the nack list.
				this->nackList.erase(this->nackList.begin(), it);

				return true;
			}

			// If this keyframe is so old it does not remove any packets from the list,
			// remove it from the list of keyframes and try the next keyframe.
			this->keyFrameList.erase(this->keyFrameList.begin());
		}

		return false;
	}

	std::vector<uint16_t> NackGenerator::GetNackBatch(NackFilter filter)
	{
		//MS_TRACE();

		uint64_t nowMs = uv_util::GetTimeMs();
		std::vector<uint16_t> nackBatch;

		std::map<uint16_t, NackInfo, RTC::SeqManager<uint16_t>::SeqLowerThan>::iterator it = this->nackList.begin();

		while (it != this->nackList.end())
		{
			NackInfo& nackInfo = it->second;
			uint16_t seq       = nackInfo.seq;

			// clang-format off
			// 完全实时系统
			if ( filter == NackFilter::SEQ && nackInfo.sentAtMs == 0 && (
				nackInfo.sendAtSeq == this->lastSeq || SeqManager<uint16_t>::IsSeqHigherThan(this->lastSeq, nackInfo.sendAtSeq) )
			)
			{
				nackBatch.emplace_back(seq);
				nackInfo.retries++;
				nackInfo.sentAtMs = nowMs;

				// TODO@chensong 2022-11-18   掉包 10 就在nacklist中删除该nack数据是为什么呢 ？？？
				if (nackInfo.retries >= MaxNackRetries)
				{
					WARNING_EX_LOG(" rtx, sequence number removed from the NACK list due to max retries [filter:seq, seq:%hu]",
					  seq);

					it = this->nackList.erase(it);
				}
				else
				{
					++it;
				}

				continue;
			}
			//NORMAL_EX_LOG("rtt = %u", rtt);
			if (filter == NackFilter::TIME && nowMs - nackInfo.sentAtMs >= this->rtt)
			{
				nackBatch.emplace_back(seq);
				nackInfo.retries++;
				nackInfo.sentAtMs = nowMs;

				if (nackInfo.retries >= MaxNackRetries)
				{
					WARNING_EX_LOG("rtx, sequence number removed from the NACK list due to max retries [filter:time, seq:%hu]",
					  seq);

					it = this->nackList.erase(it);
				}
				else
				{
					++it;
				}

				continue;
			}

			++it;
		}

#if MS_LOG_DEV_LEVEL == 3
		if (!nackBatch.empty())
		{
			std::ostringstream seqsStream;
			std::copy(
			  nackBatch.begin(), nackBatch.end() - 1, std::ostream_iterator<uint32_t>(seqsStream, ","));
			seqsStream << nackBatch.back();

			if (filter == NackFilter::SEQ)
				MS_DEBUG_DEV("[filter:SEQ, asking seqs:%s]", seqsStream.str().c_str());
			else
				MS_DEBUG_DEV("[filter:TIME, asking seqs:%s]", seqsStream.str().c_str());
		}
#endif

		return nackBatch;
	}

	void NackGenerator::Reset()
	{
		//MS_TRACE();

		this->nackList.clear();
		this->keyFrameList.clear();
		this->recoveredList.clear();

		this->started = false;
		this->lastSeq = 0u;
	}

	inline void NackGenerator::MayRunTimer() 
	{
		if (!this->nackList.empty())
		{
			 this->timer->  Start(TimerInterval);
		}
	}

	inline void NackGenerator::OnTimer(chen::ctimer * timer)
	{
	//	MS_TRACE();

		std::vector<uint16_t> nackBatch = GetNackBatch(NackFilter::TIME);

		if (!nackBatch.empty())
		{
			this->listener->OnNackGeneratorNackRequired(nackBatch);
		}

		MayRunTimer();
	}
} // namespace RTC
