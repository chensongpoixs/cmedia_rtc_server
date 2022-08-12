//#define MS_CLASS "RTC::PortManager"
// #define MS_LOG_DEV_LEVEL 3

#include "PortManager.hpp"
//#include "DepLibUV.hpp"
//#include "Logger.hpp"
//#include "MediaSoupErrors.hpp"
//#include "Settings.hpp"
//#include "Utils.hpp"
#include <tuple>   // std:make_tuple()
#include <utility> // std::piecewise_construct
#include "cuv_ip.h"
#include "cuv_util.h"
#include "ccrypto_random.h"
#include "clog.h"

using namespace chen;

/* Static methods for UV callbacks. */

static inline void onClose(uv_handle_t* handle)
{
	delete handle;
}

inline static void onFakeConnection(uv_stream_t* /*handle*/, int /*status*/)
{
	// Do nothing.
}

namespace RTC
{

	static const uint16 g_rtcMinPort = 40000;
	static const uint16 g_rtcMaxPort = 60000;
	/* Class variables. */

	thread_local std::unordered_map<std::string, std::vector<bool>> PortManager::mapUdpIpPorts;
	thread_local std::unordered_map<std::string, std::vector<bool>> PortManager::mapTcpIpPorts;

	/* Class methods. */

	uv_handle_t* PortManager::Bind(Transport transport, std::string& ip)
	{
		//MS_TRACE();

		// First normalize the IP. This may throw if invalid IP.
		uv_ip::NormalizeIp(ip);

		int err;
		int family = uv_ip::GetFamily(ip);
		struct sockaddr_storage bindAddr; // NOLINT(cppcoreguidelines-pro-type-member-init)
		size_t portIdx;
		int flags{ 0 };
		std::vector<bool>& ports = PortManager::GetPorts(transport, ip);
		size_t attempt{ 0u };
		size_t numAttempts = ports.size();
		uv_handle_t* uvHandle{ nullptr };
		uint16_t port;
		std::string transportStr;

		switch (transport)
		{
			case Transport::UDP:
				transportStr.assign("udp");
				break;

			case Transport::TCP:
				transportStr.assign("tcp");
				break;
		}

		switch (family)
		{
			case AF_INET:
			{
				err = uv_ip4_addr(ip.c_str(), 0, reinterpret_cast<struct sockaddr_in*>(&bindAddr));

				if (err != 0)
				{
					ERROR_EX_LOG("uv_ip4_addr() failed: %s", uv_strerror(err));
				}

				break;
			}

			case AF_INET6:
			{
				err = uv_ip6_addr(ip.c_str(), 0, reinterpret_cast<struct sockaddr_in6*>(&bindAddr));

				if (err != 0)
				{
					ERROR_EX_LOG("uv_ip6_addr() failed: %s", uv_strerror(err));
				}

				// Don't also bind into IPv4 when listening in IPv6.
				flags |= UV_UDP_IPV6ONLY;

				break;
			}

			// This cannot happen.
			default:
			{
				ERROR_EX_LOG("unknown IP family");
			}
		}

		// Choose a random port index to start from.
		portIdx = static_cast<size_t>(s_crypto_random. GetRandomUInt(
		  static_cast<uint32_t>(0), static_cast<uint32_t>(ports.size() - 1)));

		// Iterate all ports until getting one available. Fail if none found and also
		// if bind() fails N times in theoretically available ports.
		while (true)
		{
			// Increase attempt number.
			++attempt;

			// If we have tried all the ports in the range throw.
			if (attempt > numAttempts)
			{
				ERROR_EX_LOG( "no more available ports [transport:%s, ip:'%s', numAttempt:%zu]",
				  transportStr.c_str(),  ip.c_str(), numAttempts);
			}

			// Increase current port index.
			portIdx = (portIdx + 1) % ports.size();

			// So the corresponding port is the vector position plus the RTC minimum port.
			port = static_cast<uint16_t>(portIdx + g_rtcMinPort/*Settings::configuration.rtcMinPort*/);

			DEBUG_EX_LOG( "testing port [transport:%s, ip:'%s', port:%hu, attempt:%zu/%zu]",
			  transportStr.c_str(), ip.c_str(), port, attempt, numAttempts);

			// Check whether this port is not available.
			if (ports[portIdx])
			{
				DEBUG_EX_LOG(
				  "port in use, trying again [transport:%s, ip:'%s', port:%hu, attempt:%zu/%zu]",
				  transportStr.c_str(),
				  ip.c_str(),
				  port,
				  attempt,
				  numAttempts);

				continue;
			}

			// Here we already have a theoretically available port. Now let's check
			// whether no other process is binding into it.

			// Set the chosen port into the sockaddr struct.
			switch (family)
			{
				case AF_INET:
					(reinterpret_cast<struct sockaddr_in*>(&bindAddr))->sin_port = htons(port);
					break;

				case AF_INET6:
					(reinterpret_cast<struct sockaddr_in6*>(&bindAddr))->sin6_port = htons(port);
					break;
			}

			// Try to bind on it.
			switch (transport)
			{
				case Transport::UDP:
					uvHandle = reinterpret_cast<uv_handle_t*>(new uv_udp_t());
					err      = uv_udp_init_ex(
            uv_util::get_loop(), reinterpret_cast<uv_udp_t*>(uvHandle), UV_UDP_RECVMMSG);
					break;

				case Transport::TCP:
					uvHandle = reinterpret_cast<uv_handle_t*>(new uv_tcp_t());
					err      = uv_tcp_init(uv_util::get_loop(), reinterpret_cast<uv_tcp_t*>(uvHandle));
					break;
			}

			if (err != 0)
			{
				delete uvHandle;

				switch (transport)
				{
					case Transport::UDP:
						ERROR_EX_LOG("uv_udp_init_ex() failed: %s", uv_strerror(err));
						break;

					case Transport::TCP:
						ERROR_EX_LOG("uv_tcp_init() failed: %s", uv_strerror(err));
						break;
				}
			}

			switch (transport)
			{
				case Transport::UDP:
				{
					err = uv_udp_bind(
					  reinterpret_cast<uv_udp_t*>(uvHandle),
					  reinterpret_cast<const struct sockaddr*>(&bindAddr),
					  flags);

					if (err != 0)
					{
						WARNING_EX_LOG(
						  "uv_udp_bind() failed [transport:%s, ip:'%s', port:%hu, attempt:%zu/%zu]: %s",
						  transportStr.c_str(),
						  ip.c_str(),
						  port,
						  attempt,
						  numAttempts,
						  uv_strerror(err));
					}

					break;
				}

				case Transport::TCP:
				{
					err = uv_tcp_bind(
					  reinterpret_cast<uv_tcp_t*>(uvHandle),
					  reinterpret_cast<const struct sockaddr*>(&bindAddr),
					  flags);

					if (err != 0)
					{
						WARNING_EX_LOG(
						  "uv_tcp_bind() failed [transport:%s, ip:'%s', port:%hu, attempt:%zu/%zu]: %s",
						  transportStr.c_str(),
						  ip.c_str(),
						  port,
						  attempt,
						  numAttempts,
						  uv_strerror(err));
					}

					// uv_tcp_bind() may succeed even if later uv_listen() fails, so
					// double check it.
					if (err == 0)
					{
						err = uv_listen(
						  reinterpret_cast<uv_stream_t*>(uvHandle),
						  256,
						  static_cast<uv_connection_cb>(onFakeConnection));

						WARNING_EX_LOG(
						  "uv_listen() failed [transport:%s, ip:'%s', port:%hu, attempt:%zu/%zu]: %s",
						  transportStr.c_str(),
						  ip.c_str(),
						  port,
						  attempt,
						  numAttempts,
						  uv_strerror(err));
					}

					break;
				}
			}

			// If it succeeded, exit the loop here.
			if (err == 0)
			{
				break;
			}

			// If it failed, close the handle and check the reason.
			uv_close(reinterpret_cast<uv_handle_t*>(uvHandle), static_cast<uv_close_cb>(onClose));

			switch (err)
			{
				// If bind() fails due to "too many open files" just throw.
				case UV_EMFILE:
				{
					ERROR_EX_LOG(
					  "port bind failed due to too many open files [transport:%s, ip:'%s', port:%hu, attempt:%zu/%zu]",
					  transportStr.c_str(),
					  ip.c_str(),
					  port,
					  attempt,
					  numAttempts);

					break;
				}

				// If cannot bind in the given IP, throw.
				case UV_EADDRNOTAVAIL:
				{
					ERROR_EX_LOG(
					  "port bind failed due to address not available [transport:%s, ip:'%s', port:%hu, attempt:%zu/%zu]",
					  transportStr.c_str(),
					  ip.c_str(),
					  port,
					  attempt,
					  numAttempts);

					break;
				}

				default:
				{
					// Otherwise continue in the loop to try again with next port.
				}
			}
		}

		// If here, we got an available port. Mark it as unavailable.
		ports[portIdx] = true;

		DEBUG_EX_LOG(
		  "bind succeeded [transport:%s, ip:'%s', port:%hu, attempt:%zu/%zu]",
		  transportStr.c_str(),
		  ip.c_str(),
		  port,
		  attempt,
		  numAttempts);

		return static_cast<uv_handle_t*>(uvHandle);
	}

