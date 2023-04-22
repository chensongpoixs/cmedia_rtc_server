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
#ifndef MS_RTC_CONSUMER_HPP
#define MS_RTC_CONSUMER_HPP

//#include "common.hpp"
//#include "Channel/ChannelRequest.hpp"
#include "CompoundPacket.hpp"
#include "FeedbackPs.hpp"
#include "FeedbackPsFir.hpp"
#include "FeedbackPsPli.hpp"
#include "FeedbackRtpNack.hpp"
#include "ReceiverReport.hpp"
#include "RtpDictionaries.hpp"
#include "RtpHeaderExtensionIds.hpp"
#include "RtpPacket.hpp"
#include "RtpStream.hpp"
#include "RtpStreamSend.hpp"
//#include <nlohmann/json.hpp>
#include <string>
#include <unordered_set>
#include <vector>
#include <json/json.h>
//using json = nlohmann::json;

namespace RTC
{
	class Consumer
	{
	public:
		class Listener
		{
		public:
			virtual ~Listener() = default;

		public:
			virtual void OnConsumerSendRtpPacket(RTC::Consumer* consumer, RTC::RtpPacket* packet) = 0;
			virtual void OnConsumerRetransmitRtpPacket(RTC::Consumer* consumer, RTC::RtpPacket* packet) = 0;
			virtual void OnConsumerKeyFrameRequested(RTC::Consumer* consumer, uint32_t mappedSsrc) = 0;
			virtual void OnConsumerNeedBitrateChange(RTC::Consumer* consumer)                      = 0;
			virtual void OnConsumerNeedZeroBitrate(RTC::Consumer* consumer)                        = 0;
			virtual void OnConsumerProducerClosed(RTC::Consumer* consumer)                         = 0;
		};

	public:
		struct Layers
		{
			int16_t spatial{ -1 };
			int16_t temporal{ -1 };
		};

	private:
		struct TraceEventTypes
		{
			bool rtp{ false };
			bool keyframe{ false };
			bool nack{ false };
			bool pli{ false };
			bool fir{ false };
		};

	public:
		Consumer(
		  const std::string& id,
		  const std::string& producerId,
		  RTC::Consumer::Listener* listener,
		  Json::Value& data,
		  RTC::RtpParameters::Type type);
		virtual ~Consumer();

