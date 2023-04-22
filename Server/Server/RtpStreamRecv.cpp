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

#include "RtpStreamRecv.hpp"
//#include "Logger.hpp"
//#include "Utils.hpp"
#include "Tools.hpp"
#include "ReceiverReport.hpp"
#include "SenderReport.hpp"
#include "clog.h"
//#include "FeedbackPsPliPacket.h"
#include "FeedbackPsPli.hpp"
#include "FeedbackRtpNack.hpp"
#include "FeedbackPsFir.hpp"
namespace RTC
{
	/* Static. */

	static constexpr uint64_t InactivityCheckInterval{ 1500u };        // In ms.
	static constexpr uint64_t InactivityCheckIntervalWithDtx{ 5000u }; // In ms.

	/* TransmissionCounter methods. */

	RtpStreamRecv::TransmissionCounter::TransmissionCounter(
	  uint8_t spatialLayers, uint8_t temporalLayers, size_t windowSize)
	{
		 
		// Reserve vectors capacity.
		this->spatialLayerCounters = std::vector<std::vector<RTC::RtpDataCounter>>(spatialLayers);

		for (auto& spatialLayerCounter : this->spatialLayerCounters)
		{
			for (uint8_t tIdx{ 0u }; tIdx < temporalLayers; ++tIdx)
			{
				spatialLayerCounter.emplace_back(RTC::RtpDataCounter(windowSize));
			}
		}
	}

	void RtpStreamRecv::TransmissionCounter::Update(RTC::RtpPacket* packet)
	{
		 
		auto spatialLayer  = packet->GetSpatialLayer();
		auto temporalLayer = packet->GetTemporalLayer();

		// Sanity check. Do not allow spatial layers higher than defined.
		if (spatialLayer > this->spatialLayerCounters.size() - 1)
			spatialLayer = this->spatialLayerCounters.size() - 1;

		// Sanity check. Do not allow temporal layers higher than defined.
		if (temporalLayer > this->spatialLayerCounters[0].size() - 1)
			temporalLayer = this->spatialLayerCounters[0].size() - 1;

		auto& counter = this->spatialLayerCounters[spatialLayer][temporalLayer];

		counter.Update(packet);
	}

	uint32_t RtpStreamRecv::TransmissionCounter::GetBitrate(uint64_t nowMs)
	{
		 
		uint32_t rate{ 0u };

		for (auto& spatialLayerCounter : this->spatialLayerCounters)
		{
			for (auto& temporalLayerCounter : spatialLayerCounter)
			{
				rate += temporalLayerCounter.GetBitrate(nowMs);
			}
		}

		return rate;
	}

	uint32_t RtpStreamRecv::TransmissionCounter::GetBitrate(
	  uint64_t nowMs, uint8_t spatialLayer, uint8_t temporalLayer)
	{
		using namespace chen;
		cassert(spatialLayer < this->spatialLayerCounters.size(), "spatialLayer too high");
		cassert(
		  temporalLayer < this->spatialLayerCounters[spatialLayer].size(), "temporalLayer too high");

		// Return 0 if specified layers are not being received.
		auto& counter = this->spatialLayerCounters[spatialLayer][temporalLayer];

		if (counter.GetBitrate(nowMs) == 0)
			return 0u;

		uint32_t rate{ 0u };

		// Iterate all temporal layers of spatial layers previous to the given one.
		for (uint8_t sIdx{ 0u }; sIdx < spatialLayer; ++sIdx)
		{
			for (uint8_t tIdx{ 0u }; tIdx < this->spatialLayerCounters[sIdx].size(); ++tIdx)
			{
				auto& temporalLayerCounter = this->spatialLayerCounters[sIdx][tIdx];

				rate += temporalLayerCounter.GetBitrate(nowMs);
			}
		}

		// Add the given spatial layer with up to the given temporal layer.
		for (uint8_t tIdx{ 0u }; tIdx <= temporalLayer; ++tIdx)
		{
			auto& temporalLayerCounter = this->spatialLayerCounters[spatialLayer][tIdx];

			rate += temporalLayerCounter.GetBitrate(nowMs);
		}

		return rate;
	}

