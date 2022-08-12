/***********************************************************************************************
created: 		2022-08-10

author:			chensong

purpose:		cwebrtc_transport
************************************************************************************************/

#include "cwebrtc_transport.h"
#include "ctimer.h"
#include "clog.h"
#include "csctp_association.h"
#include "ccrypto_random.h"
#include "BweType.hpp"
 
#include "FeedbackPs.hpp"
#include "FeedbackPsAfb.hpp"
#include "FeedbackPsRemb.hpp"
#include "FeedbackPsFir.hpp"
#include "FeedbackRtp.hpp"
#include "FeedbackRtpNack.hpp"
#include "FeedbackRtpTransport.hpp"
#include "XrDelaySinceLastRr.hpp"
#include "SenderReport.hpp"
#include "CompoundPacket.hpp"

namespace chen {
	cwebrtc_transport::cwebrtc_transport()
		: ctimer()
		, m_id("")
		, m_max_message_size(262144u)
		, m_sctp_association_ptr(NULL)
		, m_tcc_client_ptr(NULL)
		, m_tcc_server_ptr(NULL)
	/*	, m_rtcp_timer_ptr(NULL)*/
		, m_recvRtpHeaderExtensionIds()  
		, m_recvTransmission()
		, m_sendTransmission()
		, m_recvRtxTransmission(1000u)
		, m_sendRtxTransmission(1000u)
		, m_sendProbationTransmission(100u)
		, m_initialAvailableOutgoingBitrate(600000u)
	{
	}
	cwebrtc_transport::~cwebrtc_transport()
	{
	}
	bool cwebrtc_transport::init(const std::string & transport_id)
	{
		// {"OS":1024,"MIS":1024}
		static const size_t sctpSendBufferSize = 262144u; //default 262144u
		static const uint16 OS = 1024u;
		static const uint16 MIS = 1024u;


		// enable sctp
		m_sctp_association_ptr = new csctp_association(this, OS, MIS, m_max_message_size, sctpSendBufferSize);
		if (!m_sctp_association_ptr)
		{
			WARNING_EX_LOG("sctp association alloc failed !!!");
			return false;
		}
		 
		if (!ctimer::init())
		{
			WARNING_EX_LOG("ctimer init failed !!!");
			return false;
		}

		return true;
	}
	void cwebrtc_transport::destroy()
	{

		if (m_sctp_association_ptr)
		{
			delete m_sctp_association_ptr;
			m_sctp_association_ptr = NULL;
		}

		if (m_tcc_client_ptr)
		{
			delete m_tcc_client_ptr;
			m_tcc_client_ptr = NULL;
		}


		if (m_tcc_server_ptr)
		{
			delete m_tcc_server_ptr;
			m_tcc_server_ptr = NULL;
		}

	}
	void cwebrtc_transport::Connected()
	{
		// Tell all Consumers.
		//for (auto& kv : this->mapConsumers)
		//{
		//	auto* consumer = kv.second;

		//	consumer->TransportConnected();
		//}

		//// Tell all DataConsumers.
		//for (auto& kv : this->mapDataConsumers)
		//{
		//	auto* dataConsumer = kv.second;

		//	dataConsumer->TransportConnected();
		//}

		// Tell the SctpAssociation.
		if (m_sctp_association_ptr)
		{
			m_sctp_association_ptr->TransportConnected();
		}

		// Start the RTCP timer.
		/*this->rtcpTimer->*/Start(static_cast<uint64_t>(RTC::RTCP::MaxVideoIntervalMs / 2));

		// Tell the TransportCongestionControlClient.
		if (m_tcc_client_ptr)
		{
			m_tcc_client_ptr->TransportConnected();
		}

		// Tell the TransportCongestionControlServer.
		if (m_tcc_server_ptr)
		{
			m_tcc_server_ptr->TransportConnected();
		}

#ifdef ENABLE_RTC_SENDER_BANDWIDTH_ESTIMATOR
		// Tell the SenderBandwidthEstimator.
		if (this->senderBwe)
			this->senderBwe->TransportConnected();
#endif
	}
	void cwebrtc_transport::Disconnected()
	{
		// Tell all Consumers.
		/*for (auto& kv : this->mapConsumers)
		{
			auto* consumer = kv.second;

			consumer->TransportDisconnected();
		}*/

		// Tell all DataConsumers.
		/*for (auto& kv : this->mapDataConsumers)
		{
			auto* dataConsumer = kv.second;

			dataConsumer->TransportDisconnected();
		}*/

		// Stop the RTCP timer.
		/*this->rtcpTimer->*/Stop();

		// Tell the TransportCongestionControlClient.
		if (m_tcc_client_ptr)
		{
			m_tcc_client_ptr->TransportDisconnected();
		}

		// Tell the TransportCongestionControlServer.
		if (m_tcc_server_ptr)
		{
			m_tcc_server_ptr->TransportDisconnected();
		}

#ifdef ENABLE_RTC_SENDER_BANDWIDTH_ESTIMATOR
		// Tell the SenderBandwidthEstimator.
		if (this->senderBwe)
			this->senderBwe->TransportDisconnected();
#endif
	}
	void cwebrtc_transport::ReceiveRtpPacket(RTC::RtpPacket * packet)
	{
		// Apply the Transport RTP header extension ids so the RTP listener can use them.
		packet->SetMidExtensionId(m_recvRtpHeaderExtensionIds.mid);
		packet->SetRidExtensionId(m_recvRtpHeaderExtensionIds.rid);
		packet->SetRepairedRidExtensionId(m_recvRtpHeaderExtensionIds.rrid);
		packet->SetAbsSendTimeExtensionId(m_recvRtpHeaderExtensionIds.absSendTime);
		packet->SetTransportWideCc01ExtensionId(m_recvRtpHeaderExtensionIds.transportWideCc01);

		auto nowMs = uv_util::GetTimeMs();

		// Feed the TransportCongestionControlServer.
		if (m_tcc_server_ptr)
		{
			m_tcc_server_ptr->IncomingPacket(nowMs, packet);
		}

		// Get the associated Producer.
		// TODO@chensong 20220811
		//RTC::Producer* producer = this->rtpListener.GetProducer(packet);

		//if (!producer)
		//{
		//	MS_WARN_TAG(
		//		rtp,
		//		"no suitable Producer for received RTP packet [ssrc:%" PRIu32 ", payloadType:%" PRIu8 "]",
		//		packet->GetSsrc(),
		//		packet->GetPayloadType());

		//	// Tell the child class to remove this SSRC.
		//	RecvStreamClosed(packet->GetSsrc());

		//	delete packet;

		//	return;
		//}

		// MS_DEBUG_DEV(
		//   "RTP packet received [ssrc:%" PRIu32 ", payloadType:%" PRIu8 ", producerId:%s]",
		//   packet->GetSsrc(),
		//   packet->GetPayloadType(),
		//   producer->id.c_str());

		// Pass the RTP packet to the corresponding Producer.
		//auto result = producer->ReceiveRtpPacket(packet);

		//switch (result)
		//{
		//case RTC::Producer::ReceiveRtpPacketResult::MEDIA:
		//	this->recvRtpTransmission.Update(packet);
		//	break;
		//case RTC::Producer::ReceiveRtpPacketResult::RETRANSMISSION:
		//	this->recvRtxTransmission.Update(packet);
		//	break;
		//case RTC::Producer::ReceiveRtpPacketResult::DISCARDED:
		//	// Tell the child class to remove this SSRC.
		//	RecvStreamClosed(packet->GetSsrc());
		//	break;
		//default:;
		//}

		delete packet;
	}
	void cwebrtc_transport::ReceiveRtcpPacket(RTC::RTCP::Packet * packet)
	{
		// Handle each RTCP packet.
		while (packet)
		{
			HandleRtcpPacket(packet);

			auto* previousPacket = packet;

			packet = packet->GetNext();

			delete previousPacket;
		}
	}
	void cwebrtc_transport::ReceiveSctpData(const uint8_t * data, size_t len)
	{
		if (!m_sctp_association_ptr)
		{
			DEBUG_EX_LOG("sctp, ignoring SCTP packet (SCTP not enabled)");

			return;
		}

		// Pass it to the SctpAssociation.
		m_sctp_association_ptr->ProcessSctpData(data, len);
	}
	void cwebrtc_transport::OnSctpAssociationConnecting(csctp_association * sctpAssociation)
	{
		// Notify the Node Transport.
		/*json data = json::object();

		data["sctpState"] = "connecting";

		Channel::ChannelNotifier::Emit(this->id, "sctpstatechange", data);*/
	}
	void cwebrtc_transport::OnSctpAssociationConnected(csctp_association * sctpAssociation)
	{
		// Tell all DataConsumers.
		/*for (auto& kv : this->mapDataConsumers)
		{
			auto* dataConsumer = kv.second;

			if (dataConsumer->GetType() == RTC::DataConsumer::Type::SCTP)
			{
				dataConsumer->SctpAssociationConnected();
			}
		}*/

		// Notify the Node Transport.
		/*json data = json::object();

		data["sctpState"] = "connected";

		Channel::ChannelNotifier::Emit(this->id, "sctpstatechange", data);*/
	}
	void cwebrtc_transport::OnSctpAssociationFailed(csctp_association * sctpAssociation)
	{
		// Tell all DataConsumers.
		/*for (auto& kv : this->mapDataConsumers)
		{
			auto* dataConsumer = kv.second;

			if (dataConsumer->GetType() == RTC::DataConsumer::Type::SCTP)
			{
				dataConsumer->SctpAssociationClosed();
			}
		}*/

		// Notify the Node Transport.
		/*json data = json::object();

		data["sctpState"] = "failed";

		Channel::ChannelNotifier::Emit(this->id, "sctpstatechange", data);*/
	}
	void cwebrtc_transport::OnSctpAssociationClosed(csctp_association * sctpAssociation)
	{
		// Tell all DataConsumers.
		/*for (auto& kv : this->mapDataConsumers)
		{
			auto* dataConsumer = kv.second;

			if (dataConsumer->GetType() == RTC::DataConsumer::Type::SCTP)
			{
				dataConsumer->SctpAssociationClosed();
			}
		}*/

		// Notify the Node Transport.
		/*json data = json::object();

		data["sctpState"] = "closed";

		Channel::ChannelNotifier::Emit(this->id, "sctpstatechange", data);*/
	}
	void cwebrtc_transport::OnSctpAssociationSendData(csctp_association * sctpAssociation, const uint8_t * data, size_t len)
	{
		// Ignore if destroying.
		// NOTE: This is because when the child class (i.e. WebRtcTransport) is deleted,
		// its destructor is called first and then the parent Transport's destructor,
		// and we would end here calling SendSctpData() which is an abstract method.
		// TODO@chensong 20220811 情况比较特殊 可能会崩溃问题？？ 要主要哦
		//if (this->destroying)
		//	return;

		if (m_sctp_association_ptr)
		{
			// TODO@chensong 20220811  增加SCTP data的发送
			//SendSctpData(data, len);
		}
	}
	void cwebrtc_transport::OnSctpAssociationMessageReceived(csctp_association * sctpAssociation, uint16_t streamId, uint32_t ppid, const uint8_t * msg, size_t len)
	{
		//RTC::DataProducer* dataProducer = this->sctpListener.GetDataProducer(streamId);

		/*if (!dataProducer)
		{
			MS_WARN_TAG(
				sctp, "no suitable DataProducer for received SCTP message [streamId:%" PRIu16 "]", streamId);

			return;
		}
*/
		// Pass the SCTP message to the corresponding DataProducer.
		//try
		//{
		//	dataProducer->ReceiveMessage(ppid, msg, len);
		//}
		//catch (std::exception& error)
		//{
		//	// Nothing to do.
		//}
	}
	void cwebrtc_transport::OnSctpAssociationBufferedAmount(csctp_association * sctpAssociation, uint32_t bufferedAmount)
	{
		/*for (const auto& kv : this->mapDataConsumers)
		{
			auto* dataConsumer = kv.second;

			if (dataConsumer->GetType() == RTC::DataConsumer::Type::SCTP)
				dataConsumer->SctpAssociationBufferedAmount(bufferedAmount);
		}*/
	}
	void cwebrtc_transport::OnTransportCongestionControlClientBitrates(RTC::TransportCongestionControlClient * tccClient, RTC::TransportCongestionControlClient::Bitrates & bitrates)
	{
		DEBUG_EX_LOG("outgoing available bitrate:%u"  , bitrates.availableBitrate);
		// TODO@chensong 20220811 
		//DistributeAvailableOutgoingBitrate();
		//ComputeOutgoingDesiredBitrate();

		// May emit 'trace' event.
		//EmitTraceEventBweType(bitrates); 
	}
	void cwebrtc_transport::OnTransportCongestionControlClientSendRtpPacket(RTC::TransportCongestionControlClient * tccClient, RTC::RtpPacket * packet, const webrtc::PacedPacketInfo & pacingInfo)
	{
		// Update abs-send-time if present.
		packet->UpdateAbsSendTime(uv_util::GetTimeMs());

		// Update transport wide sequence number if present.
		// clang-format off
		if (
			m_tcc_client_ptr->GetBweType() == RTC::BweType::TRANSPORT_CC &&
			packet->UpdateTransportWideCc01(m_transportWideCcSeq + 1)
			)
			// clang-format on
		{
			m_transportWideCcSeq++;

			// May emit 'trace' event
			// TODO@chensong 
			//EmitTraceEventProbationType(packet);

			webrtc::RtpPacketSendInfo packetInfo;

			packetInfo.ssrc = packet->GetSsrc();
			packetInfo.transport_sequence_number = m_transportWideCcSeq;
			packetInfo.has_rtp_sequence_number = true;
			packetInfo.rtp_sequence_number = packet->GetSequenceNumber();
			packetInfo.length = packet->GetSize();
			packetInfo.pacing_info = pacingInfo;

			// Indicate the pacer (and prober) that a packet is to be sent.
			m_tcc_client_ptr->InsertPacket(packetInfo);

#ifdef ENABLE_RTC_SENDER_BANDWIDTH_ESTIMATOR
			auto* senderBwe = this->senderBwe;
			RTC::SenderBandwidthEstimator::SentInfo sentInfo;

			sentInfo.wideSeq = this->transportWideCcSeq;
			sentInfo.size = packet->GetSize();
			sentInfo.isProbation = true;
			sentInfo.sendingAtMs = DepLibUV::GetTimeMs();

			auto* cb = new onSendCallback([tccClient, &packetInfo, senderBwe, &sentInfo](bool sent) {
				if (sent)
				{
					tccClient->PacketSent(packetInfo, DepLibUV::GetTimeMsInt64());

					sentInfo.sentAtMs = DepLibUV::GetTimeMs();

					senderBwe->RtpPacketSent(sentInfo);
				}
			});

			SendRtpPacket(nullptr, packet, cb);
#else
			// TODO@chensong 20220811
			 /*const  auto* cb = new onSendCallback([m_tcc_client_ptr, &packetInfo](bool sent) {
				if (sent)
				{
					m_tcc_client_ptr->PacketSent(packetInfo, uv_util::GetTimeMsInt64());
				}
			});*/

			//SendRtpPacket(nullptr, packet, cb);
#endif
		}
		else
		{
			// May emit 'trace' event.
			//EmitTraceEventProbationType(packet);

			//SendRtpPacket(nullptr, packet);
		}

		m_sendProbationTransmission.Update(packet);

		DEBUG_EX_LOG( "probation sent [seq:%hu, wideSeq:%hu, size:%zu, bitrate:%u]",
			packet->GetSequenceNumber(),
			m_transportWideCcSeq,
			packet->GetSize(),
			m_sendProbationTransmission.GetBitrate(uv_util::GetTimeMs()));
	}
	void cwebrtc_transport::OnTransportCongestionControlServerSendRtcpPacket(RTC::TransportCongestionControlServer * tccServer, RTC::RTCP::Packet * packet)
	{
		packet->Serialize(RTC::RTCP::Buffer);
		// TODO@chensong 20220811 
		//SendRtcpPacket(packet);
	}
	void cwebrtc_transport::OnTimer()
	{
		// RTCP timer.
		//if (timer == this->rtcpTimer)
		{
			auto interval = static_cast<uint64_t>(RTC::RTCP::MaxVideoIntervalMs);
			uint64_t nowMs = uv_util::GetTimeMs();
			// TODO@chensong 20220811 
		//	SendRtcp(nowMs);

			// Recalculate next RTCP interval.
			//if (!this->mapConsumers.empty())
			//{
			//	// Transmission rate in kbps.
			//	uint32_t rate{ 0 };

			//	// Get the RTP sending rate.
			//	for (auto& kv : this->mapConsumers)
			//	{
			//		auto* consumer = kv.second;

			//		rate += consumer->GetTransmissionRate(nowMs) / 1000;
			//	}

			//	// Calculate bandwidth: 360 / transmission bandwidth in kbit/s.
			//	if (rate != 0u)
			//		interval = 360000 / rate;

			//	if (interval > RTC::RTCP::MaxVideoIntervalMs)
			//		interval = RTC::RTCP::MaxVideoIntervalMs;
			//}

			/*
			 * The interval between RTCP packets is varied randomly over the range
			 * [0.5,1.5] times the calculated interval to avoid unintended synchronization
			 * of all participants.
			 */

			//s_crypto_random.GetRandomUInt(1, 1);
			interval *= static_cast<float>(s_crypto_random.GetRandomUInt(5, 15)) / 10;

			/*this->rtcpTimer->*/Start(interval);
		}
	}
	void cwebrtc_transport::HandleRtcpPacket(RTC::RTCP::Packet * packet)
	{
		switch (packet->GetType())
		{
		case RTC::RTCP::Type::RR:
		{
			auto* rr = static_cast<RTC::RTCP::ReceiverReportPacket*>(packet);

			for (auto it = rr->Begin(); it != rr->End(); ++it)
			{
				auto& report = *it;
				//auto* consumer = GetConsumerByMediaSsrc(report->GetSsrc());

				//if (!consumer)
				//{
				//	// Special case for the RTP probator.
				//	if (report->GetSsrc() == RTC::RtpProbationSsrc)
				//	{
				//		continue;
				//	}

				//	DEBUG_EX_LOG("rtcp, no Consumer found for received Receiver Report [ssrc:%u]",
				//		report->GetSsrc());

				//	continue;
				//}

				//consumer->ReceiveRtcpReceiverReport(report);
			}

			//if (this->tccClient && !this->mapConsumers.empty())
			//{
			//	float rtt = 0;

			//	// Retrieve the RTT from the first active consumer.
			//	/*for (auto& kv : this->mapConsumers)
			//	{
			//		auto* consumer = kv.second;

			//		if (consumer->IsActive())
			//		{
			//			rtt = consumer->GetRtt();

			//			break;
			//		}
			//	}*/

			//	m_tcc_client_ptr->ReceiveRtcpReceiverReport(rr, rtt, uv_util::GetTimeMsInt64());
			//}

			break;
		}

		case RTC::RTCP::Type::PSFB:
		{
			auto* feedback = static_cast<RTC::RTCP::FeedbackPsPacket*>(packet);

			switch (feedback->GetMessageType())
			{
			case RTC::RTCP::FeedbackPs::MessageType::PLI:
			{
				//auto* consumer = GetConsumerByMediaSsrc(feedback->GetMediaSsrc());

				if (feedback->GetMediaSsrc() == RTC::RtpProbationSsrc)
				{
					break;
				}
				/*else if (!consumer)
				{
					DEBUG_EX_LOG(" rtcp, no Consumer found for received PLI Feedback packet  [sender ssrc:%u, media ssrc:%u]",
						feedback->GetSenderSsrc(),
						feedback->GetMediaSsrc());

					break;
				}*/

				DEBUG_EX_LOG(" rtcp, PLI received, requesting key frame for Consumer  [sender ssrc:%u, media ssrc:%u]",
					feedback->GetSenderSsrc(),
					feedback->GetMediaSsrc());

				/*consumer->ReceiveKeyFrameRequest(
					RTC::RTCP::FeedbackPs::MessageType::PLI, feedback->GetMediaSsrc());*/

				break;
			}

			case RTC::RTCP::FeedbackPs::MessageType::FIR:
			{
				// Must iterate FIR items.
				auto* fir = static_cast<RTC::RTCP::FeedbackPsFirPacket*>(packet);

				for (auto it = fir->Begin(); it != fir->End(); ++it)
				{
					auto& item = *it;
					//auto* consumer = GetConsumerByMediaSsrc(item->GetSsrc());

					if (item->GetSsrc() == RTC::RtpProbationSsrc)
					{
						continue;
					}
					/*else if (!consumer)
					{
						DEBUG_EX_LOG(" rtcp, no Consumer found for received FIR Feedback packet  [sender ssrc:%u , media ssrc:%u , item ssrc:%u ]",
							feedback->GetSenderSsrc(),
							feedback->GetMediaSsrc(),
							item->GetSsrc());

						continue;
					}*/

					DEBUG_EX_LOG("rtcp, FIR received, requesting key frame for Consumer  [sender ssrc:%u, media ssrc:%u, item ssrc:%u]",
						feedback->GetSenderSsrc(),
						feedback->GetMediaSsrc(),
						item->GetSsrc());

					//consumer->ReceiveKeyFrameRequest(feedback->GetMessageType(), item->GetSsrc());
				}

				break;
			}

			case RTC::RTCP::FeedbackPs::MessageType::AFB:
			{
				auto* afb = static_cast<RTC::RTCP::FeedbackPsAfbPacket*>(feedback);

				// Store REMB info.
				if (afb->GetApplication() == RTC::RTCP::FeedbackPsAfbPacket::Application::REMB)
				{
					auto* remb = static_cast<RTC::RTCP::FeedbackPsRembPacket*>(afb);

					// Pass it to the TCC client.
					// clang-format off
					if (
						m_tcc_client_ptr &&
						m_tcc_client_ptr->GetBweType() == RTC::BweType::REMB
						)
						// clang-format on
					{
						m_tcc_client_ptr->ReceiveEstimatedBitrate(remb->GetBitrate());
					}

					break;
				}
				else
				{
					DEBUG_EX_LOG("rtcp, ignoring unsupported %s Feedback PS AFB packet  [sender ssrc:%u, media ssrc:%u]",
						RTC::RTCP::FeedbackPsPacket::MessageType2String(feedback->GetMessageType()).c_str(),
						feedback->GetSenderSsrc(),
						feedback->GetMediaSsrc());

					break;
				}
			}

			default:
			{
				DEBUG_EX_LOG(" rtcp, ignoring unsupported %s Feedback packet  [sender ssrc:%u , media ssrc:%u]",
					RTC::RTCP::FeedbackPsPacket::MessageType2String(feedback->GetMessageType()).c_str(),
					feedback->GetSenderSsrc(),
					feedback->GetMediaSsrc());
			}
			}

			break;
		}

		case RTC::RTCP::Type::RTPFB:
		{
			auto* feedback = static_cast<RTC::RTCP::FeedbackRtpPacket*>(packet);
			//auto* consumer = GetConsumerByMediaSsrc(feedback->GetMediaSsrc());

			// If no Consumer is found and this is not a Transport Feedback for the
			// probation SSRC or any Consumer RTX SSRC, ignore it.
			//
			// clang-format off
			//if (
			//	!consumer &&
			//	feedback->GetMessageType() != RTC::RTCP::FeedbackRtp::MessageType::TCC &&
			//	(
			//		feedback->GetMediaSsrc() != RTC::RtpProbationSsrc ||
			//		!GetConsumerByRtxSsrc(feedback->GetMediaSsrc())
			//		)
			//	)
			//	// clang-format on
			//{
			//	DEBUG_EX_LOG("rtcp, no Consumer found for received Feedback packet  [sender ssrc:%u, media ssrc:%u]",
			//		feedback->GetSenderSsrc(),
			//		feedback->GetMediaSsrc());

			//	break;
			//}

			switch (feedback->GetMessageType())
			{
			case RTC::RTCP::FeedbackRtp::MessageType::NACK:
			{
				/*if (!consumer)
				{
					DEBUG_EX_LOG("rtcp, no Consumer found for received NACK Feedback packet  [sender ssrc:%u, media ssrc:%u]",
						feedback->GetSenderSsrc(),
						feedback->GetMediaSsrc());

					break;
				}*/

				auto* nackPacket = static_cast<RTC::RTCP::FeedbackRtpNackPacket*>(packet);

				//consumer->ReceiveNack(nackPacket);

				break;
			}

			case RTC::RTCP::FeedbackRtp::MessageType::TCC:
			{
				auto* feedback = static_cast<RTC::RTCP::FeedbackRtpTransportPacket*>(packet);

				if (m_tcc_client_ptr)
				{
					m_tcc_client_ptr->ReceiveRtcpTransportFeedback(feedback);
				}

#ifdef ENABLE_RTC_SENDER_BANDWIDTH_ESTIMATOR
				// Pass it to the SenderBandwidthEstimator client.
				if (this->senderBwe)
					this->senderBwe->ReceiveRtcpTransportFeedback(feedback);
#endif

				break;
			}

			default:
			{
				DEBUG_EX_LOG("rtcp, ignoring unsupported %s Feedback packet  [sender ssrc:%u, media ssrc:%u]",
					RTC::RTCP::FeedbackRtpPacket::MessageType2String(feedback->GetMessageType()).c_str(),
					feedback->GetSenderSsrc(),
					feedback->GetMediaSsrc());
			}
			}

			break;
		}

		case RTC::RTCP::Type::SR:
		{
			auto* sr = static_cast<RTC::RTCP::SenderReportPacket*>(packet);

			// Even if Sender Report packet can only contains one report.
			/*for (auto it = sr->Begin(); it != sr->End(); ++it)
			{
				auto& report = *it;
				auto* producer = this->rtpListener.GetProducer(report->GetSsrc());

				if (!producer)
				{
					DEBUG_EX_LOG("rtcp, no Producer found for received Sender Report [ssrc:%u]",
						report->GetSsrc());

					continue;
				}

				producer->ReceiveRtcpSenderReport(report);
			}*/

			break;
		}

		case RTC::RTCP::Type::SDES:
		{
			// According to RFC 3550 section 6.1 "a CNAME item MUST be included in
			// in each compound RTCP packet". So this is true even for compound
			// packets sent by endpoints that are not sending any RTP stream to us
			// (thus chunks in such a SDES will have an SSCR does not match with
			// any Producer created in this Transport).
			// Therefore, and given that we do nothing with SDES, just ignore them.

			break;
		}

		case RTC::RTCP::Type::BYE:
		{
			DEBUG_EX_LOG("rtcp, ignoring received RTCP BYE");

			break;
		}

		case RTC::RTCP::Type::XR:
		{
			auto* xr = static_cast<RTC::RTCP::ExtendedReportPacket*>(packet);

			for (auto it = xr->Begin(); it != xr->End(); ++it)
			{
				auto& report = *it;

				switch (report->GetType())
				{
				case RTC::RTCP::ExtendedReportBlock::Type::DLRR:
				{
					auto* dlrr = static_cast<RTC::RTCP::DelaySinceLastRr*>(report);

					for (auto it2 = dlrr->Begin(); it2 != dlrr->End(); ++it2)
					{
						auto& ssrcInfo = *it2;

						// SSRC should be filled in the sub-block.
						if (ssrcInfo->GetSsrc() == 0)
						{
							ssrcInfo->SetSsrc(xr->GetSsrc());
						}

						/*auto* producer = this->rtpListener.GetProducer(ssrcInfo->GetSsrc());

						if (!producer)
						{
							DEBUG_EX_LOG("rtcp,  no Producer found for received Sender Extended Report [ssrc:%u]",
								ssrcInfo->GetSsrc());

							continue;
						}

						producer->ReceiveRtcpXrDelaySinceLastRr(ssrcInfo);*/
					}

					break;
				}

				default:;
				}
			}

			break;
		}

		default:
		{
			DEBUG_EX_LOG("rtcp, unhandled RTCP type received [type:%hhu]",
				static_cast<uint8_t>(packet->GetType()));
		}
		}
	}
	void cwebrtc_transport::SendRtcp(uint64_t nowMs)
	{
		std::unique_ptr<RTC::RTCP::CompoundPacket> packet{ nullptr };

		//for (auto& kv : this->mapConsumers)
		//{
		//	auto* consumer = kv.second;

		//	for (auto* rtpStream : consumer->GetRtpStreams())
		//	{
		//		// Reset the Compound packet.
		//		packet.reset(new RTC::RTCP::CompoundPacket());

		//		consumer->GetRtcp(packet.get(), rtpStream, nowMs);

		//		// Send the RTCP compound packet if there is a sender report.
		//		if (packet->HasSenderReport())
		//		{
		//			packet->Serialize(RTC::RTCP::Buffer);
		//			SendRtcpCompoundPacket(packet.get());
		//		}
		//	}
		//}

		// Reset the Compound packet.
		packet.reset(new RTC::RTCP::CompoundPacket());

		//for (auto& kv : this->mapProducers)
		//{
		//	auto* producer = kv.second;

		//	producer->GetRtcp(packet.get(), nowMs);

		//	// One more RR would exceed the MTU, send the compound packet now.
		//	if (packet->GetSize() + sizeof(RTCP::ReceiverReport::Header) > RTC::MtuSize)
		//	{
		//		packet->Serialize(RTC::RTCP::Buffer);
		//		SendRtcpCompoundPacket(packet.get());

		//		// Reset the Compound packet.
		//		packet.reset(new RTC::RTCP::CompoundPacket());
		//	}
		//}

		if (packet->GetReceiverReportCount() != 0u)
		{
			packet->Serialize(RTC::RTCP::Buffer);
			//SendRtcpCompoundPacket(packet.get());
		}
	}
	void cwebrtc_transport::DistributeAvailableOutgoingBitrate()
	{
		//std::multimap<uint8_t, RTC::Consumer*> multimapPriorityConsumer;

		// Fill the map with Consumers and their priority (if > 0).
		/*for (auto& kv : this->mapConsumers)
		{
			auto* consumer = kv.second;
			auto priority = consumer->GetBitratePriority();

			if (priority > 0u)
				multimapPriorityConsumer.emplace(priority, consumer);
		}*/

		// Nobody wants bitrate. Exit.
		/*if (multimapPriorityConsumer.empty())
		{
			return;
		}*/

		uint32_t availableBitrate = m_tcc_client_ptr->GetAvailableBitrate();

		m_tcc_client_ptr->RescheduleNextAvailableBitrateEvent();

		DEBUG_EX_LOG("before layer-by-layer iterations [availableBitrate:%u]", availableBitrate);

		// Redistribute the available bitrate by allowing Consumers to increase
		// layer by layer. Take into account the priority of each Consumer to
		// provide it with more bitrate.
		while (availableBitrate > 0u)
		{
			auto previousAvailableBitrate = availableBitrate;

			//for (auto it = multimapPriorityConsumer.rbegin(); it != multimapPriorityConsumer.rend(); ++it)
			//{
			//	auto priority = it->first;
			//	auto* consumer = it->second;
			//	auto bweType = m_tcc_client_ptr->GetBweType();

			//	// If a Consumer has priority > 1, call IncreaseLayer() more times to
			//	// provide it with more available bitrate to choose its preferred layers.
			//	for (uint8_t i{ 1u }; i <= priority; ++i)
			//	{
			//		uint32_t usedBitrate{ 0u };

			//		switch (bweType)
			//		{
			//		case RTC::BweType::TRANSPORT_CC:
			//			usedBitrate = consumer->IncreaseLayer(availableBitrate, /*considerLoss*/ false);
			//			break;
			//		case RTC::BweType::REMB:
			//			usedBitrate = consumer->IncreaseLayer(availableBitrate, /*considerLoss*/ true);
			//			break;
			//		}

			//		//MS_ASSERT(usedBitrate <= availableBitrate, "Consumer used more layer bitrate than given");

			//		availableBitrate -= usedBitrate;

			//		// Exit the loop fast if used bitrate is 0.
			//		if (usedBitrate == 0u)
			//			break;
			//	}
			//}

			// If no Consumer used bitrate, exit the loop.
			if (availableBitrate == previousAvailableBitrate)
			{
				break;
			}
		}

		DEBUG_EX_LOG("after layer-by-layer iterations [availableBitrate:%u]", availableBitrate);

		// Finally instruct Consumers to apply their computed layers.
		/*for (auto it = multimapPriorityConsumer.rbegin(); it != multimapPriorityConsumer.rend(); ++it)
		{
			auto* consumer = it->second;

			consumer->ApplyLayers();
		}*/
	}
	void cwebrtc_transport::ComputeOutgoingDesiredBitrate(bool forceBitrate)
	{
		//MS_ASSERT(this->tccClient, "no TransportCongestionClient");

		uint32_t totalDesiredBitrate{ 0u };

		/*for (auto& kv : this->mapConsumers)
		{
			auto* consumer = kv.second;
			auto desiredBitrate = consumer->GetDesiredBitrate();

			totalDesiredBitrate += desiredBitrate;
		}*/

		DEBUG_EX_LOG("total desired bitrate: %u" , totalDesiredBitrate);

		m_tcc_client_ptr->SetDesiredBitrate(totalDesiredBitrate, forceBitrate);
	}
	void cwebrtc_transport::EmitTraceEventProbationType(RTC::RtpPacket * packet) const
	{
		/*if (!this->traceEventTypes.probation)
			return;

		json data = json::object();

		data["type"] = "probation";
		data["timestamp"] = DepLibUV::GetTimeMs();
		data["direction"] = "out";

		packet->FillJson(data["info"]);

		Channel::ChannelNotifier::Emit(this->id, "trace", data);*/
	}
	void cwebrtc_transport::EmitTraceEventBweType(RTC::TransportCongestionControlClient::Bitrates & bitrates) const
	{
		/*if (!this->traceEventTypes.bwe)
			return;

		json data = json::object();

		data["type"] = "bwe";
		data["timestamp"] = DepLibUV::GetTimeMs();
		data["direction"] = "out";
		data["info"]["desiredBitrate"] = bitrates.desiredBitrate;
		data["info"]["effectiveDesiredBitrate"] = bitrates.effectiveDesiredBitrate;
		data["info"]["minBitrate"] = bitrates.minBitrate;
		data["info"]["maxBitrate"] = bitrates.maxBitrate;
		data["info"]["startBitrate"] = bitrates.startBitrate;
		data["info"]["maxPaddingBitrate"] = bitrates.maxPaddingBitrate;
		data["info"]["availableBitrate"] = bitrates.availableBitrate;

		switch (this->tccClient->GetBweType())
		{
		case RTC::BweType::TRANSPORT_CC:
			data["info"]["type"] = "transport-cc";
			break;
		case RTC::BweType::REMB:
			data["info"]["type"] = "remb";
			break;
		}

		Channel::ChannelNotifier::Emit(this->id, "trace", data);*/
	}
}