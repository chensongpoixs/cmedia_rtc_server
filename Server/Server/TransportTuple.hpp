#ifndef MS_RTC_TRANSPORT_TUPLE_HPP
#define MS_RTC_TRANSPORT_TUPLE_HPP

//#include "common.hpp"
//#include "Utils.hpp"
//#include "RTC/TcpConnection.hpp"
//#include "RTC/UdpSocket.hpp"
//#include <nlohmann/json.hpp>
#include <string>
#include "cudp_socket.h"
#include "cuv_ip.h"
#include "cuv_util.h"
#include "clog.h"
#include <json/json.h>
//using json = nlohmann::json;

namespace RTC
{
	class TransportTuple
	{
	protected:
		//using onSendCallback = const std::function<void(bool sent)>;
		typedef std::function<void(bool sent)> onSendCallback;
	public:
		enum class Protocol
		{
			UDP = 1,
			TCP
		};

	public:
		TransportTuple(chen::cudp_socket* udpSocket, const struct sockaddr* udpRemoteAddr)
		  : udpSocket(udpSocket), udpRemoteAddr((struct sockaddr*)udpRemoteAddr), protocol(Protocol::UDP)
		{
		}

		/*explicit TransportTuple(RTC::TcpConnection* tcpConnection)
		  : tcpConnection(tcpConnection), protocol(Protocol::TCP)
		{
		}*/

		explicit TransportTuple(const TransportTuple* tuple)
		  : udpSocket(tuple->udpSocket)
		  , udpRemoteAddr(tuple->udpRemoteAddr)
		  /*, tcpConnection(tuple->tcpConnection)*/
		  , localAnnouncedIp(tuple->localAnnouncedIp)
		  , protocol(tuple->protocol)
		{
			if (protocol == TransportTuple::Protocol::UDP)
			{
				StoreUdpRemoteAddress();
			}
		}

	public:
		//void FillJson(json& jsonObject) const;
		void reply(Json::Value & value) const;
		void Dump() const;

		void StoreUdpRemoteAddress()
		{
			// Clone the given address into our address storage and make the sockaddr
			// pointer point to it.
			this->udpRemoteAddrStorage = chen::uv_ip::CopyAddress(this->udpRemoteAddr);
			this->udpRemoteAddr        = (struct sockaddr*)&this->udpRemoteAddrStorage;
		}

		bool Compare(const TransportTuple* tuple) const
		{
			if (this->protocol == Protocol::UDP && tuple->GetProtocol() == Protocol::UDP)
			{
				return (
				  this->udpSocket == tuple->udpSocket &&
				  chen::uv_ip::CompareAddresses(this->udpRemoteAddr, tuple->GetRemoteAddress()));
			}
			else if (this->protocol == Protocol::TCP && tuple->GetProtocol() == Protocol::TCP)
			{
				using namespace chen;
				ERROR_EX_LOG("not tcp protocol !!!");
				return false;
				//return (this->tcpConnection == tuple->tcpConnection);
			}
			else
			{
				return false;
			}
		}

		void SetLocalAnnouncedIp(std::string& localAnnouncedIp)
		{
			this->localAnnouncedIp = localAnnouncedIp;
		}

		void Send(const uint8_t* data, size_t len, RTC::TransportTuple::onSendCallback* cb = nullptr)
		{
			if (this->protocol == Protocol::UDP)
			{
				this->udpSocket->Send(data, len, this->udpRemoteAddr, cb);
			}
			else
			{
				using namespace chen;
				ERROR_EX_LOG("not tcp protocol !!!");
				//this->tcpConnection->Send(data, len, cb);
			}
		}

		Protocol GetProtocol() const
		{
			return this->protocol;
		}

		const struct sockaddr* GetLocalAddress() const
		{
			if (this->protocol == Protocol::UDP)
			{
				return this->udpSocket->GetLocalAddress();
			}
			else
			{
				using namespace chen;
				ERROR_EX_LOG("not tcp protocol !!!");
				// return this->tcpConnection->GetLocalAddress();
				return NULL;
			}
				
		}

		const struct sockaddr* GetRemoteAddress() const
		{
			if (this->protocol == Protocol::UDP)
			{
				return (const struct sockaddr*)this->udpRemoteAddr;
			}
			else
			{
				using namespace chen;
				ERROR_EX_LOG("not tcp protocol !!!");
				// return this->tcpConnection->GetLocalAddress();
				return NULL;
				//return this->tcpConnection->GetPeerAddress();
			}
		}

		size_t GetRecvBytes() const
		{
			if (this->protocol == Protocol::UDP)
				return this->udpSocket->GetRecvBytes();
			else
			{
				using namespace chen;
				ERROR_EX_LOG("not tcp protocol !!!");
				// return this->tcpConnection->GetRecvBytes();
				return 0;

			}
				
		}

		size_t GetSentBytes() const
		{
			if (this->protocol == Protocol::UDP)
			{
				return this->udpSocket->GetSentBytes();
			}
			else
			{
				using namespace chen;
				 ERROR_EX_LOG("not  tcp ");
				//return this->tcpConnection->GetSentBytes();
				 return 0;
			}
		}

	private:
		// Passed by argument.
		/*RTC::UdpSocket*/chen::cudp_socket*	udpSocket{ nullptr };
		struct sockaddr*						udpRemoteAddr{ nullptr };
		//RTC::TcpConnection*					tcpConnection{ nullptr };
		std::string								localAnnouncedIp;
		// Others.
		struct sockaddr_storage					udpRemoteAddrStorage;
		Protocol								protocol;
	};
} // namespace RTC

#endif
