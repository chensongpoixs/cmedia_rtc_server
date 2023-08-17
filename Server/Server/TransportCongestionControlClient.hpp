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
#ifndef MS_RTC_TRANSPORT_CONGESTION_CONTROL_CLIENT_HPP
#define MS_RTC_TRANSPORT_CONGESTION_CONTROL_CLIENT_HPP

//#include "common.hpp"
#include "BweType.hpp"
#include "FeedbackRtpTransport.hpp"
#include "ReceiverReport.hpp"
#include "RtpPacket.hpp"
#include "RtpProbationGenerator.hpp"
#include "TrendCalculator.hpp"
//#include "handles/Timer.hpp"
#include <libwebrtc/api/transport/goog_cc_factory.h>
#include <libwebrtc/api/transport/network_types.h>
#include <libwebrtc/call/rtp_transport_controller_send.h>
#include <libwebrtc/modules/pacing/packet_router.h>
#include "ctimer.h"
namespace RTC
{
	class TransportCongestionControlClient : public webrtc::PacketRouter,
		public webrtc::TargetTransferRateObserver/*, chen::ctimer::Listener */
	{
	public:
		struct Bitrates
		{
			uint32_t desiredBitrate{ 0u };
			uint32_t effectiveDesiredBitrate{ 0u };
			uint32_t minBitrate{ 0u };
			uint32_t maxBitrate{ 0u };
			uint32_t startBitrate{ 0u };
			uint32_t maxPaddingBitrate{ 0u };
			uint32_t availableBitrate{ 0u };
		};

	public:
		class Listener
		{
		public:
			virtual ~Listener() = default;

		public:
			virtual void OnTransportCongestionControlClientBitrates(
			  RTC::TransportCongestionControlClient* tccClient,
			  RTC::TransportCongestionControlClient::Bitrates& bitrates) = 0;
			virtual void OnTransportCongestionControlClientSendRtpPacket(
			  RTC::TransportCongestionControlClient* tccClient,
			  RTC::RtpPacket* packet,
			  const webrtc::PacedPacketInfo& pacingInfo) = 0;
		};

	public:
		TransportCongestionControlClient(
		  RTC::TransportCongestionControlClient::Listener* listener,
		  RTC::BweType bweType,
		  uint32_t initialAvailableBitrate,
		  uint32_t maxOutgoingBitrate);
		virtual ~TransportCongestionControlClient();

	public:
		RTC::BweType GetBweType() const
		{
			return this->bweType;
		}
		void TransportConnected();
		void TransportDisconnected();
		void InsertPacket(webrtc::RtpPacketSendInfo& packetInfo);
		webrtc::PacedPacketInfo GetPacingInfo();
		void PacketSent(webrtc::RtpPacketSendInfo& packetInfo, int64_t nowMs);
		void ReceiveEstimatedBitrate(uint32_t bitrate);
		void ReceiveRtcpReceiverReport(RTC::RTCP::ReceiverReportPacket* packet, float rtt, int64_t nowMs);
		void ReceiveRtcpTransportFeedback(const RTC::RTCP::FeedbackRtpTransportPacket* feedback);
		void SetDesiredBitrate(uint32_t desiredBitrate, bool force);
		void SetMaxOutgoingBitrate(uint32_t maxBitrate);
		const Bitrates& GetBitrates() const
		{
			return this->bitrates;
		}
		uint32_t GetAvailableBitrate() const;
		void RescheduleNextAvailableBitrateEvent();

	private:
		void MayEmitAvailableBitrateEvent(uint32_t previousAvailableBitrate);

		// jmillan: missing.
		// void OnRemoteNetworkEstimate(NetworkStateEstimate estimate) override;

		/* Pure virtual methods inherited from webrtc::TargetTransferRateObserver. */
	public:
		void OnTargetTransferRate(webrtc::TargetTransferRate targetTransferRate) override;

		/* Pure virtual methods inherited from webrtc::PacketRouter. */
	public:
		void SendPacket(RTC::RtpPacket* packet, const webrtc::PacedPacketInfo& pacingInfo) override;
		RTC::RtpPacket* GeneratePadding(size_t size) override;

		/* Pure virtual methods inherited from RTC::Timer. */
	public:
		void OnTimer(ctimer* timer) ;

	private:
		// Passed by argument.
		Listener* listener{ nullptr };
		// Allocated by this.
		webrtc::NetworkControllerFactoryInterface* controllerFactory{ nullptr };
		webrtc::RtpTransportControllerSend* rtpTransportControllerSend{ nullptr };
		RTC::RtpProbationGenerator* probationGenerator{ nullptr };
		//ctimer* processTimer{ nullptr };
		// Others.
		RTC::BweType bweType;
		uint32_t initialAvailableBitrate{ 0u };
		uint32_t maxOutgoingBitrate{ 0u };
		Bitrates bitrates;
		bool availableBitrateEventCalled{ false };
		uint64_t lastAvailableBitrateEventAtMs{ 0u };
		RTC::TrendCalculator desiredBitrateTrend;
		uint64				  m_next_timestamp; // 
		bool				  m_stoped;
	};
} // namespace RTC

#endif
