/***********************************************************************************************
created: 		2022-12-27

author:			chensong

purpose:		socket_util


************************************************************************************************/
 
#include "csocket_util.h"
#include <map>

#if defined(_MSC_VER)

#include <iphlpapi.h>
#include <ws2spi.h>
#include <ws2tcpip.h>
#elif defined(__GNUC__)
#include <linux/if.h>
#include <linux/route.h>
 
#include <net/if.h>
 
#else
#pragma error "unknow platform!!!"

#endif
#include "cip_address.h"
#include "cdigit2str.h"
#include "cwin32.h"
#include <sstream>
#include "cnetwork.h"
#include <vector>
#include "clog.h"
#include "cifaddrs_converter.h"
#include "cifaddrs_android.h"

namespace chen {

#if !defined(__native_client__)
	bool IsIgnoredIPv6(const cinterface_address& ip) 
	{
		if (ip.family() != AF_INET6) 
		{
			return false;
		}

		// Link-local addresses require scope id to be bound successfully.
		// However, our IPAddress structure doesn't carry that so the
		// information is lost and causes binding failure.
		if (IPIsLinkLocal(ip)) {
			return true;
		}

		// Any MAC based IPv6 should be avoided to prevent the MAC tracking.
		if (IPIsMacBased(ip)) {
			return true;
		}

		// Ignore deprecated IPv6.
		if (ip.ipv6_flags() & IPV6_ADDRESS_FLAG_DEPRECATED) {
			return true;
		}

		return false;
	}
#endif  // !defined(__native_client__)
	// Test if the network name matches the type<number> pattern, e.g. eth0. The
// matching is case-sensitive.
	bool MatchTypeNameWithIndexPattern(std::string network_name, std::string type_name) 
	{
		return network_name == type_name;
		/*if (!absl::StartsWith(network_name, type_name)) {
			return false;
		}
		return absl::c_none_of(network_name.substr(type_name.size()),
			[](char c) { return !isdigit(c); });*/
	}
	// A cautious note that this method may not provide an accurate adapter type
// based on the string matching. Incorrect type of adapters can affect the
// result of the downstream network filtering, see e.g.
// BasicPortAllocatorSession::GetNetworks when
// PORTALLOCATOR_DISABLE_COSTLY_NETWORKS is turned on.
	AdapterType GetAdapterTypeFromName(const char* network_name) 
	{
		if (MatchTypeNameWithIndexPattern(network_name, "lo"))
		{
			// Note that we have a more robust way to determine if a network interface
			// is a loopback interface by checking the flag IFF_LOOPBACK in ifa_flags of
			// an ifaddr struct. See ConvertIfAddrs in this file.
			return ADAPTER_TYPE_LOOPBACK;
		}
		if (MatchTypeNameWithIndexPattern(network_name, "eth")) {
			return ADAPTER_TYPE_ETHERNET;
		}

		if (MatchTypeNameWithIndexPattern(network_name, "ipsec") ||
			MatchTypeNameWithIndexPattern(network_name, "tun") ||
			MatchTypeNameWithIndexPattern(network_name, "utun") ||
			MatchTypeNameWithIndexPattern(network_name, "tap")) {
			return ADAPTER_TYPE_VPN;
		}
#if defined(__APPLE__)
		// Cell networks are pdp_ipN on iOS.
		if (MatchTypeNameWithIndexPattern(network_name, "pdp_ip")) {
			return ADAPTER_TYPE_CELLULAR;
		}
		if (MatchTypeNameWithIndexPattern(network_name, "en")) {
			// This may not be most accurate because sometimes Ethernet interface
			// name also starts with "en" but it is better than showing it as
			// "unknown" type.
			// TODO(honghaiz): Write a proper IOS network manager.
			return ADAPTER_TYPE_WIFI;
		}
#elif defined(__android__)
		if (MatchTypeNameWithIndexPattern(network_name, "rmnet") ||
			MatchTypeNameWithIndexPattern(network_name, "rmnet_data") ||
			MatchTypeNameWithIndexPattern(network_name, "v4-rmnet") ||
			MatchTypeNameWithIndexPattern(network_name, "v4-rmnet_data") ||
			MatchTypeNameWithIndexPattern(network_name, "clat")) {
			return ADAPTER_TYPE_CELLULAR;
		}
		if (MatchTypeNameWithIndexPattern(network_name, "wlan")) {
			return ADAPTER_TYPE_WIFI;
		}
#endif

		return ADAPTER_TYPE_UNKNOWN;
	}

#if defined(_WIN32)



