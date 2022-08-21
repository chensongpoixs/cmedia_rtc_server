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
namespace chen {


	bool cwebrtc_transport::handler_connect(uint64 session_id, Json::Value & value)
	{
		return false;
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
	void cwebrtc_transport::reply_info()
	{
		Json::Value  reply;
		reply["id"] = m_id;
		// Add headerExtensionIds.
		if (0u != m_recvRtpHeaderExtensionIds.mid)
		{
			reply["recvRtpHeaderExtensions"]["mid"] = m_recvRtpHeaderExtensionIds.mid;
		}

		if (0u != m_recvRtpHeaderExtensionIds.rid)
		{
			reply["recvRtpHeaderExtensions"]["rid"] = m_recvRtpHeaderExtensionIds.rid;
		}

		if (0u != m_recvRtpHeaderExtensionIds.rrid)
		{
			reply["recvRtpHeaderExtensions"]["rrid"] = m_recvRtpHeaderExtensionIds.rrid;
		}

		if (0u != m_recvRtpHeaderExtensionIds.absSendTime)
		{
			reply["recvRtpHeaderExtensions"]["absSendTime"] = m_recvRtpHeaderExtensionIds.absSendTime;
		}


		if (0u != m_recvRtpHeaderExtensionIds.transportWideCc01)
		{
			reply["recvRtpHeaderExtensions"]["transportWideCc01"] = m_recvRtpHeaderExtensionIds.transportWideCc01;
		}

		// Add rtpListener.
		// this->rtpListener.FillJson(jsonObject["rtpListener"]);

		// Add maxMessageSize.
		reply["maxMessageSize"] = m_max_message_size;

		if (m_sctp_association_ptr)
		{
			// Add sctpParameters.
			m_sctp_association_ptr->reply(reply);
			
			// Add sctpState.
			switch (m_sctp_association_ptr->get_state())
			{
				case	ESCTP_NEW :
				{
					reply["sctpState"] = "new";
					break;
				}
				case	ESCTP_CONNECTING:
				{
					reply["sctpState"] = "connecting";
					break;
				}
				case	ESCTP_CONNECTED:
				{
					reply["sctpState"] = "connected";
					break;
				}
				case	ESCTP_FAILED:
				{
					reply["sctpState"] = "failed";
					break;
				}
				case	ESCTP_CLOSED:
				{
					reply["sctpState"] = "closed";
					break;
				}
				default:
				{
					ERROR_EX_LOG("webrtc transport state !!!");
					break;
				}
			}

			//// Add sctpListener.
			//this->sctpListener.FillJson(jsonObject["sctpListener"]);
		}
		
		// Add traceEventTypes.
		std::vector<std::string> traceEventTypes;
		std::ostringstream traceEventTypesStream;

		if (m_traceEventTypes.probation)
		{
			traceEventTypes.emplace_back("probation");
		}
		if (m_traceEventTypes.bwe)
		{
			traceEventTypes.emplace_back("bwe");
		}

		if (!traceEventTypes.empty())
		{
			std::copy(
				traceEventTypes.begin(),
				traceEventTypes.end() - 1,
				std::ostream_iterator<std::string>(traceEventTypesStream, ","));
			traceEventTypesStream << traceEventTypes.back();
		}

		reply["traceEventTypes"] = traceEventTypesStream.str();
		reply_rtc_info(reply);
	}

	void cwebrtc_transport::reply_rtc_info(Json::Value & value)
	{
		//1. Add iceRole (we are always "controlled"). ？？？？？？ 
		value["iceRole"] = "controlled";

		//2. Add iceParameters.  
		// 在 STUN 验证的 用户名和密码
		value["iceParameters"]["usernameFragment"] = m_ice_server_ptr->GetUsernameFragment();
		value["iceParameters"]["password"] =  m_ice_server_ptr->GetPassword();
		value["iceParameters"]["iceLite"] = true;


		//3. Add iceCandidates. 
		for (size_t i = 0; i < m_ice_canidates.size(); ++i)
		{
			m_ice_canidates[i].reply(value);
			//jsonIceCandidatesIt->emplace_back(json::value_t::object);

			//auto& jsonEntry = (*jsonIceCandidatesIt)[i];
			//auto& iceCandidate = this->iceCandidates[i];

			//iceCandidate.FillJson(jsonEntry);
		}

		//4. Add iceState.
		switch (m_ice_server_ptr->GetState())
		{
		case RTC::IceServer::IceState::NEW:
			value["iceState"] = "new";
			break;
		case RTC::IceServer::IceState::CONNECTED:
			value["iceState"] = "connected";
			break;
		case RTC::IceServer::IceState::COMPLETED:
			value["iceState"] = "completed";
			break;
		case RTC::IceServer::IceState::DISCONNECTED:
			value["iceState"] = "disconnected";
			break;
		}
		//5.  Add iceSelectedTuple.
		if (m_ice_server_ptr->GetSelectedTuple())
		{
			m_ice_server_ptr->GetSelectedTuple()->reply(value);
		}

		//6. Add dtlsParameters.
		//jsonObject["dtlsParameters"] = json::object();
		//auto jsonDtlsParametersIt = jsonObject.find("dtlsParameters");

		// Add dtlsParameters.fingerprints.
		//(*jsonDtlsParametersIt)["fingerprints"] = json::array();
		//auto jsonDtlsParametersFingerprintsIt = jsonDtlsParametersIt->find("fingerprints");
		//auto& fingerprints = this->dtlsTransport->GetLocalFingerprints();
		const std::vector<RTC::DtlsTransport::Fingerprint>& fingerprints = m_dtls_transport_ptr->GetLocalFingerprints();
		for (size_t i = 0; i < fingerprints.size(); ++i)
		{
			//jsonDtlsParametersFingerprintsIt->emplace_back(json::value_t::object);

			//auto& jsonEntry = (*jsonDtlsParametersFingerprintsIt)[i];
			//auto& fingerprint = fingerprints[i];
			// array 
			Json::Value arrayvalue;
			arrayvalue["algorithm"] = RTC::DtlsTransport::GetFingerprintAlgorithmString(fingerprints[i].algorithm);;;
			arrayvalue["value"] = fingerprints[i].value;
			value["dtlsParameters"]["fingerprints"].append(arrayvalue);
			//value["dtlsParameters"]["fingerprints"][i]["algorithm"] = RTC::DtlsTransport::GetFingerprintAlgorithmString(fingerprints[i].algorithm);;
			//value["dtlsParameters"]["fingerprints"][i]["value"] = fingerprints[i].value;
			/*	jsonEntry["algorithm"] =
					RTC::DtlsTransport::GetFingerprintAlgorithmString(fingerprint.algorithm);
				jsonEntry["value"] = fingerprint.value;*/
		}

		//7. Add dtlsParameters.role.
		switch (m_dtlsRole)
		{
			case RTC::DtlsTransport::Role::NONE:
			{
				value["dtlsParameters"]["role"] = "none";
				break;
			}
			case RTC::DtlsTransport::Role::AUTO:
			{
				value["dtlsParameters"]["role"] = "auto";
				break;
			}
			case RTC::DtlsTransport::Role::CLIENT:
			{
				value["dtlsParameters"]["role"] = "client";
				break;
			}
			case RTC::DtlsTransport::Role::SERVER:
			{
				value["dtlsParameters"]["role"] = "server";
				break;
			}
			default:
			{
				ERROR_EX_LOG("");
				break;
			}
		}
		// Add dtlsState.
		switch (m_dtls_transport_ptr->GetState())
		{
			case RTC::DtlsTransport::DtlsState::NEW:
			{
				value["dtlsState"] = "new";
				break;
			}
			case RTC::DtlsTransport::DtlsState::CONNECTING:
			{
				value["dtlsState"] = "connecting";
				break;
			}
			case RTC::DtlsTransport::DtlsState::CONNECTED:
			{
				value["dtlsState"] = "connected";
				break;
			}
			case RTC::DtlsTransport::DtlsState::FAILED:
			{
				value["dtlsState"] = "failed";
				break;
			}
			case RTC::DtlsTransport::DtlsState::CLOSED:
			{
				value["dtlsState"] = "closed";
				break;
			}
			default:
			{
				ERROR_EX_LOG("");
				break;
			}
		}
	}
}