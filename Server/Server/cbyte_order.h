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


#ifndef _C_BYTE_ORDER_H_
#define _C_BYTE_ORDER_H_

#include <stdint.h>

#if defined(__unix__) && !defined(__native_client__)
#include <arpa/inet.h>
#endif

//#include "rtc_base/system/arch.h"

#if defined(__APPLE__)
#include <libkern/OSByteOrder.h>

#define htobe16(v) OSSwapHostToBigInt16(v)
#define htobe32(v) OSSwapHostToBigInt32(v)
#define htobe64(v) OSSwapHostToBigInt64(v)
#define be16toh(v) OSSwapBigToHostInt16(v)
#define be32toh(v) OSSwapBigToHostInt32(v)
#define be64toh(v) OSSwapBigToHostInt64(v)

#define htole16(v) OSSwapHostToLittleInt16(v)
#define htole32(v) OSSwapHostToLittleInt32(v)
#define htole64(v) OSSwapHostToLittleInt64(v)
#define le16toh(v) OSSwapLittleToHostInt16(v)
#define le32toh(v) OSSwapLittleToHostInt32(v)
#define le64toh(v) OSSwapLittleToHostInt64(v)

#elif defined(_WIN32) || defined(__native_client__)

#if defined(_WIN32)
#include <stdlib.h>
#include <winsock2.h>
#else
#include <netinet/in.h>
#endif  // defined(WEBRTC_WIN)

#if defined(_WIN32)/*WEBRTC_ARCH_LITTLE_ENDIAN*/
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
#if defined(_WIN32)
#define htobe64(v) _byteswap_uint64(v)
#define be64toh(v) _byteswap_uint64(v)
#endif  // defined(WEBRTC_WIN)
#if defined(__native_client__)
#define htobe64(v) __builtin_bswap64(v)
#define be64toh(v) __builtin_bswap64(v)
#endif  // defined(__native_client__)

#elif defined(__unix__ /*WEBRTC_ARCH_BIG_ENDIAN*/)
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
#if defined(__WIN32)
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

#elif defined(__unix__)
#include <endian.h>
#else
#error "Missing byte order functions for this arch."
#endif  // defined(WEBRTC_MAC)

namespace chen {

// Reading and writing of little and big-endian numbers from memory

inline void Set8(void* memory, size_t offset, uint8_t v) {
  static_cast<uint8_t*>(memory)[offset] = v;
}

inline uint8_t Get8(const void* memory, size_t offset) {
  return static_cast<const uint8_t*>(memory)[offset];
}

inline void SetBE16(void* memory, uint16_t v) {
  *static_cast<uint16_t*>(memory) = htobe16(v);
}

inline void SetBE32(void* memory, uint32_t v) {
  *static_cast<uint32_t*>(memory) = htobe32(v);
}

inline void SetBE64(void* memory, uint64_t v) {
  *static_cast<uint64_t*>(memory) = htobe64(v);
}

inline uint16_t GetBE16(const void* memory) {
  return be16toh(*static_cast<const uint16_t*>(memory));
}

inline uint32_t GetBE32(const void* memory) {
  return be32toh(*static_cast<const uint32_t*>(memory));
}

inline uint64_t GetBE64(const void* memory) {
  return be64toh(*static_cast<const uint64_t*>(memory));
}

inline void SetLE16(void* memory, uint16_t v) {
  *static_cast<uint16_t*>(memory) = htole16(v);
}

inline void SetLE32(void* memory, uint32_t v) {
  *static_cast<uint32_t*>(memory) = htole32(v);
}

inline void SetLE64(void* memory, uint64_t v) {
  *static_cast<uint64_t*>(memory) = htole64(v);
}

inline uint16_t GetLE16(const void* memory) {
  return le16toh(*static_cast<const uint16_t*>(memory));
}

inline uint32_t GetLE32(const void* memory) {
  return le32toh(*static_cast<const uint32_t*>(memory));
}

inline uint64_t GetLE64(const void* memory) {
  return le64toh(*static_cast<const uint64_t*>(memory));
}

// Check if the current host is big endian.
inline bool IsHostBigEndian() {
#if defined(__unix__)/*WEBRTC_ARCH_BIG_ENDIAN*/
  return true;
#else
  return false;
#endif
}

inline uint16_t HostToNetwork16(uint16_t n) {
  return htobe16(n);
}

inline uint32_t HostToNetwork32(uint32_t n) {
  return htobe32(n);
}

inline uint64_t HostToNetwork64(uint64_t n) {
  return htobe64(n);
}

inline uint16_t NetworkToHost16(uint16_t n) {
  return be16toh(n);
}

inline uint32_t NetworkToHost32(uint32_t n) {
  return be32toh(n);
}

inline uint64_t NetworkToHost64(uint64_t n) {
  return be64toh(n);
}

}  // namespace chen

#endif  // RTC_BASE_BYTE_ORDER_H_
