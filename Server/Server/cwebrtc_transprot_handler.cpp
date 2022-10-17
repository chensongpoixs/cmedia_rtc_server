/***********************************************************************************************
created: 		2022-08-10

author:			chensong

purpose:		cwebrtc_transport_handler
************************************************************************************************/

#include "cwebrtc_transport.h"
#include <json/json.h>
#include "csctp_association.h"
#include <ostream>
#include <string>
#include <iterator>                                              // std::ostream_iterator
#include <map>                                                   // std::multimap
#include <sstream>                                               // std::ostringstream
#include "ccrypto_random.h"

#include "Producer.hpp"
namespace chen {
	bool cwebrtc_transport::handler_webrtc_sdp(const std::string & sdp)
	{
		m_client_rtc_sdp.init(sdp);
		/*
		{
					kind       : 'video',
					mimeType   : 'video/h264',
					clockRate  : 90000,
					parameters :
					{
						'packetization-mode'      : 1,
						'profile-level-id'        : '42e01f',
						'level-asymmetry-allowed' : 1,
						'x-google-start-bitrate'  : 1000,
						'x-google-max-bitrate'    : 28000,
						'x-google-min-bitrate'    : 5500,
					}
				}
		*/

		const std::vector< RTC::RtpParameters>& rtp_media_datas = m_client_rtc_sdp.get_rtp_parameters();
		// video
		int32 index = 0;
		for (size_t i = 0; i < rtp_media_datas.size(); ++i)
		{
			if (rtp_media_datas[i].m_codec_type == RTC::RtpCodecMimeType::Type::VIDEO)
			{
				index = i;
				break;
			}
		}


		std::string producerId = s_crypto_random.GetRandomString(20);

		RTC::Producer * producer_ptr = new RTC::Producer(producerId, this, rtp_media_datas[index]);
		try
		{
			m_rtpListener.AddProducer(producer_ptr);
		}
		catch (const std::exception&)
		{
			delete producer_ptr;
			ERROR_EX_LOG("rtplistener add produce failed !!!");
			return false;
		}

		if (!m_mapProducers.insert(std::make_pair(producerId, producer_ptr)).second)
		{
			ERROR_EX_LOG("map producers install failed !!! %s", producerId.c_str());
			return false;
		}
		NORMAL_EX_LOG("Producer created [producerId:%s]", producerId.c_str());


		// Take the transport related RTP header extensions of the Producer and
		// add them to the Transport.
		// NOTE: Producer::GetRtpHeaderExtensionIds() returns the original
		// header extension ids of the Producer (and not their mapped values).
		const auto& producerRtpHeaderExtensionIds = producer_ptr->GetRtpHeaderExtensionIds();

		if (producerRtpHeaderExtensionIds.mid != 0u)
		{
			this->m_recvRtpHeaderExtensionIds.mid = producerRtpHeaderExtensionIds.mid;
		}

		if (producerRtpHeaderExtensionIds.rid != 0u)
		{
			this->m_recvRtpHeaderExtensionIds.rid = producerRtpHeaderExtensionIds.rid;
		}

		if (producerRtpHeaderExtensionIds.rrid != 0u)
		{
			this->m_recvRtpHeaderExtensionIds.rrid = producerRtpHeaderExtensionIds.rrid;
		}

		if (producerRtpHeaderExtensionIds.absSendTime != 0u)
		{
			this->m_recvRtpHeaderExtensionIds.absSendTime = producerRtpHeaderExtensionIds.absSendTime;
		}

		if (producerRtpHeaderExtensionIds.transportWideCc01 != 0u)
		{
			this->m_recvRtpHeaderExtensionIds.transportWideCc01 =
				producerRtpHeaderExtensionIds.transportWideCc01;
		}

		return true;
	}

	bool cwebrtc_transport::handler_webrtc_connect()
	{
		const RTC::DtlsTransport::Fingerprint& finger_print = m_client_rtc_sdp.get_finger_print();

		if (m_connect_called)
		{
			ERROR_EX_LOG("connect() already called");
			return false;
		}
		// default 就服务器模式
		this->m_dtlsRole = RTC::DtlsTransport::Role::SERVER;
		this->m_connect_called = true;

		// Pass the remote fingerprint to the DTLS transport.
		if (this->m_dtls_transport_ptr->SetRemoteFingerprint(finger_print))
		{
			// If everything is fine, we may run the DTLS transport if ready.
			MayRunDtlsTransport();
		}
		return true;
	}

