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

#ifndef C_TRANSPORT_FEEDBACK_H_
#define C_TRANSPORT_FEEDBACK_H_

#include <memory>
#include <vector>

#include "crtp_fb.h"

namespace chen {
namespace rtcp {
 class CommonHeader;

class TransportFeedback : public crtp_fb {
 public:
  class ReceivedPacket {
   public:
    ReceivedPacket(uint16 sequence_number, int16 delta_ticks)
        : sequence_number_(sequence_number), delta_ticks_(delta_ticks) {}
    ReceivedPacket(const ReceivedPacket&) = default;
    ReceivedPacket& operator=(const ReceivedPacket&) = default;

    uint16 sequence_number() const { return sequence_number_; }
    int16 delta_ticks() const { return delta_ticks_; }
    int32 delta_us() const { return delta_ticks_ * kDeltaScaleFactor; }

   private:
    uint16 sequence_number_;
    int16 delta_ticks_;
  };
  // TODO(sprang): IANA reg?
  static const  uint8_t kFeedbackMessageType = 15;
  // Convert to multiples of 0.25ms.
  static const int kDeltaScaleFactor = 250;
  // Maximum number of packets (including missing) TransportFeedback can report.
  //TODO@chensong 2023-03-31 一个transportfeedback可以报告的最大packets数量,2字节的最大值
  static const size_t kMaxReportedPackets = 0xffff;

  TransportFeedback();
  explicit TransportFeedback(bool include_timestamps);  // If |include_timestamps| is set to false, the
                                 // created packet will not contain the receive
                                 // delta block. 也就是说，只保留是否收到的信息
  TransportFeedback(const TransportFeedback&);
  TransportFeedback(TransportFeedback&&);

  ~TransportFeedback() override;
  // TransportFeedback都有base_sequence和ref time
  void SetBase(uint16_t base_sequence,     // Seq# of first packet in this msg.
               int64_t ref_timestamp_us);  // Reference timestamp for this msg.
  void SetFeedbackSequenceNumber(uint8_t feedback_sequence);
  // NOTE: This method requires increasing sequence numbers (excepting wraps).
  //序列号和接收到的时间
  bool AddReceivedPacket(uint16_t sequence_number, int64_t timestamp_us);
  const std::vector<ReceivedPacket>& GetReceivedPackets() const;

  uint16_t GetBaseSequence() const;

  // Returns number of packets (including missing) this feedback describes.
  size_t GetPacketStatusCount() const { return num_seq_no_; }

  // Get the reference time in microseconds, including any precision loss.
  int64_t GetBaseTimeUs() const;

  // Does the feedback packet contain timestamp information?
  bool IncludeTimestamps() const { return include_timestamps_; }

  bool Parse(const CommonHeader& packet);
  static std::unique_ptr<TransportFeedback> ParseFrom(const uint8_t* buffer, size_t length);
  // Pre and postcondition for all public methods. Should always return true.
  // This function is for tests.
  bool IsConsistent() const;

  size_t BlockLength() const override;
  size_t PaddingLength() const;

  bool Create(uint8_t* packet, size_t* position, size_t max_length, PacketReadyCallback callback) const override;
  const std::vector<ReceivedPacket>& GetPacket() const { return packets_; }
  const std::vector<uint16_t>& GetEncodedChunks() const { return encoded_chunks_; }
  //const std::string ToString() const;
  //std::string ToString();
 private:
  // Size in bytes of a delta time in rtcp packet.
  // Valid values are 0 (packet wasn't received), 1 or 2.
  // delta大：2字节描述；delta小：1字节描述
  using DeltaSize = uint8_t;//1 个time delta占据的字节数
  // Keeps DeltaSizes that can be encoded into single chunk if it is last chunk.
  //核心的功能都在LastChunk里面实现的，是对单个packet
  // chunk的处理方法（包括了不同方式的编解码 行程编解码，1 bit状态向量编解码  2 bits状态向量编解码
  class LastChunk {
   public:
    using DeltaSize = TransportFeedback::DeltaSize; //取值为0， 1， 2

    LastChunk();

    bool Empty() const;
    void Clear();
    // Return if delta sizes still can be encoded into single chunk with added
    // |delta_size|.
    bool CanAdd(DeltaSize delta_size) const;
    // Add |delta_size|, assumes |CanAdd(delta_size)|,
    void Add(DeltaSize delta_size);

    // Encode chunk as large as possible removing encoded delta sizes.
    // Assume CanAdd() == false for some valid delta_size.
    uint16_t Emit();
    // Encode all stored delta_sizes into single chunk, pad with 0s if needed.
    uint16_t EncodeLast() const;

    // Decode up to |max_size| delta sizes from |chunk|.
    void Decode(uint16_t chunk, size_t max_size);
    // Appends content of the Lastchunk to |deltas|.
    void AppendTo(std::vector<DeltaSize>* deltas) const;

   

   private:
    //最大行程长度 13个1
    static constexpr size_t kMaxRunLengthCapacity = 0x1fff;
    // status vector： 1 bit(0):没有接收到，14个状态
    static constexpr size_t kMaxOneBitCapacity = 14;
    // status vector: 1 bit（1）：接收到（2位1个状态），7个packets的状态
    static constexpr size_t kMaxTwoBitCapacity = 7;
    static constexpr size_t kMaxVectorCapacity = kMaxOneBitCapacity;
    static constexpr DeltaSize kLarge = 2;

    uint16_t EncodeOneBit() const;
    void DecodeOneBit(uint16_t chunk, size_t max_size);

    uint16_t EncodeTwoBit(size_t size) const;
    void DecodeTwoBit(uint16_t chunk, size_t max_size);

    uint16_t EncodeRunLength() const;
    void DecodeRunLength(uint16_t chunk, size_t max_size);

    DeltaSize delta_sizes_[kMaxVectorCapacity];
    size_t size_;
	// TODO@chensong 2023-03-31    判断当前字节与第一个字节是否相等
    bool all_same_;
    bool has_large_delta_;
  };

  // Reset packet to consistent empty state.
  void Clear();

  bool AddDeltaSize(DeltaSize delta_size);
  // TODO@chensong 2023-03-31
  uint16_t base_seq_no_; //序列号基
  uint16_t num_seq_no_;//packets数量
  int32_t base_time_ticks_;//参考时间

  // TODO@chensong 2022-12-07 RTCP Transport-wide Congestion control (Transport-cc (15))的反馈信息包自动增加字段
  uint8_t feedback_seq_;  //反馈序列号（标识）
  bool include_timestamps_;//是否包含time

  int64_t last_timestamp_us_;//上一个RTP packet的接收时间
  std::vector<ReceivedPacket> packets_;//需要把接收到的packets打入transport feedback
  // All but last encoded packet chunks.
  std::vector<uint16_t> encoded_chunks_;//除了最后一个packet chunk
  LastChunk last_chunk_;//最后一个chunk
  size_t size_bytes_;
};

}  // namespace rtcp

std::string ToString(
    const chen::rtcp::TransportFeedback::ReceivedPacket& packet);
//std::string ToString(const webrtc::rtcp::TransportFeedback& feedback);
}  // namespace webrtc
#endif  // MODULES_RTP_RTCP_SOURCE_RTCP_PACKET_TRANSPORT_FEEDBACK_H_
