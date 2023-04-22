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
//#define MS_CLASS "RTC::TransportCongestionControlServer"
// #define MS_LOG_DEV_LEVEL 3

#include "TransportCongestionControlServer.hpp"
//#include "DepLibUV.hpp"
//#include "Logger.hpp"
#include "FeedbackPsRemb.hpp"
#include <iterator> // std::ostream_iterator
#include <sstream>  // std::ostringstream
#include "cuv_util.h"
#include "clog.h"

using namespace chen;
namespace RTC
{
	/* Static. */

	static constexpr uint64_t TransportCcFeedbackSendInterval{ 100u }; // In ms.
	static constexpr uint64_t LimitationRembInterval{ 1500u };         // In ms.
	static constexpr uint8_t UnlimitedRembNumPackets{ 4u };

	/* Instance methods. */

	TransportCongestionControlServer::TransportCongestionControlServer(
	  RTC::TransportCongestionControlServer::Listener* listener,
	  RTC::BweType bweType,
	  size_t maxRtcpPacketLen)
	  : listener(listener), bweType(bweType), maxRtcpPacketLen(maxRtcpPacketLen)
	{
		//MS_TRACE();

		switch (this->bweType)
		{
			case RTC::BweType::TRANSPORT_CC:
			{
				// Create a feedback packet.
				this->transportCcFeedbackPacket.reset(new RTC::RTCP::FeedbackRtpTransportPacket(0u, 0u));

				// Set initial packet count.
				this->transportCcFeedbackPacket->SetFeedbackPacketCount(this->transportCcFeedbackPacketCount);

				/*if (!init())
				{
					ERROR_EX_LOG("ctimer init failed !!!");
				}*/
				// Create the feedback send periodic timer.
				this->transportCcFeedbackSendPeriodicTimer = new chen::ctimer(this);

				break;
			}

			case RTC::BweType::REMB:
			{
				this->rembServer = new webrtc::RemoteBitrateEstimatorAbsSendTime(this);

				break;
			}
		}
	}

	TransportCongestionControlServer::~TransportCongestionControlServer()
	{
		//MS_TRACE();

		delete this->transportCcFeedbackSendPeriodicTimer;
		this->transportCcFeedbackSendPeriodicTimer = nullptr;

		// Delete REMB server.
		delete this->rembServer;
		this->rembServer = nullptr;
	}

	void TransportCongestionControlServer::TransportConnected()
	{
		//MS_TRACE();

		switch (this->bweType)
		{
			case RTC::BweType::TRANSPORT_CC:
			{
				this->transportCcFeedbackSendPeriodicTimer->Start(
				  TransportCcFeedbackSendInterval, TransportCcFeedbackSendInterval);

				break;
			}

			default:;
		}
	}

	void TransportCongestionControlServer::TransportDisconnected()
	{
		//MS_TRACE();

		switch (this->bweType)
		{
			case RTC::BweType::TRANSPORT_CC:
			{
				 this->transportCcFeedbackSendPeriodicTimer-> Stop();

				// Create a new feedback packet.
				this->transportCcFeedbackPacket.reset(new RTC::RTCP::FeedbackRtpTransportPacket(0u, 0u));

				break;
			}

			default:;
		}
	}

	void TransportCongestionControlServer::IncomingPacket(uint64_t nowMs, const RTC::RtpPacket* packet)
	{
		//MS_TRACE();

		switch (this->bweType)
		{
			case RTC::BweType::TRANSPORT_CC:
			{
				uint16_t wideSeqNumber;

				if (!packet->ReadTransportWideCc01(wideSeqNumber))
					break;

				// Update the RTCP media SSRC of the ongoing Transport-CC Feedback packet.
				this->transportCcFeedbackSenderSsrc = 0u;
				this->transportCcFeedbackMediaSsrc  = packet->GetSsrc();

				this->transportCcFeedbackPacket->SetSenderSsrc(0u);
				this->transportCcFeedbackPacket->SetMediaSsrc(packet->GetSsrc());

				// Provide the feedback packet with the RTP packet info. If it fails,
				// send current feedback and add the packet info to a new one.
				auto result =
				  this->transportCcFeedbackPacket->AddPacket(wideSeqNumber, nowMs, this->maxRtcpPacketLen);

				switch (result)
				{
					case RTC::RTCP::FeedbackRtpTransportPacket::AddPacketResult::SUCCESS:
					{
						// If the feedback packet is full, send it now.
						if (this->transportCcFeedbackPacket->IsFull())
						{
							DEBUG_EX_LOG("transport-cc feedback packet is full, sending feedback now");

							SendTransportCcFeedback();
						}

						break;
					}

					case RTC::RTCP::FeedbackRtpTransportPacket::AddPacketResult::MAX_SIZE_EXCEEDED:
					{
						// Send ongoing feedback packet and add the new packet info to the
						// regenerated one.
						SendTransportCcFeedback();

						this->transportCcFeedbackPacket->AddPacket(wideSeqNumber, nowMs, this->maxRtcpPacketLen);

						break;
					}

					case RTC::RTCP::FeedbackRtpTransportPacket::AddPacketResult::FATAL:
					{
						// Create a new feedback packet.
						this->transportCcFeedbackPacket.reset(new RTC::RTCP::FeedbackRtpTransportPacket(
						  this->transportCcFeedbackSenderSsrc, this->transportCcFeedbackMediaSsrc));

						// Use current packet count.
						// NOTE: Do not increment it since the previous ongoing feedback
						// packet was not sent.
						this->transportCcFeedbackPacket->SetFeedbackPacketCount(
						  this->transportCcFeedbackPacketCount);

						break;
					}
				}

				MaySendLimitationRembFeedback();

				break;
			}

			case RTC::BweType::REMB:
			{
				uint32_t absSendTime;

				if (!packet->ReadAbsSendTime(absSendTime))
					break;

				// NOTE: nowMs is uint64_t but we need to "convert" it to int64_t before
				// we give it to libwebrtc lib (althought this is implicit in the
				// conversion so it would be converted within the method call).
				auto nowMsInt64 = static_cast<int64_t>(nowMs);

				this->rembServer->IncomingPacket(nowMsInt64, packet->GetPayloadLength(), *packet, absSendTime);

				break;
			}
		}
	}