	uint32_t RtpStreamRecv::TransmissionCounter::GetSpatialLayerBitrate(uint64_t nowMs, uint8_t spatialLayer)
	{
		using namespace chen;
		cassert(spatialLayer < this->spatialLayerCounters.size(), "spatialLayer too high");

		uint32_t rate{ 0u };

		// clang-format off
		for (
			uint8_t tIdx{ 0u };
			tIdx < this->spatialLayerCounters[spatialLayer].size();
			++tIdx
		)
		// clang-format on
		{
			auto& temporalLayerCounter = this->spatialLayerCounters[spatialLayer][tIdx];

			rate += temporalLayerCounter.GetBitrate(nowMs);
		}

		return rate;
	}

	uint32_t RtpStreamRecv::TransmissionCounter::GetLayerBitrate(
	  uint64_t nowMs, uint8_t spatialLayer, uint8_t temporalLayer)
	{
		 using namespace chen;
		cassert(spatialLayer < this->spatialLayerCounters.size(), "spatialLayer too high");
		cassert(
		  temporalLayer < this->spatialLayerCounters[spatialLayer].size(), "temporalLayer too high");

		auto& counter = this->spatialLayerCounters[spatialLayer][temporalLayer];

		return counter.GetBitrate(nowMs);
	}

	size_t RtpStreamRecv::TransmissionCounter::GetPacketCount() const
	{
		 
		size_t packetCount{ 0u };

		for (auto& spatialLayerCounter : this->spatialLayerCounters)
		{
			for (auto& temporalLayerCounter : spatialLayerCounter)
			{
				packetCount += temporalLayerCounter.GetPacketCount();
			}
		}

		return packetCount;
	}

	size_t RtpStreamRecv::TransmissionCounter::GetBytes() const
	{
		 
		size_t bytes{ 0u };

		for (const auto& spatialLayerCounter : this->spatialLayerCounters)
		{
			for (const auto& temporalLayerCounter : spatialLayerCounter)
			{
				bytes += temporalLayerCounter.GetBytes();
			}
		}

		return bytes;
	}

	/* Instance methods. */

	RtpStreamRecv::RtpStreamRecv(RTC::RtpStreamRecv::Listener* listener, RTC::RtpStream::Params& params)
	  : RTC::RtpStream::RtpStream(listener, params, 10),
	    transmissionCounter(
	      params.spatialLayers, params.temporalLayers, this->params.useDtx ? 6000 : 2500)
	{
		 
		if (this->params.useNack)
		{
			this->nackGenerator.reset(new RTC::NackGenerator(this));
		}

		// Run the RTP inactivity periodic timer (use a different timeout if DTX is
		// enabled).
		this->inactivityCheckPeriodicTimer = new chen::ctimer(this);
		this->inactive                     = false;

		if (!this->params.useDtx)
		{
			this->inactivityCheckPeriodicTimer->Start(InactivityCheckInterval);
		}
		else
		{
			this->inactivityCheckPeriodicTimer->Start(InactivityCheckIntervalWithDtx);
		}
	}

	RtpStreamRecv::~RtpStreamRecv()
	{
		 
		// Close the RTP inactivity check periodic timer.
		delete this->inactivityCheckPeriodicTimer;
	}

	/*void RtpStreamRecv::FillJsonStats(json& jsonObject)
	{
		 
		uint64_t nowMs = chen::uv_util::GetTimeMs();

		RTC::RtpStream::FillJsonStats(jsonObject);

		jsonObject["type"]        = "inbound-rtp";
		jsonObject["jitter"]      = this->jitter;
		jsonObject["packetCount"] = this->transmissionCounter.GetPacketCount();
		jsonObject["byteCount"]   = this->transmissionCounter.GetBytes();
		jsonObject["bitrate"]     = this->transmissionCounter.GetBitrate(nowMs);

		if (GetSpatialLayers() > 1 || GetTemporalLayers() > 1)
		{
			jsonObject["bitrateByLayer"] = json::object();
			auto jsonBitrateByLayerIt    = jsonObject.find("bitrateByLayer");

			for (uint8_t sIdx = 0; sIdx < GetSpatialLayers(); ++sIdx)
			{
				for (uint8_t tIdx = 0; tIdx < GetTemporalLayers(); ++tIdx)
				{
					(*jsonBitrateByLayerIt)[std::to_string(sIdx) + "." + std::to_string(tIdx)] =
					  GetBitrate(nowMs, sIdx, tIdx);
				}
			}
		}
	}*/

