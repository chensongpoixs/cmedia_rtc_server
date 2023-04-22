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
//#define MS_CLASS "RTC::TransportCongestionControlClient"
// #define MS_LOG_DEV_LEVEL 3

#include "TransportCongestionControlClient.hpp"
//#include "DepLibUV.hpp"
//#include "Logger.hpp"
//#include "MediaSoupErrors.hpp"
#include <libwebrtc/api/transport/network_types.h> // webrtc::TargetRateConstraints
#include <limits>
#include "clog.h"
#include "cuv_util.h"
using namespace chen;

namespace RTC
{
	/* Static. */

	static constexpr uint32_t MinBitrate{ 30000u };
	static constexpr float MaxBitrateIncrementFactor{ 1.35f };
	static constexpr float MaxPaddingBitrateFactor{ 0.85f };
	static constexpr uint64_t AvailableBitrateEventInterval{ 2000u }; // In ms.

	/* Instance methods. */

	TransportCongestionControlClient::TransportCongestionControlClient(
	  RTC::TransportCongestionControlClient::Listener* listener,
	  RTC::BweType bweType,
	  uint32_t initialAvailableBitrate,
	  uint32_t maxOutgoingBitrate)
	  :  listener(listener), bweType(bweType),
	    initialAvailableBitrate(std::max<uint32_t>(initialAvailableBitrate, MinBitrate)),
	    maxOutgoingBitrate(maxOutgoingBitrate)
	{
		//MS_TRACE();

		webrtc::GoogCcFactoryConfig config;

		// Provide RTCP feedback as well as Receiver Reports.
		config.feedback_only = false;

		this->controllerFactory = new webrtc::GoogCcNetworkControllerFactory(std::move(config));

		webrtc::BitrateConstraints bitrateConfig;

		bitrateConfig.start_bitrate_bps = static_cast<int>(this->initialAvailableBitrate);

		this->rtpTransportControllerSend =
		  new webrtc::RtpTransportControllerSend(this, nullptr, this->controllerFactory, bitrateConfig);

		this->rtpTransportControllerSend->RegisterTargetTransferRateObserver(this);

		this->probationGenerator = new RTC::RtpProbationGenerator();

		this->processTimer = new chen::ctimer(this);
		//ctimer::init();
		// NOTE: This is supposed to recover computed available bandwidth after
		// network issues.
		this->rtpTransportControllerSend->EnablePeriodicAlrProbing(true);
		processTimer->Start(std::min(
				// Depends on probation being done and WebRTC-Pacer-MinPacketLimitMs field trial.
				this->rtpTransportControllerSend->packet_sender()->TimeUntilNextProcess(),
				// Fixed value (25ms), libwebrtc/api/transport/goog_cc_factory.cc.
				this->controllerFactory->GetProcessInterval().ms()
			));
		// clang-format off
		//this->processTimer->Start(std::min(
		//	// Depends on probation being done and WebRTC-Pacer-MinPacketLimitMs field trial.
		//	this->rtpTransportControllerSend->packet_sender()->TimeUntilNextProcess(),
		//	// Fixed value (25ms), libwebrtc/api/transport/goog_cc_factory.cc.
		//	this->controllerFactory->GetProcessInterval().ms()
		//));
		// clang-format on
	}

	TransportCongestionControlClient::~TransportCongestionControlClient()
	{
		//MS_TRACE();

		delete this->controllerFactory;
		this->controllerFactory = nullptr;

		delete this->rtpTransportControllerSend;
		this->rtpTransportControllerSend = nullptr;

		delete this->probationGenerator;
		this->probationGenerator = nullptr;

		
		 delete this->processTimer;
		 this->processTimer = nullptr;
	}

	void TransportCongestionControlClient::TransportConnected()
	{
		//MS_TRACE();

		this->rtpTransportControllerSend->OnNetworkAvailability(true);
	}