	uv_handle_t* PortManager::Bind(Transport transport, std::string& ip, uint16_t port)
	{
		//MS_TRACE();

		// First normalize the IP. This may throw if invalid IP.
		uv_ip::NormalizeIp(ip);

		int err;
		int family = uv_ip::GetFamily(ip);
		struct sockaddr_storage bindAddr; // NOLINT(cppcoreguidelines-pro-type-member-init)
		int flags{ 0 };
		uv_handle_t* uvHandle{ nullptr };
		std::string transportStr;

		switch (transport)
		{
			case Transport::UDP:
				transportStr.assign("udp");
				break;

			case Transport::TCP:
				transportStr.assign("tcp");
				break;
		}

		switch (family)
		{
			case AF_INET:
			{
				err = uv_ip4_addr(ip.c_str(), 0, reinterpret_cast<struct sockaddr_in*>(&bindAddr));

				if (err != 0)
				{
					ERROR_EX_LOG("uv_ip4_addr() failed: %s", uv_strerror(err));
				}

				break;
			}

			case AF_INET6:
			{
				err = uv_ip6_addr(ip.c_str(), 0, reinterpret_cast<struct sockaddr_in6*>(&bindAddr));

				if (err != 0)
				{
					ERROR_EX_LOG("uv_ip6_addr() failed: %s", uv_strerror(err));
				}

				// Don't also bind into IPv4 when listening in IPv6.
				flags |= UV_UDP_IPV6ONLY;

				break;
			}

			// This cannot happen.
			default:
			{
				ERROR_EX_LOG("unknown IP family");
			}
		}

		// Set the port into the sockaddr struct.
		switch (family)
		{
			case AF_INET:
				(reinterpret_cast<struct sockaddr_in*>(&bindAddr))->sin_port = htons(port);
				break;

			case AF_INET6:
				(reinterpret_cast<struct sockaddr_in6*>(&bindAddr))->sin6_port = htons(port);
				break;
		}

		// Try to bind on it.
		switch (transport)
		{
			case Transport::UDP:
				uvHandle = reinterpret_cast<uv_handle_t*>(new uv_udp_t());
				err      = uv_udp_init_ex(
          uv_util::get_loop(), reinterpret_cast<uv_udp_t*>(uvHandle), UV_UDP_RECVMMSG);
				break;

			case Transport::TCP:
				uvHandle = reinterpret_cast<uv_handle_t*>(new uv_tcp_t());
				err      = uv_tcp_init(uv_util::get_loop(), reinterpret_cast<uv_tcp_t*>(uvHandle));
				break;
		}

		if (err != 0)
		{
			delete uvHandle;

			switch (transport)
			{
				case Transport::UDP:
					ERROR_EX_LOG("uv_udp_init_ex() failed: %s", uv_strerror(err));
					break;

				case Transport::TCP:
					ERROR_EX_LOG("uv_tcp_init() failed: %s", uv_strerror(err));
					break;
			}
		}

		switch (transport)
		{
			case Transport::UDP:
			{
				err = uv_udp_bind(
				  reinterpret_cast<uv_udp_t*>(uvHandle),
				  reinterpret_cast<const struct sockaddr*>(&bindAddr),
				  flags);

				if (err != 0)
				{
					// If it failed, close the handle and check the reason.
					uv_close(reinterpret_cast<uv_handle_t*>(uvHandle), static_cast<uv_close_cb>(onClose));

					ERROR_EX_LOG(
					  "uv_udp_bind() failed [transport:%s, ip:'%s', port:%hu: %s",
					  transportStr.c_str(),
					  ip.c_str(),
					  port,
					  uv_strerror(err));
				}

				break;
			}

			case Transport::TCP:
			{
				err = uv_tcp_bind(
				  reinterpret_cast<uv_tcp_t*>(uvHandle),
				  reinterpret_cast<const struct sockaddr*>(&bindAddr),
				  flags);

				if (err != 0)
				{
					// If it failed, close the handle and check the reason.
					uv_close(reinterpret_cast<uv_handle_t*>(uvHandle), static_cast<uv_close_cb>(onClose));

					ERROR_EX_LOG(
					  "uv_tcp_bind() failed [transport:%s, ip:'%s', port:%hu]: %s",
					  transportStr.c_str(),
					  ip.c_str(),
					  port,
					  uv_strerror(err));
				}

				// uv_tcp_bind() may succeed even if later uv_listen() fails, so
				// double check it.
				err = uv_listen(
				  reinterpret_cast<uv_stream_t*>(uvHandle),
				  256,
				  static_cast<uv_connection_cb>(onFakeConnection));

				if (err != 0)
				{
					// If it failed, close the handle and check the reason.
					uv_close(reinterpret_cast<uv_handle_t*>(uvHandle), static_cast<uv_close_cb>(onClose));

					ERROR_EX_LOG(
					  "uv_listen() failed [transport:%s, ip:'%s', port:%hu]: %s",
					  transportStr.c_str(),
					  ip.c_str(),
					  port,
					  uv_strerror(err));
				}

				break;
			}
		}

		DEBUG_EX_LOG(
		  "bind succeeded [transport:%s, ip:'%s', port:%hu]",
		  transportStr.c_str(),
		  ip.c_str(),
		  port);

		return static_cast<uv_handle_t*>(uvHandle);
	}

