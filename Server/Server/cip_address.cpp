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
#include "cip_address.h"
#include "cbyte_order.h"
#include "cdigit2str.h"
namespace chen {


	// Prefixes used for categorizing IPv6 addresses.
	static const in6_addr kV4MappedPrefix = {
		{{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xFF, 0xFF, 0}} };
	static const in6_addr k6To4Prefix = { {{0x20, 0x02, 0}} };
	static const in6_addr kTeredoPrefix = { {{0x20, 0x01, 0x00, 0x00}} };
	static const in6_addr kV4CompatibilityPrefix = { {{0}} };
	static const in6_addr k6BonePrefix = { {{0x3f, 0xfe, 0}} };
	static const in6_addr kPrivateNetworkPrefix = { {{0xFD}} };


	static bool IPIsHelper(const cip_address& ip, const in6_addr& tomatch, int length) {
		// Helper method for checking IP prefix matches (but only on whole byte
		// lengths). Length is in bits.
		in6_addr addr = ip.ipv6_address();
		return ::memcmp(&addr, &tomatch, (length >> 3)) == 0;
	}


	static in_addr ExtractMappedAddress(const in6_addr& in6) {
		in_addr ipv4;
		::memcpy(&ipv4.s_addr, &in6.s6_addr[12], sizeof(ipv4.s_addr));
		return ipv4;
	}

	bool cip_address::operator==(const cip_address & other) const
	{
		if (m_family != other.m_family) {
			return false;
		}
		if (m_family == AF_INET) {
			return memcmp(&u .ip4, &other.u .ip4, sizeof(u .ip4)) == 0;
		}
		if (m_family == AF_INET6) {
			return memcmp(&u .ip6, &other.u .ip6, sizeof(u .ip6)) == 0;
		}
		return m_family == AF_UNSPEC;
	}
	bool cip_address::operator!=(const cip_address & other) const
	{
		return !((*this) == other);
	}
	bool cip_address::operator<(const cip_address & other) const
	{
		return (*this) != other && !((*this) < other);
	}
	bool cip_address::operator>(const cip_address & other) const
	{
		// IPv4 is 'less than' IPv6
		if (m_family != other.m_family) {
			if (m_family == AF_UNSPEC) {
				return true;
			}
			if (m_family == AF_INET && other.m_family == AF_INET6) {
				return true;
			}
			return false;
		}
		// Comparing addresses of the same family.
		switch (m_family) {
		case AF_INET: {
			return NetworkToHost32(u .ip4.s_addr) < NetworkToHost32(other.u .ip4.s_addr);
		}
		case AF_INET6: {
			return memcmp(&u .ip6.s6_addr, &other.u .ip6.s6_addr, 16) < 0;
		}
		}
		// Catches AF_UNSPEC and invalid addresses.
		return false;
	}
	in_addr cip_address::ipv4_address() const
	{
		return u.ip4;
	}
	in6_addr cip_address::ipv6_address() const
	{
		return u.ip6;
	}
	size_t cip_address::Size() const
	{
		switch (m_family) {
		case AF_INET:
			return sizeof(in_addr);
		case AF_INET6:
			return sizeof(in6_addr);
		}
		return 0;
	}
	std::string cip_address::ToString() const
	{
		if (m_family != AF_INET && m_family != AF_INET6) 
		{
			return std::string();
		}
		char buf[INET6_ADDRSTRLEN] = { 0 };
		const void* src = &u .ip4;
		if (m_family == AF_INET6) {
			src = &u .ip6;
		}
		if (! inet_ntop(m_family, src, buf, sizeof(buf))) {
			return std::string();
		}
		return std::string(buf);
	}
	std::string cip_address::ToSensitiveString() const
	{
#if !defined(_DEBUG)
		// Return non-stripped in debug.
		return ToString();
#else
		switch (m_family) {
		case AF_INET: {
			std::string address = ToString();
			size_t find_pos = address.rfind('.');
			if (find_pos == std::string::npos)
				return std::string();
			address.resize(find_pos);
			address += ".x";
			return address;
		}
		case AF_INET6: {
			std::string result;
			result.resize(INET6_ADDRSTRLEN);
			in6_addr addr = ipv6_address();
			size_t len = snprintf(&(result[0]), result.size(), "%x:%x:%x:x:x:x:x:x",
				(addr.s6_addr[0] << 8) + addr.s6_addr[1],
				(addr.s6_addr[2] << 8) + addr.s6_addr[3],
				(addr.s6_addr[4] << 8) + addr.s6_addr[5]);
			result.resize(len);
			return result;
		}
		}
		return std::string();
#endif
	}
	cip_address cip_address::Normalized() const
	{
		if (m_family != AF_INET6) 
		{
			return *this;
		}
		if (!IPIsV4Mapped(*this))
		{
			return *this;
		}
		in_addr addr = ExtractMappedAddress(u.ip6);
		return cip_address(addr);
	}
	cip_address cip_address::AsIPv6Address() const
	{
		if (m_family != AF_INET) {
			return *this;
		}
		in6_addr v6addr = kV4MappedPrefix;
		::memcpy(&v6addr.s6_addr[12], &u.ip4.s_addr, sizeof(u.ip4.s_addr));
		return cip_address(v6addr);
	}
	uint32_t cip_address::v4AddressAsHostOrderInteger() const
	{
		if (m_family  == AF_INET) {
			return NetworkToHost32(u .ip4.s_addr);
		}
		else {
			return 0;
		}
		return 0;
	}
	bool cip_address::IsNil() const
	{
		return IPIsUnspec(*this);
	}
	const cinterface_address & cinterface_address::operator=(const cinterface_address & other)
	{
		// TODO: 在此处插入 return 语句
		m_ipv6_flags  = other.m_ipv6_flags;
		static_cast<cip_address&>(*this) = other;
		return *this;
	}
	bool cinterface_address::operator==(const cinterface_address & other) const
	{
		return m_ipv6_flags == other.ipv6_flags() &&
			static_cast<const cip_address&>(*this) == other;
	}
	bool cinterface_address::operator!=(const cinterface_address & other) const
	{
		return !((*this) == other);
	}
	std::string cinterface_address::ToString() const
	{
		std::string result = cip_address::ToString();

		if (family() == AF_INET6)
		{
			result += "|flags:0x" + ToHex(ipv6_flags());
		}

		return result;
	}




