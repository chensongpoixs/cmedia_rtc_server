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
//#include "csctp_association.h"
#include "ctimer.h"
#include "RtpHeaderExtensionIds.hpp"
#include "crate_calculator.h"
#include "TransportCongestionControlClient.hpp"
#include "TransportCongestionControlServer.hpp"
#include "IceServer.hpp"

#include <unordered_map>
#include "DtlsTransport.hpp"
#include "csrtp_session.h"
#include "IceCandidate.hpp"
#include "CompoundPacket.hpp"
#include <json/json.h>
#include "Producer.hpp"
#include "RtpListener.hpp"
#include "crtc_sdp.h"
namespace chen
{
	class csctp_association;
	class cwebrtc_transport : public ctimer
							, public RTC::IceServer::Listener
							, public cudp_socket::Listener
							, public RTC::DtlsTransport::Listener
							, public RTC::Producer::Listener
							, public RTC::TransportCongestionControlServer::Listener
	{
	private:
		struct ListenIp
		{
			std::string ip;
			std::string announcedIp;
		};
		struct TraceEventTypes
		{
			bool probation ;
			bool bwe ;
			TraceEventTypes()
				: probation(false)
				, bwe(false) {}
		};
	public:
	protected:
		typedef  std::function<void(bool sent)>								onSendCallback ;
		typedef  std::function<void(bool queued, bool sctpSendBufferFull)>	onQueuedCallback;
	public:
		cwebrtc_transport();
		~cwebrtc_transport();

	public:
		bool init(const std::string & transport_id/*, const std::string & sdp*/);

	

		void destroy();

	public:
		void CloseProducersAndConsumers();
	public:

		bool handler_webrtc_sdp(const std::string & sdp);
		bool handler_webrtc_connect();
		/////////////////////////
		bool handler_connect(uint64 session_id, Json::Value & value);
		bool handler_restart_ice(uint64 session_id, Json::Value & value);
		bool handler_info(uint64 session_id, Json::Value & value);
		bool handler_get_stats(uint64 session_id, Json::Value & value);
		bool handler_set_max_incoming_bitrate(uint64 session_id, Json::Value& value);
		bool handler_set_max_outgoing_bitrate(uint64 session_id, Json::Value& value);
		bool handler_produce(uint64 session_id, Json::Value & value);
		bool handler_consume(uint64 session_id, Json::Value& value);
		bool handler_produce_data(uint64 session_id, Json::Value & value);
		bool handler_consume_data(uint64 session_id, Json::Value& value);
		bool handler_enable_trace_event(uint64 session_id, Json::Value & value);
		bool handler_produce_close(uint64 session_id, Json::Value & value);
		bool handler_consumer_close(uint64 session_id, Json::Value& value);

		/////////////////////produce info /////////////////////////
		bool handler_produce_info(uint64 session_id, Json::Value & value);
		bool handler_produce_get_stats(uint64 session_id, Json::Value & value);
		bool handler_produce_pause(uint64 session_id, Json::Value& value);
		bool handler_produce_resume(uint64 session_id, Json::Value& value);
		bool handler_produce_enable_trace_event(uint64 session_id, Json::Value & value);

		/////////////////////consumer info////////////////////////////////
		bool handler_consumer_info(uint64 session_id, Json::Value& value);
		bool handler_consumer_get_stats(uint64 session_id, Json::Value & value);
		bool handler_consumer_pause(uint64 session_id, Json::Value& value);
		bool handler_consumer_resume(uint64 session_id, Json::Value& value);
		bool handler_consumer_set_preferred_layers(uint64 session_id, Json::Value & value);
		bool handler_consumer_set_priority(uint64 session_id, Json::Value& value);
		bool handler_consumer_request_key_frame(uint64 session_id, Json::Value & value);
		bool handler_consumer_enable_trace_event(uint64 session_id, Json::Value & value);

		///////////////////////////////////////////////////////////
		bool handler_data_producer_close(uint64 session_id, Json::Value& value);
		bool handler_data_consumer_close(uint64 session_id, Json::Value& value);

		bool handler_data_producer_info(uint64 session_id, Json::Value& value);
		bool handler_data_producer_get_stats(uint64 session_id, Json::Value & value);


		bool handler_data_consumer_info(uint64 session_id, Json::Value & value);
		bool handler_data_consumer_get_stats(uint64 session_id, Json::Value & value);

		bool handler_data_consumer_get_buffered_amount(uint64 session_id, Json::Value& value);
		bool handler_data_consumer_set_buffered_amount_low_threshold(uint64 session_id, Json::Value& value);

	public:
		void reply_info(uint64 session_id);



		bool reply_create_webrtc(Json::Value & value);

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

	public:

		
	public:
		/* Pure virtual methods inherited from RTC::IceServer::Listener. */
		void OnIceServerSendStunPacket(
			const RTC::IceServer* iceServer,
			const RTC::StunPacket* packet,
			RTC::TransportTuple* tuple) override;
		void OnIceServerSelectedTuple(const RTC::IceServer* iceServer, RTC::TransportTuple* tuple) override;
		void OnIceServerConnected(const RTC::IceServer* iceServer) override;
		void OnIceServerCompleted(const RTC::IceServer* iceServer) override;
		void OnIceServerDisconnected(const RTC::IceServer* iceServer) override;
	public:

		/* Pure virtual methods inherited from RTC::UdpSocket::Listener. */
 
		void OnUdpSocketPacketReceived(
			cudp_socket* socket, const uint8_t* data, size_t len, const struct sockaddr* remoteAddr) override;


	public:
		/* Pure virtual methods inherited from RTC::DtlsTransport::Listener. */
		void OnDtlsTransportConnecting(const RTC::DtlsTransport* dtlsTransport) override;
		void OnDtlsTransportConnected(
			const RTC::DtlsTransport* dtlsTransport,
			/*RTC::SrtpSession::CryptoSuite*/ ECRYPTO_SUITE srtpCryptoSuite,
			uint8_t* srtpLocalKey,
			size_t srtpLocalKeyLen,
			uint8_t* srtpRemoteKey,
			size_t srtpRemoteKeyLen,
			std::string& remoteCert) override;
		void OnDtlsTransportFailed(const RTC::DtlsTransport* dtlsTransport) override;
		void OnDtlsTransportClosed(const RTC::DtlsTransport* dtlsTransport) override;
		void OnDtlsTransportSendData(
			const RTC::DtlsTransport* dtlsTransport, const uint8_t* data, size_t len) override;
		void OnDtlsTransportApplicationDataReceived(
			const RTC::DtlsTransport* dtlsTransport, const uint8_t* data, size_t len) override;
	private:
		///////// BASE  ////////////////
		void HandleRtcpPacket(RTC::RTCP::Packet* packet);
		void SendRtcp(uint64_t nowMs);

		void DistributeAvailableOutgoingBitrate();
		void ComputeOutgoingDesiredBitrate(bool forceBitrate = false);
		void EmitTraceEventProbationType(RTC::RtpPacket* packet) const;
		void EmitTraceEventBweType(RTC::TransportCongestionControlClient::Bitrates& bitrates) const;

		//////// WEBRTC ///////////////
	private:
		bool IsConnected() const ;
		void MayRunDtlsTransport();
		 void SendRtpPacket(
			/*RTC::Consumer*/void * consumer,
			RTC::RtpPacket* packet,
			/*RTC::Transport::*/onSendCallback* cb = nullptr)  ; 
		void SendRtcpPacket(RTC::RTCP::Packet* packet) ;
		void SendRtcpCompoundPacket(RTC::RTCP::CompoundPacket* packet) ;
		/*void SendMessage(
			RTC::DataConsumer* dataConsumer,
			uint32_t ppid,
			const uint8_t* msg,
			size_t len,
			onQueuedCallback* cb = nullptr) override;*/
		void SendSctpData(const uint8_t* data, size_t len)  ;
		void RecvStreamClosed(uint32_t ssrc)  ;
		void SendStreamClosed(uint32_t ssrc)  ;
		void OnPacketReceived(RTC::TransportTuple* tuple, const uint8_t* data, size_t len);
		void OnStunDataReceived(RTC::TransportTuple* tuple, const uint8_t* data, size_t len);
		void OnDtlsDataReceived(const RTC::TransportTuple* tuple, const uint8_t* data, size_t len);
		void OnRtpDataReceived(RTC::TransportTuple* tuple, const uint8_t* data, size_t len);
		void OnRtcpDataReceived(RTC::TransportTuple* tuple, const uint8_t* data, size_t len);



		
	public:
		/* Pure virtual methods inherited from RTC::Producer::Listener. */
		void OnProducerPaused(RTC::Producer* producer) override;
		void OnProducerResumed(RTC::Producer* producer) override;
		void OnProducerNewRtpStream(
			RTC::Producer* producer, RTC::RtpStream* rtpStream, uint32_t mappedSsrc) override;
		void OnProducerRtpStreamScore(
			RTC::Producer* producer, RTC::RtpStream* rtpStream, uint8_t score, uint8_t previousScore) override;
		void OnProducerRtcpSenderReport(
			RTC::Producer* producer, RTC::RtpStream* rtpStream, bool first) override;
		void OnProducerRtpPacketReceived(RTC::Producer* producer, RTC::RtpPacket* packet) override;
		void OnProducerSendRtcpPacket(RTC::Producer* producer, RTC::RTCP::Packet* packet) override;
		void OnProducerNeedWorstRemoteFractionLost(
			RTC::Producer* producer, uint32_t mappedSsrc, uint8_t& worstRemoteFractionLost) override;
	//public:
	//	/* Pure virtual methods inherited from RTC::TransportCongestionControlServer::Listener. */

	//	void OnTransportCongestionControlServerSendRtcpPacket(
	//		RTC::TransportCongestionControlServer* tccServer, RTC::RTCP::Packet* packet) override;
	private:
		void reply_rtc_info(/*Json::Value& value*/);

	private:
		///////////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////RTC BASE//////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////////////////////////
		std::string											m_id; 
		size_t												m_max_message_size; // default 262144
		csctp_association *									m_sctp_association_ptr; 
		//ctimer			*								m_rtcp_timer_ptr;
		RTC::TransportCongestionControlClient*				m_tcc_client_ptr;
		RTC::TransportCongestionControlServer*				m_tcc_server_ptr;



		/////////RTP //////////////////////////////////////////////
		struct RTC::RtpHeaderExtensionIds					m_recvRtpHeaderExtensionIds;


		//////////////////////////

		RateCalculator										m_recvTransmission;
		RateCalculator										m_sendTransmission;

		 // 这个两个暂时没有使用到
		//RtpDataCounter									m_recvRtpTransmission; 
		//RtpDataCounter									m_sendRtpTransmission;
		////////////////////////////////
		RtpDataCounter										m_recvRtxTransmission;
		RtpDataCounter										m_sendRtxTransmission;
		RtpDataCounter										m_sendProbationTransmission;
		//////////////////////////////////////////////////////
		uint16_t											m_transportWideCcSeq;
		//////////////////////////////////////////////////

		uint32_t											m_initialAvailableOutgoingBitrate;

		/////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////WEBRTC//////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////
		RTC::IceServer*										m_ice_server_ptr;
		std::unordered_map<cudp_socket*, std::string>		m_udp_sockets_map;
		RTC::DtlsTransport	*								m_dtls_transport_ptr;
		csrtp_session		*								m_srtp_recv_session_ptr;
		csrtp_session		*								m_srtp_send_session_ptr;
		bool												m_connect_called;
		std::vector<RTC::IceCandidate>						m_ice_canidates;
		RTC::DtlsTransport::Role							m_dtlsRole;// { RTC::DtlsTransport::Role::AUTO };

		struct TraceEventTypes								m_traceEventTypes;
		std::unordered_map<std::string, RTC::Producer*>		m_mapProducers;
		RTC::RtpListener									m_rtpListener;
		uint32_t											m_maxIncomingBitrate{ 0u };
		uint32_t											m_maxOutgoingBitrate{ 0u };
		crtc_sdp											m_client_rtc_sdp;
};
}

#endif // _C_WEBRTC_TRANSPORT_H_