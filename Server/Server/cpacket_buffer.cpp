/***********************************************************************************************
created: 		2023-07-23

author:			chensong

purpose:		vmc packet

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
#include "cpacket_buffer.h"
#include "csequence_number.h"
//#include "absl/numeric/s"
#include "clog.h"
namespace chen {
	cpacket_buffer::~cpacket_buffer()
	{
	}
	bool cpacket_buffer::init(uint32 start_buffer_size, uint32 max_buffer_size)
	{
		m_size = start_buffer_size;
		m_max_size = max_buffer_size;
		m_data_buffer.resize(start_buffer_size);
		m_sequence_buffer.resize(start_buffer_size);
		return true;
	}
	void cpacket_buffer::destroy()
	{
		m_data_buffer.clear();
		m_sequence_buffer.clear();
	}
	bool cpacket_buffer::insert_packet(cvcm_packet * packet)
	{

		uint16 seq_num = packet->m_seq_num;
		uint32 index = seq_num % m_size;
		if (!m_first_packet_received)
		{
			m_first_seq_num = seq_num;
			m_first_packet_received = true;
		}
		else if (AheadOf(m_first_seq_num, seq_num))
		{

			if (m_is_cleared_to_first_seq_num)
			{
				delete[] packet->m_data_ptr;
				packet->m_data_ptr = NULL;
				return false;
			}
			m_first_seq_num = seq_num;
		}
		if (m_sequence_buffer[index].used) 
		{
			// Duplicate packet, just delete the payload.
			if (m_data_buffer[index].m_seq_num == packet->m_seq_num) 
			{
				delete[] packet->m_data_ptr;
				packet->m_data_ptr = nullptr;
				return true;
			}

			// The packet buffer is full, try to expand the buffer.
			while (_expand_buffer_size() && m_sequence_buffer[seq_num % m_size].used) {
			}
			index = seq_num % m_size;

			//// Packet buffer is still full.
			if (m_sequence_buffer[index].used) {
				delete[] packet->m_data_ptr;
				packet->m_data_ptr = nullptr;
				return false;
			}
		}

		m_sequence_buffer[index].frame_begin = packet->is_first_packet_in_frame();
		m_sequence_buffer[index].frame_end = packet->is_last_packet_in_frame();
		m_sequence_buffer[index].seq_num = packet->m_seq_num;
		m_sequence_buffer[index].continuous = false;
		m_sequence_buffer[index].frame_created = false;
		m_sequence_buffer[index].used = true;
		m_data_buffer[index] = *packet;
		packet->m_data_ptr = nullptr;

		//UpdateMissingPackets(packet->seqNum);

		//int64_t now_ms = clock_->TimeInMilliseconds();
		//last_received_packet_ms_ = now_ms;
		//if (packet->frameType == VideoFrameType::kVideoFrameKey)
		//	last_received_keyframe_packet_ms_ = now_ms;

		///found_frames = FindFrames(seq_num);
		std::vector<cvcm_encoded_frame> fonds_frames = _find_frames(seq_num);
		for ( cvcm_encoded_frame& frame_data : fonds_frames)
		{
#if 0
			static FILE * out_file_ptr = ::fopen("test_webrtc.mp4", "wb+");
			if (out_file_ptr)
			{
				::fwrite(frame_data.data(), 1, frame_data.data_size(), out_file_ptr);
				::fflush(out_file_ptr);
			}
#endif
			frame_data.free();
		}
		return true;
	}
	bool cpacket_buffer::_expand_buffer_size()
	{
		if (m_size == m_max_size)
		{
			/*RTC_LOG(LS_WARNING) << "PacketBuffer is already at max size (" << max_size_
				<< "), failed to increase size. Clearing PacketBuffer.";
			Clear();*/
			WARNING_EX_LOG("PacketBuffer is already at max size (%u), failed to increase size. Clearing PacketBuffer.", m_max_size);
			return false;
		}

		size_t new_size = std::min(m_max_size, 2 * m_size);
		std::vector<cvcm_packet> new_data_buffer(new_size);
		std::vector<ContinuityInfo> new_sequence_buffer(new_size);
		for (size_t i = 0; i < m_size; ++i) 
		{
			if (m_sequence_buffer[i].used) 
			{
				size_t index = m_sequence_buffer[i].seq_num % new_size;
				new_sequence_buffer[index] = m_sequence_buffer[i];
				new_data_buffer[index] = m_data_buffer[i];
			}
		}
		m_size = new_size;
		m_sequence_buffer = std::move(new_sequence_buffer);
		m_data_buffer = std::move(new_data_buffer);
		//RTC_LOG(LS_INFO) << "PacketBuffer size expanded to " << new_size;

		NORMAL_EX_LOG("PacketBuffer size expanded to --> [%u]", new_size);
		
		return true;
	}

	std::vector<cvcm_encoded_frame> cpacket_buffer::_find_frames(uint16 seq_num)
	{
		std::vector<cvcm_encoded_frame> found_frames;
		for (size_t i = 0; i < m_size && _potential_new_frame(seq_num); ++i) 
		{
			size_t index = seq_num % m_size;
			m_sequence_buffer[index].continuous = true;

			// If all packets of the frame is continuous, find the first packet of the
			// frame and create an RtpFrameObject.
			//如果帧的所有数据包都是连续的，则查找
			//框架并创建RtpFrameObject。
			if (m_sequence_buffer[index].frame_end)
			{
				size_t frame_size = 0;
				int max_nack_count = -1;
				uint16_t start_seq_num = seq_num;
				//int64_t min_recv_time = m_data_buffer[index].receive_time_ms;
				//int64_t max_recv_time = m_data_buffer[index].receive_time_ms;

				// Find the start index by searching backward until the packet with
				// the |frame_begin| flag is set.
				int start_index = index;
				size_t tested_packets = 0;
				int64_t frame_timestamp = m_data_buffer[start_index].m_timestamp;

				// Identify H.264 keyframes by means of SPS, PPS, and IDR.
				bool is_h264 = false; // m_data_buffer[start_index].codec() == kVideoCodecH264;
				bool has_h264_sps = false;
				bool has_h264_pps = false;
				bool has_h264_idr = false;
				bool is_h264_keyframe = false;

				while (true) {
					++tested_packets;
					frame_size += m_data_buffer[start_index].m_size_bytes;
					max_nack_count = std::max(max_nack_count, m_data_buffer[start_index].m_timers_nacked);
					m_sequence_buffer[start_index].frame_created = true;

					//min_recv_time = std::min(min_recv_time, m_data_buffer[start_index].receive_time_ms);
					//max_recv_time = std::max(max_recv_time, m_data_buffer[start_index].receive_time_ms);

					if (!is_h264 && m_sequence_buffer[start_index].frame_begin)
					{
						break;
					}

					if (is_h264 && !is_h264_keyframe)
					{
						//const RTPVideoHeaderH264* h264_header = absl::get_if<RTPVideoHeaderH264>(&data_buffer_[start_index].video_header.video_type_header);
						//if (!h264_header || h264_header->nalus_length >= kMaxNalusPerPacket)
						//{
						//	return found_frames;
						//}
						//
						//for (size_t j = 0; j < h264_header->nalus_length; ++j)
						//{
						//	if (h264_header->nalus[j].type == H264::NaluType::kSps)
						//	{
						//		has_h264_sps = true;
						//	}
						//	else if (h264_header->nalus[j].type == H264::NaluType::kPps)
						//	{
						//		has_h264_pps = true;
						//	}
						//	else if (h264_header->nalus[j].type == H264::NaluType::kIdr)
						//	{
						//		has_h264_idr = true;
						//	}
						//}
						//if ((sps_pps_idr_is_h264_keyframe_ && has_h264_idr && has_h264_sps && has_h264_pps) ||
						//	(!sps_pps_idr_is_h264_keyframe_ && has_h264_idr))
						//{
						//	is_h264_keyframe = true;
						//}
					}

					if (tested_packets == m_size)
					{
						break;
					}

					start_index = start_index > 0 ? start_index - 1 : m_size - 1;

					// In the case of H264 we don't have a frame_begin bit (yes,
					// |frame_begin| might be set to true but that is a lie). So instead
					// we traverese backwards as long as we have a previous packet and
					// the timestamp of that packet is the same as this one. This may cause
					// the PacketBuffer to hand out incomplete frames.
					// See: https://bugs.chromium.org/p/webrtc/issues/detail?id=7106
					if (is_h264 &&
						(!m_sequence_buffer[start_index].used ||
							m_data_buffer[start_index].m_timestamp != frame_timestamp)) {
						break;
					}

					--start_seq_num;
				}

				if (is_h264) 
				{
					// Warn if this is an unsafe frame.
					//if (has_h264_idr && (!has_h264_sps || !has_h264_pps)) {
					//	RTC_LOG(LS_WARNING)
					//		<< "Received H.264-IDR frame "
					//		<< "(SPS: " << has_h264_sps << ", PPS: " << has_h264_pps
					//		<< "). Treating as "
					//		<< (sps_pps_idr_is_h264_keyframe_ ? "delta" : "key")
					//		<< " frame since WebRTC-SpsPpsIdrIsH264Keyframe is "
					//		<< (sps_pps_idr_is_h264_keyframe_ ? "enabled." : "disabled");
					//}

					// Now that we have decided whether to treat this frame as a key frame
					// or delta frame in the frame buffer, we update the field that
					// determines if the RtpFrameObject is a key frame or delta frame.
					//const size_t first_packet_index = start_seq_num % size_;
					//RTC_CHECK_LT(first_packet_index, size_);
					//if (is_h264_keyframe) {
					//	data_buffer_[first_packet_index].frameType =
					//		VideoFrameType::kVideoFrameKey;
					//}
					//else {
					//	data_buffer_[first_packet_index].frameType =
					//		VideoFrameType::kVideoFrameDelta;
					//}
					//
					//// If this is not a keyframe, make sure there are no gaps in the
					//// packet sequence numbers up until this point.
					//if (!is_h264_keyframe && missing_packets_.upper_bound(start_seq_num) !=
					//	missing_packets_.begin()) {
					//	uint16_t stop_index = (index + 1) % size_;
					//	while (start_index != stop_index) {
					//		sequence_buffer_[start_index].frame_created = false;
					//		start_index = (start_index + 1) % size_;
					//	}
					//
					//	return found_frames;
					//}
				}

				/*missing_packets_.erase(missing_packets_.begin(),
					missing_packets_.upper_bound(seq_num));*/
				cvcm_encoded_frame  vcm_frame;
				if (!vcm_frame.alloc(frame_size))
				{
					WARNING_EX_LOG("alloc failed !!! size = %u", frame_size);
				}
				else
				{
					if (!_get_bitstream(start_seq_num, seq_num, 0, vcm_frame))
					{
						found_frames.push_back(vcm_frame);
					}
				}
				//found_frames.emplace_back(
				//	new RtpFrameObject(this, start_seq_num, seq_num, frame_size,
				//		max_nack_count, min_recv_time, max_recv_time));
			}
			++seq_num;
		}



		return found_frames;
		//return std::vector<cvcm_encoded_frame>();
	}

	bool cpacket_buffer::_potential_new_frame(uint16 seq_num) const
	{
		size_t index = seq_num % m_size;
		int prev_index = index > 0 ? index - 1 : m_size - 1;

		if (!m_sequence_buffer[index].used)
		{
			return false;
		}
		if (m_sequence_buffer[index].seq_num != seq_num)
		{
			return false;
		}
		if (m_sequence_buffer[index].frame_created)
		{
			return false;
		}
		if (m_sequence_buffer[index].frame_begin)
		{
			return true;
		}
		if (!m_sequence_buffer[prev_index].used)
		{
			return false;
		}
		if (m_sequence_buffer[prev_index].frame_created)
		{
			return false;
		}
		if (m_sequence_buffer[prev_index].seq_num !=
			static_cast<uint16_t>(m_sequence_buffer[index].seq_num - 1)) {
			return false;
		}
		if (m_data_buffer[prev_index].m_timestamp != m_data_buffer[index].m_timestamp)
		{
			return false;
		}
		if (m_sequence_buffer[prev_index].continuous)
		{
			return true;
		}
		 
		return true;
	}

	bool cpacket_buffer::_get_bitstream(uint16 first_seq_num, uint16 last_seq_num, uint32 timestamp_, cvcm_encoded_frame & vcm_frame)
	{
		size_t index = first_seq_num % m_size;//frame.first_seq_num() % size_;
		size_t end = (last_seq_num +1) % m_size; //(frame.last_seq_num() + 1) % size_;
		uint16_t seq_num = first_seq_num;// frame.first_seq_num();
		uint32_t timestamp = timestamp_;// frame.Timestamp();
		uint8* destination = vcm_frame.data();
		uint8_t* destination_end = destination + vcm_frame.data_size();

		do {
			// Check both seq_num and timestamp to handle the case when seq_num wraps
			// around too quickly for high packet rates.
			if (!m_sequence_buffer[index].used ||
				m_sequence_buffer[index].seq_num != seq_num/* ||
				m_data_buffer[index].m_timestamp != timestamp*/) {
				return false;
			}

			RTC_DCHECK_EQ(m_data_buffer[index].m_seq_num, m_sequence_buffer[index].seq_num);
			size_t length = m_data_buffer[index].m_size_bytes;
			if (destination + length > destination_end) 
			{
				/*RTC_LOG(LS_WARNING) << "Frame (" << frame.id.picture_id << ":"
					<< static_cast<int>(frame.id.spatial_layer) << ")"
					<< " bitstream buffer is not large enough.";*/
				WARNING_EX_LOG("--");
				return false;
			}

			const uint8_t* source = m_data_buffer[index].m_data_ptr;
			memcpy(destination, source, length);
			destination += length;
			index = (index + 1) % m_size;
			++seq_num;
		} while (index != end);

		return true;
	}

//	for (std::unique_ptr<RtpFrameObject>& frame : found_frames)
//	{
//
//#if 0
//		static FILE * out_file_ptr = ::fopen("./test_packet_buffer.mp4", "wb+");
//		if (out_file_ptr)
//		{
//			::fwrite(frame->data(), 1, frame->size(), out_file_ptr);
//			::fflush(out_file_ptr);
//		}
//#endif // #if 0
//		assembled_frame_callback_->OnAssembledFrame(std::move(frame));
//	}
//
	
}