	bool RtpStreamRecv::ReceivePacket(RTC::RtpPacket* packet)
	{
		 
		// Call the parent method.
		if (!RTC::RtpStream::ReceivePacket(packet))
		{
			using namespace chen;
			WARNING_EX_LOG( "rtp packet discarded");

			return false;
		}

		// Process the packet at codec level.
		if (packet->GetPayloadType() == GetPayloadType())
		{
			RTC::Codecs::Tools::ProcessRtpPacket(packet, GetMimeType());
			//packet->Dump();
		}

		// Pass the packet to the NackGenerator.
		if (this->params.useNack)
		{
			// If there is RTX just provide the NackGenerator with the packet.
			if (HasRtx())
			{
				this->nackGenerator->ReceivePacket(packet, /*isRecovered*/ false);
			}
			// If there is no RTX and NackGenerator returns true it means that it
			// was a NACKed packet.
			else if (this->nackGenerator->ReceivePacket(packet, /*isRecovered*/ false))
			{
				// Mark the packet as retransmitted and repaired.
				RTC::RtpStream::PacketRetransmitted(packet);
				RTC::RtpStream::PacketRepaired(packet);
			}
		}

		// Calculate Jitter.
		CalculateJitter(packet->GetTimestamp());

		// Increase transmission counter.
		this->transmissionCounter.Update(packet);

		// Increase media transmission counter.
		this->mediaTransmissionCounter.Update(packet);

		// Not inactive anymore.
		if (this->inactive)
		{
			this->inactive = false;

			ResetScore(10, /*notify*/ true);
		}

		// Restart the inactivityCheckPeriodicTimer.
		if (this->inactivityCheckPeriodicTimer)
		{
			this->inactivityCheckPeriodicTimer->Restart();
		}

		return true;
	}

	bool RtpStreamRecv::ReceiveRtxPacket(RTC::RtpPacket* packet)
	{
		using namespace chen;
		if (!this->params.useNack)
		{
			
			WARNING_EX_LOG( "NACK not supported");

			return false;
		}

		cassert(packet->GetSsrc() == this->params.rtxSsrc, "invalid ssrc on RTX packet");

		// Check that the payload type corresponds to the one negotiated.
		if (packet->GetPayloadType() != this->params.rtxPayloadType)
		{
			WARNING_EX_LOG( "ignoring RTX packet with invalid payload type [ssrc:%" PRIu32 ", seq:%" PRIu16
			  ", pt:%" PRIu8 "]",
			  packet->GetSsrc(),
			  packet->GetSequenceNumber(),
			  packet->GetPayloadType());

			return false;
		}

		if (HasRtx())
		{
			if (!this->rtxStream->ReceivePacket(packet))
			{
				WARNING_EX_LOG("rtx, RTX packet discarded");

				return false;
			}
		}

 
		// Get the RTX packet sequence number for logging purposes.
		auto rtxSeq = packet->GetSequenceNumber();
 

		// Get the original RTP packet.
		if (!packet->RtxDecode(this->params.payloadType, this->params.ssrc))
		{
			DEBUG_EX_LOG(
			  "ignoring empty RTX packet [ssrc:%" PRIu32 ", seq:%" PRIu16 ", pt:%" PRIu8 "]",
			  packet->GetSsrc(),
			  packet->GetSequenceNumber(),
			  packet->GetPayloadType());

			return false;
		}

		DEBUG_EX_LOG(
		  "received RTX packet [ssrc:%" PRIu32 ", seq:%" PRIu16 "] recovering original [ssrc:%" PRIu32
		  ", seq:%" PRIu16 "]",
		  this->params.rtxSsrc,
		  rtxSeq,
		  packet->GetSsrc(),
		  packet->GetSequenceNumber());

		// If not a valid packet ignore it.
		if (!RTC::RtpStream::UpdateSeq(packet))
		{
			WARNING_EX_LOG( "invalid RTX packet [ssrc:%" PRIu32 ", seq:%" PRIu16 "]",
			  packet->GetSsrc(),
			  packet->GetSequenceNumber());

			return false;
		}

		// Process the packet at codec level.
		if (packet->GetPayloadType() == GetPayloadType())
		{
			RTC::Codecs::Tools::ProcessRtpPacket(packet, GetMimeType());
		}

		// Mark the packet as retransmitted.
		RTC::RtpStream::PacketRetransmitted(packet);

		// Pass the packet to the NackGenerator and return true just if this was a
		// NACKed packet.
		if (this->nackGenerator->ReceivePacket(packet, /*isRecovered*/ true))
		{
			// Mark the packet as repaired.
			RTC::RtpStream::PacketRepaired(packet);

			// Increase transmission counter.
			this->transmissionCounter.Update(packet);

			// Not inactive anymore.
			if (this->inactive)
			{
				this->inactive = false;

				ResetScore(10, /*notify*/ true);
			}

			// Restart the inactivityCheckPeriodicTimer.
			if (this->inactivityCheckPeriodicTimer)
				this->inactivityCheckPeriodicTimer->Restart();

			return true;
		}

		return false;
	}

