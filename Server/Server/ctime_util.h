/*
 *  Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
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
 */

#ifndef MODULES_RTP_RTCP_SOURCE_TIME_UTIL_H_
#define MODULES_RTP_RTCP_SOURCE_TIME_UTIL_H_

#include <stdint.h>

#include "cntp_time.h"

namespace chen {

// Converts time obtained using rtc::TimeMicros to ntp format.
// TimeMicrosToNtp guarantees difference of the returned values matches
// difference of the passed values.
// As a result TimeMicrosToNtp(rtc::TimeMicros()) doesn't guarantee to match
// system time.
// However, TimeMicrosToNtp Guarantees that returned NtpTime will be offsetted
// from rtc::TimeMicros() by integral number of milliseconds.
// Use NtpOffsetMs() to get that offset value.
NtpTime TimeMicrosToNtp(int64_t time_us);

// Difference between Ntp time and local relative time returned by
// rtc::TimeMicros()
int64_t NtpOffsetMs();

// Helper function for compact ntp representation:
// RFC 3550, Section 4. Time Format.
// Wallclock time is represented using the timestamp format of
// the Network Time Protocol (NTP).
// ...
// In some fields where a more compact representation is
// appropriate, only the middle 32 bits are used; that is, the low 16
// bits of the integer part and the high 16 bits of the fractional part.
//压缩ntp表示的帮助函数：
// RFC 3550，第4节。时间格式。
//时钟时间使用时间戳格式表示
//网络时间协议（NTP）。
// ...
//在某些领域中
//适当地，仅使用中间32位；即低16
//整数部分的位和小数部分的高16位。
inline uint32_t CompactNtp(NtpTime ntp) 
{
  return (ntp.seconds() << 16) | (ntp.fractions() >> 16);
}

// Converts interval in microseconds to compact ntp (1/2^16 seconds) resolution.
// Negative values converted to 0, Overlarge values converted to max uint32_t.
uint32_t SaturatedUsToCompactNtp(int64_t us);

// Converts interval between compact ntp timestamps to milliseconds.
// This interval can be up to ~9.1 hours (2^15 seconds).
// Values close to 2^16 seconds consider negative and result in minimum rtt = 1.
int64_t CompactNtpRttToMs(uint32_t compact_ntp_interval);

}  // namespace webrtc
#endif  // MODULES_RTP_RTCP_SOURCE_TIME_UTIL_H_
