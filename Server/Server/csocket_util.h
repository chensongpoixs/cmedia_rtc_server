/***********************************************************************************************
created: 		2022-12-27

author:			chensong

purpose:		socket_util

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

#ifndef _C_SOCKET_UTIL_H_
#define _C_SOCKET_UTIL_H_
#include "cnet_type.h"
#include "cip_address.h"
#include "cnetwork_constants.h"
#include <set>


#if defined(__linux) || defined(__linux__) 
#include <sys/types.h>         
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h> 
#include <netinet/ether.h>   
#include <netinet/ip.h>  
#include <netpacket/packet.h>   
#include <arpa/inet.h>
#include <net/ethernet.h>   
#include <net/route.h>  
#include <netdb.h>
#include <net/if.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/select.h>
#define SOCKET int
#define INVALID_SOCKET  (-1)
#define SOCKET_ERROR    (-1) 

#elif defined(WIN32) || defined(_WIN32) 
#define FD_SETSIZE      1024
#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#define SHUT_RD 0
#define SHUT_WR 1 
#define SHUT_RDWR 2

#else

#endif

#include <cstdint>
#include <cstring>


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


	// Split the host:port to host and port.
// @remark the hostport format in <host[:port]>, where port is optional.
	void parse_hostport(std::string hostport, std::string& host, int32& port);


	// Whether domain is an IPv4 address.
	 bool  is_ipv4(std::string domain);

	  // The dns resolve utility, return the resolved ip address.
	 std::string  dns_resolve(std::string host, int32& family);

	 class socket_util
	 {
	 public:
		 static bool bind(SOCKET sockfd, std::string ip, uint16_t port);
		 static void setnonblock(SOCKET fd);
		 static void setblock(SOCKET fd, int write_timeout = 0);
		 static void setreuseaddr(SOCKET fd);
		 static void setreuseport(SOCKET sockfd);
		 static void setnodelay(SOCKET sockfd);
		 static void setkeepalive(SOCKET sockfd);
		 static void setnosigpipe(SOCKET sockfd);
		 static void setsendbufsize(SOCKET sockfd, int size);
		 static void setrecvbufsize(SOCKET sockfd, int size);
		 static std::string getpeerip(SOCKET sockfd);
		 static std::string getsocketip(SOCKET sockfd);
		 static int getsocketaddr(SOCKET sockfd, struct sockaddr_in* addr);
		 static uint16_t getpeerport(SOCKET sockfd);
		 static int getpeeraddr(SOCKET sockfd, struct sockaddr_in *addr);
		 static void close(SOCKET sockfd);
		 static bool connect(SOCKET sockfd, std::string ip, uint16_t port, int timeout = 0);
	 };

}

#endif // _C_SOCKET_UTIL_H_