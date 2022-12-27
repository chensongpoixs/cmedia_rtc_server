/***********************************************************************************************
created: 		2022-12-27

author:			chensong

purpose:		cmedia_server


************************************************************************************************/


#ifndef _C_IFADDRS_CONVERTER_H_
#define _C_IFADDRS_CONVERTER_H_

#if defined(__linux__)
//#include "rtc_base/ifaddrs_android.h"
//#else

#include <ifaddrs.h>
#include "cnetwork.h"
//#endif  // WEBRTC_ANDROID

//#include "rtc_base/ip_address.h"

namespace chen {

// This class converts native interface addresses to our internal IPAddress
// class. Subclasses should override ConvertNativeToIPAttributes to implement
// the different ways of retrieving IPv6 attributes for various POSIX platforms.
class cifaddrs_converter {
 public:
	explicit cifaddrs_converter();
  virtual ~cifaddrs_converter();
  virtual bool ConvertIfAddrsToIPAddress(const struct ifaddrs* interface,
                                         cinterface_address* ipaddress,
                                         cip_address* mask);

 protected:
  virtual bool ConvertNativeAttributesToIPAttributes(
      const struct ifaddrs* interface,
      int* ip_attributes);
};

cifaddrs_converter* CreateIfAddrsConverter();

}  // namespace chen
  #endif
#endif  // _C_IFADDRS_CONVERTER_H_