	std::string MakeNetworkKey(const std::string& name,
		const cip_address& prefix,
		int32 prefix_length)
	{
		std::stringstream ost;
		ost << name << "%" << prefix.ToString() << "/" << prefix_length;
		return ost.str();
	}

	unsigned int GetPrefix(PIP_ADAPTER_PREFIX prefixlist,
		const cip_address& ip, cip_address* prefix) 
	{
		cip_address current_prefix;
		cip_address best_prefix;
		unsigned int best_length = 0;
		while (prefixlist) {
			// Look for the longest matching prefix in the prefixlist.
			if (prefixlist->Address.lpSockaddr == nullptr ||
				prefixlist->Address.lpSockaddr->sa_family != ip.family()) {
				prefixlist = prefixlist->Next;
				continue;
			}
			switch (prefixlist->Address.lpSockaddr->sa_family) {
			case AF_INET: {
				sockaddr_in* v4_addr =
					reinterpret_cast<sockaddr_in*>(prefixlist->Address.lpSockaddr);
				current_prefix = cip_address(v4_addr->sin_addr);
				break;
			}
			case AF_INET6: {
				sockaddr_in6* v6_addr =
					reinterpret_cast<sockaddr_in6*>(prefixlist->Address.lpSockaddr);
				current_prefix = cip_address(v6_addr->sin6_addr);
				break;
			}
			default: {
				prefixlist = prefixlist->Next;
				continue;
			}
			}
			if (TruncateIP(ip, prefixlist->PrefixLength) == current_prefix &&
				prefixlist->PrefixLength > best_length) {
				best_prefix = current_prefix;
				best_length = prefixlist->PrefixLength;
			}
			prefixlist = prefixlist->Next;
		}
		*prefix = best_prefix;
		return best_length;
	}