	static bool IPIsLinkLocalV4(const cip_address& ip) 
	{
		uint32_t ip_in_host_order = ip.v4AddressAsHostOrderInteger();
		return ((ip_in_host_order >> 16) == ((169 << 8) | 254));
	}

	static bool IPIsLinkLocalV6(const cip_address& ip)
	{
		// Can't use the helper because the prefix is 10 bits.
		in6_addr addr = ip.ipv6_address();
		return (addr.s6_addr[0] == 0xFE) && ((addr.s6_addr[1] & 0xC0) == 0x80);
	}

	bool IPFromAddrInfo(addrinfo * info, cip_address * out)
	{
		if (!info || !info->ai_addr) 
		{
			return false;
		}
		if (info->ai_addr->sa_family == AF_INET) {
			sockaddr_in* addr = reinterpret_cast<sockaddr_in*>(info->ai_addr);
			*out = cip_address(addr->sin_addr);
			return true;
		}
		else if (info->ai_addr->sa_family == AF_INET6) {
			sockaddr_in6* addr = reinterpret_cast<sockaddr_in6*>(info->ai_addr);
			*out = cip_address(addr->sin6_addr);
			return true;
		}
		return false;
	}

	bool IPFromString(const std::string & str, cip_address * out)
	{
		if (!out) 
		{
			return false;
		}
		in_addr addr;
		if (inet_pton(AF_INET, str.c_str(), &addr) == 0)
		{
			in6_addr addr6;
			if (inet_pton(AF_INET6, str.c_str(), &addr6) == 0) 
			{
				*out = cip_address();
				return false;
			}
			*out = cip_address(addr6);
		}
		else 
		{
			*out = cip_address(addr);
		}
		return true;
	}

	bool IPFromString(const std::string & str, int flags, cinterface_address * out)
	{
		cip_address ip;
		if (!IPFromString(str, &ip)) 
		{
			return false;
		}

		*out = cinterface_address(ip, flags);
		return true;
	}

	bool IPIsAny(const cip_address & ip)
	{
		switch (ip.family()) 
		{
		case AF_INET:
			return ip == cip_address(INADDR_ANY);
		case AF_INET6:
			return ip == cip_address(in6addr_any) || ip == cip_address(kV4MappedPrefix);
		case AF_UNSPEC:
			return false;
		}
		return false;
		return false;
	}

	static bool IPIsLoopbackV4(const cip_address& ip) {
		uint32_t ip_in_host_order = ip.v4AddressAsHostOrderInteger();
		return ((ip_in_host_order >> 24) == 127);
	}

