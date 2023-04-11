///***********************************************************************************************
//created: 		2023-01-15
//
//author:			chensong
//
//purpose:		rtc_connection
//
//
//************************************************************************************************/
//
//#ifndef _C_PRODUCER_H_
//#define _C_PRODUCER_H_
//#include "cnet_type.h"
//#include <sstream>
//#include <iostream>
//#include <vector>
//#include <map>
//#include "crtc_sdp.h"
//
//
//namespace chen {
//
//	class cproducer : public RTC::RtpStreamRecv::Listener, public RTC::KeyFrameRequestManager::Listener
//	{
//	public:
//		class Listener
//		{
//		public:
//			virtual ~Listener() = default;
//
//		public:
//			virtual void OnProducerPaused(RTC::Producer* producer) = 0;
//			virtual void OnProducerResumed(RTC::Producer* producer) = 0;
//			virtual void OnProducerNewRtpStream(
//				RTC::Producer* producer, RTC::RtpStream* rtpStream, uint32_t mappedSsrc) = 0;
//			virtual void OnProducerRtpStreamScore(
//				RTC::Producer* producer, RTC::RtpStream* rtpStream, uint8_t score, uint8_t previousScore) = 0;
//			virtual void OnProducerRtcpSenderReport(
//				RTC::Producer* producer, RTC::RtpStream* rtpStream, bool first) = 0;
//			virtual void OnProducerRtpPacketReceived(RTC::Producer* producer, RTC::RtpPacket* packet) = 0;
//			virtual void OnProducerSendRtcpPacket(RTC::Producer* producer, RTC::RTCP::Packet* packet) = 0;
//			virtual void OnProducerNeedWorstRemoteFractionLost(
//				RTC::Producer* producer, uint32_t mappedSsrc, uint8_t& worstRemoteFractionLost) = 0;
//		};
//	public:
//		cproducer(cproducer::Listener *listener);
//		virtual ~cproducer();
//
//	public:
//		RTC::Media::Kind GetKind() const
//		{
//			return this->kind;
//		}
//		const RTC::RtpParameters& GetRtpParameters() const
//		{
//			return this->rtpParameters;
//		}
//		/*const struct RTC::RtpHeaderExtensionIds& GetRtpHeaderExtensionIds() const
//		{
//			return this->rtpHeaderExtensionIds;
//		}*/
//		RTC::RtpParameters::Type GetType() const
//		{
//			return this->type;
//		}
//		bool IsPaused() const
//		{
//			return this->paused;
//		}
//		const std::map<RTC::RtpStreamRecv*, uint32_t>& GetRtpStreams()
//		{
//			return this->mapRtpStreamMappedSsrc;
//		}
//		const std::vector<uint8_t>* GetRtpStreamScores() const
//		{
//			return std::addressof(this->rtpStreamScores);
//		}
//		ReceiveRtpPacketResult ReceiveRtpPacket(RTC::RtpPacket* packet);
//		void ReceiveRtcpSenderReport(RTC::RTCP::SenderReport* report);
//		void ReceiveRtcpXrDelaySinceLastRr(RTC::RTCP::DelaySinceLastRr::SsrcInfo* ssrcInfo);
//		void GetRtcp(RTC::RTCP::CompoundPacket* packet, uint64_t nowMs);
//		void RequestKeyFrame(uint32_t mappedSsrc);
//	protected:
//	private:
//
//	};
//}
//#endif //_C_PRODUCER_H_