	void TransportCongestionControlClient::TransportDisconnected()
	{
		//MS_TRACE();

		auto nowMs = uv_util::GetTimeMs(); //DepLibUV::GetTimeMsInt64();

		this->bitrates.desiredBitrate          = 0u;
		this->bitrates.effectiveDesiredBitrate = 0u;

		this->desiredBitrateTrend.ForceUpdate(0u, nowMs);
		this->rtpTransportControllerSend->OnNetworkAvailability(false);
	}

	void TransportCongestionControlClient::InsertPacket(webrtc::RtpPacketSendInfo& packetInfo)
	{
		//MS_TRACE();

		this->rtpTransportControllerSend->packet_sender()->InsertPacket(packetInfo.length);
		this->rtpTransportControllerSend->OnAddPacket(packetInfo);
	}

	webrtc::PacedPacketInfo TransportCongestionControlClient::GetPacingInfo()
	{
		//MS_TRACE();

		return this->rtpTransportControllerSend->packet_sender()->GetPacingInfo();
	}

	void TransportCongestionControlClient::PacketSent(webrtc::RtpPacketSendInfo& packetInfo, int64_t nowMs)
	{
		//MS_TRACE();

		// Notify the transport feedback adapter about the sent packet.
		rtc::SentPacket sentPacket(packetInfo.transport_sequence_number, nowMs);
		this->rtpTransportControllerSend->OnSentPacket(sentPacket, packetInfo.length);
	}

	void TransportCongestionControlClient::ReceiveEstimatedBitrate(uint32_t bitrate)
	{
		//MS_TRACE();

		this->rtpTransportControllerSend->OnReceivedEstimatedBitrate(bitrate);
	}

	void TransportCongestionControlClient::ReceiveRtcpReceiverReport(
	  RTC::RTCP::ReceiverReportPacket* packet, float rtt, int64_t nowMs)
	{
		//MS_TRACE();

		webrtc::ReportBlockList reportBlockList;

		for (auto it = packet->Begin(); it != packet->End(); ++it)
		{
			auto& report = *it;

			reportBlockList.emplace_back(
			  packet->GetSsrc(),
			  report->GetSsrc(),
			  report->GetFractionLost(),
			  report->GetTotalLost(),
			  report->GetLastSeq(),
			  report->GetJitter(),
			  report->GetLastSenderReport(),
			  report->GetDelaySinceLastSenderReport());
		}

		this->rtpTransportControllerSend->OnReceivedRtcpReceiverReport(
		  reportBlockList, static_cast<int64_t>(rtt), nowMs);
	}

	void TransportCongestionControlClient::ReceiveRtcpTransportFeedback(
	  const RTC::RTCP::FeedbackRtpTransportPacket* feedback)
	{
		//MS_TRACE();

		this->rtpTransportControllerSend->OnTransportFeedback(*feedback);
	}

	void TransportCongestionControlClient::SetMaxOutgoingBitrate(uint32_t maxBitrate)
	{
		if (maxBitrate < this->initialAvailableBitrate)
		{
			ERROR_EX_LOG("maxOutgoingBitrate must be >= initialAvailableOutgoingBitrate");
		}

		if (maxBitrate < MinBitrate)
		{
			ERROR_EX_LOG("maxOutgoingBitrate must be >= 30000bps");
		}

		this->maxOutgoingBitrate = maxBitrate;
	}

