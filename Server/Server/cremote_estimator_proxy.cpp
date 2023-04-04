/***********************************************************************************************
created: 		2023-04-03

author:			chensong

purpose:		api_rtc_publish


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