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
#include "csrtp_session.h"

namespace chen {


	/* Static. */

	static constexpr uint16_t IceCandidateDefaultLocalPriority{ 10000 };
	// We just provide "host" candidates so type preference is fixed.
	static constexpr uint16_t IceTypePreference{ 64 };
	// We do not support non rtcp-mux so component is always 1.
	static constexpr uint16_t IceComponent{ 1 };

	static inline uint32_t generateIceCandidatePriority(uint16_t localPreference)
	{
		//MS_TRACE();

		return std::pow(2, 24) * IceTypePreference + std::pow(2, 8) * localPreference +
			std::pow(2, 0) * (256 - IceComponent);
	}


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
		, m_ice_server_ptr(NULL)
		, m_udp_sockets_map()
		, m_dtls_transport_ptr(NULL)
		, m_srtp_recv_session_ptr(NULL)
		, m_srtp_send_session_ptr(NULL)
		, m_connect_called(false)
		, m_ice_canidates()
		, m_dtlsRole(RTC::DtlsTransport::Role::AUTO)
		, m_traceEventTypes()
	{

		//m_ice_server_ptr;
		//m_udp_sockets_map;
		//m_dtls_transport;
		//m_srtp_recv_session_ptr;
		//m_srtp_send_session_ptr;
		//m_connect_called;
		//m_ice_canidates;
		//m_dtlsRole;// { RTC::DtlsTransport::Role::AUTO };


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


		///////////////////////////WEBRTC init///////////////////////////////////////


		uint16 port = 0;
		std::vector<ListenIp> listenIps;

		ListenIp listenip;
		listenip.announcedIp = "0.0.0.0";
		listenip.ip = "0.0.0.0";
		listenIps.push_back(listenip);

		try
		{
			uint16_t iceLocalPreferenceDecrement{ 0 };

			m_ice_canidates.reserve(listenIps.size());

			for (std::vector<ListenIp>::iterator iter = listenIps.begin(); iter != listenIps.end(); ++iter)
			{
				// ICE 优先级    就是 从是先开始连接
				uint16_t iceLocalPreference = IceCandidateDefaultLocalPriority - iceLocalPreferenceDecrement;

				//if (preferUdp)
					iceLocalPreference += 1000;

				uint32_t icePriority = generateIceCandidatePriority(iceLocalPreference);

				// This may throw.
				/*RTC::UdpSocket*/ cudp_socket* udpSocket;
				// TODO@chensong 这边port 是使用同一个端口吗？？？
				if (port != 0)
				{
					udpSocket = new cudp_socket(this, iter->ip, port);
				}
				else
				{
					udpSocket = new cudp_socket(this, iter->ip);
				}

				m_udp_sockets_map[udpSocket] = iter->announcedIp;

				if (iter->announcedIp.empty())
				{
					m_ice_canidates.emplace_back(udpSocket, icePriority);
				}
				else
				{
					m_ice_canidates.emplace_back(udpSocket, icePriority, iter->announcedIp);
				}


				// Decrement initial ICE local preference for next IP.
				iceLocalPreferenceDecrement += 100;
			}
			/*
						WebRTC  ICE  协议协商 流程图


  |     client      |                协议                      |   mediasoup   |
  |                 |            1. 交换SDP信息                |               |
  |                 |            客户端发送SDP                 |               |
  |   一、SDP       |            -------------->               |               |
  |                 |    2. 发送SDP中含有用户名和密码          |               |
  |                 |            <------------                 |               |
  |————————————————————————————————————————————————————————————————————————————|
  |                 |      1. 验证客户端用户名和密码           |               |
  |                 |                                          |               | |
  |                 |              request BINDING             |               |
  |                 |           ------------------>            |               |
  |   二、STUN      |               REQUEST                    |               |
  |                 |           <-----------------             |               |
  |                 |                                          |               |
  |                 |                                          |               |
  |                 |                                          |               |
  |                 |                                          |               |
  |————————————————————————————————————————————————————————————————————————————|
  |                 |                                          |               |
  |                 |                                          |               |
  |                 |             交换数字签名                 |               |
  |                 |          流程 需要四次握手               |               |
  |                 |                                          |               |
  |                 |               hello                      |               |
  |                 |            ------------>                 |               |
  |   三、DTLS      |               hello                      |               |
  |                 |            <------------                 |               |
  |                 |               数字签名                   |               |
  |                 |           ------------->                 |               |
  |                 |               数字签名                   |               |
  |                 |           <-------------                 |               |
  |                 |              fflush                      |               |
  |                 |           ------------->                 |               |
  |—————————————————————————————————————————————————————————————————————————————|																	  |
  |                 |                                          |               |
  |                 |                                          |               |
  |                 |               rtp data                   |               |
  |                 |           ------------->                 |               |
  |   四、Media Data|                                          |               |
  |                 |              rtcp 反馈数据               |               |
  |                 |           <-------------                 |               |
  |                 |                                          |               |





一、 STUN 															  |				  |


   BINDING


   REQUEST




   INDICATION



   SUCCESS_RESPONSE



   ERROR_RESPONSE
*/

				// Create a ICE server.  这边创建ICE 协商    STURN 服务器操作  验证用户合法性   
				// 1. 验证用户名（16）
				// 2. 验证密码 （32）
			m_ice_server_ptr = new RTC::IceServer(this, s_crypto_random.GetRandomString(16), s_crypto_random.GetRandomString(16));
			if (!m_ice_server_ptr)
			{
				ERROR_EX_LOG("alloc ice server failed !!!");
				return false;
			}
			m_dtls_transport_ptr = new RTC::DtlsTransport(this);
			if (!m_dtls_transport_ptr)
			{
				ERROR_EX_LOG("alloc dtls transport !!!");
				return false;
			}
		}
		catch (const std::exception& e)
		{
			// Must delete everything since the destructor won't be called.

			if (m_dtls_transport_ptr)
			{
				delete m_dtls_transport_ptr;
				m_dtls_transport_ptr = nullptr;
			}
			if (m_ice_server_ptr)
			{

				delete m_ice_server_ptr;
				m_ice_server_ptr = nullptr;
			}
			for (auto& kv : m_udp_sockets_map)
			{
				auto* udpSocket = kv.first;

				delete udpSocket;
			}
			m_udp_sockets_map.clear();

			/*for (auto& kv : this->tcpServers)
			{
				auto* tcpServer = kv.first;

				delete tcpServer;
			}
			this->tcpServers.clear();*/

			//this->iceCandidates.clear();
			m_ice_canidates.clear();
			return false;
			//throw;
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

	void cwebrtc_transport::CloseProducersAndConsumers()
	{
		// This method is called by the Router and must notify him about all Producers
		// and Consumers that we are gonna close.
		//
		// The caller is supposed to delete this Transport instance after calling
		// this method.

		// Close all Producers.
		//for (auto& kv : this->mapProducers)
		//{
		//	auto* producer = kv.second;

		//	// Notify the listener.
		//	this->listener->OnTransportProducerClosed(this, producer);

		//	delete producer;
		//}
		//this->mapProducers.clear();

		// Delete all Consumers.
		//for (auto& kv : this->mapConsumers)
		//{
		//	auto* consumer = kv.second;

		//	// Notify the listener.
		//	this->listener->OnTransportConsumerClosed(this, consumer);

		//	delete consumer;
		//}
		/*this->mapConsumers.clear();
		this->mapSsrcConsumer.clear();
		this->mapRtxSsrcConsumer.clear();*/

		// Delete all DataProducers.
		//for (auto& kv : this->mapDataProducers)
		//{
		//	auto* dataProducer = kv.second;

		//	// Notify the listener.
		//	this->listener->OnTransportDataProducerClosed(this, dataProducer);

		//	delete dataProducer;
		//}
		//this->mapDataProducers.clear();

		// Delete all DataConsumers.
		//for (auto& kv : this->mapDataConsumers)
		//{
		//	auto* dataConsumer = kv.second;

		//	// Notify the listener.
		//	this->listener->OnTransportDataConsumerClosed(this, dataConsumer);

		//	delete dataConsumer;
		//}
		//this->mapDataConsumers.clear();
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
		DistributeAvailableOutgoingBitrate();
		ComputeOutgoingDesiredBitrate();

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
			EmitTraceEventProbationType(packet);

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
			onSendCallback*  cb = new onSendCallback([tccClient, &packetInfo](bool sent) {
				if (sent)
				{
					tccClient->PacketSent(packetInfo, uv_util::GetTimeMsInt64());
				}
			});

			SendRtpPacket(nullptr, packet, cb);
#endif
		}
		else
		{
			// May emit 'trace' event.
			EmitTraceEventProbationType(packet);

			SendRtpPacket(nullptr, packet);
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
		SendRtcpPacket(packet);
	}
	void cwebrtc_transport::OnTimer()
	{
		// RTCP timer.
		//if (timer == this->rtcpTimer)
		{
			auto interval = static_cast<uint64_t>(RTC::RTCP::MaxVideoIntervalMs);
			uint64_t nowMs = uv_util::GetTimeMs();
			// TODO@chensong 20220811 
			SendRtcp(nowMs);

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
	void cwebrtc_transport::OnIceServerSendStunPacket(const RTC::IceServer * iceServer, const RTC::StunPacket * packet, RTC::TransportTuple * tuple)
	{
		// Send the STUN response over the same transport tuple.
		tuple->Send(packet->GetData(), packet->GetSize());

		// Increase send transmission.
		/*RTC::Transport::*/DataSent(packet->GetSize());
	}
	void cwebrtc_transport::OnIceServerSelectedTuple(const RTC::IceServer * iceServer, RTC::TransportTuple * tuple)
	{
		/*
		 * RFC 5245 section 11.2 "Receiving Media":
		 *
		 * ICE implementations MUST be prepared to receive media on each component
		 * on any candidates provided for that component.
		 */

		DEBUG_EX_LOG("ice, ICE selected tuple");

		// Notify the Node WebRtcTransport.
		///*json data = json::object();

		//this->iceServer->GetSelectedTuple()->FillJson(data["iceSelectedTuple"]);
		//DEBUG_EX_LOG("data = %s", data.dump().c_str());
		//Channel::ChannelNotifier::Emit(this->id, "iceselectedtuplechange", data);*/
	}
	void cwebrtc_transport::OnIceServerConnected(const RTC::IceServer * iceServer)
	{
		DEBUG_EX_LOG("ice, ICE connected");

		// Notify the Node WebRtcTransport.
		/*json data = json::object();

		data["iceState"] = "connected";

		Channel::ChannelNotifier::Emit(this->id, "icestatechange", data);*/

		// If ready, run the DTLS handler.
		MayRunDtlsTransport();

		// If DTLS was already connected, notify the parent class.
		if (m_dtls_transport_ptr->GetState() == RTC::DtlsTransport::DtlsState::CONNECTED)
		{
			/*RTC::Transport::*/Connected();
		}

		//DEBUG_EX_LOG("data = %s", data.dump().c_str());
	}
	void cwebrtc_transport::OnIceServerCompleted(const RTC::IceServer * iceServer)
	{
		DEBUG_EX_LOG("ice, ICE completed");

		// Notify the Node WebRtcTransport.
		/*json data = json::object();

		data["iceState"] = "completed";

		Channel::ChannelNotifier::Emit(this->id, "icestatechange", data);*/

		// If ready, run the DTLS handler.
		MayRunDtlsTransport();

		// If DTLS was already connected, notify the parent class.
		if (m_dtls_transport_ptr->GetState() == RTC::DtlsTransport::DtlsState::CONNECTED)
		{
			/*RTC::Transport::*/Connected();
		}
		//DEBUG_EX_LOG("data = %s", data.dump().c_str());
	}
	void cwebrtc_transport::OnIceServerDisconnected(const RTC::IceServer * iceServer)
	{
		DEBUG_EX_LOG("ice, ICE disconnected");

		// Notify the Node WebRtcTransport.
		/*json data = json::object();

		data["iceState"] = "disconnected";

		Channel::ChannelNotifier::Emit(this->id, "icestatechange", data);*/

		// If DTLS was already connected, notify the parent class.
		if (m_dtls_transport_ptr->GetState() == RTC::DtlsTransport::DtlsState::CONNECTED)
		{
			/*RTC::Transport::*/Disconnected();
		}
	}
	//TODO@chensong 2022-04-17 底层发送到上层 
	void cwebrtc_transport::OnUdpSocketPacketReceived(cudp_socket * socket, const uint8_t * data, size_t len, const sockaddr * remoteAddr)
	{
		RTC::TransportTuple tuple(socket, remoteAddr);

		OnPacketReceived(&tuple, data, len);
	}
	void cwebrtc_transport::OnDtlsTransportConnecting(const RTC::DtlsTransport * dtlsTransport)
	{
		DEBUG_EX_LOG("dtls, DTLS connecting");

		// Notify the Node WebRtcTransport.
		/*json data = json::object();

		data["dtlsState"] = "connecting";
		DEBUG_EX_LOG("data = %s", data.dump().c_str());
		Channel::ChannelNotifier::Emit(this->id, "dtlsstatechange", data);*/
	}
	void cwebrtc_transport::OnDtlsTransportConnected(const RTC::DtlsTransport * dtlsTransport, ECRYPTO_SUITE srtpCryptoSuite, uint8_t * srtpLocalKey, size_t srtpLocalKeyLen, uint8_t * srtpRemoteKey, size_t srtpRemoteKeyLen, std::string & remoteCert)
	{
		DEBUG_EX_LOG("dtls, DTLS connected");

		// Close it if it was already set and update it.
		if (m_srtp_send_session_ptr)
		{
			delete m_srtp_send_session_ptr;
			m_srtp_send_session_ptr = NULL;
		}
		/*delete this->srtpSendSession;
		this->srtpSendSession = nullptr;*/
		if (m_srtp_recv_session_ptr)
		{
			delete m_srtp_recv_session_ptr;
			m_srtp_recv_session_ptr = NULL;
		}
		/*delete this->srtpRecvSession;
		this->srtpRecvSession = nullptr;*/

		try
		{
			m_srtp_send_session_ptr = new csrtp_session(EOUTBOUND, srtpCryptoSuite, srtpLocalKey, srtpLocalKeyLen);
		}
		catch (...)
		{
			ERROR_EX_LOG("error creating SRTP sending session:  " );
		}

		try
		{
			m_srtp_recv_session_ptr = new csrtp_session( EINBOUND, srtpCryptoSuite, srtpRemoteKey, srtpRemoteKeyLen);

			// Notify the Node WebRtcTransport.
			/*json data = json::object();

			data["dtlsState"] = "connected";
			data["dtlsRemoteCert"] = remoteCert;

			Channel::ChannelNotifier::Emit(this->id, "dtlsstatechange", data);
			DEBUG_EX_LOG("data = %s", data.dump().c_str());*/
			// Tell the parent class.
			/*RTC::Transport::*/Connected();
		}
		catch (...)
		{
			ERROR_EX_LOG("error creating SRTP receiving session:  " );

			if (m_srtp_send_session_ptr)
			{
				delete m_srtp_send_session_ptr;
				m_srtp_send_session_ptr = NULL;
			}
		}
	}
	void cwebrtc_transport::OnDtlsTransportFailed(const RTC::DtlsTransport * dtlsTransport)
	{
		WARNING_EX_LOG("dtls, DTLS failed");

		// Notify the Node WebRtcTransport.
		/*json data = json::object();

		data["dtlsState"] = "failed";
		DEBUG_EX_LOG("data = %s", data.dump().c_str());
		Channel::ChannelNotifier::Emit(this->id, "dtlsstatechange", data);*/
	}
	void cwebrtc_transport::OnDtlsTransportClosed(const RTC::DtlsTransport * dtlsTransport)
	{
		WARNING_EX_LOG("dtls, DTLS remotely closed");

		// Notify the Node WebRtcTransport.
		/*json data = json::object();

		data["dtlsState"] = "closed";
		DEBUG_EX_LOG("data = %s", data.dump().c_str());
		Channel::ChannelNotifier::Emit(this->id, "dtlsstatechange", data);*/

		// Tell the parent class.
		/*RTC::Transport::*/Disconnected();
	}
	void cwebrtc_transport::OnDtlsTransportSendData(const RTC::DtlsTransport * dtlsTransport, const uint8_t * data, size_t len)
	{
		if (!m_ice_server_ptr->GetSelectedTuple())
		{
			WARNING_EX_LOG("dtls, no selected tuple set, cannot send DTLS packet");

			return;
		}
		//<<<<<<< HEAD
		//		DEBUG_EX_LOG("len = %lu", len);
		//=======
		DEBUG_EX_LOG("len = %lu", len);

		// TODO@chensong 20220522     
		//TLSv1.3 发送 Server Hello、 Certificate、Certificate Status、 Server key Exchange、 Server Hello Done
//>>>>>>> d40fa1c367378f962a8c8dd093974a106997055a
		m_ice_server_ptr->GetSelectedTuple()->Send(data, len);

		// Increase send transmission.
		/*RTC::Transport::*/DataSent(len);
	}
	void cwebrtc_transport::OnDtlsTransportApplicationDataReceived(const RTC::DtlsTransport * dtlsTransport, const uint8_t * data, size_t len)
	{
		// Pass it to the parent transport.
		/*RTC::Transport::*/ReceiveSctpData(data, len);
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
	bool cwebrtc_transport::IsConnected() const
	{
		// clang-format off
		return (
			(
				m_ice_server_ptr->GetState() == RTC::IceServer::IceState::CONNECTED ||
				m_ice_server_ptr->GetState() == RTC::IceServer::IceState::COMPLETED
				) &&
			 m_dtls_transport_ptr->GetState() == RTC::DtlsTransport::DtlsState::CONNECTED
			);
		// clang-format on
	}
	void cwebrtc_transport::MayRunDtlsTransport()
	{
		// Do nothing if we have the same local DTLS role as the DTLS transport.
		// NOTE: local role in DTLS transport can be NONE, but not ours.
		if (/*this->dtlsTransport->*/ m_dtls_transport_ptr-> GetLocalRole() == m_dtlsRole)
		{
			return;
		}

		// Check our local DTLS role.
		switch (m_dtlsRole)
		{
			// If still 'auto' then transition to 'server' if ICE is 'connected' or
			// 'completed'.
		case RTC::DtlsTransport::Role::AUTO:
		{
			// clang-format off
			if (
				/*this->iceServer->*/m_ice_server_ptr-> GetState() == RTC::IceServer::IceState::CONNECTED ||
				m_ice_server_ptr->GetState() == RTC::IceServer::IceState::COMPLETED
				)
				// clang-format on
			{
				DEBUG_EX_LOG("dtls, transition from DTLS local role 'auto' to 'server' and running DTLS transport");

				m_dtlsRole = RTC::DtlsTransport::Role::SERVER;
				/*this->dtlsTransport->*/m_dtls_transport_ptr-> Run(RTC::DtlsTransport::Role::SERVER);
			}

			break;
		}

		// 'client' is only set if a 'connect' request was previously called with
		// remote DTLS role 'server'.
		//
		// If 'client' then wait for ICE to be 'completed' (got USE-CANDIDATE).
		//
		// NOTE: This is the theory, however let's be more flexible as told here:
		//   https://bugs.chromium.org/p/webrtc/issues/detail?id=3661
		case RTC::DtlsTransport::Role::CLIENT:
		{
			// clang-format off
			if (
				/*this->iceServer*/ m_ice_server_ptr->GetState() == RTC::IceServer::IceState::CONNECTED ||
				/*this->iceServer*/ m_ice_server_ptr->GetState() == RTC::IceServer::IceState::COMPLETED
				)
				// clang-format on
			{
				DEBUG_EX_LOG("dtls, running DTLS transport in local role 'client'");

				/*this->dtlsTransport*/m_dtls_transport_ptr ->Run(RTC::DtlsTransport::Role::CLIENT);
			}

			break;
		}

		// If 'server' then run the DTLS transport if ICE is 'connected' (not yet
		// USE-CANDIDATE) or 'completed'.
		case RTC::DtlsTransport::Role::SERVER:
		{
			// clang-format off
			if (
				/*this->iceServer*/ m_ice_server_ptr->GetState() == RTC::IceServer::IceState::CONNECTED ||
				/*this->iceServer*/ m_ice_server_ptr->GetState() == RTC::IceServer::IceState::COMPLETED
				)
				// clang-format on
			{
				DEBUG_EX_LOG("dtls, running DTLS transport in local role 'server'");

				m_dtls_transport_ptr->Run(RTC::DtlsTransport::Role::SERVER);
			}

			break;
		}

		case RTC::DtlsTransport::Role::NONE:
		{
			ERROR_EX_LOG("local DTLS role not set");
		}
		}
	}
	void cwebrtc_transport::SendRtpPacket(void * consumer, RTC::RtpPacket * packet, onSendCallback * cb)
	{
		if (!IsConnected())
		{
			if (cb)
			{
				(*cb)(false);
				delete cb;
			}

			return;
		}

		// Ensure there is sending SRTP session.
		if (!m_srtp_send_session_ptr)
		{
			WARNING_EX_LOG("ignoring RTP packet due to non sending SRTP session");

			if (cb)
			{
				(*cb)(false);
				delete cb;
			}

			return;
		}

		const uint8_t* data = packet->GetData();
		size_t len = packet->GetSize();

		if (!m_srtp_send_session_ptr->EncryptRtp(&data, &len))
		{
			if (cb)
			{
				(*cb)(false);
				delete cb;
			}

			return;
		}

		m_ice_server_ptr->GetSelectedTuple()->Send(data, len, cb);

		// Increase send transmission.
		/*RTC::Transport::*/DataSent(len);
	}
	void cwebrtc_transport::SendRtcpPacket(RTC::RTCP::Packet * packet)
	{
		if (!IsConnected())
		{
			return;
		}

		const uint8_t* data = packet->GetData();
		size_t len = packet->GetSize();

		// Ensure there is sending SRTP session.
		if (/*!this->srtpSendSession*/ !m_srtp_send_session_ptr)
		{
			WARNING_EX_LOG("ignoring RTCP packet due to non sending SRTP session");

			return;
		}

		if (!m_srtp_send_session_ptr->EncryptRtcp(&data, &len))
		{
			return;
		}

		m_ice_server_ptr->GetSelectedTuple()->Send(data, len);

		// Increase send transmission.
		/*RTC::Transport::*/DataSent(len);
	}
	void cwebrtc_transport::SendRtcpCompoundPacket(RTC::RTCP::CompoundPacket * packet)
	{
		if (!IsConnected())
		{
			return;
		}

		const uint8_t* data = packet->GetData();
		size_t len = packet->GetSize();
		//DEBUG_EX_LOG("len = %lu", len);
		// Ensure there is sending SRTP session.
		if (!m_srtp_send_session_ptr)
		{
			WARNING_EX_LOG("rtcp, ignoring RTCP compound packet due to non sending SRTP session");

			return;
		}

		if (!m_srtp_send_session_ptr->EncryptRtcp(&data, &len))
		{
			return;
		}

		m_ice_server_ptr->GetSelectedTuple()->Send(data, len);

		// Increase send transmission.
		/*RTC::Transport::*/DataSent(len);
	}
	/*void cwebrtc_transport::SendRtcpCompoundPacket(RTC::RTCP::CompoundPacket * packet)
	{
	}*/
	void cwebrtc_transport::SendSctpData(const uint8_t * data, size_t len)
	{
		// clang-format on
		if (!IsConnected())
		{
			WARNING_EX_LOG("sctp, DTLS not connected, cannot send SCTP data");

			return;
		}

		m_dtls_transport_ptr->SendApplicationData(data, len);
	}
	void cwebrtc_transport::RecvStreamClosed(uint32_t ssrc)
	{
		if (m_srtp_recv_session_ptr)
		{
			m_srtp_recv_session_ptr->RemoveStream(ssrc);
		}
	}
	void cwebrtc_transport::SendStreamClosed(uint32_t ssrc)
	{
		if (m_srtp_send_session_ptr)
		{
			m_srtp_send_session_ptr->RemoveStream(ssrc);
		}
	}
	// TODO@chensong 处理底层协议 RTCP/RTP/STUN/ICE
	void cwebrtc_transport::OnPacketReceived(RTC::TransportTuple * tuple, const uint8_t * data, size_t len)
	{
		// Increase receive transmission.
		/*RTC::Transport::*/DataReceived(len);

		
		if (RTC::StunPacket::IsStun(data, len))// Check if it's STUN.
		{ 
			OnStunDataReceived(tuple, data, len);
		} 
		else if (RTC::RTCP::Packet::IsRtcp(data, len))// Check if it's RTCP.
		{ 
			OnRtcpDataReceived(tuple, data, len);
		} 
		else if (RTC::RtpPacket::IsRtp(data, len))// Check if it's RTP.
		{ 
			OnRtpDataReceived(tuple, data, len);
		} 
		else if (RTC::DtlsTransport::IsDtls(data, len))// Check if it's DTLS.
		{ 
			OnDtlsDataReceived(tuple, data, len);
		}
		else
		{ 
			WARNING_EX_LOG("ignoring received packet of unknown type");
		}
	}
	void cwebrtc_transport::OnStunDataReceived(RTC::TransportTuple * tuple, const uint8_t * data, size_t len)
	{
		RTC::StunPacket* packet = RTC::StunPacket::Parse(data, len);

		if (!packet)
		{
			WARNING_EX_LOG("ignoring wrong STUN packet received");

			return;
		}
		//DEBUG_EX_ID_LOG("[]");
		// Pass it to the IceServer.
		m_ice_server_ptr->ProcessStunPacket(packet, tuple);

		delete packet;
	}
	void cwebrtc_transport::OnDtlsDataReceived(const RTC::TransportTuple * tuple, const uint8_t * data, size_t len)
	{
		// Ensure it comes from a valid tuple.
		if (!m_ice_server_ptr->IsValidTuple(tuple))
		{
			WARNING_EX_LOG("dtls, ignoring DTLS data coming from an invalid tuple");

			return;
		}

		// Trick for clients performing aggressive ICE regardless we are ICE-Lite.
		m_ice_server_ptr->ForceSelectedTuple(tuple);

		// Check that DTLS status is 'connecting' or 'connected'.
		if (
			m_dtls_transport_ptr->GetState() == RTC::DtlsTransport::DtlsState::CONNECTING ||
			m_dtls_transport_ptr->GetState() == RTC::DtlsTransport::DtlsState::CONNECTED)
		{
			DEBUG_EX_LOG("DTLS data received, passing it to the DTLS transport");
			// 这边修改dtls 连接状态 了
			m_dtls_transport_ptr->ProcessDtlsData(data, len);
		}
		else
		{
			WARNING_EX_LOG("dtls, Transport is not 'connecting' or 'connected', ignoring received DTLS data");

			return;
		}
	}
	void cwebrtc_transport::OnRtpDataReceived(RTC::TransportTuple * tuple, const uint8_t * data, size_t len)
	{
		// Ensure DTLS is connected.
		if (m_dtls_transport_ptr->GetState() != RTC::DtlsTransport::DtlsState::CONNECTED)
		{
			DEBUG_EX_LOG("dtls, rtp, ignoring RTP packet while DTLS not connected");

			return;
		}

		// Ensure there is receiving SRTP session.
		if (!m_srtp_recv_session_ptr)
		{
			DEBUG_EX_LOG("srtp, ignoring RTP packet due to non receiving SRTP session");

			return;
		}

		// Ensure it comes from a valid tuple.
		if (!m_ice_server_ptr->IsValidTuple(tuple))
		{
			WARNING_EX_LOG("rtp, ignoring RTP packet coming from an invalid tuple");

			return;
		}

		// Decrypt the SRTP packet.
		if (!m_srtp_recv_session_ptr->DecryptSrtp(const_cast<uint8_t*>(data), &len))
		{
			RTC::RtpPacket* packet = RTC::RtpPacket::Parse(data, len);

			if (!packet)
			{
				WARNING_EX_LOG(" srtp, DecryptSrtp() failed due to an invalid RTP packet");
			}
			else
			{
				WARNING_EX_LOG(" srtp, DecryptSrtp() failed [ssrc:%u, payloadType:%hhu, seq:%hu]",
					packet->GetSsrc(),
					packet->GetPayloadType(),
					packet->GetSequenceNumber());

				delete packet;
			}

			return;
		}

		RTC::RtpPacket* packet = RTC::RtpPacket::Parse(data, len);

		if (!packet)
		{
			WARNING_EX_LOG("rtp, received data is not a valid RTP packet");

			return;
		}

		// Trick for clients performing aggressive ICE regardless we are ICE-Lite.
		m_ice_server_ptr->ForceSelectedTuple(tuple);

		// Pass the packet to the parent transport.
		/*RTC::Transport::*/ReceiveRtpPacket(packet);
	}
	void cwebrtc_transport::OnRtcpDataReceived(RTC::TransportTuple * tuple, const uint8_t * data, size_t len)
	{
		// Ensure DTLS is connected.
		if ( m_dtls_transport_ptr->GetState() != RTC::DtlsTransport::DtlsState::CONNECTED)
		{
			DEBUG_EX_LOG("dtls, rtcp, ignoring RTCP packet while DTLS not connected");

			return;
		}

		// Ensure there is receiving SRTP session.
		if (!m_srtp_recv_session_ptr)
		{
			DEBUG_EX_LOG("srtp, ignoring RTCP packet due to non receiving SRTP session");

			return;
		}

		// Ensure it comes from a valid tuple.
		if (!m_ice_server_ptr->IsValidTuple(tuple))
		{
			WARNING_EX_LOG("rtcp, ignoring RTCP packet coming from an invalid tuple");

			return;
		}

		// Decrypt the SRTCP packet.
		if (!m_srtp_recv_session_ptr->DecryptSrtcp(const_cast<uint8_t*>(data), &len))
		{
			return;
		}

		RTC::RTCP::Packet* packet = RTC::RTCP::Packet::Parse(data, len);

		if (!packet)
		{
			WARNING_EX_LOG("rtcp, received data is not a valid RTCP compound or single packet");

			return;
		}

		// Pass the packet to the parent transport.
		/*RTC::Transport::*/ReceiveRtcpPacket(packet);
	}
	
}