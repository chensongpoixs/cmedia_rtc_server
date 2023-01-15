/***********************************************************************************************
created: 		2022-12-27

author:			chensong

purpose:		ip_address


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