	void TransportCongestionControlClient::SetDesiredBitrate(uint32_t desiredBitrate, bool force)
	{
		//MS_TRACE();

		auto nowMs = uv_util::GetTimeMsInt64();//DepLibUV::GetTimeMsInt64();

		// Manage it via trending and increase it a bit to avoid immediate oscillations.
		if (!force)
		{
			this->desiredBitrateTrend.Update(desiredBitrate, nowMs);
		}
		else
		{
			this->desiredBitrateTrend.ForceUpdate(desiredBitrate, nowMs);
		}

		this->bitrates.desiredBitrate          = desiredBitrate;
		this->bitrates.effectiveDesiredBitrate = this->desiredBitrateTrend.GetValue();
		this->bitrates.minBitrate              = MinBitrate;
		// NOTE: Setting 'startBitrate' to 'availableBitrate' has proven to generate
		// more stable values.
		this->bitrates.startBitrate = std::max<uint32_t>(MinBitrate, this->bitrates.availableBitrate);

		if (this->desiredBitrateTrend.GetValue() > 0u)
		{
			this->bitrates.maxBitrate = std::max<uint32_t>(
			  this->initialAvailableBitrate,
			  this->desiredBitrateTrend.GetValue() * MaxBitrateIncrementFactor);

			if (this->maxOutgoingBitrate > 0u)
			{
				this->bitrates.maxBitrate =
				  std::min<uint32_t>(this->maxOutgoingBitrate, this->bitrates.maxBitrate);
			}

			this->bitrates.maxPaddingBitrate = this->bitrates.maxBitrate * MaxPaddingBitrateFactor;
		}
		else
		{
			this->bitrates.maxBitrate        = this->initialAvailableBitrate;
			this->bitrates.maxPaddingBitrate = 0u;
		}

		DEBUG_EX_LOG(
		  "[desiredBitrate:%u, startBitrate:%u, minBitrate:%u, maxBitrate:%u, maxPaddingBitrate:%u]",
		  this->bitrates.desiredBitrate,
		  this->bitrates.startBitrate,
		  this->bitrates.minBitrate,
		  this->bitrates.maxBitrate,
		  this->bitrates.maxPaddingBitrate);

		this->rtpTransportControllerSend->SetAllocatedSendBitrateLimits(
		  this->bitrates.minBitrate, this->bitrates.maxPaddingBitrate, this->bitrates.maxBitrate);

		webrtc::TargetRateConstraints constraints;

		constraints.at_time = webrtc::Timestamp::ms(uv_util::GetTimeMs() /*DepLibUV::GetTimeMs()*/);
		constraints.min_data_rate = webrtc::DataRate::bps(this->bitrates.minBitrate);
		constraints.max_data_rate = webrtc::DataRate::bps(this->bitrates.maxBitrate);
		constraints.starting_rate = webrtc::DataRate::bps(this->bitrates.startBitrate);

		this->rtpTransportControllerSend->SetClientBitratePreferences(constraints);
	}

	uint32_t TransportCongestionControlClient::GetAvailableBitrate() const
	{
		//MS_TRACE();

		return this->bitrates.availableBitrate;
	}

	void TransportCongestionControlClient::RescheduleNextAvailableBitrateEvent()
	{
		//MS_TRACE();

		this->lastAvailableBitrateEventAtMs = uv_util::GetTimeMs();
	}

	void TransportCongestionControlClient::MayEmitAvailableBitrateEvent(uint32_t previousAvailableBitrate)
	{
		//MS_TRACE();

		uint64_t nowMs = uv_util::GetTimeMsInt64();// DepLibUV::GetTimeMsInt64();
		bool notify{ false };

		// Ignore if first event.
		// NOTE: Otherwise it will make the Transport crash since this event also happens
		// during the constructor of this class.
		if (this->lastAvailableBitrateEventAtMs == 0u)
		{
			this->lastAvailableBitrateEventAtMs = nowMs;

			return;
		}

		// Emit if this is the first valid event.
		if (!this->availableBitrateEventCalled)
		{
			this->availableBitrateEventCalled = true;

			notify = true;
		}
		// Emit event if AvailableBitrateEventInterval elapsed.
		else if (nowMs - this->lastAvailableBitrateEventAtMs >= AvailableBitrateEventInterval)
		{
			notify = true;
		}
		// Also emit the event fast if we detect a high BWE value decrease.
		else if (this->bitrates.availableBitrate < previousAvailableBitrate * 0.75)
		{
			WARNING_EX_LOG( " bwe, high BWE value decrease detected, notifying the listener [now:%u, before:%u]",
			  this->bitrates.availableBitrate,
			  previousAvailableBitrate);

			notify = true;
		}
		// Also emit the event fast if we detect a high BWE value increase.
		else if (this->bitrates.availableBitrate > previousAvailableBitrate * 1.50)
		{
		//	DEBUG_EX_LOG("bwe, high BWE value increase detected, notifying the listener [now:%u, before:%u]",
		//	  this->bitrates.availableBitrate,
		//	  previousAvailableBitrate);

			notify = true;
		}

		if (notify)
		{
			//DEBUG_EX_LOG( "notifying the listener with new available bitrate:%u"  ,
			 // this->bitrates.availableBitrate);

			this->lastAvailableBitrateEventAtMs = nowMs;

			this->listener->OnTransportCongestionControlClientBitrates(this, this->bitrates);
		}
	}

