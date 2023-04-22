
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
 
#include "RtpStream.hpp"
//#include "Logger.hpp"
#include "SeqManager.hpp"
#include "RtpDictionaries.hpp"
namespace RTC
{
	/* Static. */

	static constexpr uint16_t MaxDropout{ 3000 };
	static constexpr uint16_t MaxMisorder{ 1500 };
	static constexpr uint32_t RtpSeqMod{ 1 << 16 };
	static constexpr size_t ScoreHistogramLength{ 24 };

	/* Instance methods. */

	RtpStream::RtpStream(
	  RTC::RtpStream::Listener* listener, RTC::RtpStream::Params& params, uint8_t initialScore)
	  : listener(listener), params(params), score(initialScore), activeSinceMs(uv_util::GetTimeMs())
	{
		 
	}

	RtpStream::~RtpStream()
	{
		 

		delete this->rtxStream;
	}

	//void RtpStream::FillJson(json& jsonObject) const
	//{
	//	MS_TRACE();

	//	// Add params.
	//	this->params.FillJson(jsonObject["params"]);

	//	// Add score.
	//	jsonObject["score"] = this->score;

	//	// Add rtxStream.
	//	if (HasRtx())
	//		this->rtxStream->FillJson(jsonObject["rtxStream"]);
	//}

	/*void RtpStream::FillJsonStats(json& jsonObject)
	{
		MS_TRACE();

		uint64_t nowMs = DepLibUV::GetTimeMs();

		jsonObject["timestamp"]            = nowMs;
		jsonObject["ssrc"]                 = this->params.ssrc;
		jsonObject["kind"]                 = RtpCodecMimeType::type2String[this->params.mimeType.type];
		jsonObject["mimeType"]             = this->params.mimeType.ToString();
		jsonObject["packetsLost"]          = this->packetsLost;
		jsonObject["fractionLost"]         = this->fractionLost;
		jsonObject["packetsDiscarded"]     = this->packetsDiscarded;
		jsonObject["packetsRetransmitted"] = this->packetsRetransmitted;
		jsonObject["packetsRepaired"]      = this->packetsRepaired;
		jsonObject["nackCount"]            = this->nackCount;
		jsonObject["nackPacketCount"]      = this->nackPacketCount;
		jsonObject["pliCount"]             = this->pliCount;
		jsonObject["firCount"]             = this->firCount;
		jsonObject["score"]                = this->score;

		if (!this->params.rid.empty())
			jsonObject["rid"] = this->params.rid;

		if (this->params.rtxSsrc)
			jsonObject["rtxSsrc"] = this->params.rtxSsrc;

		if (this->rtxStream)
			jsonObject["rtxPacketsDiscarded"] = this->rtxStream->GetPacketsDiscarded();

		if (this->hasRtt)
			jsonObject["roundTripTime"] = this->rtt;
	}*/

	void RtpStream::SetRtx(uint8_t payloadType, uint32_t ssrc)
	{
		 
		this->params.rtxPayloadType = payloadType;
		this->params.rtxSsrc        = ssrc;

		if (HasRtx())
		{
			delete this->rtxStream;
			this->rtxStream = nullptr;
		}

		// Set RTX stream params.
		RTC::RtxStream::Params params;

		params.ssrc             = ssrc;
		params.payloadType      = payloadType;
		params.mimeType.type    = GetMimeType().type;
		params.mimeType.subtype = RTC::RtpCodecMimeType::Subtype::RTX;
		params.clockRate        = GetClockRate();
		params.rrid             = GetRid();
		params.cname            = GetCname();

		// Tell the RtpCodecMimeType to update its string based on current type and subtype.
		params.mimeType.UpdateMimeType();

		this->rtxStream = new RTC::RtxStream(params);
	}

	bool RtpStream::ReceivePacket(RTC::RtpPacket* packet)
	{
		 
		uint16_t seq = packet->GetSequenceNumber();

		// If this is the first packet seen, initialize stuff.
		if (!this->started)
		{
			InitSeq(seq);

			this->started     = true;
			this->maxSeq      = seq - 1;
			this->maxPacketTs = packet->GetTimestamp();
			this->maxPacketMs = uv_util::GetTimeMs();
		}

		// If not a valid packet ignore it.
		if (!UpdateSeq(packet))
		{
			WARNING_EX_LOG(
			   
			  "invalid packet [ssrc:%" PRIu32 ", seq:%" PRIu16 "]",
			  packet->GetSsrc(),
			  packet->GetSequenceNumber());

			return false;
		}

		// Update highest seen RTP timestamp.
		if (RTC::SeqManager<uint32_t>::IsSeqHigherThan(packet->GetTimestamp(), this->maxPacketTs))
		{
			this->maxPacketTs = packet->GetTimestamp();
			this->maxPacketMs = uv_util::GetTimeMs();
		}

		return true;
	}

	void RtpStream::ResetScore(uint8_t score, bool notify)
	{
		 
		this->scores.clear();

		if (this->score != score)
		{
			auto previousScore = this->score;

			this->score = score;

			// If previous score was 0 (and new one is not 0) then update activeSinceMs.
			if (previousScore == 0u)
				this->activeSinceMs = uv_util::GetTimeMs();

			// Notify the listener.
			if (notify)
				this->listener->OnRtpStreamScore(this, score, previousScore);
		}
	}