	RTC::RTCP::ReceiverReport* RtpStreamRecv::GetRtcpReceiverReport()
	{
		 
		uint8_t worstRemoteFractionLost{ 0 };

		if (this->params.useInBandFec)
		{
			// Notify the listener so we'll get the worst remote fraction lost.
			static_cast<RTC::RtpStreamRecv::Listener*>(this->listener)
			  ->OnRtpStreamNeedWorstRemoteFractionLost(this, worstRemoteFractionLost);

			if (worstRemoteFractionLost > 0)
				DEBUG_EX_LOG(  "rtcp using worst remote fraction lost:%" PRIu8, worstRemoteFractionLost);
		}

		auto* report = new RTC::RTCP::ReceiverReport();

		report->SetSsrc(GetSsrc());

		uint32_t prevPacketsLost = this->packetsLost;

		// Calculate Packets Expected and Lost.
		auto expected = GetExpectedPackets();

		if (expected > this->mediaTransmissionCounter.GetPacketCount())
			this->packetsLost = expected - this->mediaTransmissionCounter.GetPacketCount();
		else
			this->packetsLost = 0u;

		// Calculate Fraction Lost.
		uint32_t expectedInterval = expected - this->expectedPrior;

		this->expectedPrior = expected;

		uint32_t receivedInterval = this->mediaTransmissionCounter.GetPacketCount() - this->receivedPrior;

		this->receivedPrior = this->mediaTransmissionCounter.GetPacketCount();

		int32_t lostInterval = expectedInterval - receivedInterval;

		if (expectedInterval == 0 || lostInterval <= 0)
			this->fractionLost = 0;
		else
			this->fractionLost = std::round((static_cast<double>(lostInterval << 8) / expectedInterval));

		// Worst remote fraction lost is not worse than local one.
		if (worstRemoteFractionLost <= this->fractionLost)
		{
			this->reportedPacketLost += (this->packetsLost - prevPacketsLost);

			report->SetTotalLost(this->reportedPacketLost);
			report->SetFractionLost(this->fractionLost);
		}
		else
		{
			// Recalculate packetsLost.
			uint32_t newLostInterval = (worstRemoteFractionLost * expectedInterval) >> 8;

			this->reportedPacketLost += newLostInterval;

			report->SetTotalLost(this->reportedPacketLost);
			report->SetFractionLost(worstRemoteFractionLost);
		}

		// Fill the rest of the report.
		report->SetLastSeq(static_cast<uint32_t>(this->maxSeq) + this->cycles);
		report->SetJitter(this->jitter);

		if (this->lastSrReceived != 0)
		{
			// Get delay in milliseconds.
			auto delayMs = static_cast<uint32_t>(chen::uv_util::GetTimeMs() - this->lastSrReceived);
			// Express delay in units of 1/65536 seconds.
			uint32_t dlsr = (delayMs / 1000) << 16;

			dlsr |= uint32_t{ (delayMs % 1000) * 65536 / 1000 };

			report->SetDelaySinceLastSenderReport(dlsr);
			report->SetLastSenderReport(this->lastSrTimestamp);
		}
		else
		{
			report->SetDelaySinceLastSenderReport(0);
			report->SetLastSenderReport(0);
		}

		return report;
	}