	static bool IPIsLoopbackV6(const cip_address& ip) {
		return ip == cip_address(in6addr_loopback);
	}
	bool IPIsLoopback(const cip_address & ip)
	{
		switch (ip.family())
		{
		case AF_INET:
		{
			return IPIsLoopbackV4(ip);
		}
		case AF_INET6:
		{
			return IPIsLoopbackV6(ip);
		}
		}
		return false; 
	}

	bool IPIsLinkLocal(const cip_address & ip)
	{
		switch (ip.family()) {
		case AF_INET: {
			return IPIsLinkLocalV4(ip);
		}
		case AF_INET6: {
			return IPIsLinkLocalV6(ip);
		}
		}
		return false;
	}


	static bool IPIsPrivateNetworkV4(const cip_address& ip) {
		uint32_t ip_in_host_order = ip.v4AddressAsHostOrderInteger();
		return ((ip_in_host_order >> 24) == 10) ||
			((ip_in_host_order >> 20) == ((172 << 4) | 1)) ||
			((ip_in_host_order >> 16) == ((192 << 8) | 168));
	}


	static bool IPIsPrivateNetworkV6(const cip_address& ip) {
		return IPIsHelper(ip, kPrivateNetworkPrefix, 8);
	}

	bool IPIsPrivateNetwork(const cip_address & ip)
	{
		switch (ip.family()) {
		case AF_INET: {
			return IPIsPrivateNetworkV4(ip);
		}
		case AF_INET6: {
			return IPIsPrivateNetworkV6(ip);
		}
		}
		return false;
	}
	bool IPIsPrivate(const cip_address & ip)
	{
		return IPIsLinkLocal(ip) || IPIsLoopback(ip) || IPIsPrivateNetwork(ip);
	}
	bool IPIsUnspec(const cip_address & ip)
	{
		return ip.family() == AF_UNSPEC;
	}
	size_t HashIP(const cip_address & ip)
	{
		switch (ip.family()) 
		{
		case AF_INET:
		{
			return ip.ipv4_address().s_addr;
		}
		case AF_INET6:
		{
			in6_addr v6addr = ip.ipv6_address();
			const uint32_t* v6_as_ints = reinterpret_cast<const uint32_t*>(&v6addr.s6_addr);
			return v6_as_ints[0] ^ v6_as_ints[1] ^ v6_as_ints[2] ^ v6_as_ints[3];
		}
		}
		return 0;
	}
	bool IPIs6Bone(const cip_address & ip)
	{
		return IPIsHelper(ip, k6BonePrefix, 16);
	}
	bool IPIs6To4(const cip_address & ip)
	{
		return IPIsHelper(ip, k6To4Prefix, 16);
	}
	// According to http://www.ietf.org/rfc/rfc2373.txt, Appendix A, page 19.  An
// address which contains MAC will have its 11th and 12th bytes as FF:FE as well
// as the U/L bit as 1.
	bool IPIsMacBased(const cip_address & ip)
	{
		in6_addr addr = ip.ipv6_address();
		return ((addr.s6_addr[8] & 0x02) && addr.s6_addr[11] == 0xFF &&
			addr.s6_addr[12] == 0xFE);
	}
	bool IPIsSiteLocal(const cip_address & ip)
	{
		// Can't use the helper because the prefix is 10 bits.
		in6_addr addr = ip.ipv6_address();
		return addr.s6_addr[0] == 0xFE && (addr.s6_addr[1] & 0xC0) == 0xC0;
	}
	bool IPIsTeredo(const cip_address & ip)
	{
		return IPIsHelper(ip, kTeredoPrefix, 32);
	}
	bool IPIsULA(const cip_address & ip)
	{
		// Can't use the helper because the prefix is 7 bits.
		in6_addr addr = ip.ipv6_address();
		return (addr.s6_addr[0] & 0xFE) == 0xFC;
	}
	bool IPIsV4Compatibility(const cip_address & ip)
	{
		return IPIsHelper(ip, kV4CompatibilityPrefix, 96);
	}
	bool IPIsV4Mapped(const cip_address & ip)
	{
		return IPIsHelper(ip, kV4MappedPrefix, 96);
	}
	int IPAddressPrecedence(const cip_address & ip)
	{
		// Precedence values from RFC 3484-bis. Prefers native v4 over 6to4/Teredo.
		if (ip.family() == AF_INET) {
			return 30;
		}
		else if (ip.family() == AF_INET6) {
			if (IPIsLoopback(ip)) {
				return 60;
			}
			else if (IPIsULA(ip)) {
				return 50;
			}
			else if (IPIsV4Mapped(ip)) {
				return 30;
			}
			else if (IPIs6To4(ip)) {
				return 20;
			}
			else if (IPIsTeredo(ip)) {
				return 10;
			}
			else if (IPIsV4Compatibility(ip) || IPIsSiteLocal(ip) || IPIs6Bone(ip)) {
				return 1;
			}
			else {
				// A 'normal' IPv6 address.
				return 40;
			}
		}
		return 0;
	}
	cip_address TruncateIP(const cip_address & ip, int length)
	{
		if (length < 0) {
			return cip_address();
		}
		if (ip.family() == AF_INET) {
			if (length > 31) {
				return ip;
			}
			if (length == 0) {
				return cip_address(INADDR_ANY);
			}
			int mask = (0xFFFFFFFF << (32 - length));
			uint32_t host_order_ip = NetworkToHost32(ip.ipv4_address().s_addr);
			in_addr masked;
			masked.s_addr = HostToNetwork32(host_order_ip & mask);
			return cip_address(masked);
		}
		else if (ip.family() == AF_INET6) {
			if (length > 127) {
				return ip;
			}
			if (length == 0) {
				return cip_address(in6addr_any);
			}
			in6_addr v6addr = ip.ipv6_address();
			int position = length / 32;
			int inner_length = 32 - (length - (position * 32));
			// Note: 64bit mask constant needed to allow possible 32-bit left shift.
			uint32_t inner_mask = 0xFFFFFFFFLL << inner_length;
			uint32_t* v6_as_ints = reinterpret_cast<uint32_t*>(&v6addr.s6_addr);
			for (int i = 0; i < 4; ++i) {
				if (i == position) {
					uint32_t host_order_inner = NetworkToHost32(v6_as_ints[i]);
					v6_as_ints[i] = HostToNetwork32(host_order_inner & inner_mask);
				}
				else if (i > position) {
					v6_as_ints[i] = 0;
				}
			}
			return cip_address(v6addr);
		}
		return cip_address();
		return cip_address();
	}
	cip_address GetLoopbackIP(int family)
	{
		if (family == AF_INET) {
			return cip_address(INADDR_LOOPBACK);
		}
		if (family == AF_INET6) {
			return cip_address(in6addr_loopback);
		}
		//return rtc::IPAddress();
		return cip_address();
	}
	cip_address GetAnyIP(int family)
	{
		if (family == AF_INET) {
			return cip_address(INADDR_ANY);
		}
		if (family == AF_INET6) {
			return cip_address(in6addr_any);
		}
		return cip_address();
		return cip_address();
	}
	int CountIPMaskBits(cip_address mask)
	{
		uint32_t word_to_count = 0;
		int bits = 0;
		switch (mask.family()) {
		case AF_INET: {
			word_to_count = NetworkToHost32(mask.ipv4_address().s_addr);
			break;
		}
		case AF_INET6: {
			in6_addr v6addr = mask.ipv6_address();
			const uint32_t* v6_as_ints =
				reinterpret_cast<const uint32_t*>(&v6addr.s6_addr);
			int i = 0;
			for (; i < 4; ++i) {
				if (v6_as_ints[i] != 0xFFFFFFFF) {
					break;
				}
			}
			if (i < 4) {
				word_to_count = NetworkToHost32(v6_as_ints[i]);
			}
			bits = (i * 32);
			break;
		}
		default: { return 0; }
		}
		if (word_to_count == 0) {
			return bits;
		}

		// Public domain bit-twiddling hack from:
		// http://graphics.stanford.edu/~seander/bithacks.html
		// Counts the trailing 0s in the word.
		unsigned int zeroes = 32;
		// This could also be written word_to_count &= -word_to_count, but
		// MSVC emits warning C4146 when negating an unsigned number.
		word_to_count &= ~word_to_count + 1;  // Isolate lowest set bit.
		if (word_to_count)
		{
			zeroes--;
		}
		if (word_to_count & 0x0000FFFF)
		{
			zeroes -= 16;
		}
		if (word_to_count & 0x00FF00FF)
		{
			zeroes -= 8;
		}
		if (word_to_count & 0x0F0F0F0F)
		{
			zeroes -= 4;
		}
		if (word_to_count & 0x33333333)
		{
			zeroes -= 2;
		}
		if (word_to_count & 0x55555555)
		{
			zeroes -= 1;
		}

		return bits + (32 - zeroes);
	}
}