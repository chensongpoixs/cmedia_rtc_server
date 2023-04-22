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

#ifndef _C_IP_ADDRESS_H_
#define _C_IP_ADDRESS_H_
#include "cnet_type.h"
#include "cbyte_order.h"
namespace chen {


	enum IPv6AddressFlag {
		IPV6_ADDRESS_FLAG_NONE = 0x00,

		// Temporary address is dynamic by nature and will not carry MAC
		// address.
		IPV6_ADDRESS_FLAG_TEMPORARY = 1 << 0,

		// Temporary address could become deprecated once the preferred
		// lifetime is reached. It is still valid but just shouldn't be used
		// to create new connection.
		IPV6_ADDRESS_FLAG_DEPRECATED = 1 << 1,
	};

	class cip_address
	{
	public:
		explicit cip_address() 
		: m_family(AF_UNSPEC)
		{
			::memset(&u, 0, sizeof(u));
		}

		explicit cip_address(const in_addr& ip4)
			: m_family(AF_INET)
		{
			::memset(&u, 0, sizeof(u));
			u.ip4 = ip4;
		}

		explicit cip_address(const in6_addr& ip6)
			: m_family(AF_INET6)
		{
			::memset(&u, 0, sizeof(u));
			u.ip6 = ip6;
		}

		explicit cip_address(uint32_t ip_in_host_byte_order) : m_family(AF_INET) {
			memset(&u , 0, sizeof(u ));
			u .ip4.s_addr = HostToNetwork32(ip_in_host_byte_order);
		}

		cip_address(const cip_address& other) : m_family(other.m_family) {
			::memcpy(&u , &other.u , sizeof(u ));
		}
		~cip_address() {}

	public:

		const cip_address& operator=(const cip_address& other)
		{
			m_family = other.m_family;
			::memcpy(&u, &other.u, sizeof(u));
			return *this;
		}

		bool operator==(const cip_address& other) const;
		bool operator!=(const cip_address& other) const;
		bool operator<(const cip_address& other) const;
		bool operator>(const cip_address& other) const;

	public:
		int32 family() const { return m_family; }
		in_addr ipv4_address() const;
		in6_addr ipv6_address() const;

		// Returns the number of bytes needed to store the raw address.
		size_t Size() const;

		// Wraps inet_ntop.
		std::string ToString() const;

		// Same as ToString but anonymizes it by hiding the last part.
		std::string ToSensitiveString() const;

		// Returns an unmapped address from a possibly-mapped address.
		// Returns the same address if this isn't a mapped address.
		cip_address Normalized() const;

		// Returns this address as an IPv6 address.
		// Maps v4 addresses (as ::ffff:a.b.c.d), returns v6 addresses unchanged.
		cip_address AsIPv6Address() const;

		// For socketaddress' benefit. Returns the IP in host byte order.
		uint32_t v4AddressAsHostOrderInteger() const;

		// Whether this is an unspecified IP address.
		bool IsNil() const;

	protected:
	private:
		int32				m_family;
		union 
		{
			in_addr    ip4;
			in_addr6   ip6;
		}u;
	};


	// IP class which could represent IPv6 address flags which is only
// meaningful in IPv6 case.
	class cinterface_address : public cip_address {
	public:
		cinterface_address() : m_ipv6_flags(IPV6_ADDRESS_FLAG_NONE) {}

		explicit cinterface_address(cip_address ip)
			: cip_address(ip), m_ipv6_flags(IPV6_ADDRESS_FLAG_NONE) {}

		cinterface_address(cip_address addr, int ipv6_flags)
			: cip_address(addr), m_ipv6_flags(ipv6_flags) {}

		cinterface_address(const in6_addr& ip6, int ipv6_flags)
			: cip_address(ip6), m_ipv6_flags(ipv6_flags) {}

		const cinterface_address& operator=(const cinterface_address& other);

		bool operator==(const cinterface_address& other) const;
		bool operator!=(const cinterface_address& other) const;

		int ipv6_flags() const { return m_ipv6_flags; }

		std::string ToString() const;

	private:
		int32			m_ipv6_flags;
	};


	bool IPFromAddrInfo(struct addrinfo* info, cip_address* out);
	bool IPFromString(const std::string& str, cip_address* out);
	bool IPFromString(const std::string& str, int flags, cinterface_address* out);
	bool IPIsAny(const cip_address& ip);
	bool IPIsLoopback(const cip_address& ip);
	bool IPIsLinkLocal(const cip_address& ip);
	// Identify a private network address like "192.168.111.222"
	// (see https://en.wikipedia.org/wiki/Private_network )
	bool IPIsPrivateNetwork(const cip_address& ip);
	// Identify if an IP is "private", that is a loopback
	// or an address belonging to a link-local or a private network.
	bool IPIsPrivate(const cip_address& ip);
	bool IPIsUnspec(const cip_address& ip);
	size_t HashIP(const cip_address& ip);

	// These are only really applicable for IPv6 addresses.
	bool IPIs6Bone(const cip_address& ip);
	bool IPIs6To4(const cip_address& ip);
	bool IPIsMacBased(const cip_address& ip);
	bool IPIsSiteLocal(const cip_address& ip);
	bool IPIsTeredo(const cip_address& ip);
	bool IPIsULA(const cip_address& ip);
	bool IPIsV4Compatibility(const cip_address& ip);
	bool IPIsV4Mapped(const cip_address& ip);

	// Returns the precedence value for this IP as given in RFC3484.
	int IPAddressPrecedence(const cip_address& ip);

	// Returns 'ip' truncated to be 'length' bits long.
	cip_address TruncateIP(const cip_address& ip, int length);

	cip_address GetLoopbackIP(int family);
	cip_address GetAnyIP(int family);

	// Returns the number of contiguously set bits, counting from the MSB in network
	// byte order, in this IPAddress. Bits after the first 0 encountered are not
	// counted.
	int CountIPMaskBits(cip_address mask);

}

#endif // _C_SOCKET_UTIL_H_