	void PortManager::Unbind(Transport transport, std::string& ip, uint16_t port)
	{
		//MS_TRACE();

		if (
		  (static_cast<size_t>(port) < g_rtcMinPort) ||
		  (static_cast<size_t>(port) > g_rtcMaxPort))
		{
			ERROR_EX_LOG("given port %hu is out of range", port);

			return;
		}

		size_t portIdx = static_cast<size_t>(port) - g_rtcMinPort;

		switch (transport)
		{
			case Transport::UDP:
			{
				auto it = PortManager::mapUdpIpPorts.find(ip);

				if (it == PortManager::mapUdpIpPorts.end())
					return;

				auto& ports = it->second;

				// Mark the port as available.
				ports[portIdx] = false;

				break;
			}

			case Transport::TCP:
			{
				auto it = PortManager::mapTcpIpPorts.find(ip);

				if (it == PortManager::mapTcpIpPorts.end())
					return;

				auto& ports = it->second;

				// Mark the port as available.
				ports[portIdx] = false;

				break;
			}
		}
	}

	std::vector<bool>& PortManager::GetPorts(Transport transport, const std::string& ip)
	{
		//MS_TRACE();

		// Make GCC happy so it does not print:
		// "control reaches end of non-void function [-Wreturn-type]"
		static std::vector<bool> emptyPorts;

		switch (transport)
		{
			case Transport::UDP:
			{
				auto it = PortManager::mapUdpIpPorts.find(ip);

				// If the IP is already handled, return its ports vector.
				if (it != PortManager::mapUdpIpPorts.end())
				{
					auto& ports = it->second;

					return ports;
				}

				// Otherwise add an entry in the map and return it.
				uint16_t numPorts = g_rtcMaxPort - g_rtcMinPort + 1;

				// Emplace a new vector filled with numPorts false values, meaning that
				// all ports are available.
				auto pair = PortManager::mapUdpIpPorts.emplace(
				  std::piecewise_construct, std::make_tuple(ip), std::make_tuple(numPorts, false));

				// pair.first is an iterator to the inserted value.
				auto& ports = pair.first->second;

				return ports;
			}

			case Transport::TCP:
			{
				auto it = PortManager::mapTcpIpPorts.find(ip);

				// If the IP is already handled, return its ports vector.
				if (it != PortManager::mapTcpIpPorts.end())
				{
					auto& ports = it->second;

					return ports;
				}

				// Otherwise add an entry in the map and return it.
				uint16_t numPorts = g_rtcMaxPort - g_rtcMinPort + 1;

				// Emplace a new vector filled with numPorts false values, meaning that
				// all ports are available.
				auto pair = PortManager::mapTcpIpPorts.emplace(
				  std::piecewise_construct, std::make_tuple(ip), std::make_tuple(numPorts, false));

				// pair.first is an iterator to the inserted value.
				auto& ports = pair.first->second;

				return ports;
			}
		}

		return emptyPorts;
	}

