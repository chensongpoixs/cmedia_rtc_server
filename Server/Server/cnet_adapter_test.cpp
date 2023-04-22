/***********************************************************************************************
created: 		2022-12-27

author:			chensong

purpose:		ip_address

输赢不重要，答案对你们有什么意义才重要。

光阴者，百代之过客也，唯有奋力奔跑，方能生风起时，是时代造英雄，英雄存在于时代。或许世人道你轻狂，可你本就年少啊。 看护好，自己的理想和激情。


我可能会遇到很多的人，听他们讲好2多的故事，我来写成故事或编成歌，用我学来的各种乐器演奏它。
然后还可能在一个国家遇到一个心仪我的姑娘，她可能会被我帅气的外表捕获，又会被我深邃的内涵吸引，在某个下雨的夜晚，她会全身淋透然后要在我狭小的住处换身上的湿衣服。
3小时候后她告诉我她其实是这个国家的公主，她愿意向父皇求婚。我不得已告诉她我是穿越而来的男主角，我始终要回到自己的世界。
然后我的身影慢慢消失，我看到她眼里的泪水，心里却没有任何痛苦，我才知道，原来我的心被丢掉了，我游历全世界的原因，就是要找回自己的本心。
于是我开始有意寻找各种各样失去心的人，我变成一块砖头，一颗树，一滴水，一朵白云，去听大家为什么会失去自己的本心。
我发现，刚出生的宝宝，本心还在，慢慢的，他们的本心就会消失，收到了各种黑暗之光的侵蚀。
从一次争论，到嫉妒和悲愤，还有委屈和痛苦，我看到一只只无形的手，把他们的本心扯碎，蒙蔽，偷走，再也回不到主人都身边。
我叫他本心猎手。他可能是和宇宙同在的级别 但是我并不害怕，我仔细回忆自己平淡的一生 寻找本心猎手的痕迹。
沿着自己的回忆，一个个的场景忽闪而过，最后发现，我的本心，在我写代码的时候，会回来。
安静，淡然，代码就是我的一切，写代码就是我本心回归的最好方式，我还没找到本心猎手，但我相信，顺着这个线索，我一定能顺藤摸瓜，把他揪出来。
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