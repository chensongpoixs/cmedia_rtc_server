/***********************************************************************************************
created: 		2022-12-27

author:			chensong

purpose:		socket_util

 
************************************************************************************************/

#ifndef _C_SOCKET_UTIL_H_
#define _C_SOCKET_UTIL_H_
#include "cnet_type.h"
#include "cip_address.h"
#include <set>
namespace chen {


	// Makes a string key for this network. Used in the network manager's maps.
// Network objects are keyed on interface name, network prefix and the
// length of that prefix.
	std::string MakeNetworkKey(const std::string& name,
		const cip_address& prefix,
		int32 prefix_length);

	// Utility function that attempts to determine an adapter type by an interface
	// name (e.g., "wlan0"). Can be used by NetworkManager subclasses when other
	// mechanisms fail to determine the type.
	AdapterType GetAdapterTypeFromName(const char* network_name);

	// webrtc\src\rtc_base\network.h =>CreateNetworks
	bool get_network_ips(std::set<std::string> &ips);

	// rtc namespaced wrappers for inet_ntop and inet_pton so we can avoid
// the windows-native versions of these.
	const char* inet_ntop(int32 af, const void* src, char* dst, socklen_t size);
	int32 inet_pton(int32 af, const char* src, void* dst);

	bool HasIPv4Enabled();
	bool HasIPv6Enabled();
}

#endif // _C_SOCKET_UTIL_H_