 #elif defined(__linux__)
	void  ConvertIfAddrs(struct ifaddrs* interfaces, cifaddrs_converter* ifaddrs_converter, std::set<std::string> & ips) const
	{
		//NetworkMap current_networks;

		for (struct ifaddrs* cursor = interfaces; cursor != nullptr; cursor = cursor->ifa_next)
		{
			cip_address prefix;
			cip_address mask;
			cinterface_address ip;
			int scope_id = 0;

			// Some interfaces may not have address assigned.
			if (!cursor->ifa_addr || !cursor->ifa_netmask)
			{
				continue;
			}
			// Skip ones which are down.
			if (!(cursor->ifa_flags & IFF_RUNNING))
			{
				continue;
			}
			// Skip unknown family.
			if (cursor->ifa_addr->sa_family != AF_INET &&
				cursor->ifa_addr->sa_family != AF_INET6)
			{
				continue;
			}
			// Convert to InterfaceAddress.
			if (!ifaddrs_converter->ConvertIfAddrsToIPAddress(cursor, &ip, &mask))
			{
				continue;
			}

			// Special case for IPv6 address.
			if (cursor->ifa_addr->sa_family == AF_INET6)
			{
				if (IsIgnoredIPv6(ip)) {
					continue;
				}
				scope_id = reinterpret_cast<sockaddr_in6*>(cursor->ifa_addr)->sin6_scope_id;
			}

			AdapterType adapter_type = ADAPTER_TYPE_UNKNOWN;
			AdapterType vpn_underlying_adapter_type = ADAPTER_TYPE_UNKNOWN;
			if (cursor->ifa_flags & IFF_LOOPBACK)
			{
				adapter_type = ADAPTER_TYPE_LOOPBACK;
			}
			else
			{
				// If there is a network_monitor, use it to get the adapter type.
				// Otherwise, get the adapter type based on a few name matching rules.
				//if (network_monitor_)
				//{
				//	adapter_type = network_monitor_->GetAdapterType(cursor->ifa_name);
				//}
				if (adapter_type == ADAPTER_TYPE_UNKNOWN)
				{
					adapter_type = GetAdapterTypeFromName(cursor->ifa_name);
				}
			}

			/*if (adapter_type == ADAPTER_TYPE_VPN && network_monitor_)
			{
				vpn_underlying_adapter_type = network_monitor_->GetVpnUnderlyingAdapterType(cursor->ifa_name);
			}*/
			int prefix_length = CountIPMaskBits(mask);
			prefix = TruncateIP(ip, prefix_length);
			std::string key = MakeNetworkKey(std::string(cursor->ifa_name), prefix, prefix_length);
			cnetwork  network_ptr(cursor->ifa_name, cursor->ifa_name, prefix, prefix_length,
				//			adapter_type);

			network_ptr.set_scope_id(scope_id);
			network_ptr.AddIP(ip);
			bool ignored = IsIgnoredNetwork(network_ptr);
			network_ptr.set_ignored(ignored);
			NORMAL_EX_LOG("[name = %s][description = %s][ip = %s][adapter_type = %d][ignored = %d]", name.c_str(), description.c_str(), ip.ToString().c_str(), adapter_type, ignored);
			if (network_ptr.ignored() || adapter_type == ADAPTER_TYPE_LOOPBACK)
			{
				WARNING_EX_LOG("[name = %s][description = %s][ip = %s][adapter_type = %d][ignored = %d]", name.c_str(), description.c_str(), ip.ToString().c_str(), adapter_type, ignored);
			}
			else
			{
				ips.insert(ip.ToString());
			}
			//auto iter = current_networks.find(key);
			//if (iter == current_networks.end()) {
			//	// TODO(phoglund): Need to recognize other types as well.
			//	std::unique_ptr<Network> network(
			//		new Network(cursor->ifa_name, cursor->ifa_name, prefix, prefix_length,
			//			adapter_type));
			//	network->set_default_local_address_provider(this);
			//	network->set_scope_id(scope_id);
			//	network->AddIP(ip);
			//	network->set_ignored(IsIgnoredNetwork(*network));
			//	network->set_underlying_type_for_vpn(vpn_underlying_adapter_type);
			//	if (include_ignored || !network->ignored()) {
			//		current_networks[key] = network.get();
			//		networks->push_back(network.release());
			//	}
			//}
			//else {
			//	Network* existing_network = iter->second;
			//	existing_network->AddIP(ip);
			//	if (adapter_type != ADAPTER_TYPE_UNKNOWN) {
			//		existing_network->set_type(adapter_type);
			//		existing_network->set_underlying_type_for_vpn(
			//			vpn_underlying_adapter_type);
			//	}
			//}
		}
	 }


 

#endif // 

	static std::vector<std::string>		g_network_ignore_list;
	bool  IsIgnoredNetwork(const cnetwork& network)  
	{

		// Ignore networks on the explicit ignore list.
		for (const std::string& ignored_name : g_network_ignore_list) 
		{
			if (network.name() == ignored_name) 
			{
				return true;
			}
		}

#if defined(__unix__)
		// Filter out VMware/VirtualBox interfaces, typically named vmnet1,
		// vmnet8, or vboxnet0.
		if (strncmp(network.name().c_str(), "vmnet", 5) == 0 ||
			strncmp(network.name().c_str(), "vnic", 4) == 0 ||
			strncmp(network.name().c_str(), "vboxnet", 7) == 0) {
			return true;
		}
#if defined(__linux__)
		// Make sure this is a default route, if we're ignoring non-defaults.
		if (ignore_non_default_routes_ && !IsDefaultRoute(network.name())) {
			return true;
		}
#endif
#elif defined(_WIN32)
		// Ignore any HOST side vmware adapters with a description like:
		// VMware Virtual Ethernet Adapter for VMnet1
		// but don't ignore any GUEST side adapters with a description like:
		// VMware Accelerated AMD PCNet Adapter #2
		if (strstr(network.description().c_str(), "VMnet") != nullptr) 
		{
			//不排除WMnet 网卡
			WARNING_EX_LOG("VMnet  [ip = %s]", network.ip().ToString().c_str());
			//return true;
		}
#endif

		// Ignore any networks with a 0.x.y.z IP
		if (network.prefix().family() == AF_INET)
		{
			return (network.prefix().v4AddressAsHostOrderInteger() < 0x01000000);
		}
		return true;
	}
	bool get_network_ips(std::set<std::string> &ips)
	{

		//std::map<std::string, cnetwork*> current_networks;
#if defined(_WIN32)

		// MSDN recommends a 15KB buffer for the first try at GetAdaptersAddresses.
		size_t buffer_size = 16384;
		//std::unique_ptr<char[]> adapter_info(new char[buffer_size]);
		uint8 * adapter_info = new uint8[buffer_size];


		PIP_ADAPTER_ADDRESSES adapter_addrs = reinterpret_cast<PIP_ADAPTER_ADDRESSES>(adapter_info);
		int32 adapter_flags = (GAA_FLAG_SKIP_DNS_SERVER | GAA_FLAG_SKIP_ANYCAST |
			GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_INCLUDE_PREFIX);


		int ret = 0;
		do {
			//adapter_info.reset(new char[buffer_size]);
			adapter_addrs = reinterpret_cast<PIP_ADAPTER_ADDRESSES>(adapter_info);
			// 查找机器的IP地址
			ret = GetAdaptersAddresses(AF_UNSPEC, adapter_flags, 0, adapter_addrs, reinterpret_cast<PULONG>(&buffer_size));
		} while (ret == ERROR_BUFFER_OVERFLOW);
		if (ret != ERROR_SUCCESS) 
		{
			if (adapter_info)
			{
				delete[] adapter_info;
				adapter_info = NULL;
			}
			return false;
		}
		int32 count = 0;
		while (adapter_addrs)
		{
			if (adapter_addrs->OperStatus == IfOperStatusUp)
			{
				PIP_ADAPTER_UNICAST_ADDRESS address = adapter_addrs->FirstUnicastAddress;
				PIP_ADAPTER_PREFIX prefixlist = adapter_addrs->FirstPrefix;
				std::string name;
				std::string description;
#if !defined(NDEBUG)
				name = ToUtf8(adapter_addrs->FriendlyName, wcslen(adapter_addrs->FriendlyName));
#endif
				description = ToUtf8(adapter_addrs->Description, wcslen(adapter_addrs->Description));
				for (; address; address = address->Next) {
#if defined(NDEBUG)
					name = rtc::ToString(count);
#endif

					cip_address ip;
					int scope_id = 0;
					//std::unique_ptr<Network> network;
					switch (address->Address.lpSockaddr->sa_family) {
					case AF_INET: {
						sockaddr_in* v4_addr = reinterpret_cast<sockaddr_in*>(address->Address.lpSockaddr);
						ip = cip_address(v4_addr->sin_addr);
						break;
					}
					case AF_INET6: {
						sockaddr_in6* v6_addr = reinterpret_cast<sockaddr_in6*>(address->Address.lpSockaddr);
						scope_id = v6_addr->sin6_scope_id;
						ip = cip_address(v6_addr->sin6_addr);

						if (IsIgnoredIPv6(cinterface_address(ip))) 
						{
							continue;
						}

						break;
					}
					default: {
						continue;
					}
					}
					
					cip_address prefix;
					int prefix_length = GetPrefix(prefixlist, ip, &prefix);
					std::string key = MakeNetworkKey(name, prefix, prefix_length);
					AdapterType adapter_type = ADAPTER_TYPE_UNKNOWN;
					switch (adapter_addrs->IfType) {
					case IF_TYPE_SOFTWARE_LOOPBACK:
						adapter_type = ADAPTER_TYPE_LOOPBACK;
						break;
					case IF_TYPE_ETHERNET_CSMACD:
					case IF_TYPE_ETHERNET_3MBIT:
					case IF_TYPE_IEEE80212:
					case IF_TYPE_FASTETHER:
					case IF_TYPE_FASTETHER_FX:
					case IF_TYPE_GIGABITETHERNET:
						adapter_type = ADAPTER_TYPE_ETHERNET;
						break;
					case IF_TYPE_IEEE80211:
						adapter_type = ADAPTER_TYPE_WIFI;
						break;
					case IF_TYPE_WWANPP:
					case IF_TYPE_WWANPP2:
						adapter_type = ADAPTER_TYPE_CELLULAR;
						break;
					default:
						// TODO(phoglund): Need to recognize other types as well.
						adapter_type = ADAPTER_TYPE_UNKNOWN;
						break;
					}
					cnetwork  network_ptr  (name, description, prefix, prefix_length, adapter_type);
				
					network_ptr.set_scope_id(scope_id);
					network_ptr.AddIP(ip);
					bool ignored = IsIgnoredNetwork(network_ptr);
					network_ptr.set_ignored(ignored);
					NORMAL_EX_LOG("[name = %s][description = %s][ip = %s][adapter_type = %d][ignored = %d]", name.c_str(), description.c_str(), ip.ToString().c_str(), adapter_type, ignored);
					if (network_ptr.ignored() || adapter_type == ADAPTER_TYPE_LOOPBACK)
					{
						 WARNING_EX_LOG("[name = %s][description = %s][ip = %s][adapter_type = %d][ignored = %d]", name.c_str(), description.c_str(), ip.ToString().c_str(), adapter_type, ignored);
					}
					else
					{
						ips.insert(ip.ToString());
					}
					
				}
				// Count is per-adapter - all 'Networks' created from the same
				// adapter need to have the same name.
				++count;
			}
			adapter_addrs = adapter_addrs->Next;
		}

		if (adapter_info)
		{
			delete[] adapter_info;
			adapter_info = NULL;
		}
		return true;
#else  defined(__linux__)
			struct ifaddrs* interfaces;
		  int32 error = getifaddrs(&interfaces);
		  if (error != 0) 
		  {
			ERROR_EX_LOG("getifaddrs failed to gather interface data: %d", error);					 
			return false;
		  }
		  
		  //std::unique_ptr<IfAddrsConverter> ifaddrs_converter(CreateIfAddrsConverter());
		   IfAddrsConverter ifaddrs_converter;
		  ConvertIfAddrs(interfaces, &ifaddrs_converter, ips);
		   
		  freeifaddrs(interfaces);
		  return true;
		return true;
#endif // linux
	}
	const char * inet_ntop(int32 af, const void * src, char * dst, socklen_t size)
	{
#if defined(_WIN32)
		return win32_inet_ntop(af, src, dst, size);
#else
		return ::inet_ntop(af, src, dst, size);
#endif
	}
	int32 inet_pton(int32 af, const char * src, void * dst)
	{
#if defined(_WIN32)
		return win32_inet_pton(af, src, dst);
#else
		return ::inet_pton(af, src, dst);
#endif
	}
	bool HasIPv4Enabled()
	{
#if defined(__linx__) && !defined(__native_client__)
		bool has_ipv4 = false;
		struct ifaddrs* ifa;
		if (getifaddrs(&ifa) < 0) {
			return false;
		}
		for (struct ifaddrs* cur = ifa; cur != nullptr; cur = cur->ifa_next) {
			if (cur->ifa_addr->sa_family == AF_INET) {
				has_ipv4 = true;
				break;
			}
		}
		freeifaddrs(ifa);
		return has_ipv4;
#else
		return true;
#endif
	}
	bool HasIPv6Enabled()
	{
#if defined(WINUWP)
		// WinUWP always has IPv6 capability.
		return true;
#elif defined(_WIN32)
		if (IsWindowsVistaOrLater()) {
			return true;
		}
		if (!IsWindowsXpOrLater()) {
			return false;
		}
		DWORD protbuff_size = 4096;
		std::unique_ptr<char[]> protocols;
		LPWSAPROTOCOL_INFOW protocol_infos = nullptr;
		int requested_protocols[2] = { AF_INET6, 0 };

		int err = 0;
		int ret = 0;
		// Check for protocols in a do-while loop until we provide a buffer large
		// enough. (WSCEnumProtocols sets protbuff_size to its desired value).
		// It is extremely unlikely that this will loop more than once.
		do {
			protocols.reset(new char[protbuff_size]);
			protocol_infos = reinterpret_cast<LPWSAPROTOCOL_INFOW>(protocols.get());
			ret = WSCEnumProtocols(requested_protocols, protocol_infos, &protbuff_size,
				&err);
		} while (ret == SOCKET_ERROR && err == WSAENOBUFS);

		if (ret == SOCKET_ERROR) {
			return false;
		}

		// Even if ret is positive, check specifically for IPv6.
		// Non-IPv6 enabled WinXP will still return a RAW protocol.
		for (int i = 0; i < ret; ++i) {
			if (protocol_infos[i].iAddressFamily == AF_INET6) {
				return true;
			}
		}
		return false;
#elif defined(__linux__) && !defined(__native_client__)
		bool has_ipv6 = false;
		struct ifaddrs* ifa;
		if (getifaddrs(&ifa) < 0) {
			return false;
		}
		for (struct ifaddrs* cur = ifa; cur != nullptr; cur = cur->ifa_next) {
			if (cur->ifa_addr->sa_family == AF_INET6) {
				has_ipv6 = true;
				break;
			}
		}
		freeifaddrs(ifa);
		return has_ipv6;
#else
		return true;
#endif
	}
}

 