	public:
		//virtual void FillJson(Json::Value& jsonObject) const;
		virtual void FillJsonStats(Json::Value& jsonArray) const  = 0;
		virtual void FillJsonScore(Json::Value& jsonObject) const = 0;
		//virtual void HandleRequest(Channel::ChannelRequest* request);
		RTC::Media::Kind GetKind() const
		{
			return this->kind;
		}
		const RTC::RtpParameters& GetRtpParameters() const
		{
			return this->rtpParameters;
		}
		const struct RTC::RtpHeaderExtensionIds& GetRtpHeaderExtensionIds() const
		{
			return this->rtpHeaderExtensionIds;
		}
		RTC::RtpParameters::Type GetType() const
		{
			return this->type;
		}
		virtual Layers GetPreferredLayers() const
		{
			// By default return 1:1.
			Consumer::Layers layers;

			return layers;
		}
		const std::vector<uint32_t>& GetMediaSsrcs() const
		{
			return this->mediaSsrcs;
		}
		const std::vector<uint32_t>& GetRtxSsrcs() const
		{
			return this->rtxSsrcs;
		}
		virtual bool IsActive() const
		{
			// The parent Consumer just checks whether Consumer and Producer are
			// not paused and the transport connected.
			// clang-format off
			return (
				this->transportConnected &&
				!this->paused &&
				!this->producerPaused &&
				!this->producerClosed
			);
			// clang-format on
		}
		void TransportConnected();
		void TransportDisconnected();
		bool IsPaused() const
		{
			return this->paused;
		}
		bool IsProducerPaused() const
		{
			return this->producerPaused;
		}
		void ProducerPaused();
		void ProducerResumed();
		virtual void ProducerRtpStream(RTC::RtpStream* rtpStream, uint32_t mappedSsrc)    = 0;
		virtual void ProducerNewRtpStream(RTC::RtpStream* rtpStream, uint32_t mappedSsrc) = 0;
		void ProducerRtpStreamScores(const std::vector<uint8_t>* scores);
		virtual void ProducerRtpStreamScore(
		  RTC::RtpStream* rtpStream, uint8_t score, uint8_t previousScore)           = 0;
		virtual void ProducerRtcpSenderReport(RTC::RtpStream* rtpStream, bool first) = 0;
		void ProducerClosed();
		void SetExternallyManagedBitrate()
		{
			this->externallyManagedBitrate = true;
		}
		virtual uint8_t GetBitratePriority() const                          = 0;
		virtual uint32_t IncreaseLayer(uint32_t bitrate, bool considerLoss) = 0;
		virtual void ApplyLayers()                                          = 0;
		virtual uint32_t GetDesiredBitrate() const                          = 0;
		virtual void SendRtpPacket(RTC::RtpPacket* packet)                  = 0;
		virtual std::vector<RTC::RtpStreamSend*> GetRtpStreams()            = 0;
		virtual void GetRtcp(
		  RTC::RTCP::CompoundPacket* packet, RTC::RtpStreamSend* rtpStream, uint64_t nowMs) = 0;
		virtual void NeedWorstRemoteFractionLost(uint32_t mappedSsrc, uint8_t& worstRemoteFractionLost) = 0;
		virtual void ReceiveNack(RTC::RTCP::FeedbackRtpNackPacket* nackPacket) = 0;
		virtual void ReceiveKeyFrameRequest(
		  RTC::RTCP::FeedbackPs::MessageType messageType, uint32_t ssrc)          = 0;
		virtual void ReceiveRtcpReceiverReport(RTC::RTCP::ReceiverReport* report) = 0;
		virtual uint32_t GetTransmissionRate(uint64_t nowMs)                      = 0;
		virtual float GetRtt() const                                              = 0;

	protected:
		void EmitTraceEventRtpAndKeyFrameTypes(RTC::RtpPacket* packet, bool isRtx = false) const;
		void EmitTraceEventKeyFrameType(RTC::RtpPacket* packet, bool isRtx = false) const;
		void EmitTraceEventPliType(uint32_t ssrc) const;
		void EmitTraceEventFirType(uint32_t ssrc) const;
		void EmitTraceEventNackType() const;

	private:
		virtual void UserOnTransportConnected()    = 0;
		virtual void UserOnTransportDisconnected() = 0;
		virtual void UserOnPaused()                = 0;
		virtual void UserOnResumed()               = 0;

	public:
		// Passed by argument.
		const std::string id;
		const std::string producerId;

	protected:
		// Passed by argument.
		RTC::Consumer::Listener* listener{ nullptr };
		RTC::Media::Kind kind;
		RTC::RtpParameters rtpParameters;
		RTC::RtpParameters::Type type{ RTC::RtpParameters::Type::NONE };
		std::vector<RTC::RtpEncodingParameters> consumableRtpEncodings;
		struct RTC::RtpHeaderExtensionIds rtpHeaderExtensionIds;
		const std::vector<uint8_t>* producerRtpStreamScores{ nullptr };
		// Others.
		std::unordered_set<uint8_t> supportedCodecPayloadTypes;
		uint64_t lastRtcpSentTime{ 0u };
		uint16_t maxRtcpInterval{ 0u };
		bool externallyManagedBitrate{ false };
		uint8_t priority{ 1u };
		struct TraceEventTypes traceEventTypes;

	private:
		// Others.
		std::vector<uint32_t> mediaSsrcs;
		std::vector<uint32_t> rtxSsrcs;
		bool transportConnected{ false };
		bool paused{ false };
		bool producerPaused{ false };
		bool producerClosed{ false };
	};
} // namespace RTC

#endif
