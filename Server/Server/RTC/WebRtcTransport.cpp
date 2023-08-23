﻿#define MS_CLASS "RTC::WebRtcTransport"
// #define MS_LOG_DEV_LEVEL 3

#include "RTC/WebRtcTransport.hpp"
#include "Logger.hpp"
#include "MediaSoupErrors.hpp"
#include "Utils.hpp"
#include "Channel/ChannelNotifier.hpp"
#include <cmath> // std::pow()

namespace RTC
{
	/* Static. */

	static constexpr uint16_t IceCandidateDefaultLocalPriority{ 10000 };
	// We just provide "host" candidates so type preference is fixed.
	static constexpr uint16_t IceTypePreference{ 64 };
	// We do not support non rtcp-mux so component is always 1.
	static constexpr uint16_t IceComponent{ 1 };

	static inline uint32_t generateIceCandidatePriority(uint16_t localPreference)
	{
		MS_TRACE();

		return std::pow(2, 24) * IceTypePreference + std::pow(2, 8) * localPreference +
		       std::pow(2, 0) * (256 - IceComponent);
	}

	/* Instance methods. */

	WebRtcTransport::WebRtcTransport(const std::string& id, RTC::Transport::Listener* listener, json& data)
	  : RTC::Transport::Transport(id, listener, data)
	{
		MS_TRACE();
		DEBUG_EX_LOG("[id = %s][data = %s]", id.c_str(), data.dump().c_str());
		bool enableUdp{ true };
		auto jsonEnableUdpIt = data.find("enableUdp");

		if (jsonEnableUdpIt != data.end())
		{
			if (!jsonEnableUdpIt->is_boolean())
			{
				MS_THROW_TYPE_ERROR("wrong enableUdp (not a boolean)");
			}

			enableUdp = jsonEnableUdpIt->get<bool>();
		}

		bool enableTcp{ false };
		auto jsonEnableTcpIt = data.find("enableTcp");

		if (jsonEnableTcpIt != data.end())
		{
			if (!jsonEnableTcpIt->is_boolean())
				MS_THROW_TYPE_ERROR("wrong enableTcp (not a boolean)");

			enableTcp = jsonEnableTcpIt->get<bool>();
		}

		bool preferUdp{ false };
		auto jsonPreferUdpIt = data.find("preferUdp");

		if (jsonPreferUdpIt != data.end())
		{
			if (!jsonPreferUdpIt->is_boolean())
				MS_THROW_TYPE_ERROR("wrong preferUdp (not a boolean)");

			preferUdp = jsonPreferUdpIt->get<bool>();
		}

		bool preferTcp{ false };
		auto jsonPreferTcpIt = data.find("preferTcp");

		if (jsonPreferTcpIt != data.end())
		{
			if (!jsonPreferTcpIt->is_boolean())
				MS_THROW_TYPE_ERROR("wrong preferTcp (not a boolean)");

			preferTcp = jsonPreferTcpIt->get<bool>();
		}

		auto jsonListenIpsIt = data.find("listenIps");

		if (jsonListenIpsIt == data.end())
			MS_THROW_TYPE_ERROR("missing listenIps");
		else if (!jsonListenIpsIt->is_array())
			MS_THROW_TYPE_ERROR("wrong listenIps (not an array)");
		else if (jsonListenIpsIt->empty())
			MS_THROW_TYPE_ERROR("wrong listenIps (empty array)");
		else if (jsonListenIpsIt->size() > 8)
			MS_THROW_TYPE_ERROR("wrong listenIps (too many IPs)");

		std::vector<ListenIp> listenIps(jsonListenIpsIt->size());

		for (size_t i{ 0 }; i < jsonListenIpsIt->size(); ++i)
		{
			auto& jsonListenIp = (*jsonListenIpsIt)[i];
			auto& listenIp     = listenIps[i];

			if (!jsonListenIp.is_object())
				MS_THROW_TYPE_ERROR("wrong listenIp (not an object)");

			auto jsonIpIt = jsonListenIp.find("ip");

			if (jsonIpIt == jsonListenIp.end())
				MS_THROW_TYPE_ERROR("missing listenIp.ip");
			else if (!jsonIpIt->is_string())
				MS_THROW_TYPE_ERROR("wrong listenIp.ip (not an string");

			listenIp.ip.assign(jsonIpIt->get<std::string>());

			// This may throw.
			Utils::IP::NormalizeIp(listenIp.ip);

			auto jsonAnnouncedIpIt = jsonListenIp.find("announcedIp");

			if (jsonAnnouncedIpIt != jsonListenIp.end())
			{
				if (!jsonAnnouncedIpIt->is_string())
					MS_THROW_TYPE_ERROR("wrong listenIp.announcedIp (not an string)");

				listenIp.announcedIp.assign(jsonAnnouncedIpIt->get<std::string>());
			}
		}

		uint16_t port{ 0 };
		auto jsonPortIt = data.find("port");

		if (jsonPortIt != data.end())
		{
			if (!(jsonPortIt->is_number() && Utils::Json::IsPositiveInteger(*jsonPortIt)))
				MS_THROW_TYPE_ERROR("wrong port (not a positive number)");

			port = jsonPortIt->get<uint16_t>();
		}

		try
		{
			uint16_t iceLocalPreferenceDecrement{ 0 };

			if (enableUdp && enableTcp)
				this->iceCandidates.reserve(2 * jsonListenIpsIt->size());
			else
				this->iceCandidates.reserve(jsonListenIpsIt->size());

			for (auto& listenIp : listenIps)
			{
				if (enableUdp)
				{
					// ICE 优先级    就是 从是先开始连接
					uint16_t iceLocalPreference =
					  IceCandidateDefaultLocalPriority - iceLocalPreferenceDecrement;

					if (preferUdp)
						iceLocalPreference += 1000;

					uint32_t icePriority = generateIceCandidatePriority(iceLocalPreference);

					// This may throw.
					RTC::UdpSocket* udpSocket;
					// TODO@chensong 这边port 是使用同一个端口吗？？？
					if (port != 0)
					{
						udpSocket = new RTC::UdpSocket(this, listenIp.ip, port);
					}
					else
					{
						udpSocket = new RTC::UdpSocket(this, listenIp.ip);
					}

					this->udpSockets[udpSocket] = listenIp.announcedIp;

					if (listenIp.announcedIp.empty())
					{
						this->iceCandidates.emplace_back(udpSocket, icePriority);
					}
					else
					{
						this->iceCandidates.emplace_back(udpSocket, icePriority, listenIp.announcedIp);
					}
				}

				if (enableTcp)
				{
					uint16_t iceLocalPreference =
					  IceCandidateDefaultLocalPriority - iceLocalPreferenceDecrement;

					if (preferTcp)
						iceLocalPreference += 1000;

					uint32_t icePriority = generateIceCandidatePriority(iceLocalPreference);

					// This may throw.
					RTC::TcpServer* tcpServer;
					if (port != 0)
						tcpServer = new RTC::TcpServer(this, this, listenIp.ip, port);
					else
						tcpServer = new RTC::TcpServer(this, this, listenIp.ip);

					this->tcpServers[tcpServer] = listenIp.announcedIp;

					if (listenIp.announcedIp.empty())
						this->iceCandidates.emplace_back(tcpServer, icePriority);
					else
						this->iceCandidates.emplace_back(tcpServer, icePriority, listenIp.announcedIp);
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
			this->iceServer = new RTC::IceServer(
			  this, Utils::Crypto::GetRandomString(16), Utils::Crypto::GetRandomString(32));

			// Create a DTLS transport.
			this->dtlsTransport = new RTC::DtlsTransport(this);
		}
		catch (const MediaSoupError& error)
		{
			// Must delete everything since the destructor won't be called.

			delete this->dtlsTransport;
			this->dtlsTransport = nullptr;

			delete this->iceServer;
			this->iceServer = nullptr;

			for (auto& kv : this->udpSockets)
			{
				auto* udpSocket = kv.first;

				delete udpSocket;
			}
			this->udpSockets.clear();

			for (auto& kv : this->tcpServers)
			{
				auto* tcpServer = kv.first;

				delete tcpServer;
			}
			this->tcpServers.clear();

			this->iceCandidates.clear();

			throw;
		}
	}

	WebRtcTransport::~WebRtcTransport()
	{
		MS_TRACE();
		DEBUG_EX_LOG("");
		// Must delete the DTLS transport first since it will generate a DTLS alert
		// to be sent.
		delete this->dtlsTransport;
		this->dtlsTransport = nullptr;

		delete this->iceServer;
		this->iceServer = nullptr;

		for (auto& kv : this->udpSockets)
		{
			auto* udpSocket = kv.first;

			delete udpSocket;
		}
		this->udpSockets.clear();

		for (auto& kv : this->tcpServers)
		{
			auto* tcpServer = kv.first;

			delete tcpServer;
		}
		this->tcpServers.clear();

		this->iceCandidates.clear();

		delete this->srtpSendSession;
		this->srtpSendSession = nullptr;

		delete this->srtpRecvSession;
		this->srtpRecvSession = nullptr;
	}

	void WebRtcTransport::FillJson(json& jsonObject) const
	{
		MS_TRACE();

		// Call the parent method.
		RTC::Transport::FillJson(jsonObject);

		// Add iceRole (we are always "controlled"). ？？？？？？ 
		jsonObject["iceRole"] = "controlled";

		// Add iceParameters.
		jsonObject["iceParameters"] = json::object();
		auto jsonIceParametersIt    = jsonObject.find("iceParameters");
		// 在 STUN 验证的 用户名和密码
		(*jsonIceParametersIt)["usernameFragment"] = this->iceServer->GetUsernameFragment();
		(*jsonIceParametersIt)["password"]         = this->iceServer->GetPassword();
		(*jsonIceParametersIt)["iceLite"]          = true;

		// Add iceCandidates.
		jsonObject["iceCandidates"] = json::array();
		auto jsonIceCandidatesIt    = jsonObject.find("iceCandidates");

		for (size_t i{ 0 }; i < this->iceCandidates.size(); ++i)
		{
			jsonIceCandidatesIt->emplace_back(json::value_t::object);

			auto& jsonEntry    = (*jsonIceCandidatesIt)[i];
			auto& iceCandidate = this->iceCandidates[i];

			iceCandidate.FillJson(jsonEntry);
		}

		// Add iceState.
		switch (this->iceServer->GetState())
		{
			case RTC::IceServer::IceState::NEW:
				jsonObject["iceState"] = "new";
				break;
			case RTC::IceServer::IceState::CONNECTED:
				jsonObject["iceState"] = "connected";
				break;
			case RTC::IceServer::IceState::COMPLETED:
				jsonObject["iceState"] = "completed";
				break;
			case RTC::IceServer::IceState::DISCONNECTED:
				jsonObject["iceState"] = "disconnected";
				break;
		}

		// Add iceSelectedTuple.
		if (this->iceServer->GetSelectedTuple())
		{
			this->iceServer->GetSelectedTuple()->FillJson(jsonObject["iceSelectedTuple"]);
		}

		// Add dtlsParameters.
		jsonObject["dtlsParameters"] = json::object();
		auto jsonDtlsParametersIt    = jsonObject.find("dtlsParameters");

		// Add dtlsParameters.fingerprints.
		(*jsonDtlsParametersIt)["fingerprints"] = json::array();
		auto jsonDtlsParametersFingerprintsIt   = jsonDtlsParametersIt->find("fingerprints");
		auto& fingerprints                      = this->dtlsTransport->GetLocalFingerprints();

		for (size_t i{ 0 }; i < fingerprints.size(); ++i)
		{
			jsonDtlsParametersFingerprintsIt->emplace_back(json::value_t::object);

			auto& jsonEntry   = (*jsonDtlsParametersFingerprintsIt)[i];
			auto& fingerprint = fingerprints[i];

			jsonEntry["algorithm"] =
			  RTC::DtlsTransport::GetFingerprintAlgorithmString(fingerprint.algorithm);
			jsonEntry["value"] = fingerprint.value;
		}

		// Add dtlsParameters.role.
		switch (this->dtlsRole)
		{
			case RTC::DtlsTransport::Role::NONE:
				(*jsonDtlsParametersIt)["role"] = "none";
				break;
			case RTC::DtlsTransport::Role::AUTO:
				(*jsonDtlsParametersIt)["role"] = "auto";
				break;
			case RTC::DtlsTransport::Role::CLIENT:
				(*jsonDtlsParametersIt)["role"] = "client";
				break;
			case RTC::DtlsTransport::Role::SERVER:
				(*jsonDtlsParametersIt)["role"] = "server";
				break;
		}

		// Add dtlsState.
		switch (this->dtlsTransport->GetState())
		{
			case RTC::DtlsTransport::DtlsState::NEW:
				jsonObject["dtlsState"] = "new";
				break;
			case RTC::DtlsTransport::DtlsState::CONNECTING:
				jsonObject["dtlsState"] = "connecting";
				break;
			case RTC::DtlsTransport::DtlsState::CONNECTED:
				jsonObject["dtlsState"] = "connected";
				break;
			case RTC::DtlsTransport::DtlsState::FAILED:
				jsonObject["dtlsState"] = "failed";
				break;
			case RTC::DtlsTransport::DtlsState::CLOSED:
				jsonObject["dtlsState"] = "closed";
				break;
		}

		DEBUG_EX_LOG("jsonObject  = %s", jsonObject.dump().c_str());
	}

	void WebRtcTransport::FillJsonStats(json& jsonArray)
	{
		MS_TRACE();

		// Call the parent method.
		RTC::Transport::FillJsonStats(jsonArray);

		auto& jsonObject = jsonArray[0];

		// Add type.
		jsonObject["type"] = "webrtc-transport";

		// Add iceRole (we are always "controlled").
		jsonObject["iceRole"] = "controlled";

		// Add iceState.
		switch (this->iceServer->GetState())
		{
			case RTC::IceServer::IceState::NEW:
				jsonObject["iceState"] = "new";
				break;
			case RTC::IceServer::IceState::CONNECTED:
				jsonObject["iceState"] = "connected";
				break;
			case RTC::IceServer::IceState::COMPLETED:
				jsonObject["iceState"] = "completed";
				break;
			case RTC::IceServer::IceState::DISCONNECTED:
				jsonObject["iceState"] = "disconnected";
				break;
		}

		if (this->iceServer->GetSelectedTuple())
		{
			// Add iceSelectedTuple.
			this->iceServer->GetSelectedTuple()->FillJson(jsonObject["iceSelectedTuple"]);
		}

		// Add dtlsState.
		switch (this->dtlsTransport->GetState())
		{
			case RTC::DtlsTransport::DtlsState::NEW:
				jsonObject["dtlsState"] = "new";
				break;
			case RTC::DtlsTransport::DtlsState::CONNECTING:
				jsonObject["dtlsState"] = "connecting";
				break;
			case RTC::DtlsTransport::DtlsState::CONNECTED:
				jsonObject["dtlsState"] = "connected";
				break;
			case RTC::DtlsTransport::DtlsState::FAILED:
				jsonObject["dtlsState"] = "failed";
				break;
			case RTC::DtlsTransport::DtlsState::CLOSED:
				jsonObject["dtlsState"] = "closed";
				break;
		}
		DEBUG_EX_LOG("jsonArray = %s", jsonArray.dump().c_str());
	}

	void WebRtcTransport::HandleRequest(Channel::ChannelRequest* request)
	{
		MS_TRACE();
		DEBUG_EX_LOG("[method = %s][data = %s][internal = %s]", request->method.c_str(), request->data.dump().c_str(), request->internal.dump().c_str());
		switch (request->methodId)
		{
			case Channel::ChannelRequest::MethodId::TRANSPORT_CONNECT:
			{
				/*data = 
				{
					"dtlsParameters":
					{
						"fingerprints":
						[
							{
							"algorithm":"sha-256",
							"value":"42:38:72:B0:0F:AC:5E:C0:F7:62:B0:F8:41:81:A9:A8:EC:AD:0D:8D:AF:35:A8:DF:3A:50:BA:43:25:F8:74:C6"
							}
						],
						"role":"client"
					}
				}

				internal = {
					"routerId":"5bd717af-b754-48f5-bd21-2a0321815863",
					"transportId":"c2499ff3-b11e-4ad2-823a-c8934dbbe672"
				}

				*/

				// Ensure this method is not called twice.
				if (this->connectCalled)
				{
					MS_THROW_ERROR("connect() already called");
				}

				RTC::DtlsTransport::Fingerprint dtlsRemoteFingerprint;
				RTC::DtlsTransport::Role dtlsRemoteRole;

				auto jsonDtlsParametersIt = request->data.find("dtlsParameters");

				if (jsonDtlsParametersIt == request->data.end() || !jsonDtlsParametersIt->is_object())
					MS_THROW_TYPE_ERROR("missing dtlsParameters");

				auto jsonFingerprintsIt = jsonDtlsParametersIt->find("fingerprints");

				if (jsonFingerprintsIt == jsonDtlsParametersIt->end() || !jsonFingerprintsIt->is_array())
				{
					MS_THROW_TYPE_ERROR("missing dtlsParameters.fingerprints");
				}
				else if (jsonFingerprintsIt->empty())
				{
					MS_THROW_TYPE_ERROR("empty dtlsParameters.fingerprints array");
				}

				// NOTE: Just take the first fingerprint.
				for (auto& jsonFingerprint : *jsonFingerprintsIt)
				{
					if (!jsonFingerprint.is_object())
						MS_THROW_TYPE_ERROR("wrong entry in dtlsParameters.fingerprints (not an object)");

					auto jsonAlgorithmIt = jsonFingerprint.find("algorithm");

					if (jsonAlgorithmIt == jsonFingerprint.end())
						MS_THROW_TYPE_ERROR("missing fingerprint.algorithm");
					else if (!jsonAlgorithmIt->is_string())
						MS_THROW_TYPE_ERROR("wrong fingerprint.algorithm (not a string)");

					dtlsRemoteFingerprint.algorithm =
					  RTC::DtlsTransport::GetFingerprintAlgorithm(jsonAlgorithmIt->get<std::string>());

					if (dtlsRemoteFingerprint.algorithm == RTC::DtlsTransport::FingerprintAlgorithm::NONE)
					{
						MS_THROW_TYPE_ERROR("invalid fingerprint.algorithm value");
					}

					auto jsonValueIt = jsonFingerprint.find("value");

					if (jsonValueIt == jsonFingerprint.end())
						MS_THROW_TYPE_ERROR("missing fingerprint.value");
					else if (!jsonValueIt->is_string())
						MS_THROW_TYPE_ERROR("wrong fingerprint.value (not a string)");

					dtlsRemoteFingerprint.value = jsonValueIt->get<std::string>();

					// Just use the first fingerprint.
					break;
				}

				auto jsonRoleIt = jsonDtlsParametersIt->find("role");

				if (jsonRoleIt != jsonDtlsParametersIt->end())
				{
					if (!jsonRoleIt->is_string())
						MS_THROW_TYPE_ERROR("wrong dtlsParameters.role (not a string)");

					dtlsRemoteRole = RTC::DtlsTransport::StringToRole(jsonRoleIt->get<std::string>());

					if (dtlsRemoteRole == RTC::DtlsTransport::Role::NONE)
						MS_THROW_TYPE_ERROR("invalid dtlsParameters.role value");
				}
				else
				{
					
				}

				// Set local DTLS role.
				switch (dtlsRemoteRole)
				{
					case RTC::DtlsTransport::Role::CLIENT:
					{
						this->dtlsRole = RTC::DtlsTransport::Role::SERVER;

						break;
					}
					// If the peer has role "auto" we become "client" since we are ICE controlled.
					case RTC::DtlsTransport::Role::SERVER:
					case RTC::DtlsTransport::Role::AUTO:
					{
						this->dtlsRole = RTC::DtlsTransport::Role::CLIENT;

						break;
					}
					case RTC::DtlsTransport::Role::NONE:
					{
						MS_THROW_TYPE_ERROR("invalid remote DTLS role");
					}
				}

				this->connectCalled = true;

				// Pass the remote fingerprint to the DTLS transport.
				if (this->dtlsTransport->SetRemoteFingerprint(dtlsRemoteFingerprint))
				{
					// If everything is fine, we may run the DTLS transport if ready.
					MayRunDtlsTransport();
				}

				// Tell the caller about the selected local DTLS role.
				json data = json::object();
				// 角色  
				switch (this->dtlsRole)
				{
					case RTC::DtlsTransport::Role::CLIENT:
						data["dtlsLocalRole"] = "client";
						break;

					case RTC::DtlsTransport::Role::SERVER:
						data["dtlsLocalRole"] = "server";
						break;

					default:
						MS_ABORT("invalid local DTLS role");
				}
				//DEBUG_EX_LOG("[reply = %s]", data.dump().c_str());
				request->Accept(data);

				break;
			}

			case Channel::ChannelRequest::MethodId::TRANSPORT_RESTART_ICE:
			{
				// TODO@chensong 2022-04-17  从新启动ICE服务  ->  生成新的用户和密码
				std::string usernameFragment = Utils::Crypto::GetRandomString(16);
				std::string password         = Utils::Crypto::GetRandomString(32);

				this->iceServer->SetUsernameFragment(usernameFragment);
				this->iceServer->SetPassword(password);

				MS_DEBUG_DEV(
				  "WebRtcTransport ICE usernameFragment and password changed [id:%s]", this->id.c_str());

				// Reply with the updated ICE local parameters.
				json data = json::object();

				data["iceParameters"]    = json::object();
				auto jsonIceParametersIt = data.find("iceParameters");

				(*jsonIceParametersIt)["usernameFragment"] = this->iceServer->GetUsernameFragment();
				(*jsonIceParametersIt)["password"]         = this->iceServer->GetPassword();
				(*jsonIceParametersIt)["iceLite"]          = true;
				DEBUG_EX_LOG("reply = %s", data.dump().c_str());
				request->Accept(data);

				break;
			}

			default:
			{
				// Pass it to the parent class.
				RTC::Transport::HandleRequest(request);
			}
		}
	}

	void WebRtcTransport::HandleNotification(PayloadChannel::Notification* notification)
	{
		MS_TRACE();
		//DEBUG_EX_LOG("[event = %s][data = %s][%s]", notification->event.c_str(), notification->data.dump().c_str(), notification->internal.dump().c_str());
		// Pass it to the parent class.
		RTC::Transport::HandleNotification(notification);
	}

	inline bool WebRtcTransport::IsConnected() const
	{
		MS_TRACE();
		//DEBUG_EX_LOG("");
		// clang-format off
		return (
			(
				this->iceServer->GetState() == RTC::IceServer::IceState::CONNECTED ||
				this->iceServer->GetState() == RTC::IceServer::IceState::COMPLETED
			) &&
			this->dtlsTransport->GetState() == RTC::DtlsTransport::DtlsState::CONNECTED
		);
		// clang-format on
	}

	void WebRtcTransport::MayRunDtlsTransport()
	{
		MS_TRACE();
		//DEBUG_EX_LOG("");
		// Do nothing if we have the same local DTLS role as the DTLS transport.
		// NOTE: local role in DTLS transport can be NONE, but not ours.
		if (this->dtlsTransport->GetLocalRole() == this->dtlsRole)
		{
			return;
		}

		// Check our local DTLS role.
		switch (this->dtlsRole)
		{
			// If still 'auto' then transition to 'server' if ICE is 'connected' or
			// 'completed'.
			case RTC::DtlsTransport::Role::AUTO:
			{
				// clang-format off
				if (
					this->iceServer->GetState() == RTC::IceServer::IceState::CONNECTED ||
					this->iceServer->GetState() == RTC::IceServer::IceState::COMPLETED
				)
				// clang-format on
				{
					MS_DEBUG_TAG(
					  dtls, "transition from DTLS local role 'auto' to 'server' and running DTLS transport");

					this->dtlsRole = RTC::DtlsTransport::Role::SERVER;
					this->dtlsTransport->Run(RTC::DtlsTransport::Role::SERVER);
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
					this->iceServer->GetState() == RTC::IceServer::IceState::CONNECTED ||
					this->iceServer->GetState() == RTC::IceServer::IceState::COMPLETED
				)
				// clang-format on
				{
					MS_DEBUG_TAG(dtls, "running DTLS transport in local role 'client'");

					this->dtlsTransport->Run(RTC::DtlsTransport::Role::CLIENT);
				}

				break;
			}

			// If 'server' then run the DTLS transport if ICE is 'connected' (not yet
			// USE-CANDIDATE) or 'completed'.
			case RTC::DtlsTransport::Role::SERVER:
			{
				// clang-format off
				if (
					this->iceServer->GetState() == RTC::IceServer::IceState::CONNECTED ||
					this->iceServer->GetState() == RTC::IceServer::IceState::COMPLETED
				)
				// clang-format on
				{
					MS_DEBUG_TAG(dtls, "running DTLS transport in local role 'server'");

					this->dtlsTransport->Run(RTC::DtlsTransport::Role::SERVER);
				}

				break;
			}

			case RTC::DtlsTransport::Role::NONE:
			{
				MS_ABORT("local DTLS role not set");
			}
		}
	}

	void WebRtcTransport::SendRtpPacket(
	  RTC::Consumer* /*consumer*/, RTC::RtpPacket* packet, RTC::Transport::onSendCallback* cb)
	{
		MS_TRACE();
		//DEBUG_EX_LOG("");
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
		if (!this->srtpSendSession)
		{
			MS_WARN_DEV("ignoring RTP packet due to non sending SRTP session");

			if (cb)
			{
				(*cb)(false);
				delete cb;
			}

			return;
		}

		const uint8_t* data = packet->GetData();
		size_t len          = packet->GetSize();

		if (!this->srtpSendSession->EncryptRtp(&data, &len))
		{
			if (cb)
			{
				(*cb)(false);
				delete cb;
			}

			return;
		}

		this->iceServer->GetSelectedTuple()->Send(data, len, cb);

		// Increase send transmission.
		RTC::Transport::DataSent(len);
	}

	void WebRtcTransport::SendRtcpPacket(RTC::RTCP::Packet* packet)
	{
		MS_TRACE();
	//	DEBUG_EX_LOG("");
		if (!IsConnected())
			return;

		const uint8_t* data = packet->GetData();
		size_t len          = packet->GetSize();

		// Ensure there is sending SRTP session.
		if (!this->srtpSendSession)
		{
			MS_WARN_DEV("ignoring RTCP packet due to non sending SRTP session");

			return;
		}

		if (!this->srtpSendSession->EncryptRtcp(&data, &len))
			return;

		this->iceServer->GetSelectedTuple()->Send(data, len);

		// Increase send transmission.
		RTC::Transport::DataSent(len);
	}

	void WebRtcTransport::SendRtcpCompoundPacket(RTC::RTCP::CompoundPacket* packet)
	{
		MS_TRACE();
		
		if (!IsConnected())
			return;

		const uint8_t* data = packet->GetData();
		size_t len          = packet->GetSize();
		//DEBUG_EX_LOG("len = %lu", len);
		// Ensure there is sending SRTP session.
		if (!this->srtpSendSession)
		{
			MS_WARN_TAG(rtcp, "ignoring RTCP compound packet due to non sending SRTP session");

			return;
		}

		if (!this->srtpSendSession->EncryptRtcp(&data, &len))
			return;

		this->iceServer->GetSelectedTuple()->Send(data, len);

		// Increase send transmission.
		RTC::Transport::DataSent(len);
	}

	void WebRtcTransport::SendMessage(
	  RTC::DataConsumer* dataConsumer, uint32_t ppid, const uint8_t* msg, size_t len, onQueuedCallback* cb)
	{
		MS_TRACE();
		//DEBUG_EX_LOG("msg = %s", msg);
		this->sctpAssociation->SendSctpMessage(dataConsumer, ppid, msg, len, cb);
	}

	void WebRtcTransport::SendSctpData(const uint8_t* data, size_t len)
	{
		MS_TRACE();
		//DEBUG_EX_LOG("");
		// clang-format on
		if (!IsConnected())
		{
			MS_WARN_TAG(sctp, "DTLS not connected, cannot send SCTP data");

			return;
		}

		this->dtlsTransport->SendApplicationData(data, len);
	}

	void WebRtcTransport::RecvStreamClosed(uint32_t ssrc)
	{
		MS_TRACE();
		//DEBUG_EX_LOG("ssrc = %lu", ssrc);
		if (this->srtpRecvSession)
		{
			this->srtpRecvSession->RemoveStream(ssrc);
		}
	}

	void WebRtcTransport::SendStreamClosed(uint32_t ssrc)
	{
		MS_TRACE();
		//DEBUG_EX_LOG("ssrc = %lu", ssrc);
		if (this->srtpSendSession)
		{
			this->srtpSendSession->RemoveStream(ssrc);
		}
	}
	// TODO@chensong 处理底层协议 RTCP/RTP/STUN/ICE
	inline void WebRtcTransport::OnPacketReceived(
	  RTC::TransportTuple* tuple, const uint8_t* data, size_t len)
	{
		MS_TRACE();
		
		// Increase receive transmission.
		RTC::Transport::DataReceived(len);

		// Check if it's STUN.
		if (RTC::StunPacket::IsStun(data, len))
		{
//<<<<<<< HEAD
//			DEBUG_EX_LOG("stun");
//=======
			//DEBUG_EX_ID_LOG("stun");
//>>>>>>> d40fa1c367378f962a8c8dd093974a106997055a
			OnStunDataReceived(tuple, data, len);
		}
		// Check if it's RTCP.
		else if (RTC::RTCP::Packet::IsRtcp(data, len))
		{
//<<<<<<< HEAD
//			DEBUG_EX_LOG("IsRtcp");
//=======
			//DEBUG_EX_ID_LOG("IsRtcp");
//>>>>>>> d40fa1c367378f962a8c8dd093974a106997055a
			OnRtcpDataReceived(tuple, data, len);
		}
		// Check if it's RTP.
		else if (RTC::RtpPacket::IsRtp(data, len))
		{
//<<<<<<< HEAD
//			DEBUG_EX_LOG("IsRtp");
//=======
			//DEBUG_EX_ID_LOG("IsRtp");
//>>>>>>> d40fa1c367378f962a8c8dd093974a106997055a
			OnRtpDataReceived(tuple, data, len);
		}
		// Check if it's DTLS.
		else if (RTC::DtlsTransport::IsDtls(data, len))
		{
//<<<<<<< HEAD
//			DEBUG_EX_LOG("IsDtls");
//=======
//<<<<<<< HEAD
//			DEBUG_EX_LOG("IsDtls");
//=======
//			DEBUG_EX_ID_LOG("IsDtls"); // 这边修改DTLS的状态的哈 ？？
//>>>>>>> 69463cce016535ae4b8531ff725a35bc270954e5
//>>>>>>> d40fa1c367378f962a8c8dd093974a106997055a
			OnDtlsDataReceived(tuple, data, len);
		}
		else
		{
//<<<<<<< HEAD
//			DEBUG_EX_LOG("error type");
//=======
	//		DEBUG_EX_ID_LOG("error type");
//>>>>>>> d40fa1c367378f962a8c8dd093974a106997055a
			MS_WARN_DEV("ignoring received packet of unknown type");
		}
	}

	inline void WebRtcTransport::OnStunDataReceived(
	  RTC::TransportTuple* tuple, const uint8_t* data, size_t len)
	{
		MS_TRACE();
		//DEBUG_EX_LOG("len = %lu", len);
		RTC::StunPacket* packet = RTC::StunPacket::Parse(data, len);

		if (!packet)
		{
			MS_WARN_DEV("ignoring wrong STUN packet received");

			return;
		}
//		DEBUG_EX_ID_LOG("[]");
		// Pass it to the IceServer.
		this->iceServer->ProcessStunPacket(packet, tuple);

		delete packet;
	}

	inline void WebRtcTransport::OnDtlsDataReceived(
	  const RTC::TransportTuple* tuple, const uint8_t* data, size_t len)
	{
		MS_TRACE();
	//	DEBUG_EX_LOG("len = %lu", len);
		// Ensure it comes from a valid tuple.
		if (!this->iceServer->IsValidTuple(tuple))
		{
			MS_WARN_TAG(dtls, "ignoring DTLS data coming from an invalid tuple");

			return;
		}

		// Trick for clients performing aggressive ICE regardless we are ICE-Lite.
		this->iceServer->ForceSelectedTuple(tuple);

		// Check that DTLS status is 'connecting' or 'connected'.
		if (
		  this->dtlsTransport->GetState() == RTC::DtlsTransport::DtlsState::CONNECTING ||
		  this->dtlsTransport->GetState() == RTC::DtlsTransport::DtlsState::CONNECTED)
		{
			MS_DEBUG_DEV("DTLS data received, passing it to the DTLS transport");
			// 这边修改dtls 连接状态 了
			this->dtlsTransport->ProcessDtlsData(data, len);
		}
		else
		{
			MS_WARN_TAG(dtls, "Transport is not 'connecting' or 'connected', ignoring received DTLS data");

			return;
		}
	}

	inline void WebRtcTransport::OnRtpDataReceived(
	  RTC::TransportTuple* tuple, const uint8_t* data, size_t len)
	{
		MS_TRACE();
	//	DEBUG_EX_LOG("len = %lu", len);
		// Ensure DTLS is connected.
		if (this->dtlsTransport->GetState() != RTC::DtlsTransport::DtlsState::CONNECTED)
		{
			MS_DEBUG_2TAGS(dtls, rtp, "ignoring RTP packet while DTLS not connected");

			return;
		}

		// Ensure there is receiving SRTP session.
		if (!this->srtpRecvSession)
		{
			MS_DEBUG_TAG(srtp, "ignoring RTP packet due to non receiving SRTP session");

			return;
		}

		// Ensure it comes from a valid tuple.
		if (!this->iceServer->IsValidTuple(tuple))
		{
			MS_WARN_TAG(rtp, "ignoring RTP packet coming from an invalid tuple");

			return;
		}

		// Decrypt the SRTP packet.
		if (!this->srtpRecvSession->DecryptSrtp(const_cast<uint8_t*>(data), &len))
		{
			RTC::RtpPacket* packet = RTC::RtpPacket::Parse(data, len);

			if (!packet)
			{
				MS_WARN_TAG(srtp, "DecryptSrtp() failed due to an invalid RTP packet");
			}
			else
			{
				MS_WARN_TAG(
				  srtp,
				  "DecryptSrtp() failed [ssrc:%" PRIu32 ", payloadType:%" PRIu8 ", seq:%" PRIu16 "]",
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
			MS_WARN_TAG(rtp, "received data is not a valid RTP packet");

			return;
		}

		// Trick for clients performing aggressive ICE regardless we are ICE-Lite.
		this->iceServer->ForceSelectedTuple(tuple);

		// Pass the packet to the parent transport.
		RTC::Transport::ReceiveRtpPacket(packet);
	}

	inline void WebRtcTransport::OnRtcpDataReceived(
	  RTC::TransportTuple* tuple, const uint8_t* data, size_t len)
	{
		MS_TRACE();
	//	DEBUG_EX_LOG("len = %lu", len);
		// Ensure DTLS is connected.
		if (this->dtlsTransport->GetState() != RTC::DtlsTransport::DtlsState::CONNECTED)
		{
			MS_DEBUG_2TAGS(dtls, rtcp, "ignoring RTCP packet while DTLS not connected");

			return;
		}

		// Ensure there is receiving SRTP session.
		if (!this->srtpRecvSession)
		{
			MS_DEBUG_TAG(srtp, "ignoring RTCP packet due to non receiving SRTP session");

			return;
		}

		// Ensure it comes from a valid tuple.
		if (!this->iceServer->IsValidTuple(tuple))
		{
			MS_WARN_TAG(rtcp, "ignoring RTCP packet coming from an invalid tuple");

			return;
		}

		// Decrypt the SRTCP packet.
		if (!this->srtpRecvSession->DecryptSrtcp(const_cast<uint8_t*>(data), &len))
			return;

		RTC::RTCP::Packet* packet = RTC::RTCP::Packet::Parse(data, len);

		if (!packet)
		{
			MS_WARN_TAG(rtcp, "received data is not a valid RTCP compound or single packet");

			return;
		}

		// Pass the packet to the parent transport.
		RTC::Transport::ReceiveRtcpPacket(packet);
	}
	//TODO@chensong 2022-04-17 底层发送到上层 
	inline void WebRtcTransport::OnUdpSocketPacketReceived(
	  RTC::UdpSocket* socket, const uint8_t* data, size_t len, const struct sockaddr* remoteAddr)
	{
		MS_TRACE();
	//	DEBUG_EX_LOG("len = %lu", len);
		RTC::TransportTuple tuple(socket, remoteAddr);

		OnPacketReceived(&tuple, data, len);
	}

	inline void WebRtcTransport::OnRtcTcpConnectionClosed(
	  RTC::TcpServer* /*tcpServer*/, RTC::TcpConnection* connection)
	{
		MS_TRACE();
	//	DEBUG_EX_LOG("");
		RTC::TransportTuple tuple(connection);

		this->iceServer->RemoveTuple(&tuple);
	}

	inline void WebRtcTransport::OnTcpConnectionPacketReceived(
	  RTC::TcpConnection* connection, const uint8_t* data, size_t len)
	{
		MS_TRACE();
		//DEBUG_EX_LOG("len = %lu", len);
		RTC::TransportTuple tuple(connection);

		OnPacketReceived(&tuple, data, len);
	}

	inline void WebRtcTransport::OnIceServerSendStunPacket(
	  const RTC::IceServer* /*iceServer*/, const RTC::StunPacket* packet, RTC::TransportTuple* tuple)
	{
		MS_TRACE();
		//DEBUG_EX_LOG("");
		// Send the STUN response over the same transport tuple.
		tuple->Send(packet->GetData(), packet->GetSize());

		// Increase send transmission.
		RTC::Transport::DataSent(packet->GetSize());
	}

	inline void WebRtcTransport::OnIceServerSelectedTuple(
	  const RTC::IceServer* /*iceServer*/, RTC::TransportTuple* /*tuple*/)
	{
		MS_TRACE();

		/*
		 * RFC 5245 section 11.2 "Receiving Media":
		 *
		 * ICE implementations MUST be prepared to receive media on each component
		 * on any candidates provided for that component.
		 */

		MS_DEBUG_TAG(ice, "ICE selected tuple");

		// Notify the Node WebRtcTransport.
		json data = json::object();

		this->iceServer->GetSelectedTuple()->FillJson(data["iceSelectedTuple"]);
		DEBUG_EX_LOG("data = %s", data.dump().c_str());
		Channel::ChannelNotifier::Emit(this->id, "iceselectedtuplechange", data);
	}

	inline void WebRtcTransport::OnIceServerConnected(const RTC::IceServer* /*iceServer*/)
	{
		MS_TRACE();

		MS_DEBUG_TAG(ice, "ICE connected");

		// Notify the Node WebRtcTransport.
		json data = json::object();

		data["iceState"] = "connected";

		Channel::ChannelNotifier::Emit(this->id, "icestatechange", data);

		// If ready, run the DTLS handler.
		MayRunDtlsTransport();

		// If DTLS was already connected, notify the parent class.
		if (this->dtlsTransport->GetState() == RTC::DtlsTransport::DtlsState::CONNECTED)
		{
			RTC::Transport::Connected();
		}

		DEBUG_EX_LOG("data = %s", data.dump().c_str());
	}

	inline void WebRtcTransport::OnIceServerCompleted(const RTC::IceServer* /*iceServer*/)
	{
		MS_TRACE();

		MS_DEBUG_TAG(ice, "ICE completed");

		// Notify the Node WebRtcTransport.
		json data = json::object();

		data["iceState"] = "completed";

		Channel::ChannelNotifier::Emit(this->id, "icestatechange", data);

		// If ready, run the DTLS handler.
		MayRunDtlsTransport();

		// If DTLS was already connected, notify the parent class.
		if (this->dtlsTransport->GetState() == RTC::DtlsTransport::DtlsState::CONNECTED)
		{
			RTC::Transport::Connected();
		}
		DEBUG_EX_LOG("data = %s", data.dump().c_str());
	}

	inline void WebRtcTransport::OnIceServerDisconnected(const RTC::IceServer* /*iceServer*/)
	{
		MS_TRACE();

		MS_DEBUG_TAG(ice, "ICE disconnected");

		// Notify the Node WebRtcTransport.
		json data = json::object();

		data["iceState"] = "disconnected";

		Channel::ChannelNotifier::Emit(this->id, "icestatechange", data);

		// If DTLS was already connected, notify the parent class.
		if (this->dtlsTransport->GetState() == RTC::DtlsTransport::DtlsState::CONNECTED)
		{
			RTC::Transport::Disconnected();
		}
		//DEBUG_EX_LOG("data = %s", data.dump().c_str());
	}

	inline void WebRtcTransport::OnDtlsTransportConnecting(const RTC::DtlsTransport* /*dtlsTransport*/)
	{
		MS_TRACE();

		MS_DEBUG_TAG(dtls, "DTLS connecting");

		// Notify the Node WebRtcTransport.
		json data = json::object();

		data["dtlsState"] = "connecting";
		//DEBUG_EX_LOG("data = %s", data.dump().c_str());
		Channel::ChannelNotifier::Emit(this->id, "dtlsstatechange", data);
	}

	inline void WebRtcTransport::OnDtlsTransportConnected(
	  const RTC::DtlsTransport* /*dtlsTransport*/,
	  RTC::SrtpSession::CryptoSuite srtpCryptoSuite,
	  uint8_t* srtpLocalKey,
	  size_t srtpLocalKeyLen,
	  uint8_t* srtpRemoteKey,
	  size_t srtpRemoteKeyLen,
	  std::string& remoteCert)
	{
		MS_TRACE();

		MS_DEBUG_TAG(dtls, "DTLS connected");

		// Close it if it was already set and update it.
		delete this->srtpSendSession;
		this->srtpSendSession = nullptr;

		delete this->srtpRecvSession;
		this->srtpRecvSession = nullptr;

		try
		{
			this->srtpSendSession = new RTC::SrtpSession(
			  RTC::SrtpSession::Type::OUTBOUND, srtpCryptoSuite, srtpLocalKey, srtpLocalKeyLen);
		}
		catch (const MediaSoupError& error)
		{
			MS_ERROR("error creating SRTP sending session: %s", error.what());
		}

		try
		{
			this->srtpRecvSession = new RTC::SrtpSession(
			  RTC::SrtpSession::Type::INBOUND, srtpCryptoSuite, srtpRemoteKey, srtpRemoteKeyLen);

			// Notify the Node WebRtcTransport.
			json data = json::object();

			data["dtlsState"]      = "connected";
			data["dtlsRemoteCert"] = remoteCert;

			Channel::ChannelNotifier::Emit(this->id, "dtlsstatechange", data);
			DEBUG_EX_LOG("data = %s", data.dump().c_str());
			// Tell the parent class.
			RTC::Transport::Connected();
		}
		catch (const MediaSoupError& error)
		{
			MS_ERROR("error creating SRTP receiving session: %s", error.what());

			delete this->srtpSendSession;
			this->srtpSendSession = nullptr;
		}
	}

	inline void WebRtcTransport::OnDtlsTransportFailed(const RTC::DtlsTransport* /*dtlsTransport*/)
	{
		MS_TRACE();

		MS_WARN_TAG(dtls, "DTLS failed");

		// Notify the Node WebRtcTransport.
		json data = json::object();

		data["dtlsState"] = "failed";
		//DEBUG_EX_LOG("data = %s", data.dump().c_str());
		Channel::ChannelNotifier::Emit(this->id, "dtlsstatechange", data);
	}

	inline void WebRtcTransport::OnDtlsTransportClosed(const RTC::DtlsTransport* /*dtlsTransport*/)
	{
		MS_TRACE();

		MS_WARN_TAG(dtls, "DTLS remotely closed");

		// Notify the Node WebRtcTransport.
		json data = json::object();

		data["dtlsState"] = "closed";
		//DEBUG_EX_LOG("data = %s", data.dump().c_str());
		Channel::ChannelNotifier::Emit(this->id, "dtlsstatechange", data);

		// Tell the parent class.
		RTC::Transport::Disconnected();
	}

	inline void WebRtcTransport::OnDtlsTransportSendData(
	  const RTC::DtlsTransport* /*dtlsTransport*/, const uint8_t* data, size_t len)
	{
		MS_TRACE();

		if (!this->iceServer->GetSelectedTuple())
		{
			MS_WARN_TAG(dtls, "no selected tuple set, cannot send DTLS packet");

			return;
		}
//<<<<<<< HEAD
//		DEBUG_EX_LOG("len = %lu", len);
//=======
		//DEBUG_EX_LOG("len = %lu", len); 

		// TODO@chensong 20220522     
		//TLSv1.3 发送 Server Hello、 Certificate、Certificate Status、 Server key Exchange、 Server Hello Done
//>>>>>>> d40fa1c367378f962a8c8dd093974a106997055a
		this->iceServer->GetSelectedTuple()->Send(data, len);

		// Increase send transmission.
		RTC::Transport::DataSent(len);
	}

	inline void WebRtcTransport::OnDtlsTransportApplicationDataReceived(
	  const RTC::DtlsTransport* /*dtlsTransport*/, const uint8_t* data, size_t len)
	{
		MS_TRACE();
		//DEBUG_EX_LOG("len = %lu", len);
		// Pass it to the parent transport.
		RTC::Transport::ReceiveSctpData(data, len);
	}
} // namespace RTC
