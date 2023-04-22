/***********************************************************************************************
created: 		2023-04-03

author:			chensong

purpose:		api_rtc_publish
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
#include "cremote_estimator_proxy.h"
#include "clog.h"
#include "ctransport_feedback.h"
#include "crtc_transport.h"
namespace chen {


	// The maximum allowed value for a timestamp in milliseconds. This is lower
// than the numerical limit since we often convert to microseconds.
	static const  int64  kMaxTimeMs = std::numeric_limits<int64_t>::max() / 1000;
	static const  int64  kBackWindowMs =  500; // 
	static const  int64  kMaxNumberOfPackets =  32768;
	/*cremote_estimator_proxy::cremote_estimator_proxy()
	{
	}*/
	cremote_estimator_proxy::~cremote_estimator_proxy()
	{
	}
	bool cremote_estimator_proxy::init()
	{
		return true;
	}
	void cremote_estimator_proxy::update(uint32 uDeltaTime)
	{
	}
	void cremote_estimator_proxy::destroy()
	{
	}
	void cremote_estimator_proxy::on_packet_arrival(uint16 sequence_number, uint32 media_ssrc, int64 arrival_time)
	{
		 
		if (arrival_time < 0 || arrival_time > kMaxTimeMs)
		{
			WARNING_EX_LOG("Arrival time out of bounds: %u" , arrival_time);
			return;
		}

		if (m_periodic_window_start_seq && m_packet_arrival_times.lower_bound(m_periodic_window_start_seq) == m_packet_arrival_times.end())
		{
			for (std::map<int64, int64>::iterator it = m_packet_arrival_times.begin();
				it != m_packet_arrival_times.end() && it->first < sequence_number && arrival_time - it->second >= kBackWindowMs/*500*/;)
			{//
				//WARNING_EX_LOG("500 ms ===> [cur sequence_number = %u][del seq  = %u][time = %u]", sequence_number, it->first, it->second);
				it = m_packet_arrival_times.erase(it);
			}
		}
		// 是否第一次发送包
		if (!m_periodic_window_start_seq || sequence_number <   m_periodic_window_start_seq)
		{
			m_periodic_window_start_seq = sequence_number;
		}
		if (m_packet_arrival_times.find(sequence_number) != m_packet_arrival_times.end())
		{
			return;
		}
		m_packet_arrival_times[sequence_number] = arrival_time;
		m_media_ssrc = media_ssrc;
		//删除多余统计包的数据
		std::map<int64, int64>::iterator first_arrival_time_to_keep = m_packet_arrival_times.lower_bound(m_packet_arrival_times.rbegin()->first - kMaxNumberOfPackets /* 32768 */);
		if (first_arrival_time_to_keep != m_packet_arrival_times.begin())
		{
			m_packet_arrival_times.erase(m_packet_arrival_times.begin(), first_arrival_time_to_keep);
			//if (send_periodic_feedback_)
			{
				//WARNING_EX_LOG("[m_periodic_window_start_seq = %u][m_packet_arrival_times.begin()->first = %u]", m_periodic_window_start_seq, m_packet_arrival_times.begin()->first);
				// |packet_arrival_times_| cannot be empty since we just added one element
				// and the last element is not deleted.
				//RTC_DCHECK(!packet_arrival_times_.empty());
				m_periodic_window_start_seq = m_packet_arrival_times.begin()->first;
			}
		}

		if (false)
		{
			// SendFeedbackOnRequests();
		}
	}
	void cremote_estimator_proxy::send_periodic_Feedbacks()
	{
		if (!m_periodic_window_start_seq || !m_rtc_transport_ptr)
		{
			return;
		}
		auto callback = [&]( ArrayView<const uint8 > packet)
		{
			//DEBUG_EX_LOG(" --->>>>");
			m_rtc_transport_ptr->send_rtcp(packet.data(), packet.size());
			/*if (transport_->SendRtcp(packet.data(), packet.size()))
			{
				if (event_log_)
					event_log_->Log(absl::make_unique<RtcEventRtcpPacketOutgoing>(packet));
			}
			else {
				send_failure = true;
			}*/
		};
		for (std::map<int64, int64>::iterator begin_iterator = m_packet_arrival_times.lower_bound(m_periodic_window_start_seq);
			begin_iterator != m_packet_arrival_times.cend(); begin_iterator = m_packet_arrival_times.lower_bound(m_periodic_window_start_seq))
		{
			rtcp::TransportFeedback feedback_packet;
			// TODO@chensong 2022-12-01  发送feedback packet check data size
			 m_periodic_window_start_seq = BuildFeedbackPacket(m_feedback_packet_count++, m_media_ssrc, m_periodic_window_start_seq,
			begin_iterator, m_packet_arrival_times.cend(), &feedback_packet);

			 feedback_packet.Build(1500 - 28, callback);
			//RTC_DCHECK(feedback_sender_ != nullptr);
			//feedback_sender_->SendTransportFeedback(&feedback_packet);
			// Note: Don't erase items from packet_arrival_times_ after sending, in case
			// they need to be re-sent after a reordering. Removal will be handled
			// by OnPacketArrival once packets are too old.
		}
	}


	int64_t cremote_estimator_proxy::BuildFeedbackPacket(uint8_t feedback_packet_count, uint32_t media_ssrc, int64_t base_sequence_number,
		std::map<int64, int64>::const_iterator begin_iterator, std::map<int64, int64>::const_iterator end_iterator,
		rtcp::TransportFeedback* feedback_packet)
	{
		RTC_DCHECK(begin_iterator != end_iterator);

		// TODO(sprang): Measure receive times in microseconds and remove the
		// conversions below.
		feedback_packet->SetMediaSsrc(media_ssrc);
		// Base sequence number is the expected first sequence number. This is known,
		// but we might not have actually received it, so the base time shall be the
		// time of the first received packet in the feedback.
		// TODO@chensong 2023-03-31   设置基类数  和时间戳毫秒还是微妙呢
		feedback_packet->SetBase(static_cast<uint16_t>(base_sequence_number & 0xFFFF), begin_iterator->second * 1000);
		// TODO@chensong 2022-12-02 RTCP的反馈信息 中 feedback_packet_number
		feedback_packet->SetFeedbackSequenceNumber(feedback_packet_count);
		int64_t next_sequence_number = base_sequence_number;




		for (auto it = begin_iterator; it != end_iterator; ++it)
		{

			if (!feedback_packet->AddReceivedPacket(static_cast<uint16_t>(it->first & 0xFFFF), it->second * 1000))
			{
				// If we can't even add the first seq to the feedback packet, we won't be
				// able to build it at all.
				cassert(begin_iterator != it);

				// Could not add timestamp, feedback packet might be full. Return and
				// try again with a fresh packet.
				break;
			}
			next_sequence_number = it->first + 1;
		}
		return next_sequence_number;
	}

}