	RTC::RTCP::ReceiverReport* RtpStreamRecv::GetRtxRtcpReceiverReport()
	{
		 
		if (HasRtx())
			return this->rtxStream->GetRtcpReceiverReport();

		return nullptr;
	}

	void RtpStreamRecv::ReceiveRtcpSenderReport(RTC::RTCP::SenderReport* report)
	{
		 
		this->lastSrReceived  = uv_util::GetTimeMs();
		this->lastSrTimestamp = report->GetNtpSec() << 16;
		this->lastSrTimestamp += report->GetNtpFrac() >> 16;

		// Update info about last Sender Report.
		rtc_time::Ntp ntp; // NOLINT(cppcoreguidelines-pro-type-member-init)

		ntp.seconds   = report->GetNtpSec();
		ntp.fractions = report->GetNtpFrac();

		this->lastSenderReportNtpMs = rtc_time::Ntp2TimeMs(ntp);
		this->lastSenderReporTs     = report->GetRtpTs();

		// Update the score with the current RR.
		UpdateScore();
	}

	void RtpStreamRecv::ReceiveRtxRtcpSenderReport(RTC::RTCP::SenderReport* report)
	{
		 
		if (HasRtx())
			this->rtxStream->ReceiveRtcpSenderReport(report);
	}

	void RtpStreamRecv::ReceiveRtcpXrDelaySinceLastRr(RTC::RTCP::DelaySinceLastRr::SsrcInfo* ssrcInfo)
	{
		 
		/* Calculate RTT. */

		// Get the NTP representation of the current timestamp.
		uint64_t nowMs = uv_util::GetTimeMs();
		auto ntp       = rtc_time::TimeMs2Ntp(nowMs);

		// Get the compact NTP representation of the current timestamp.
		uint32_t compactNtp = (ntp.seconds & 0x0000FFFF) << 16;

		compactNtp |= (ntp.fractions & 0xFFFF0000) >> 16;

		uint32_t lastRr = ssrcInfo->GetLastReceiverReport();
		uint32_t dlrr   = ssrcInfo->GetDelaySinceLastReceiverReport();

		// RTT in 1/2^16 second fractions.
		uint32_t rtt{ 0 };

		// If no Receiver Extended Report was received by the remote endpoint yet,
		// ignore lastRr and dlrr values in the Sender Extended Report.
		if (lastRr && dlrr && (compactNtp > dlrr + lastRr))
			rtt = compactNtp - dlrr - lastRr;

		// RTT in milliseconds.
		this->rtt = static_cast<float>(rtt >> 16) * 1000;
		this->rtt += (static_cast<float>(rtt & 0x0000FFFF) / 65536) * 1000;

		if (this->rtt > 0.0f)
			this->hasRtt = true;

		// Tell it to the NackGenerator.
		if (this->params.useNack)
			this->nackGenerator->UpdateRtt(static_cast<uint32_t>(this->rtt));
	}