	bool RtpStream::UpdateSeq(RTC::RtpPacket* packet)
	{
		 
		uint16_t seq    = packet->GetSequenceNumber();
		uint16_t udelta = seq - this->maxSeq;

		// If the new packet sequence number is greater than the max seen but not
		// "so much bigger", accept it.
		// NOTE: udelta also handles the case of a new cycle, this is:
		//    maxSeq:65536, seq:0 => udelta:1
		if (udelta < MaxDropout)
		{
			// In order, with permissible gap.
			if (seq < this->maxSeq)
			{
				// Sequence number wrapped: count another 64K cycle.
				this->cycles += RtpSeqMod;
			}

			this->maxSeq = seq;
		}
		// Too old packet received (older than the allowed misorder).
		// Or to new packet (more than acceptable dropout).
		else if (udelta <= RtpSeqMod - MaxMisorder)
		{
			// The sequence number made a very large jump. If two sequential packets
			// arrive, accept the latter.
			if (seq == this->badSeq)
			{
				// Two sequential packets. Assume that the other side restarted without
				// telling us so just re-sync (i.e., pretend this was the first packet).
				WARNING_EX_LOG( "too bad sequence number, re-syncing RTP [ssrc:%" PRIu32 ", seq:%" PRIu16 "]",
				  packet->GetSsrc(),
				  packet->GetSequenceNumber());

				InitSeq(seq);

				this->maxPacketTs = packet->GetTimestamp();
				this->maxPacketMs = uv_util::GetTimeMs();
			}
			else
			{
				WARNING_EX_LOG( "bad sequence number, ignoring packet [ssrc:%" PRIu32 ", seq:%" PRIu16 "]",
				  packet->GetSsrc(),
				  packet->GetSequenceNumber());

				this->badSeq = (seq + 1) & (RtpSeqMod - 1);

				// Packet discarded due to late or early arriving.
				this->packetsDiscarded++;

				return false;
			}
		}
		// Acceptable misorder.
		else
		{
			// Do nothing.
		}

		return true;
	}

	void RtpStream::UpdateScore(uint8_t score)
	{
		 
		// Add the score into the histogram.
		if (this->scores.size() == ScoreHistogramLength)
			this->scores.erase(this->scores.begin());

		auto previousScore = this->score;

		// Compute new effective score taking into accout entries in the histogram.
		this->scores.push_back(score);

		/*
		 * Scoring mechanism is a weighted average.
		 *
		 * The more recent the score is, the more weight it has.
		 * The oldest score has a weight of 1 and subsequent scores weight is
		 * increased by one sequentially.
		 *
		 * Ie:
		 * - scores: [1,2,3,4]
		 * - this->scores = ((1) + (2+2) + (3+3+3) + (4+4+4+4)) / 10 = 2.8 => 3
		 */

		size_t weight{ 0 };
		size_t samples{ 0 };
		size_t totalScore{ 0 };
		// åŠ æƒå¹³å‡æ•°  è®¡ç®— å“ˆ
		// 1. æƒé‡è¶Šå¤§ åˆ¤æ–­çš„å æœ‰æ ‡é‡è¶Šå¤§å“ˆ
		for (auto score : this->scores)
		{
			weight++;
			samples += weight;
			totalScore += weight * score;
		}

		// clang-tidy "thinks" that this can lead to division by zero but we are
		// smarter.
		// NOLINTNEXTLINE(clang-analyzer-core.DivideZero)  // Round å‡½æ•°å³å››èˆäº”å…¥å–å¶
		this->score = static_cast<uint8_t>(std::round(static_cast<double>(totalScore) / samples));

		// Call the listener if the global score has changed.
		if (this->score != previousScore)
		{
			DEBUG_EX_LOG( "[added score:%" PRIu8 ", previous computed score:%" PRIu8 ", new computed score:%" PRIu8
			  "] (calling listener)",
			  score,
			  previousScore,
			  this->score);

			// If previous score was 0 (and new one is not 0) then update activeSinceMs.
			if (previousScore == 0u)
				this->activeSinceMs = uv_util::GetTimeMs();

			this->listener->OnRtpStreamScore(this, this->score, previousScore);
		}
		else
		{
#if MS_LOG_DEV_LEVEL == 3
			MS_DEBUG_TAG(
			  score,
			  "[added score:%" PRIu8 ", previous computed score:%" PRIu8 ", new computed score:%" PRIu8
			  "] (no change)",
			  score,
			  previousScore,
			  this->score);
#endif
		}
	}

	void RtpStream::PacketRetransmitted(RTC::RtpPacket* /*packet*/)
	{
		 
		this->packetsRetransmitted++;
	}

	void RtpStream::PacketRepaired(RTC::RtpPacket* /*packet*/)
	{
		 
		this->packetsRepaired++;
	}

	inline void RtpStream::InitSeq(uint16_t seq)
	{
		 
		// Initialize/reset RTP counters.
		this->baseSeq = seq;
		this->maxSeq  = seq;
		this->badSeq  = RtpSeqMod + 1; // So seq == badSeq is false.
	}

	/*void RtpStream::Params::FillJson(json& jsonObject) const
	{

		jsonObject["encodingIdx"] = this->encodingIdx;
		jsonObject["ssrc"]        = this->ssrc;
		jsonObject["payloadType"] = this->payloadType;
		jsonObject["mimeType"]    = this->mimeType.ToString();
		jsonObject["clockRate"]   = this->clockRate;

		if (!this->rid.empty())
			jsonObject["rid"] = this->rid;

		jsonObject["cname"] = this->cname;

		if (this->rtxSsrc != 0)
		{
			jsonObject["rtxSsrc"]        = this->rtxSsrc;
			jsonObject["rtxPayloadType"] = this->rtxPayloadType;
		}

		jsonObject["useNack"]        = this->useNack;
		jsonObject["usePli"]         = this->usePli;
		jsonObject["useFir"]         = this->useFir;
		jsonObject["useInBandFec"]   = this->useInBandFec;
		jsonObject["useDtx"]         = this->useDtx;
		jsonObject["spatialLayers"]  = this->spatialLayers;
		jsonObject["temporalLayers"] = this->temporalLayers;
	}*/
} // namespace RTC