	void TransportCongestionControlClient::OnTargetTransferRate(webrtc::TargetTransferRate targetTransferRate)
	{
		//MS_TRACE();

		// NOTE: The same value as 'this->initialAvailableBitrate' is received periodically
		// regardless of the real available bitrate. Skip such value except for the first time
		// this event is called.
		// clang-format off
		if (
			this->availableBitrateEventCalled &&
			targetTransferRate.target_rate.bps() == this->initialAvailableBitrate
		)
		// clang-format on
		{
			return;
		}

		auto previousAvailableBitrate = this->bitrates.availableBitrate;

		// Update availableBitrate.
		// NOTE: Just in case.
		if (targetTransferRate.target_rate.bps() > std::numeric_limits<uint32_t>::max())
			this->bitrates.availableBitrate = std::numeric_limits<uint32_t>::max();
		else
			this->bitrates.availableBitrate = static_cast<uint32_t>(targetTransferRate.target_rate.bps());

		//DEBUG_EX_LOG("new available bitrate:%u"  , this->bitrates.availableBitrate);

		MayEmitAvailableBitrateEvent(previousAvailableBitrate);
	}

	// Called from PacedSender in order to send probation packets.
	void TransportCongestionControlClient::SendPacket(
	  RTC::RtpPacket* packet, const webrtc::PacedPacketInfo& pacingInfo)
	{
		//MS_TRACE();

		// Send the packet.
		this->listener->OnTransportCongestionControlClientSendRtpPacket(this, packet, pacingInfo);
	}

	RTC::RtpPacket* TransportCongestionControlClient::GeneratePadding(size_t size)
	{
		//MS_TRACE();

		return this->probationGenerator->GetNextPacket(size);
	}

	void TransportCongestionControlClient::OnTimer(ctimer* timer)
	{
		//MS_TRACE();

		 if (timer == this->processTimer)
		{
			// Time to call RtpTransportControllerSend::Process().
			this->rtpTransportControllerSend->Process();

			// Time to call PacedSender::Process().
			this->rtpTransportControllerSend->packet_sender()->Process();

			/* clang-format off */
			//this->processTimer->Start(std::min<uint64_t>(
			//	// Depends on probation being done and WebRTC-Pacer-MinPacketLimitMs field trial.
			//	this->rtpTransportControllerSend->packet_sender()->TimeUntilNextProcess(),
			//	// Fixed value (25ms), libwebrtc/api/transport/goog_cc_factory.cc.
			//	this->controllerFactory->GetProcessInterval().ms()
			//));
			processTimer->Start(std::min<uint64_t>(
				// Depends on probation being done and WebRTC-Pacer-MinPacketLimitMs field trial.
				this->rtpTransportControllerSend->packet_sender()->TimeUntilNextProcess(),
				// Fixed value (25ms), libwebrtc/api/transport/goog_cc_factory.cc.
				this->controllerFactory->GetProcessInterval().ms()
				));
			/* clang-format on */

			MayEmitAvailableBitrateEvent(this->bitrates.availableBitrate);
		}
	}
} // namespace RTC
