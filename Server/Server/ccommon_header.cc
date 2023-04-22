/*
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

#include "ccommon_header.h"

#include "cbyte_io.h"
#include "clog.h"
//#include "rtc_base/logging.h"

namespace chen {
namespace rtcp {
const size_t CommonHeader::kHeaderSizeBytes;
//    0                   1           1       2                   3
//    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// 0 |V=2|P|   C/F   |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// 1                 |  Packet Type  |
//   ----------------+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// 2                                 |             length            |
//   --------------------------------+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//// TODO@chensong 2022-12-25 
// Common header for all RTCP packets, 4 octets.
bool CommonHeader::Parse(const uint8_t* buffer, size_t size_bytes) 
{
  const uint8_t kVersion = 2;

  if (size_bytes < kHeaderSizeBytes) 
  {
    WARNING_EX_LOG ( "Too little data (%u  byte %c ) remaining in buffer to parse RTCP header (4 bytes).", size_bytes, (size_bytes != 1 ? "s" : ""));
    return false;
  }
  // rtcp 版本
  uint8_t version = buffer[0] >> 6;
  if (version != kVersion) 
  {
	  WARNING_EX_LOG ( "Invalid RTCP header: Version must be %u  but was  %u", static_cast<int>(kVersion), static_cast<int>(version));
    return false;
  }

  // 是否有扩展的数据包
  bool has_padding = (buffer[0] & 0x20) != 0;
  count_or_format_ = buffer[0] & 0x1F;
  // rtcp 包类型
  packet_type_ = buffer[1];

  // rtcp 包中数据大小 读取4个字节 就是32bit
  payload_size_ = ByteReader<uint16_t>::ReadBigEndian(&buffer[2]) * 4;

  // rtcp 包中实际数据开始地址的位置
  payload_ = buffer + kHeaderSizeBytes /*default kHeaderSizeBytes = 4*/;
  padding_size_ = 0;

  if (size_bytes < kHeaderSizeBytes + payload_size_) 
  {
	  WARNING_EX_LOG( "Buffer too small (%u  bytes) to fit an RtcpPacket with a header and  %u bytes.", size_bytes, payload_size_);
    return false;
  }

  if (has_padding) 
  {
    if (payload_size_ == 0) 
	{
		WARNING_EX_LOG("Invalid RTCP header: Padding bit set but 0 payload  size specified.");
      return false;
    }

    padding_size_ = payload_[payload_size_ - 1];
    if (padding_size_ == 0) 
	{
		WARNING_EX_LOG( "Invalid RTCP header: Padding bit set but 0 padding  size specified.");
      return false;
    }
    if (padding_size_ > payload_size_) 
	{
		WARNING_EX_LOG("Invalid RTCP header: Too many padding bytes ( %u) for a packet payload size of  %u bytes.", padding_size_, payload_size_);
      return false;
    }
    payload_size_ -= padding_size_;
  }
  return true;
}
}  // namespace rtcp
}  // namespace chen