	void RtpStreamRecv::RequestKeyFrame()
	{
		 
		///////////////////////////////////////////////////////////////////////////
		////                         IDR Request

	    //     关键帧也叫做即时刷新帧，简称IDR帧。对视频来说，IDR帧的解码无需参考之前的帧，因此在丢包严重时可以通过发送关键帧请求进行画面的恢复。
		// 关键帧的请求方式分为三种：RTCP FIR反馈（Full intra frame request）、RTCP PLI 反馈（Picture Loss Indictor）或SIP Info消息，
		//							具体使用哪种可通过协商确定.
 
		///////////////////////////////////////////////////////////////////////////
		if (this->params.usePli)
		{
			DEBUG_EX_LOG( "sending PLI [ssrc:%" PRIu32 "]", GetSsrc());

			// Sender SSRC should be 0 since there is no media sender involved, but
			// some implementations like gstreamer will fail to process it otherwise.
			RTC::RTCP::FeedbackPsPliPacket packet(GetSsrc(), GetSsrc());

			packet.Serialize(RTC::RTCP::Buffer);

			this->pliCount++;

			// Notify the listener.
			static_cast<RTC::RtpStreamRecv::Listener*>(this->listener)->OnRtpStreamSendRtcpPacket(this, &packet);
		}
		else if (this->params.useFir)
		{
			DEBUG_EX_LOG(  "sending FIR [ssrc:%" PRIu32 "]", GetSsrc());

			// Sender SSRC should be 0 since there is no media sender involved, but
			// some implementations like gstreamer will fail to process it otherwise.
			RTC::RTCP::FeedbackPsFirPacket packet(GetSsrc(), GetSsrc());
			auto* item = new RTC::RTCP::FeedbackPsFirItem(GetSsrc(), ++this->firSeqNumber);

			packet.AddItem(item);
			packet.Serialize(RTC::RTCP::Buffer);

			this->firCount++;

			// Notify the listener.
			static_cast<RTC::RtpStreamRecv::Listener*>(this->listener)->OnRtpStreamSendRtcpPacket(this, &packet);
		}
	}

	void RtpStreamRecv::Pause()
	{
		 
		if (this->inactivityCheckPeriodicTimer)
			this->inactivityCheckPeriodicTimer->Stop();

		if (this->params.useNack)
			this->nackGenerator->Reset();

		// Reset jitter.
		this->transit = 0;
		this->jitter  = 0;
	}

	void RtpStreamRecv::Resume()
	{
		 
		if (this->inactivityCheckPeriodicTimer && !this->inactive)
			this->inactivityCheckPeriodicTimer->Restart();
	}

	void RtpStreamRecv::CalculateJitter(uint32_t rtpTimestamp)
	{
		 
		if (this->params.clockRate == 0u)
			return;

		auto transit =
		  static_cast<int>(uv_util::GetTimeMs() - (rtpTimestamp * 1000 / this->params.clockRate));
		int d = transit - this->transit;

		// First transit calculation, save and return.
		if (this->transit == 0)
		{
			this->transit = transit;

			return;
		}

		this->transit = transit;

		if (d < 0)
			d = -d;

		this->jitter += (1. / 16.) * (static_cast<double>(d) - this->jitter);
	}

	void RtpStreamRecv::UpdateScore()
	{
		 
		// Calculate number of packets expected in this interval.
		auto totalExpected = GetExpectedPackets();
		uint32_t expected  = totalExpected - this->expectedPriorScore;

		this->expectedPriorScore = totalExpected;

		// Calculate number of packets received in this interval.
		auto totalReceived = this->mediaTransmissionCounter.GetPacketCount();
		uint32_t received  = totalReceived - this->receivedPriorScore;

		this->receivedPriorScore = totalReceived;

		// Calculate number of packets lost in this interval.
		uint32_t lost;

		if (expected < received)
			lost = 0;
		else
			lost = expected - received;

		// Calculate number of packets repaired in this interval.
		auto totalRepaired = this->packetsRepaired;
		uint32_t repaired  = totalRepaired - this->repairedPriorScore;

		this->repairedPriorScore = totalRepaired;

		// Calculate number of packets retransmitted in this interval.
		auto totatRetransmitted = this->packetsRetransmitted;
		uint32_t retransmitted  = totatRetransmitted - this->retransmittedPriorScore;

		this->retransmittedPriorScore = totatRetransmitted;

		if (this->inactive)
			return;

		// We didn't expect more packets to come.
		if (expected == 0)
		{
			RTC::RtpStream::UpdateScore(10);

			return;
		}

		if (lost > received)
			lost = received;

		if (repaired > lost)
		{
			if (HasRtx())
			{
				repaired = lost;
				retransmitted -= repaired - lost;
			}
			else
			{
				lost = repaired;
			}
		}

#if MS_LOG_DEV_LEVEL == 3
		MS_DEBUG_TAG(
		  score,
		  "[totalExpected:%" PRIu32 ", totalReceived:%zu, totalRepaired:%zu",
		  totalExpected,
		  totalReceived,
		  totalRepaired);

		MS_DEBUG_TAG(
		  score,
		  "fixed values [expected:%" PRIu32 ", received:%" PRIu32 ", lost:%" PRIu32
		  ", repaired:%" PRIu32 ", retransmitted:%" PRIu32,
		  expected,
		  received,
		  lost,
		  repaired,
		  retransmitted);
#endif

		auto repairedRatio  = static_cast<float>(repaired) / static_cast<float>(received);
		auto repairedWeight = std::pow(1 / (repairedRatio + 1), 4);

		cassert(retransmitted >= repaired, "repaired packets cannot be more than retransmitted ones");

		if (retransmitted > 0)
		{
			repairedWeight *= static_cast<float>(repaired) / retransmitted;
		}

		lost -= repaired * repairedWeight;

		auto deliveredRatio = static_cast<float>(received - lost) / static_cast<float>(received);
		auto score          = static_cast<uint8_t>(std::round(std::pow(deliveredRatio, 4) * 10));

#if MS_LOG_DEV_LEVEL == 3
		MS_DEBUG_TAG(
		  score,
		  "[deliveredRatio:%f, repairedRatio:%f, repairedWeight:%f, new lost:%" PRIu32 ", score:%" PRIu8
		  "]",
		  deliveredRatio,
		  repairedRatio,
		  repairedWeight,
		  lost,
		  score);
#endif

		RtpStream::UpdateScore(score);
	}

