/***********************************************************************************************
created: 		2019-03-01

author:			chensong

purpose:		net_types
************************************************************************************************/
#ifndef _C_NET_TYPE_H_
#define _C_NET_TYPE_H_

#include <algorithm>  // std::transform(), std::find(), std::min(), std::max()
#include <cinttypes>  // PRIu64, etc
#include <cstddef>    // size_t
#include <cstdint>    // uint8_t, etc
#include <functional> // std::function
#include <memory>     // std::addressof()
#ifdef _WIN32
#include <winsock2.h>
#include <ws2def.h>
// https://stackoverflow.com/a/27443191/2085408
#undef max
#undef min
// avoid uv/win.h: error C2628 'intptr_t' followed by 'int' is illegal.
#if !defined(_SSIZE_T_) && !defined(_SSIZE_T_DEFINED)
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#define SSIZE_MAX INTPTR_MAX
#define _SSIZE_T_
#define _SSIZE_T_DEFINED
#endif
#else
#include <arpa/inet.h>  // htonl(), htons(), ntohl(), ntohs()
#include <netinet/in.h> // sockaddr_in, sockaddr_in6
#include <sys/socket.h> // struct sockaddr, struct sockaddr_storage, AF_INET, AF_INET6
#endif

//#include <uv.h>
#include <string>
#include <mutex>

//#include "cnet_define.h"

namespace chen
{
	typedef signed char		int8;
	typedef unsigned char	uint8;

	typedef signed short	int16;
	typedef unsigned short	uint16;

	typedef signed int		int32;
	typedef unsigned int	uint32;

#if defined(_MSC_VER)

	typedef signed long long	int64;
	typedef unsigned long long	uint64;

#elif defined(__GNUC__)

	typedef signed long		int64;
	typedef unsigned long	uint64;
#else
#pragma error "unknow platform!!!"

#endif

	typedef std::mutex							clock_type;
	typedef std::lock_guard<clock_type>			clock_guard;

}



#endif // !_C_NET_TYPE_H_
