/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
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
