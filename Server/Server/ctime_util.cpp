﻿/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
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

#include "ctime_util.h"

#include <algorithm>

//#include "rtc_base/checks.h"
//#include "rtc_base/time_utils.h"
#include "cuv_util.h"
namespace chen {
	static const int64_t kNumMillisecsPerSec = INT64_C(1000);
	static const int64_t kNumMicrosecsPerSec = INT64_C(1000000);
	static const int64_t kNumNanosecsPerSec = INT64_C(1000000000);

	static const int64_t kNumMicrosecsPerMillisec =
		kNumMicrosecsPerSec / kNumMillisecsPerSec;
	static const int64_t kNumNanosecsPerMillisec =
		kNumNanosecsPerSec / kNumMillisecsPerSec;
	static const int64_t kNumNanosecsPerMicrosec =
		kNumNanosecsPerSec / kNumMicrosecsPerSec;
namespace {
// TODO(danilchap): Make generic, optimize and move to base.
inline int64_t DivideRoundToNearest(int64_t x, uint32_t y)
{
  // Callers ensure x is positive and x + y / 2 doesn't overflow.
  return (x + y / 2) / y;
}

int64_t NtpOffsetMsCalledOnce() {
  constexpr int64_t kNtpJan1970Sec = 2208988800;
  int64_t clock_time = uv_util::GetTimeMs();// rtc::TimeMillis();
  int64_t utc_time = uv_util::GetTimeMs();// rtc::TimeUTCMillis();
  return utc_time - clock_time + kNtpJan1970Sec * 1000;
}

}  // namespace

int64_t NtpOffsetMs() {
  // Calculate the offset once.
  static int64_t ntp_offset_ms = NtpOffsetMsCalledOnce();
  return ntp_offset_ms;
}

NtpTime TimeMicrosToNtp(int64_t time_us)
{
  // Since this doesn't return a wallclock time, but only NTP representation
  // of rtc::TimeMillis() clock, the exact offset doesn't matter.
  // To simplify conversions between NTP and RTP time, this offset is
  // limited to milliseconds in resolution.
  //因为这不会返回挂钟时间，而只返回NTP表示
  //对于rtc:：TimeMillis（）时钟，精确的偏移量无关紧要。
  //为了简化NTP和RTP时间之间的转换，此偏移量为
  //分辨率限制为毫秒。
  int64_t time_ntp_us = time_us + NtpOffsetMs() * 1000;
  RTC_DCHECK_GE(time_ntp_us, 0);  // Time before year 1900 is unsupported.

  // TODO(danilchap): Convert both seconds and fraction together using int128
  // when that type is easily available.
  // Currently conversion is done separetly for seconds and fraction of a second
  // to avoid overflow.

  // Convert seconds to uint32 through uint64 for well-defined cast.
  // Wrap around (will happen in 2036) is expected for ntp time.

  // TODO（danilcap）：使用int128同时转换秒和分数
  //当这种类型很容易获得时。
  //当前，转换是以秒和几分之一秒的时间分别完成的
  //以避免溢出。
  //通过uint64将秒转换为uint32，以实现定义良好的转换。
  //预计ntp时间会出现环绕（将于2036年发生）。
  uint32_t ntp_seconds = static_cast<uint64_t>(time_ntp_us /  kNumMicrosecsPerSec);

  // Scale fractions of the second to ntp resolution.
  constexpr int64_t kNtpInSecond = 1LL << 32;
  int64_t us_fractions = time_ntp_us %  kNumMicrosecsPerSec;
  uint32_t ntp_fractions = us_fractions * kNtpInSecond /  kNumMicrosecsPerSec;
  return NtpTime(ntp_seconds, ntp_fractions);
}

uint32_t SaturatedUsToCompactNtp(int64_t us) {
  constexpr uint32_t kMaxCompactNtp = 0xFFFFFFFF;
  constexpr int kCompactNtpInSecond = 0x10000;
  if (us <= 0)
    return 0;
  if (us >= kMaxCompactNtp *  kNumMicrosecsPerSec / kCompactNtpInSecond)
    return kMaxCompactNtp;
  // To convert to compact ntp need to divide by 1e6 to get seconds,
  // then multiply by 0x10000 to get the final result.
  // To avoid float operations, multiplication and division swapped.
  return DivideRoundToNearest(us * kCompactNtpInSecond,
                               kNumMicrosecsPerSec);
}

int64_t CompactNtpRttToMs(uint32_t compact_ntp_interval) {
  // Interval to convert expected to be positive, e.g. rtt or delay.
  // Because interval can be derived from non-monotonic ntp clock,
  // it might become negative that is indistinguishable from very large values.
  // Since very large rtt/delay are less likely than non-monotonic ntp clock,
  // those values consider to be negative and convert to minimum value of 1ms.

	//转换间隔预期为正，例如rtt或delay。
  //因为间隔可以从非单调ntp时钟导出，
  //它可能变成负值，与非常大的值无法区分。
  //由于非常大的rtt/延迟比非单调ntp时钟不太可能，
  //这些值被认为是负值，并转换为1ms的最小值。
	if (compact_ntp_interval > 0x80000000)
	{
		return 1;
	}
  // Convert to 64bit value to avoid multiplication overflow.
  int64_t value = static_cast<int64_t>(compact_ntp_interval);
  // To convert to milliseconds need to divide by 2^16 to get seconds,
  // then multiply by 1000 to get milliseconds. To avoid float operations,
  // multiplication and division swapped.
  //要转换为毫秒，需要除以2^16得到秒，
  //然后乘以1000得到毫秒。为了避免浮动操作，
  //乘法和除法互换。
  int64_t ms = DivideRoundToNearest(value * 1000, 1 << 16);
  // Rtt value 0 considered too good to be true and increases to 1.
  return std::max<int64_t>(ms, 1);
}
}  // namespace webrtc
