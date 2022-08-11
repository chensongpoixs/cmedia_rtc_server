/***********************************************************************************************
created: 		2022-08-10

author:			chensong

purpose:		cwebrtc_transport 
************************************************************************************************/

#ifndef _C_WEBRTC_TRANSPORT_H_
#define _C_WEBRTC_TRANSPORT_H_
#include <string>
#include "cnet_type.h"
#include <map>
#include <json/json.h>
#include "csctp_association.h"
#include "ctimer.h"
#include "RtpHeaderExtensionIds.hpp"
#include "crate_calculator.h"
#include "TransportCongestionControlClient.hpp"
#include "TransportCongestionControlServer.hpp"
namespace chen
{
	class cwebrtc_transport : public ctimer
	{
	public:
		cwebrtc_transport();
		~cwebrtc_transport();

	public:
		bool init(const std::string & transport_id);


		void destroy();


	public:
		// Must be called from the subclass.
		void Connected();
		void Disconnected();
		void DataReceived(size_t len)
		{
			m_recvTransmission.Update(len, uv_util::GetTimeMs());
		}
		void DataSent(size_t len)
		{
			m_sendTransmission.Update(len, uv_util::GetTimeMs());
		}
		void ReceiveRtpPacket(RTC::RtpPacket* packet);
		void ReceiveRtcpPacket(RTC::RTCP::Packet* packet);
		void ReceiveSctpData(const uint8_t* data, size_t len);
		//void SetNewProducerIdFromInternal(json& internal, std::string& producerId) const;
		//RTC::Producer* GetProducerFromInternal(json& internal) const;
		//void SetNewConsumerIdFromInternal(json& internal, std::string& consumerId) const;
		//RTC::Consumer* GetConsumerFromInternal(json& internal) const;
		//RTC::Consumer* GetConsumerByMediaSsrc(uint32_t ssrc) const;
		//RTC::Consumer* GetConsumerByRtxSsrc(uint32_t ssrc) const;
		//void SetNewDataProducerIdFromInternal(json& internal, std::string& dataProducerId) const;
		//RTC::DataProducer* GetDataProducerFromInternal(json& internal) const;
		//void SetNewDataConsumerIdFromInternal(json& internal, std::string& dataConsumerId) const;
		//RTC::DataConsumer* GetDataConsumerFromInternal(json& internal) const;

	public:
		// sctp association  callback
		void OnSctpAssociationConnecting(csctp_association* sctpAssociation)  ;
		void OnSctpAssociationConnected(csctp_association* sctpAssociation)  ;
		void OnSctpAssociationFailed(csctp_association* sctpAssociation)  ;
		void OnSctpAssociationClosed(csctp_association* sctpAssociation)  ;
		void OnSctpAssociationSendData( csctp_association* sctpAssociation, const uint8_t* data, size_t len)  ;
		void OnSctpAssociationMessageReceived( csctp_association* sctpAssociation,
			uint16_t streamId,
			uint32_t ppid,
			const uint8_t* msg,
			size_t len)  ;
		void OnSctpAssociationBufferedAmount(
			csctp_association* sctpAssociation, uint32_t bufferedAmount)  ;


	public:
		/* Pure virtual methods inherited from RTC::TransportCongestionControlClient::Listener. */
		void OnTransportCongestionControlClientBitrates(
			RTC::TransportCongestionControlClient* tccClient,
			RTC::TransportCongestionControlClient::Bitrates& bitrates)  ;
		void OnTransportCongestionControlClientSendRtpPacket(
			RTC::TransportCongestionControlClient* tccClient,
			RTC::RtpPacket* packet,
			const webrtc::PacedPacketInfo& pacingInfo)  ;

		
	public:
		/* Pure virtual methods inherited from RTC::TransportCongestionControlServer::Listener. */
		void OnTransportCongestionControlServerSendRtcpPacket(
			RTC::TransportCongestionControlServer* tccServer, RTC::RTCP::Packet* packet) ;

	public:
		// Callback
		void OnTimer();


	private:
		void HandleRtcpPacket(RTC::RTCP::Packet* packet);
		void SendRtcp(uint64_t nowMs);

		void DistributeAvailableOutgoingBitrate();
		void ComputeOutgoingDesiredBitrate(bool forceBitrate = false);
		void EmitTraceEventProbationType(RTC::RtpPacket* packet) const;
		void EmitTraceEventBweType(RTC::TransportCongestionControlClient::Bitrates& bitrates) const;
	private:
		std::string								m_id; 
		size_t									m_max_message_size; // default 262144
		csctp_association *						m_sctp_association_ptr; 
		//ctimer			*						m_rtcp_timer_ptr;
		RTC::TransportCongestionControlClient*	m_tcc_client_ptr;
		RTC::TransportCongestionControlServer*  m_tcc_server_ptr;



		/////////RTP //////////////////////////////////////////////
		struct RTC::RtpHeaderExtensionIds		m_recvRtpHeaderExtensionIds;


		//////////////////////////
		 // 这个两个暂时没有使用到
		//RtpDataCounter							m_recvRtpTransmission; 
		//RtpDataCounter							m_sendRtpTransmission;
		////////////////////////////////
		RtpDataCounter							m_recvRtxTransmission;
		RtpDataCounter							m_sendRtxTransmission;
		RtpDataCounter							m_sendProbationTransmission;
		//////////////////////////////////////////////////////
		uint16_t								m_transportWideCcSeq;
		//////////////////////////////////////////////////

		uint32_t								m_initialAvailableOutgoingBitrate;
	};
}

#endif // _C_WEBRTC_TRANSPORT_H_