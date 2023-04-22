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

#ifndef MS_RTC_RTP_STREAM_HPP
#define MS_RTC_RTP_STREAM_HPP

//#include "common.hpp"
//#include "DepLibUV.hpp"
#include "FeedbackPsFir.hpp"
#include "FeedbackPsPli.hpp"
#include "FeedbackRtpNack.hpp"
#include "Packet.hpp"
#include "ReceiverReport.hpp"
#include "Sdes.hpp"
#include "SenderReport.hpp"
#include "RtpDictionaries.hpp"
#include "RtxStream.hpp"
//#include <nlohmann/json.hpp>
#include <string>
#include <vector>

//using json = nlohmann::json;

namespace RTC
{
	class RtpStream
	{
	protected:
		class Listener
		{
		public:
			virtual ~Listener() = default;

		public:
			virtual void OnRtpStreamScore(RTC::RtpStream* rtpStream, uint8_t score, uint8_t previousScore) = 0;
		};

	public:
		struct Params
		{
			 
			size_t encodingIdx{ 0u };
			uint32_t ssrc{ 0u };
			uint8_t payloadType{ 0u };
			RTC::RtpCodecMimeType mimeType;
			uint32_t clockRate{ 0u };
			std::string rid;
			std::string cname;
			uint32_t rtxSsrc{ 0u };
			uint8_t rtxPayloadType{ 0u };
			bool useNack{ false };
			bool usePli{ false };
			bool useFir{ false };
			bool useInBandFec{ false };
			bool useDtx{ false };
			uint8_t spatialLayers{ 1u };
			uint8_t temporalLayers{ 1u };
		};

	public:
		RtpStream(RTC::RtpStream::Listener* listener, RTC::RtpStream::Params& params, uint8_t initialScore);
		virtual ~RtpStream();

		//void FillJson(json& jsonObject) const;
		//virtual void FillJsonStats(json& jsonObject);
		uint32_t GetEncodingIdx() const
		{
			return this->params.encodingIdx;
		}
		uint32_t GetSsrc() const
		{
			return this->params.ssrc;
		}
		uint8_t GetPayloadType() const
		{
			return this->params.payloadType;
		}
		const RTC::RtpCodecMimeType& GetMimeType() const
		{
			return this->params.mimeType;
		}
		uint32_t GetClockRate() const
		{
			return this->params.clockRate;
		}
		const std::string& GetRid() const
		{
			return this->params.rid;
		}
		const std::string& GetCname() const
		{
			return this->params.cname;
		}
		bool HasRtx() const
		{
			return this->rtxStream != nullptr;
		}
		virtual void SetRtx(uint8_t payloadType, uint32_t ssrc);
		uint32_t GetRtxSsrc() const
		{
			return this->params.rtxSsrc;
		}
		uint8_t GetRtxPayloadType() const
		{
			return this->params.rtxPayloadType;
		}
		uint8_t GetSpatialLayers() const
		{
			return this->params.spatialLayers;
		}
		bool HasDtx() const
		{
			return this->params.useDtx;
		}
		uint8_t GetTemporalLayers() const
		{
			return this->params.temporalLayers;
		}
		virtual bool ReceivePacket(RTC::RtpPacket* packet);
		virtual void Pause()                                                                     = 0;
		virtual void Resume()                                                                    = 0;
		virtual uint32_t GetBitrate(uint64_t nowMs)                                              = 0;
		virtual uint32_t GetBitrate(uint64_t nowMs, uint8_t spatialLayer, uint8_t temporalLayer) = 0;
		virtual uint32_t GetSpatialLayerBitrate(uint64_t nowMs, uint8_t spatialLayer)            = 0;
		virtual uint32_t GetLayerBitrate(uint64_t nowMs, uint8_t spatialLayer, uint8_t temporalLayer) = 0;
		void ResetScore(uint8_t score, bool notify);
		uint8_t GetFractionLost() const
		{
			return this->fractionLost;
		}
		float GetLossPercentage() const
		{
			return static_cast<float>(this->fractionLost) * 100 / 256;
		}
		float GetRtt() const
		{
			return this->rtt;
		}
		uint64_t GetMaxPacketMs() const
		{
			return this->maxPacketMs;
		}
		uint32_t GetMaxPacketTs() const
		{
			return this->maxPacketTs;
		}
		uint64_t GetSenderReportNtpMs() const
		{
			return this->lastSenderReportNtpMs;
		}
		uint32_t GetSenderReportTs() const
		{
			return this->lastSenderReporTs;
		}
		uint8_t GetScore() const
		{
			return this->score;
		}
		uint64_t GetActiveMs() const
		{
			return uv_util::GetTimeMs() - this->activeSinceMs;
		}

	protected:
		bool UpdateSeq(RTC::RtpPacket* packet);
		void UpdateScore(uint8_t score);
		void PacketRetransmitted(RTC::RtpPacket* packet);
		void PacketRepaired(RTC::RtpPacket* packet);
		uint32_t GetExpectedPackets() const
		{
			return (this->cycles + this->maxSeq) - this->baseSeq + 1;
		}

	private:
		void InitSeq(uint16_t seq);

	protected:
		// Given as argument.
		RTC::RtpStream::Listener* listener{ nullptr };
		Params params;
		// Others.
		//   https://tools.ietf.org/html/rfc3550#appendix-A.1 stuff.
		uint16_t maxSeq{ 0u };      // Highest seq. number seen.
		uint32_t cycles{ 0u };      // Shifted count of seq. number cycles.
		uint32_t baseSeq{ 0u };     // Base seq number.
		uint32_t badSeq{ 0u };      // Last 'bad' seq number + 1.
		uint32_t maxPacketTs{ 0u }; // Highest timestamp seen.
		uint64_t maxPacketMs{ 0u }; // When the packet with highest timestammp was seen.
		uint32_t packetsLost{ 0u };
		uint8_t fractionLost{ 0u };
		size_t packetsDiscarded{ 0u };
		size_t packetsRetransmitted{ 0u };
		size_t packetsRepaired{ 0u };
		size_t nackCount{ 0u };
		size_t nackPacketCount{ 0u };
		size_t pliCount{ 0u };
		size_t firCount{ 0u };
		size_t repairedPriorScore{ 0u }; // Packets repaired at last interval for score calculation.
		size_t retransmittedPriorScore{ 0u }; // Packets retransmitted at last interval for score calculation.
		uint64_t lastSenderReportNtpMs{ 0u }; // NTP timestamp in last Sender Report (in ms).
		uint32_t lastSenderReporTs{ 0u };     // RTP timestamp in last Sender Report.
		float rtt{ 0 };
		bool hasRtt{ false };
		// Instance of RtxStream.
		RTC::RtxStream* rtxStream{ nullptr };

	private:
		// Score related.
		uint8_t score{ 0u };
		std::vector<uint8_t> scores;
		// Whether at least a RTP packet has been received.
		bool started{ false };
		// Last time since the stream is active.
		uint64_t activeSinceMs{ 0u };
	};
} // namespace RTC

#endif