	void TransportCongestionControlServer::SetMaxIncomingBitrate(uint32_t bitrate)
	{
		//MS_TRACE();

		auto previousMaxIncomingBitrate = this->maxIncomingBitrate;

		this->maxIncomingBitrate = bitrate;

		if (previousMaxIncomingBitrate != 0u && this->maxIncomingBitrate == 0u)
		{
			// This is to ensure that we send N REMB packets with bitrate 0 (unlimited).
			this->unlimitedRembCounter = UnlimitedRembNumPackets;

			MaySendLimitationRembFeedback();
		}
	}

	inline void TransportCongestionControlServer::SendTransportCcFeedback()
	{
		//MS_TRACE();

		if (!this->transportCcFeedbackPacket->IsSerializable())
		{
			return;
		}

		auto latestWideSeqNumber = this->transportCcFeedbackPacket->GetLatestSequenceNumber();
		auto latestTimestamp     = this->transportCcFeedbackPacket->GetLatestTimestamp();

		// Notify the listener.
		this->listener->OnTransportCongestionControlServerSendRtcpPacket(
		  this, this->transportCcFeedbackPacket.get());

		// Create a new feedback packet.
		this->transportCcFeedbackPacket.reset(new RTC::RTCP::FeedbackRtpTransportPacket(
		  this->transportCcFeedbackSenderSsrc, this->transportCcFeedbackMediaSsrc));

		// Increment packet count.
		this->transportCcFeedbackPacket->SetFeedbackPacketCount(++this->transportCcFeedbackPacketCount);

		// Pass the latest packet info (if any) as pre base for the new feedback packet.
		if (latestTimestamp > 0u)
		{
			this->transportCcFeedbackPacket->AddPacket(
			  latestWideSeqNumber, latestTimestamp, this->maxRtcpPacketLen);
		}
	}

	inline void TransportCongestionControlServer::MaySendLimitationRembFeedback()
	{
		//MS_TRACE();

		auto nowMs = uv_util::GetTimeMs();

		// May fix unlimitedRembCounter.
		if (this->unlimitedRembCounter > 0u && this->maxIncomingBitrate != 0u)
			this->unlimitedRembCounter = 0u;

		// In case this is the first unlimited REMB packet, send it fast.
		// clang-format off
		if (
			(
				(this->bweType != RTC::BweType::REMB && this->maxIncomingBitrate != 0u) ||
				this->unlimitedRembCounter > 0u
			) &&
			(
				nowMs - this->limitationRembSentAtMs > LimitationRembInterval ||
				this->unlimitedRembCounter == UnlimitedRembNumPackets
			)
		)
		// clang-format on
		{
			DEBUG_EX_LOG(
			  "sending limitation RTCP REMB packet [bitrate:%u]", this->maxIncomingBitrate);

			RTC::RTCP::FeedbackPsRembPacket packet(0u, 0u);

			packet.SetBitrate(this->maxIncomingBitrate);
			packet.Serialize(RTC::RTCP::Buffer);

			// Notify the listener.
			this->listener->OnTransportCongestionControlServerSendRtcpPacket(this, &packet);

			this->limitationRembSentAtMs = nowMs;

			if (this->unlimitedRembCounter > 0u)
				this->unlimitedRembCounter--;
		}
	}

	inline void TransportCongestionControlServer::OnRembServerAvailableBitrate(
	  const webrtc::RemoteBitrateEstimator* /*rembServer*/,
	  const std::vector<uint32_t>& ssrcs,
	  uint32_t availableBitrate)
	{
		//MS_TRACE();

		// Limit announced bitrate if requested via API.
		if (this->maxIncomingBitrate != 0u)
			availableBitrate = std::min(availableBitrate, this->maxIncomingBitrate);

#if MS_LOG_DEV_LEVEL == 3
		std::ostringstream ssrcsStream;

		if (!ssrcs.empty())
		{
			std::copy(ssrcs.begin(), ssrcs.end() - 1, std::ostream_iterator<uint32_t>(ssrcsStream, ","));
			ssrcsStream << ssrcs.back();
		}

		MS_DEBUG_DEV(
		  "sending RTCP REMB packet [bitrate:%" PRIu32 ", ssrcs:%s]",
		  availableBitrate,
		  ssrcsStream.str().c_str());
#endif

		RTC::RTCP::FeedbackPsRembPacket packet(0u, 0u);

		packet.SetBitrate(availableBitrate);
		packet.SetSsrcs(ssrcs);
		packet.Serialize(RTC::RTCP::Buffer);

		// Notify the listener.
		this->listener->OnTransportCongestionControlServerSendRtcpPacket(this, &packet);
	}

	inline void TransportCongestionControlServer::OnTimer(chen::ctimer * timer)
	{
		//MS_TRACE();

		if (timer == transportCcFeedbackSendPeriodicTimer)
		{
			SendTransportCcFeedback();
		 }
	}
} // namespace RTC
