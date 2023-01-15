/***********************************************************************************************
created: 		2022-12-27

author:			chensong

purpose:		cmedia_server


************************************************************************************************/


#include "cifaddrs_converter.h"
#if defined(__linux__)
namespace chen {

	cifaddrs_converter::cifaddrs_converter() {}

	cifaddrs_converter::~cifaddrs_converter() {}

bool cifaddrs_converter::ConvertIfAddrsToIPAddress(
    const struct ifaddrs* interface, cinterface_address* ip, cip_address* mask) 
{
  switch (interface->ifa_addr->sa_family) {
    case AF_INET: {
      *ip = cinterface_address(cip_address(
          reinterpret_cast<sockaddr_in*>(interface->ifa_addr)->sin_addr));
      *mask = cip_address(
          reinterpret_cast<sockaddr_in*>(interface->ifa_netmask)->sin_addr);
      return true;
    }
    case AF_INET6: {
      int ip_attributes = IPV6_ADDRESS_FLAG_NONE;
      if (!ConvertNativeAttributesToIPAttributes(interface, &ip_attributes)) {
        return false;
      }
      *ip = cinterface_address(
          reinterpret_cast<sockaddr_in6*>(interface->ifa_addr)->sin6_addr,
          ip_attributes);
      *mask = cip_address(
          reinterpret_cast<sockaddr_in6*>(interface->ifa_netmask)->sin6_addr);
      return true;
    }
    default: { return false; }
  }
}

bool cifaddrs_converter::ConvertNativeAttributesToIPAttributes(
    const struct ifaddrs* interface,
    int* ip_attributes) {
  *ip_attributes = IPV6_ADDRESS_FLAG_NONE;
  return true;
}

#if !defined(__APPLE__)
// For MAC and IOS, it's defined in macifaddrs_converter.cc
cifaddrs_converter* CreateIfAddrsConverter() {
  return new cifaddrs_converter();
}
#endif



}  // namespace chen

#endif
