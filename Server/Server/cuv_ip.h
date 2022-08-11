/***********************************************************************************************
created: 		2022-08-12

author:			chensong

purpose:		cuv_ip
************************************************************************************************/


#ifndef _C_UV_IP_H_
#define _C_UV_IP_H_

#include "clog.h"
namespace chen {
	namespace uv_ip
	{
		static int GetFamily(const std::string& ip);

		static void GetAddressInfo(const struct sockaddr* addr, int& family, std::string& ip, uint16_t& port);

		static bool CompareAddresses(const struct sockaddr* addr1, const struct sockaddr* addr2)
		{
			// Compare family.
			if (addr1->sa_family != addr2->sa_family || (addr1->sa_family != AF_INET && addr1->sa_family != AF_INET6))
			{
				return false;
			}

			// Compare port.
			if (
				reinterpret_cast<const struct sockaddr_in*>(addr1)->sin_port !=
				reinterpret_cast<const struct sockaddr_in*>(addr2)->sin_port)
			{
				return false;
			}

			// Compare IP.
			switch (addr1->sa_family)
			{
			case AF_INET:
			{
				return (
					reinterpret_cast<const struct sockaddr_in*>(addr1)->sin_addr.s_addr ==
					reinterpret_cast<const struct sockaddr_in*>(addr2)->sin_addr.s_addr);
			}

			case AF_INET6:
			{
				return (
					std::memcmp(
						std::addressof(reinterpret_cast<const struct sockaddr_in6*>(addr1)->sin6_addr),
						std::addressof(reinterpret_cast<const struct sockaddr_in6*>(addr2)->sin6_addr),
						16) == 0
					? true
					: false);
			}

			default:
			{
				return false;
			}
			}
		}

		static struct sockaddr_storage CopyAddress(const struct sockaddr* addr)
		{
			struct sockaddr_storage copiedAddr;

			switch (addr->sa_family)
			{
			case AF_INET:
				std::memcpy(std::addressof(copiedAddr), addr, sizeof(struct sockaddr_in));
				break;

			case AF_INET6:
				std::memcpy(std::addressof(copiedAddr), addr, sizeof(struct sockaddr_in6));
				break;
			}

			return copiedAddr;
		}

		static void NormalizeIp(std::string& ip);
	}
}

#endif //_C_UV_IP_H_