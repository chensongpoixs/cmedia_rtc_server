/***********************************************************************************************
created: 		2022-09-04

author:			chensong

purpose:		crtc_sdp_util

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


#include "crtc_sdp_util.h"
#if defined(_MSC_VER)
#define htobe16(v) htons(v)
#define htobe32(v) htonl(v)
#define be16toh(v) ntohs(v)
#define be32toh(v) ntohl(v)
#define htole16(v) (v)
#define htole32(v) (v)
#define htole64(v) (v)
#define le16toh(v) (v)
#define le32toh(v) (v)
#define le64toh(v) (v)
#if defined(WEBRTC_WIN)
#define htobe64(v) _byteswap_uint64(v)
#define be64toh(v) _byteswap_uint64(v)
#endif  // defined(WEBRTC_WIN)
#if defined(__native_client__)
#define htobe64(v) __builtin_bswap64(v)
#define be64toh(v) __builtin_bswap64(v)
#endif  // defined(__native_client__)

#elif defined(WEBRTC_ARCH_BIG_ENDIAN)
#define htobe16(v) (v)
#define htobe32(v) (v)
#define be16toh(v) (v)
#define be32toh(v) (v)
#define htole16(v) __builtin_bswap16(v)
#define htole32(v) __builtin_bswap32(v)
#define htole64(v) __builtin_bswap64(v)
#define le16toh(v) __builtin_bswap16(v)
#define le32toh(v) __builtin_bswap32(v)
#define le64toh(v) __builtin_bswap64(v)
#if defined(WEBRTC_WIN)
#define htobe64(v) (v)
#define be64toh(v) (v)
#endif  // defined(WEBRTC_WIN)
#if defined(__native_client__)
#define htobe64(v) (v)
#define be64toh(v) (v)
#endif  // defined(__native_client__)
#else
#error WEBRTC_ARCH_BIG_ENDIAN or WEBRTC_ARCH_LITTLE_ENDIAN must be defined.
#endif  // defined(WEBRTC_ARCH_LITTLE_ENDIAN)
namespace chen {
	namespace rtc_sdp_util
	{



		// RTP Profile names
		// http://www.iana.org/assignments/rtp-parameters/rtp-parameters.xml
		// RFC4585
		const char kMediaProtocolAvpf[] = "RTP/AVPF";
		// RFC5124
		const char kMediaProtocolDtlsSavpf[] = "UDP/TLS/RTP/SAVPF";

		// We always generate offers with "UDP/TLS/RTP/SAVPF" when using DTLS-SRTP,
		// but we tolerate "RTP/SAVPF" in offers we receive, for compatibility.
		const char kMediaProtocolSavpf[] = "RTP/SAVPF";

		const char kMediaProtocolRtpPrefix[] = "RTP/";

		const char kMediaProtocolSctp[] = "SCTP";
		const char kMediaProtocolDtlsSctp[] = "DTLS/SCTP";
		const char kMediaProtocolUdpDtlsSctp[] = "UDP/DTLS/SCTP";
		const char kMediaProtocolTcpDtlsSctp[] = "TCP/DTLS/SCTP";


		static const char kWhitespace[] = " \n\r\t";

		// Note that the below functions support some protocol strings purely for
		// legacy compatibility, as required by JSEP in Section 5.1.2, Profile Names
		// and Interoperability.

		static bool IsDtlsRtp(const std::string& protocol) {
			// Most-likely values first.
			return protocol == "UDP/TLS/RTP/SAVPF" || protocol == "TCP/TLS/RTP/SAVPF" ||
				protocol == "UDP/TLS/RTP/SAVP" || protocol == "TCP/TLS/RTP/SAVP";
		}

		static bool IsPlainRtp(const std::string& protocol) {
			// Most-likely values first.
			return protocol == "RTP/SAVPF" || protocol == "RTP/AVPF" ||
				protocol == "RTP/SAVP" || protocol == "RTP/AVP";
		}

		static bool IsDtlsSctp(const std::string& protocol) 
		{
			return protocol == kMediaProtocolDtlsSctp ||
				protocol == kMediaProtocolUdpDtlsSctp ||
				protocol == kMediaProtocolTcpDtlsSctp;
		}

		static bool IsPlainSctp(const std::string& protocol) {
			return protocol == kMediaProtocolSctp;
		}

		static bool IsSctp(const std::string& protocol) {
			return IsPlainSctp(protocol) || IsDtlsSctp(protocol);
		}

		std::string string_trim(const std::string & s)
		{
			std::string::size_type first = s.find_first_not_of(kWhitespace);
			std::string::size_type last = s.find_last_not_of(kWhitespace);

			if (first == std::string::npos || last == std::string::npos) {
				return std::string("");
			}

			return s.substr(first, last - first + 1);
		}

		size_t split(const std::string & source, char delimiter, std::vector<std::string>* fields)
		{
			//RTC_DCHECK(fields);
			fields->clear();
			size_t last = 0;
			for (size_t i = 0; i < source.length(); ++i) 
			{
				if (source[i] == delimiter) 
				{
					fields->push_back(source.substr(last, i - last));
					last = i + 1;
				}
			}
			fields->push_back(source.substr(last, source.length() - last));
			return fields->size();
		}

		bool tokenize_first(const std::string& source,
			const char delimiter,
			std::string* token,
			std::string* rest) {
			// Find the first delimiter
			size_t left_pos = source.find(delimiter);
			if (left_pos == std::string::npos) {
				return false;
			}

			// Look for additional occurrances of delimiter.
			size_t right_pos = left_pos + 1;
			while (source[right_pos] == delimiter) {
				right_pos++;
			}

			*token = source.substr(0, left_pos);
			*rest = source.substr(right_pos);
			return true;
		}
#if defined(_MSC_VER)
#include <ws2spi.h>
#include <ws2tcpip.h>
#include <windows.h>

		inline uint16_t HostToNetwork16(uint16_t n) {
			return htobe16(n);
		}
		// Helper function for inet_pton for IPv4 addresses.
		// |src| points to a character string containing an IPv4 network address in
		// dotted-decimal format, "ddd.ddd.ddd.ddd", where ddd is a decimal number
		// of up to three digits in the range 0 to 255.
		// The address is converted and copied to dst,
		// which must be sizeof(struct in_addr) (4) bytes (32 bits) long.
		int inet_pton_v4(const char* src, void* dst) {
			const int kIpv4AddressSize = 4;
			int found = 0;
			const char* src_pos = src;
			unsigned char result[kIpv4AddressSize] = { 0 };

			while (*src_pos != '\0') {
				// strtol won't treat whitespace characters in the begining as an error,
				// so check to ensure this is started with digit before passing to strtol.
				if (!isdigit(*src_pos)) {
					return 0;
				}
				char* end_pos;
				long value = strtol(src_pos, &end_pos, 10);
				if (value < 0 || value > 255 || src_pos == end_pos) {
					return 0;
				}
				++found;
				if (found > kIpv4AddressSize) {
					return 0;
				}
				result[found - 1] = static_cast<unsigned char>(value);
				src_pos = end_pos;
				if (*src_pos == '.') {
					// There's more.
					++src_pos;
				}
				else if (*src_pos != '\0') {
					// If it's neither '.' nor '\0' then return fail.
					return 0;
				}
			}
			if (found != kIpv4AddressSize) {
				return 0;
			}
			memcpy(dst, result, sizeof(result));
			return 1;
		}

		// Helper function for inet_pton for IPv6 addresses.
		int inet_pton_v6(const char* src, void* dst) {
			// sscanf will pick any other invalid chars up, but it parses 0xnnnn as hex.
			// Check for literal x in the input string.
			const char* readcursor = src;
			char c = *readcursor++;
			while (c) {
				if (c == 'x') {
					return 0;
				}
				c = *readcursor++;
			}
			readcursor = src;

			struct in6_addr an_addr;
			memset(&an_addr, 0, sizeof(an_addr));

			uint16_t* addr_cursor = reinterpret_cast<uint16_t*>(&an_addr.s6_addr[0]);
			uint16_t* addr_end = reinterpret_cast<uint16_t*>(&an_addr.s6_addr[16]);
			bool seencompressed = false;

			// Addresses that start with "::" (i.e., a run of initial zeros) or
			// "::ffff:" can potentially be IPv4 mapped or compatibility addresses.
			// These have dotted-style IPv4 addresses on the end (e.g. "::192.168.7.1").
			if (*readcursor == ':' && *(readcursor + 1) == ':' &&
				*(readcursor + 2) != 0) {
				// Check for periods, which we'll take as a sign of v4 addresses.
				const char* addrstart = readcursor + 2;
				if (strchr(addrstart, '.')) {
					const char* colon = strchr(addrstart, ':');
					if (colon) {
						uint16_t a_short;
						int bytesread = 0;
						if (sscanf(addrstart, "%hx%n", &a_short, &bytesread) != 1 ||
							a_short != 0xFFFF || bytesread != 4) {
							// Colons + periods means has to be ::ffff:a.b.c.d. But it wasn't.
							return 0;
						}
						else {
							an_addr.s6_addr[10] = 0xFF;
							an_addr.s6_addr[11] = 0xFF;
							addrstart = colon + 1;
						}
					}
					struct in_addr v4;
					if (inet_pton_v4(addrstart, &v4.s_addr)) {
						memcpy(&an_addr.s6_addr[12], &v4, sizeof(v4));
						memcpy(dst, &an_addr, sizeof(an_addr));
						return 1;
					}
					else {
						// Invalid v4 address.
						return 0;
					}
				}
			}

			// For addresses without a trailing IPv4 component ('normal' IPv6 addresses).
			while (*readcursor != 0 && addr_cursor < addr_end) {
				if (*readcursor == ':') {
					if (*(readcursor + 1) == ':') {
						if (seencompressed) {
							// Can only have one compressed run of zeroes ("::") per address.
							return 0;
						}
						// Hit a compressed run. Count colons to figure out how much of the
						// address is skipped.
						readcursor += 2;
						const char* coloncounter = readcursor;
						int coloncount = 0;
						if (*coloncounter == 0) {
							// Special case - trailing ::.
							addr_cursor = addr_end;
						}
						else {
							while (*coloncounter) {
								if (*coloncounter == ':') {
									++coloncount;
								}
								++coloncounter;
							}
							// (coloncount + 1) is the number of shorts left in the address.
							// If this number is greater than the number of available shorts, the
							// address is malformed.
							if (coloncount + 1 > addr_end - addr_cursor) {
								return 0;
							}
							addr_cursor = addr_end - (coloncount + 1);
							seencompressed = true;
						}
					}
					else {
						++readcursor;
					}
				}
				else {
					uint16_t word;
					int bytesread = 0;
					if (sscanf(readcursor, "%4hx%n", &word, &bytesread) != 1) {
						return 0;
					}
					else {
						*addr_cursor = HostToNetwork16(word);
						++addr_cursor;
						readcursor += bytesread;
						if (*readcursor != ':' && *readcursor != '\0') {
							return 0;
						}
					}
				}
			}

			if (*readcursor != '\0' || addr_cursor < addr_end) {
				// Catches addresses too short or too long.
				return 0;
			}
			memcpy(dst, &an_addr, sizeof(an_addr));
			return 1;
		}
		// As above, but for inet_pton. Implements inet_pton for v4 and v6.
		// Note that our inet_ntop will output normal 'dotted' v4 addresses only.
		int win32_inet_pton(int af, const char* src, void* dst) {
			if (!src || !dst) {
				return 0;
			}
			if (af == AF_INET) {
				return inet_pton_v4(src, dst);
			}
			else if (af == AF_INET6) {
				return inet_pton_v6(src, dst);
			}
			return -1;
		}

#endif 
		int cinet_pton(int af, const char* src, void* dst) 
		{
#if defined(_MSC_VER)
#include <ws2spi.h>
#include <ws2tcpip.h>
#include <windows.h>
			return win32_inet_pton(af, src, dst);
#else
			return ::inet_pton(af, src, dst);
#endif
		}

		bool is_rtp(const std::string &protocol)
		{
			return protocol.empty() ||
				(protocol.find( kMediaProtocolRtpPrefix) != std::string::npos);
		}

		bool is_dtls_sctp(const std::string & protocol)
		{
			// This intentionally excludes "SCTP" and "SCTP/DTLS".
			return protocol.find( kMediaProtocolDtlsSctp) != std::string::npos;
		}


		std::string  fmt(const char* fmt, ...)
		{
			va_list ap;
			va_start(ap, fmt);

			char buf[8192] = {0};
			int r0 = vsnprintf(buf, sizeof(buf), fmt, ap);
			va_end(ap);

			std::string v;
			if (r0 > 0 && r0 < (int)sizeof(buf)) {
				v.append(buf, r0);
			}

			return v;
		}

		std::vector<std::string> string_split(std::string s, std::string seperator)
		{
			std::vector<std::string> result;
			if (seperator.empty()) 
			{
				result.push_back(s);
				return result;
			}

			size_t posBegin = 0;
			size_t posSeperator = s.find(seperator);
			while (posSeperator != std::string::npos) 
			{
				result.push_back(s.substr(posBegin, posSeperator - posBegin));
				posBegin = posSeperator + seperator.length(); // next byte of seperator
				posSeperator = s.find(seperator, posBegin);
			}
			// push the last element
			result.push_back(s.substr(posBegin));
			return result;
		}


		std::vector<std::string> split_str(const std::string& str, const std::string& delim)
		{
			std::vector<std::string> ret;
			size_t pre_pos = 0;
			std::string tmp;
			size_t pos = 0;
			do {
				pos = str.find(delim, pre_pos);
				tmp = str.substr(pre_pos, pos - pre_pos);
				ret.push_back(tmp);
				pre_pos = pos + delim.size();
			} while (pos != std::string::npos);

			return ret;
		}

		void skip_first_spaces(std::string& str)
		{
			while (!str.empty() && str[0] == ' ') 
			{
				str.erase(0, 1);
			}
		}

		std::string string_trim_end(std::string str, std::string trim_chars)
		{
			std::string ret = str;

			for (int i = 0; i < (int)trim_chars.length(); ++i)
			{
				char ch = trim_chars.at(i);

				while (!ret.empty() && ret.at(ret.length() - 1) == ch)
				{
					ret.erase(ret.end() - 1);

					// ok, matched, should reset the search
					i = -1;
				}
			}

			return ret;
		}

	}
}