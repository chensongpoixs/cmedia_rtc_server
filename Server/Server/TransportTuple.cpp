//#define MS_CLASS "RTC::TransportTuple"
// #define MS_LOG_DEV_LEVEL 3

#include "TransportTuple.hpp"
//#include "Logger.hpp"
//#include "Utils.hpp"
#include <string>
#include "clog.h"

using namespace chen;

namespace RTC
{
	/* Instance methods. */

	//void TransportTuple::FillJson(json& jsonObject) const
	//{
	//	MS_TRACE();

	//	int family;
	//	std::string ip;
	//	uint16_t port;

	//	Utils::IP::GetAddressInfo(GetLocalAddress(), family, ip, port);

	//	// Add localIp.
	//	if (this->localAnnouncedIp.empty())
	//		jsonObject["localIp"] = ip;
	//	else
	//		jsonObject["localIp"] = this->localAnnouncedIp;

	//	// Add localPort.
	//	jsonObject["localPort"] = port;

	//	Utils::IP::GetAddressInfo(GetRemoteAddress(), family, ip, port);

	//	// Add remoteIp.
	//	jsonObject["remoteIp"] = ip;

	//	// Add remotePort.
	//	jsonObject["remotePort"] = port;

	//	// Add protocol.
	//	switch (GetProtocol())
	//	{
	//		case Protocol::UDP:
	//			jsonObject["protocol"] = "udp";
	//			break;

	//		case Protocol::TCP:
	//			jsonObject["protocol"] = "tcp";
	//			break;
	//	}
	//}

	void TransportTuple::reply(Json::Value & value) const
	{
		//value["iceSelectedTuple"]
		int family;
		std::string ip;
		uint16_t port;

		uv_ip::GetAddressInfo(GetLocalAddress(), family, ip, port);

		// Add localIp.
		if (this->localAnnouncedIp.empty())
		{
			value["iceSelectedTuple"]["localIp"] = ip;
		}
		else
		{
			value["iceSelectedTuple"]["localIp"] = this->localAnnouncedIp;
		}

		// Add localPort.
		value["iceSelectedTuple"]["localPort"] = port;

		uv_ip::GetAddressInfo(GetRemoteAddress(), family, ip, port);

		// Add remoteIp.
		value["iceSelectedTuple"]["remoteIp"] = ip;

		// Add remotePort.
		value["iceSelectedTuple"]["remotePort"] = port;

		// Add protocol.
		switch (GetProtocol())
		{
		case Protocol::UDP:
			value["iceSelectedTuple"]["protocol"] = "udp";
			break;

		case Protocol::TCP:
			value["iceSelectedTuple"]["protocol"] = "tcp";
			break;
		}
	}

	void TransportTuple::Dump() const
	{
		//MS_TRACE();

		DEBUG_LOG("<TransportTuple>");

		int family;
		std::string ip;
		uint16_t port;

		chen::uv_ip::GetAddressInfo(GetLocalAddress(), family, ip, port);

		DEBUG_LOG("  localIp    : %s", ip.c_str());
		DEBUG_LOG("  localPort  : %hu"  , port);

		chen::uv_ip::GetAddressInfo(GetRemoteAddress(), family, ip, port);

		DEBUG_LOG("  remoteIp   : %s", ip.c_str());
		DEBUG_LOG("  remotePort : %hu"  , port);

		switch (GetProtocol())
		{
			case Protocol::UDP:
				DEBUG_LOG("  protocol   : udp");
				break;

			case Protocol::TCP:
				DEBUG_LOG("  protocol   : tcp");
				break;
		}

		DEBUG_LOG("</TransportTuple>");
	}
} // namespace RTC