	bool cwebrtc_transport::handler_connect(uint64 session_id, Json::Value & value)
	{
		if (m_connect_called)
		{
			ERROR_EX_LOG("connect() already called");
			return false;
		}
		RTC::DtlsTransport::Fingerprint dtlsRemoteFingerprint;
		RTC::DtlsTransport::Role dtlsRemoteRole;
		if (!value.isMember("dtlsParameters") || !value["dtlsParameters"].isObject())
		{
			ERROR_EX_LOG("missing dtlsParameters");
			return false;
		}

		const Json::Value& DtlsParameter = value["dtlsParameters"];
		if (!DtlsParameter.isMember("fingerprints") || !DtlsParameter["fingerprints"].isArray())
		{
			ERROR_EX_LOG("missing dtlsParameters.fingerprints");
			return false;
		}

		const Json::Value & fingerprintsvalue = DtlsParameter["fingerprints"];
		if (fingerprintsvalue.empty())
		{
			ERROR_EX_LOG("empty dtlsParameters.fingerprints array");
			return false;
		}
		for (auto iter : fingerprintsvalue)
		{
			if (!iter.isObject())
			{
				ERROR_EX_LOG("wrong entry in dtlsParameters.fingerprints (not an object)");
				continue;
			}
			if (!iter.isMember("algorithm") || !iter["algorithm"].isString())
			{
				ERROR_EX_LOG("missing fingerprint.algorithm");
				continue;;
			}
			if (!iter.isMember("value") || !iter["value"].isString())
			{
				ERROR_EX_LOG("missing fingerprint.value");
				continue;;
			}
			dtlsRemoteFingerprint.algorithm =
				RTC::DtlsTransport::GetFingerprintAlgorithm(iter["algorithm"].asCString());
			dtlsRemoteFingerprint.value = iter["value"].asCString();
			break;
		}

		if (!fingerprintsvalue.isMember("role") || !fingerprintsvalue["role"].isString())
		{
			dtlsRemoteRole = RTC::DtlsTransport::Role::AUTO;
		}
		else
		{
			dtlsRemoteRole = RTC::DtlsTransport::StringToRole(fingerprintsvalue["role"].asCString());

			if (dtlsRemoteRole == RTC::DtlsTransport::Role::NONE)
			{
				ERROR_EX_LOG("invalid dtlsParameters.role value");
				return false;
			}
		}


		// Set local DTLS role.
		switch (dtlsRemoteRole)
		{
		case RTC::DtlsTransport::Role::CLIENT:
		{
			this->m_dtlsRole = RTC::DtlsTransport::Role::SERVER;

			break;
		}
		// If the peer has role "auto" we become "client" since we are ICE controlled.
		case RTC::DtlsTransport::Role::SERVER:
		case RTC::DtlsTransport::Role::AUTO:
		{
			this->m_dtlsRole = RTC::DtlsTransport::Role::CLIENT;

			break;
		}
		case RTC::DtlsTransport::Role::NONE:
		{
			ERROR_EX_LOG("invalid remote DTLS role");
			return false;
		}
		}

		this->m_connect_called = true;

		// Pass the remote fingerprint to the DTLS transport.
		if (this->m_dtls_transport_ptr->SetRemoteFingerprint(dtlsRemoteFingerprint))
		{
			// If everything is fine, we may run the DTLS transport if ready.
			MayRunDtlsTransport();
		}

		// Tell the caller about the selected local DTLS role.
		//json data = json::object();
		// 角色  
		switch (this->m_dtlsRole)
		{
		case RTC::DtlsTransport::Role::CLIENT:
		//	data["dtlsLocalRole"] = "client";
			break;

		case RTC::DtlsTransport::Role::SERVER:
			//data["dtlsLocalRole"] = "server";
			break;

		default:
			ERROR_EX_LOG("invalid local DTLS role");
		}
		//DEBUG_EX_LOG("[reply = %s]", data.dump().c_str());
		//request->Accept(data);
		return true;
	}
	bool cwebrtc_transport::handler_restart_ice(uint64 session_id, Json::Value & value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_info(uint64 session_id, Json::Value & value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_get_stats(uint64 session_id, Json::Value & value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_set_max_incoming_bitrate(uint64 session_id, Json::Value & value)
	{
		return false;
	}


	bool cwebrtc_transport::handler_set_max_outgoing_bitrate(uint64 session_id, Json::Value& value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_produce(uint64 session_id, Json::Value & value)
	{
		std::string producerId = s_crypto_random.GetRandomString(20);
		RTC::RtpParameters rtp;
		RTC::Producer * producer_ptr = new RTC::Producer(producerId, this, rtp);
		try
		{
			m_rtpListener.AddProducer(producer_ptr);
		}
		catch (const std::exception&)
		{
			delete producer_ptr;
			ERROR_EX_LOG("rtplistener add produce failed !!!");
			return false;
		}

		if (!m_mapProducers.insert(std::make_pair(producerId, producer_ptr)).second)
		{
			ERROR_EX_LOG("map producers install failed !!! %s", producerId.c_str());
			return false;
		}
		NORMAL_EX_LOG("Producer created [producerId:%s]", producerId.c_str());


		// Take the transport related RTP header extensions of the Producer and
		// add them to the Transport.
		// NOTE: Producer::GetRtpHeaderExtensionIds() returns the original
		// header extension ids of the Producer (and not their mapped values).
		const auto& producerRtpHeaderExtensionIds = producer_ptr->GetRtpHeaderExtensionIds();

		if (producerRtpHeaderExtensionIds.mid != 0u)
		{
			this->m_recvRtpHeaderExtensionIds.mid = producerRtpHeaderExtensionIds.mid;
		}

		if (producerRtpHeaderExtensionIds.rid != 0u)
		{
			this->m_recvRtpHeaderExtensionIds.rid = producerRtpHeaderExtensionIds.rid;
		}

		if (producerRtpHeaderExtensionIds.rrid != 0u)
		{
			this->m_recvRtpHeaderExtensionIds.rrid = producerRtpHeaderExtensionIds.rrid;
		}

		if (producerRtpHeaderExtensionIds.absSendTime != 0u)
		{
			this->m_recvRtpHeaderExtensionIds.absSendTime = producerRtpHeaderExtensionIds.absSendTime;
		}

		if (producerRtpHeaderExtensionIds.transportWideCc01 != 0u)
		{
			this->m_recvRtpHeaderExtensionIds.transportWideCc01 =
				producerRtpHeaderExtensionIds.transportWideCc01;
		}





		// Create status response.
		/*json data = json::object();

		data["type"] = RTC::RtpParameters::GetTypeString(producer->GetType());

		request->Accept(data);*/

		// Check if TransportCongestionControlServer or REMB server must be
		// created.
		const auto& rtpHeaderExtensionIds = producer_ptr->GetRtpHeaderExtensionIds();
		const auto& codecs = producer_ptr->GetRtpParameters().codecs;

		// Set TransportCongestionControlServer.
		if (!this->m_tcc_server_ptr)
		{
			bool createTccServer{ false };
			RTC::BweType bweType;
			// TODO@chensong 20220910  RTC 服务Qos
			// Use transport-cc if:
			// - there is transport-wide-cc-01 RTP header extension, and
			// - there is "transport-cc" in codecs RTCP feedback.
			//
			// clang-format off
			//if (
			//	rtpHeaderExtensionIds.transportWideCc01 != 0u &&
			//	std::any_of(
			//		codecs.begin(), codecs.end(), [](const RTC::RtpCodecParameters& codec)
			//{
			//	/*return std::any_of(
			//		codec.rtcpFeedback.begin(), codec.rtcpFeedback.end(), [](const RTC::RtcpFeedback& fb)
			//	{
			//		return fb.type == "transport-cc";
			//	});*/
			//})
			//	)
			//	// clang-format on
			//{
			//	NORMAL_EX_LOG("bwe, enabling TransportCongestionControlServer with transport-cc");

			//	createTccServer = true;
			//	bweType = RTC::BweType::TRANSPORT_CC;
			//}
			// Use REMB if:
			// - there is abs-send-time RTP header extension, and
			// - there is "remb" in codecs RTCP feedback.
			//
			// clang-format off
			//else if (
			//	rtpHeaderExtensionIds.absSendTime != 0u &&
			//	std::any_of(
			//		codecs.begin(), codecs.end(), [](const RTC::RtpCodecParameters& codec)
			//{
			//	/*return std::any_of(
			//		codec.rtcpFeedback.begin(), codec.rtcpFeedback.end(), [](const RTC::RtcpFeedback& fb)
			//	{
			//		return fb.type == "goog-remb";
			//	});*/
			//})
			//	)
			//	// clang-format on
			//{
			//	NORMAL_EX_LOG("bwe, enabling TransportCongestionControlServer with REMB");

			//	createTccServer = true;
			//	bweType = RTC::BweType::REMB;
			//}

			if (createTccServer)
			{
				this->m_tcc_server_ptr = new RTC::TransportCongestionControlServer(this, bweType, RTC::MtuSize);

				if (this->m_maxIncomingBitrate != 0u)
				{
					this->m_tcc_server_ptr->SetMaxIncomingBitrate(this->m_maxIncomingBitrate);

				}
				if (IsConnected())
				{
					this->m_tcc_server_ptr->TransportConnected();
				}
			}
		}


		return true;
	}
	bool cwebrtc_transport::handler_consume(uint64 session_id, Json::Value& value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_produce_data(uint64 session_id, Json::Value & value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_consume_data(uint64 session_id, Json::Value& value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_enable_trace_event(uint64 session_id, Json::Value & value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_produce_close(uint64 session_id, Json::Value & value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_consumer_close(uint64 session_id, Json::Value& value)
	{
		return false;
	}

	/////////////////////produce info /////////////////////////
	bool cwebrtc_transport::handler_produce_info(uint64 session_id, Json::Value & value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_produce_get_stats(uint64 session_id, Json::Value & value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_produce_pause(uint64 session_id, Json::Value& value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_produce_resume(uint64 session_id, Json::Value& value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_produce_enable_trace_event(uint64 session_id, Json::Value & value)
	{
		return false;
	}

	/////////////////////consumer info////////////////////////////////
	bool cwebrtc_transport::handler_consumer_info(uint64 session_id, Json::Value& value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_consumer_get_stats(uint64 session_id, Json::Value & value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_consumer_pause(uint64 session_id, Json::Value& value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_consumer_resume(uint64 session_id, Json::Value& value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_consumer_set_preferred_layers(uint64 session_id, Json::Value & value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_consumer_set_priority(uint64 session_id, Json::Value& value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_consumer_request_key_frame(uint64 session_id, Json::Value & value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_consumer_enable_trace_event(uint64 session_id, Json::Value & value)
	{
		return false;
	}

	///////////////////////////////////////////////////////////
	bool cwebrtc_transport::handler_data_producer_close(uint64 session_id, Json::Value& value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_data_consumer_close(uint64 session_id, Json::Value& value)
	{
		return false;
	}

	bool cwebrtc_transport::handler_data_producer_info(uint64 session_id, Json::Value& value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_data_producer_get_stats(uint64 session_id, Json::Value & value)
	{
		return false;
	}


	bool cwebrtc_transport::handler_data_consumer_info(uint64 session_id, Json::Value & value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_data_consumer_get_stats(uint64 session_id, Json::Value & value)
	{
		return false;
	}

	bool cwebrtc_transport::handler_data_consumer_get_buffered_amount(uint64 session_id, Json::Value& value)
	{
		return false;
	}
	bool cwebrtc_transport::handler_data_consumer_set_buffered_amount_low_threshold(uint64 session_id, Json::Value& value)
	{
		return false;
	}
	void cwebrtc_transport::reply_info(uint64 session_id)
	{
		Json::Value  reply;
		reply["id"] = m_id;
		

		if (!m_dtls_transport_ptr)
		{
			WARNING_EX_LOG("m_dtls_transport_ptr == null !!!");
			return;
		}
		if (!m_ice_server_ptr)
		{
			WARNING_EX_LOG("m_ice_server_ptr == NULL!!!");
			return;
		}
		if (m_ice_canidates.empty())
		{
			WARNING_EX_LOG("m_ice_canidates size = %lu !!!", m_ice_canidates.size());
			return;
		}
		std::vector<RTC::DtlsTransport::Fingerprint>& fingerprints = m_dtls_transport_ptr->GetLocalFingerprints();
		if (fingerprints.empty())
		{
			WARNING_EX_LOG("m_dtls_transport_ptr  fingerprints size == 0 !!!!");
			return;
		}

		std::string algorithm =  RTC::DtlsTransport::GetFingerprintAlgorithmString(fingerprints[0].algorithm);


		/*std::ostringstream sdp;
		sdp << "v=0\r\n";
		sdp << "o=chensong 10000 1 IN IP4 0.0.0.0\r\n";
		sdp << "s=-\r\n";
		sdp << "t=0 0\r\n";
		sdp << "a=ice-lite\r\n" ;
		sdp << "a=fingerprint:" << algorithm << " " << fingerprints[0].value << "\r\n";
		sdp << "a=msid-semantic: WMS *\r\n";
		sdp << m_client_rtc_sdp.get_webrtc_sdp();*/
		//sdp << "m=video 7 UDP/TLS/RTP/SAVPF 100 101\r\n"; // TODO@chensong 编码类型
		//sdp << "c=IN IP4 127.0.0.1\r\n";
		//sdp << "a=rtpmap:100 H264/90000\r\n";
		//sdp << "a=rtpmap:101 rtx/90000\r\n";
		//sdp << "a=fmtp:100 level-asymmetry-allowed=1;packetization-mode=1;profile-level-id=42e01f;x-google-max-bitrate=28000;x-google-min-bitrate=5500;x-google-start-bitrate=1000\r\n";
		//sdp << "a=fmtp:101 apt=100\r\n";
		//sdp << "a=rtcp-fb:100 transport-cc\r\n";
		//sdp << "a=rtcp-fb:100 ccm fir\r\n";
		//sdp << "a=rtcp-fb:100 nack\r\n";
		//sdp << "a=rtcp-fb:100 nack pli\r\n";
		//sdp << "a=extmap:3 urn:ietf:params:rtp-hdrext:sdes:mid\r\n";
		//sdp << "a=extmap:4 urn:ietf:params:rtp-hdrext:sdes:rtp-stream-id\r\n";
		//sdp << "a=extmap:5 urn:ietf:params:rtp-hdrext:sdes:repaired-rtp-stream-id\r\n";
		//sdp << "a=extmap:13 http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time\r\n";
		//sdp << "a=extmap:2 http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01\r\n";
		//sdp << "a=extmap:8 http://tools.ietf.org/html/draft-ietf-avtext-framemarking-07\r\n";  //  关键帧 服务器的掉包处理
		//sdp << "a=extmap:12 urn:3gpp:video-orientation\r\n"; 
		//sdp << "a=extmap:14 urn:ietf:params:rtp-hdrext:toffset\r\n";
		//sdp << "a=setup:server\r\n";
		//sdp << "a=mid:0\r\n";
		//sdp << "a=sendrecv\r\n";
		//sdp << "a=ice-ufrag:"<< m_ice_server_ptr->GetUsernameFragment() /*xjg2o2h2v8bfzqkv*/ << "\r\n";
		//sdp << "a=ice-pwd:"<< m_ice_server_ptr->GetPassword() /*z5ywy0b4le11t42tcqzpz5kdmr22i9rm*/ <<"\r\n";
		//sdp << "a=candidate:" << m_ice_canidates[0].candidate() /*udpcandidate 1 udp 1076302079 192.168.1.73 43717 typ host*/ <<"\r\n";
		//sdp << "a=end-of-candidates\r\n";
		////	//a=ice-options 用于描述 ICE 连接的属性信息，ice-options 的定义有很多种，WebRTC 中常见的有：
		////	//a=ice-options:trickle client 一边收集 candidate 一边发送给对端并开始连通性检查，可以缩短 ICE 建立连接的时间。
		////	//a=ice-options:renomination 允许 ICE controlling 一方动态重新提名新的 candidate ，
		////                                 默认情况 Offer 一方为controlling 角色，answer 一方为 controlled 角色；同时 Lite 一方只能为 controlled 角色。
		//sdp << "a=ice-options:renomination";
		//////  音频、视频和数据通道同一个m行
		//sdp << "a=rtcp-mux";
		//sdp << "a=rtcp-rsize";


		//reply["sdp"] =  sdp.str().c_str();
		 
		//sdp << "";

		//{
		//	std::string("v = 0\r\n\
		//				o=chensong 10000 1 IN IP4 0.0.0.0\r\n\
		//				s=-\r\n\
		//				t=0 0\r\n\
		//				a=ice-lite\r\n\
		//				a=fingerprint:sha-512 3D:9A:9C:39:15:B1:1F:B4:43:F8:F0:95:D1:F6:DE:C1:3C:09:C1:B9:83:14:A0:E8:31:11:92:69:56:7F:6D:DD:9E:76:C9:9B:CD:82:10:DF:FB:64:B5:0B:7C:FB:DF:E0:83:54:5C:2F:0E:D7:1A:A7:E6:0C:AC:6D:FD:9B:AE:75\r\n\
		//				a=msid-semantic: WMS *\r\n\
		//				a=group:BUNDLE 0\r\n\
		//				m=video 7 UDP/TLS/RTP/SAVPF 100 101\r\n\
		//				c=IN IP4 127.0.0.1\r\n\
		//				a=rtpmap:100 H264/90000\r\n\
		//				a=rtpmap:101 rtx/90000\r\n\
		//				a=fmtp:100 level-asymmetry-allowed=1;packetization-mode=1;profile-level-id=42e01f;x-google-max-bitrate=28000;x-google-min-bitrate=5500;x-google-start-bitrate=1000\r\n\
		//				a=fmtp:101 apt=100\r\n\
		//				a=rtcp-fb:100 transport-cc\r\n\
		//				a=rtcp-fb:100 ccm fir\r\n\
		//				a=rtcp-fb:100 nack\r\n\
		//				a=rtcp-fb:100 nack pli\r\n\
		//				a=extmap:3 urn:ietf:params:rtp-hdrext:sdes:mid\r\n\
		//				a=extmap:4 urn:ietf:params:rtp-hdrext:sdes:rtp-stream-id\r\n\
		//				a=extmap:5 urn:ietf:params:rtp-hdrext:sdes:repaired-rtp-stream-id\r\n\
		//				a=extmap:13 http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time\r\n\
		//				a=extmap:2 http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01\r\n\
		//				a=extmap:8 http://tools.ietf.org/html/draft-ietf-avtext-framemarking-07\r\n\
		//				a=extmap:12 urn:3gpp:video-orientation\r\n\
		//				a=extmap:14 urn:ietf:params:rtp-hdrext:toffset\r\n\
		//				a=setup:active\r\n\
		//				a=mid:0\r\n\
		//				a=recvonly\r\n\
		//				a=ice-ufrag:xjg2o2h2v8bfzqkv\r\n\
		//				a=ice-pwd:z5ywy0b4le11t42tcqzpz5kdmr22i9rm\r\n");

		//}



		// Add headerExtensionIds.
		//if (0u != m_recvRtpHeaderExtensionIds.mid)
		//{
		//	reply["recvRtpHeaderExtensions"]["mid"] = m_recvRtpHeaderExtensionIds.mid;
		//}

		//if (0u != m_recvRtpHeaderExtensionIds.rid)
		//{
		//	reply["recvRtpHeaderExtensions"]["rid"] = m_recvRtpHeaderExtensionIds.rid;
		//}

		//if (0u != m_recvRtpHeaderExtensionIds.rrid)
		//{
		//	reply["recvRtpHeaderExtensions"]["rrid"] = m_recvRtpHeaderExtensionIds.rrid;
		//}

		//if (0u != m_recvRtpHeaderExtensionIds.absSendTime)
		//{
		//	reply["recvRtpHeaderExtensions"]["absSendTime"] = m_recvRtpHeaderExtensionIds.absSendTime;
		//}


		//if (0u != m_recvRtpHeaderExtensionIds.transportWideCc01)
		//{
		//	reply["recvRtpHeaderExtensions"]["transportWideCc01"] = m_recvRtpHeaderExtensionIds.transportWideCc01;
		//}

		//// Add rtpListener.
		//// this->rtpListener.FillJson(jsonObject["rtpListener"]);

		//// Add maxMessageSize.
		//reply["maxMessageSize"] = m_max_message_size;

		//if (m_sctp_association_ptr)
		//{
		//	// Add sctpParameters.
		//	m_sctp_association_ptr->reply(reply);
		//	
		//	// Add sctpState.
		//	switch (m_sctp_association_ptr->get_state())
		//	{
		//		case	ESCTP_NEW :
		//		{
		//			reply["sctpState"] = "new";
		//			break;
		//		}
		//		case	ESCTP_CONNECTING:
		//		{
		//			reply["sctpState"] = "connecting";
		//			break;
		//		}
		//		case	ESCTP_CONNECTED:
		//		{
		//			reply["sctpState"] = "connected";
		//			break;
		//		}
		//		case	ESCTP_FAILED:
		//		{
		//			reply["sctpState"] = "failed";
		//			break;
		//		}
		//		case	ESCTP_CLOSED:
		//		{
		//			reply["sctpState"] = "closed";
		//			break;
		//		}
		//		default:
		//		{
		//			ERROR_EX_LOG("webrtc transport state !!!");
		//			break;
		//		}
		//	}

		//	//// Add sctpListener.
		//	//this->sctpListener.FillJson(jsonObject["sctpListener"]);
		//}
		//
		//// Add traceEventTypes.
		//std::vector<std::string> traceEventTypes;
		//std::ostringstream traceEventTypesStream;

		//if (m_traceEventTypes.probation)
		//{
		//	traceEventTypes.emplace_back("probation");
		//}
		//if (m_traceEventTypes.bwe)
		//{
		//	traceEventTypes.emplace_back("bwe");
		//}

		//if (!traceEventTypes.empty())
		//{
		//	std::copy(
		//		traceEventTypes.begin(),
		//		traceEventTypes.end() - 1,
		//		std::ostream_iterator<std::string>(traceEventTypesStream, ","));
		//	traceEventTypesStream << traceEventTypes.back();
		//}

		//reply["traceEventTypes"] = traceEventTypesStream.str();
		//reply_rtc_info(reply);
	}

	bool cwebrtc_transport::reply_create_webrtc(Json::Value & value)
	{

		value["id"] = m_id;

		/// ICE parameters
		
		Json::Value ice_parameters;
		ice_parameters["iceLite"] = true;
		ice_parameters["password"] = m_ice_server_ptr->GetPassword();
		ice_parameters["usernameFragment"] = m_ice_server_ptr->GetUsernameFragment();


		value["iceParameters"] = ice_parameters;

		// ICE candidates
		//Json::Value ice_candidates;
		for ( RTC::IceCandidate & ice: m_ice_canidates)
		{
			ice.reply(value["iceCandidates"]);
		}
		 
		// DTLS Parameters
		if (m_dtls_transport_ptr)
		{
			m_dtls_transport_ptr->reply(value["dtlsParameters"]);
		}
		
		if (m_sctp_association_ptr)
		{
			m_sctp_association_ptr->reply(value["sctpParameters"]);
		}


		switch (this->m_dtlsRole)
		{
		case RTC::DtlsTransport::Role::NONE:
			value["dtlsParameters"]["role"] = "none";
			break;
		case RTC::DtlsTransport::Role::AUTO:
			value["dtlsParameters"]["role"] = "auto";
			break;
		case RTC::DtlsTransport::Role::CLIENT:
			value["fingerprints"]["role"] = "client";
			break;
		case RTC::DtlsTransport::Role::SERVER:
			value["dtlsParameters"]["role"] = "server";
			break;
		}

		return true;
	}

	void cwebrtc_transport::reply_rtc_info(/*Json::Value & value*/)
	{
		//1. Add iceRole (we are always "controlled"). ？？？？？？ 
		//value["iceRole"] = "controlled";

		////2. Add iceParameters.  
		//// 在 STUN 验证的 用户名和密码
		//value["iceParameters"]["usernameFragment"] = m_ice_server_ptr->GetUsernameFragment();
		//value["iceParameters"]["password"] =  m_ice_server_ptr->GetPassword();
		//value["iceParameters"]["iceLite"] = true;


		////3. Add iceCandidates. 
		//for (size_t i = 0; i < m_ice_canidates.size(); ++i)
		//{
		//	m_ice_canidates[i].reply(value);
		//	//jsonIceCandidatesIt->emplace_back(json::value_t::object);

		//	//auto& jsonEntry = (*jsonIceCandidatesIt)[i];
		//	//auto& iceCandidate = this->iceCandidates[i];

		//	//iceCandidate.FillJson(jsonEntry);
		//}

		////4. Add iceState.
		//switch (m_ice_server_ptr->GetState())
		//{
		//case RTC::IceServer::IceState::NEW:
		//	value["iceState"] = "new";
		//	break;
		//case RTC::IceServer::IceState::CONNECTED:
		//	value["iceState"] = "connected";
		//	break;
		//case RTC::IceServer::IceState::COMPLETED:
		//	value["iceState"] = "completed";
		//	break;
		//case RTC::IceServer::IceState::DISCONNECTED:
		//	value["iceState"] = "disconnected";
		//	break;
		//}
		////5.  Add iceSelectedTuple.
		//if (m_ice_server_ptr->GetSelectedTuple())
		//{
		//	m_ice_server_ptr->GetSelectedTuple()->reply(value);
		//}

		////6. Add dtlsParameters.
		////jsonObject["dtlsParameters"] = json::object();
		////auto jsonDtlsParametersIt = jsonObject.find("dtlsParameters");

		//// Add dtlsParameters.fingerprints.
		////(*jsonDtlsParametersIt)["fingerprints"] = json::array();
		////auto jsonDtlsParametersFingerprintsIt = jsonDtlsParametersIt->find("fingerprints");
		////auto& fingerprints = this->dtlsTransport->GetLocalFingerprints();
		//const std::vector<RTC::DtlsTransport::Fingerprint>& fingerprints = m_dtls_transport_ptr->GetLocalFingerprints();
		//for (size_t i = 0; i < fingerprints.size(); ++i)
		//{
		//	//jsonDtlsParametersFingerprintsIt->emplace_back(json::value_t::object);

		//	//auto& jsonEntry = (*jsonDtlsParametersFingerprintsIt)[i];
		//	//auto& fingerprint = fingerprints[i];
		//	// array 
		//	Json::Value arrayvalue;
		//	arrayvalue["algorithm"] = RTC::DtlsTransport::GetFingerprintAlgorithmString(fingerprints[i].algorithm);;;
		//	arrayvalue["value"] = fingerprints[i].value;
		//	value["dtlsParameters"]["fingerprints"].append(arrayvalue);
		//	//value["dtlsParameters"]["fingerprints"][i]["algorithm"] = RTC::DtlsTransport::GetFingerprintAlgorithmString(fingerprints[i].algorithm);;
		//	//value["dtlsParameters"]["fingerprints"][i]["value"] = fingerprints[i].value;
		//	/*	jsonEntry["algorithm"] =
		//			RTC::DtlsTransport::GetFingerprintAlgorithmString(fingerprint.algorithm);
		//		jsonEntry["value"] = fingerprint.value;*/
		//}

		////7. Add dtlsParameters.role.
		//switch (m_dtlsRole)
		//{
		//	case RTC::DtlsTransport::Role::NONE:
		//	{
		//		value["dtlsParameters"]["role"] = "none";
		//		break;
		//	}
		//	case RTC::DtlsTransport::Role::AUTO:
		//	{
		//		value["dtlsParameters"]["role"] = "auto";
		//		break;
		//	}
		//	case RTC::DtlsTransport::Role::CLIENT:
		//	{
		//		value["dtlsParameters"]["role"] = "client";
		//		break;
		//	}
		//	case RTC::DtlsTransport::Role::SERVER:
		//	{
		//		value["dtlsParameters"]["role"] = "server";
		//		break;
		//	}
		//	default:
		//	{
		//		ERROR_EX_LOG("");
		//		break;
		//	}
		//}
		//// Add dtlsState.
		//switch (m_dtls_transport_ptr->GetState())
		//{
		//	case RTC::DtlsTransport::DtlsState::NEW:
		//	{
		//		value["dtlsState"] = "new";
		//		break;
		//	}
		//	case RTC::DtlsTransport::DtlsState::CONNECTING:
		//	{
		//		value["dtlsState"] = "connecting";
		//		break;
		//	}
		//	case RTC::DtlsTransport::DtlsState::CONNECTED:
		//	{
		//		value["dtlsState"] = "connected";
		//		break;
		//	}
		//	case RTC::DtlsTransport::DtlsState::FAILED:
		//	{
		//		value["dtlsState"] = "failed";
		//		break;
		//	}
		//	case RTC::DtlsTransport::DtlsState::CLOSED:
		//	{
		//		value["dtlsState"] = "closed";
		//		break;
		//	}
		//	default:
		//	{
		//		ERROR_EX_LOG("");
		//		break;
		//	}
		//}
	}
}