	//void PortManager::FillJson(json& jsonObject)
	//{
	//	MS_TRACE();

	//	// Add udp.
	//	jsonObject["udp"] = json::object();
	//	auto jsonUdpIt    = jsonObject.find("udp");

	//	for (auto& kv : PortManager::mapUdpIpPorts)
	//	{
	//		const auto& ip = kv.first;
	//		auto& ports    = kv.second;

	//		(*jsonUdpIt)[ip] = json::array();
	//		auto jsonIpIt    = jsonUdpIt->find(ip);

	//		for (size_t i{ 0 }; i < ports.size(); ++i)
	//		{
	//			if (!ports[i])
	//				continue;

	//			auto port = static_cast<uint16_t>(i + g_rtcMinPort);

	//			jsonIpIt->push_back(port);
	//		}
	//	}

	//	// Add tcp.
	//	jsonObject["tcp"] = json::object();
	//	auto jsonTcpIt    = jsonObject.find("tcp");

	//	for (auto& kv : PortManager::mapTcpIpPorts)
	//	{
	//		const auto& ip = kv.first;
	//		auto& ports    = kv.second;

	//		(*jsonTcpIt)[ip] = json::array();
	//		auto jsonIpIt    = jsonTcpIt->find(ip);

	//		for (size_t i{ 0 }; i < ports.size(); ++i)
	//		{
	//			if (!ports[i])
	//				continue;

	//			auto port = static_cast<uint16_t>(i + g_rtcMinPort);

	//			jsonIpIt->emplace_back(port);
	//		}
	//	}
	//}
} // namespace RTC
