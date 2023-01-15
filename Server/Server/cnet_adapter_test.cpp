/***********************************************************************************************
created: 		2022-12-27

author:			chensong

purpose:		ip_address


************************************************************************************************/

#include "cnet_adapter_test.h"
#include "csocket_util.h"
#include "clog.h"
namespace chen {
	int32 net_adapter_test( )
	{
		std::set<std::string> ips;
		chen::get_network_ips(ips);
		for (const std::string & ip : ips)
		{
			NORMAL_EX_LOG("[ip = %s]", ip.c_str());
		}
		return 0;
	}


#if __linux__test_adapter_net
#include <set>
#include <iostream>
#include <string>
#include <ifaddrs.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>

	void  ConvertIfAddrs(struct ifaddrs* interfaces, std::set<std::string> & ips)
	{
		//NetworkMap current_networks;

		for (struct ifaddrs* cursor = interfaces; cursor != nullptr; cursor = cursor->ifa_next)
		{
			// cip_address prefix;
			// cip_address mask;
			// cinterface_address ip;
			int scope_id = 0;

			// Some interfaces may not have address assigned.
			if (!cursor->ifa_addr || !cursor->ifa_netmask)
			{
				continue;
			}
			// Skip ones which are down.
			// if (!(cursor->ifa_flags & IFF_RUNNING))
			// {
			// 	continue;
			// }
			// Skip unknown family.
			if (cursor->ifa_addr->sa_family != AF_INET &&
				cursor->ifa_addr->sa_family != AF_INET6)
			{
				continue;
			}
			// Convert to InterfaceAddress.
			//if (!ifaddrs_converter->ConvertIfAddrsToIPAddress(cursor, &ip, &mask))
			{
				//	continue;
			}

			// Special case for IPv6 address.
			if (cursor->ifa_addr->sa_family == AF_INET6)
			{

				scope_id = reinterpret_cast<sockaddr_in6*>(cursor->ifa_addr)->sin6_scope_id;
			}
			char saddr[64];
			char* h = (char*)saddr;
			socklen_t nbh = (socklen_t)sizeof(saddr);
			const int r0 = getnameinfo(cursor->ifa_addr, sizeof(sockaddr_storage), h, nbh, NULL, 0, NI_NUMERICHOST);
			if (r0) {
				printf("convert local ip failed: %s\n", gai_strerror(r0));
				return;
			}

			std::string ip(saddr, strlen(saddr));

			ips.insert(ip);
			// AdapterType adapter_type = ADAPTER_TYPE_UNKNOWN;
			// AdapterType vpn_underlying_adapter_type = ADAPTER_TYPE_UNKNOWN;
			// if (cursorip->ifa_flags & IFF_LOOPBACK)
			// {
			// 	adapter_type = ADAPTER_TYPE_LOOPBACK;
			// }
			// else
			// {
			// 	// If there is a network_monitor, use it to get the adapter type.
			// 	// Otherwise, get the adapter type based on a few name matching rules.
			// 	//if (network_monitor_)
			// 	//{
			// 	//	adapter_type = network_monitor_->GetAdapterType(cursor->ifa_name);
			// 	//}
			// 	if (adapter_type == ADAPTER_TYPE_UNKNOWN)
			// 	{
			// 		adapter_type = GetAdapterTypeFromName(cursor->ifa_name);
			// 	}
			// }

			/*if (adapter_type == ADAPTER_TYPE_VPN && network_monitor_)
			{
				vpn_underlying_adapter_type = network_monitor_->GetVpnUnderlyingAdapterType(cursor->ifa_name);
			}*/
			// int prefix_length = CountIPMaskBits(mask);
			// prefix = TruncateIP(ip, prefix_length);
			// std::string key = MakeNetworkKey(std::string(cursor->ifa_name), prefix, prefix_length);
			// cnetwork  network_ptr(cursor->ifa_name, cursor->ifa_name, prefix, prefix_length,
			// 	//			adapter_type);

			// network_ptr.set_scope_id(scope_id);
			// network_ptr.AddIP(ip);
			// bool ignored = IsIgnoredNetwork(network_ptr);
			// network_ptr.set_ignored(ignored);
			// printf("[name = %s][description = %s][ip = %s][adapter_type = %d][ignored = %d]\n", name.c_str(), description.c_str(), ip.ToString().c_str(), adapter_type, ignored);
			// if (network_ptr.ignored() || adapter_type == ADAPTER_TYPE_LOOPBACK)
			// {
			// 	printf("[warrn][name = %s][description = %s][ip = %s][adapter_type = %d][ignored = %d]\n", name.c_str(), description.c_str(), ip.ToString().c_str(), adapter_type, ignored);
			// }
			// else
			{
				//ips.insert(ip.ToString());
			}

		}
	}

	int test_net_adapter_demo(int argc, char *argv[])
	{
		struct ifaddrs* interfaces;
		int32_t error = getifaddrs(&interfaces);
		if (error != 0)
		{
			printf("getifaddrs failed to gather interface data: %d\n", error);
			return -1;
		}
		std::set<std::string> ips;
		ConvertIfAddrs(interfaces, ips);
		for (const std::string & ip : ips)
		{
			printf("ip = %s\n", ip.c_str());
		}
		freeifaddrs(interfaces);
		return 0;
	}

	/*
	stdout

	ip = 127.0.0.1
	ip = 192.168.139.128
	ip = ::1
	ip = fe80::2ad2:e740:6f79:688b%ens33
	*/

#endif


}