	inline void RtpStreamRecv::OnTimer(chen::ctimer* timer)
	{
		 
		if (timer == this->inactivityCheckPeriodicTimer)
		{
			this->inactive = true;

			if (GetScore() != 0)
			{
				WARNING_EX_LOG( "RTP inactivity detected, resetting score to 0 [ssrc:%" PRIu32 "]", GetSsrc());
			}

			ResetScore(0, /*notify*/ true);
		}
	}

	inline void RtpStreamRecv::OnNackGeneratorNackRequired(const std::vector<uint16_t>& seqNumbers)
	{
		 
		cassert(this->params.useNack, "NACK required but not supported");

		DEBUG_EX_LOG( "triggering NACK [ssrc:%" PRIu32 ", first seq:%" PRIu16 ", num packets:%zu]",
		  this->params.ssrc,
		  seqNumbers[0],
		  seqNumbers.size());

		RTC::RTCP::FeedbackRtpNackPacket packet(0, GetSsrc());

		auto it        = seqNumbers.begin();
		const auto end = seqNumbers.end();
		size_t numPacketsRequested{ 0 };

		while (it != end)
		{
			uint16_t seq;
			uint16_t bitmask{ 0 };

			seq = *it;
			++it;
			//////////////////////////////////////////////////////////////
			//[uint16_t ]	[uint16_t]
			//[seq]			[0000 0000 0000 0000 ]
			//////////////////////////////////////////////////////////////
			while (it != end)
			{
				uint16_t shift = *it - seq - 1;

				if (shift > 15)
				{
					break;
				}

				bitmask |= (1 << shift);
				++it;
			}

			auto* nackItem = new RTC::RTCP::FeedbackRtpNackItem(seq, bitmask);

			packet.AddItem(nackItem);

			numPacketsRequested += nackItem->CountRequestedPackets();
		}

		// Ensure that the RTCP packet fits into the RTCP buffer.
		if (packet.GetSize() > RTC::RTCP::BufferSize)
		{
			WARNING_EX_LOG( "rtx cannot send RTCP NACK packet, size too big (%zu bytes)", packet.GetSize());

			return;
		}
		// 发送feedback 个数
		this->nackCount++;
		// 发送一共nack个数
		this->nackPacketCount += numPacketsRequested;

		packet.Serialize(RTC::RTCP::Buffer);

		// Notify the listener.
		static_cast<RTC::RtpStreamRecv::Listener*>(this->listener)->OnRtpStreamSendRtcpPacket(this, &packet);
	}

	inline void RtpStreamRecv::OnNackGeneratorKeyFrameRequired()
	{
		 
		DEBUG_EX_LOG(  "requesting key frame [ssrc:%" PRIu32 "]", this->params.ssrc);

		RequestKeyFrame();
	}
} // namespace RTC
