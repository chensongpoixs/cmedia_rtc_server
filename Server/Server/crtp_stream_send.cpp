/***********************************************************************************************
created: 		2022-08-10

author:			chensong

purpose:		room

Copyright boost

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
#include "crtp_stream_send.h"
#include "crandom.h"
#include "crtc_consumer.h"
namespace chen {


	static constexpr size_t MaxRequestedPackets{ 17 };
	thread_local static std::vector<crtp_stream_send::StorageItem*> RetransmissionContainer(
		MaxRequestedPackets + 1);

	// Don't retransmit packets older than this (ms).
	static constexpr uint32_t MaxRetransmissionDelay{ 500u  };
	static constexpr uint32_t DefaultRtt{ 100 };



	static void resetStorageItem(crtp_stream_send::StorageItem* storageItem)
	{
		 

		cassert(storageItem, "storageItem cannot be nullptr");

		delete storageItem->packet;

		storageItem->packet = nullptr;
		storageItem->resentAtMs = 0;
		storageItem->sentTimes = 0;
		storageItem->rtxEncoded = false;
	}
	crtp_stream_send::~crtp_stream_send()
	{
		if (this->m_storage.empty())
		{
			return;
		}

		for (uint32_t idx{ 0 }; idx < this->m_buffer.size(); ++idx)
		{
			auto* storageItem = this->m_buffer[idx];

			if (!storageItem)
			{
				cassert(!this->m_buffer[idx], "key should be NULL");

				continue;
			}

			// Reset (free RTP packet) the storage item.
			resetStorageItem(storageItem);

			// Unfill the buffer item.
			this->m_buffer[idx] = nullptr;
		}
		m_buffer.clear();
		m_storage.clear();
		// Reset buffer.
		this->m_buffer_start_idx = 0;
		this->m_buffer_size = 0;
	}
	void crtp_stream_send::set_rtx(uint8 payload_type, uint32 ssrc)
	{
		crtp_stream::set_rtx(payload_type, ssrc);

		m_rtx_seq = c_rand.rand(0u, 0xFFFF);
	}
	bool crtp_stream_send::receive_packet(RTC::RtpPacket * packet)
	{
		if (!crtp_stream::receive_packet(packet))
		{
			return false;
		}



		if (!m_storage.empty())
		{ 
			_store_packet(packet);
		}
		// Increase transmission counter.
		m_transmission_counter.Update(packet);
		return true;
	}

	void crtp_stream_send::receive_nack(RTC::RTCP::FeedbackRtpNackPacket* nackPacket)
	{
		//MS_TRACE();

		m_nack_count++;

		for (auto it = nackPacket->Begin(); it != nackPacket->End(); ++it)
		{
			RTC::RTCP::FeedbackRtpNackItem* item = *it;

			m_nack_packet_count += item->CountRequestedPackets();
			//NORMAL_EX_LOG("[nack size = %u][item->GetPacketId() = %u]", item->CountRequestedPackets(), item->GetPacketId());
			FillRetransmissionContainer(item->GetPacketId(), item->GetLostPacketBitmask());

			for (auto* storageItem : RetransmissionContainer)
			{
				if (!storageItem)
				{
					break;
				}

				// Note that this is an already RTX encoded packet if RTX is used
				// (FillRetransmissionContainer() did it).
				RTC::RtpPacket* packet = storageItem->packet;

				// Retransmit the packet.
				/*static_cast<RTC::RtpStreamSend::Listener*>(this->listener)
					->OnRtpStreamRetransmitRtpPacket(this, packet);*/
				if (m_rtc_consumer_ptr)
				{
					//NORMAL_EX_LOG("rtx ---> seq = %u", packet->GetSequenceNumber());
					m_rtc_consumer_ptr->OnRtpStreamRetransmitRtpPacket(this, packet);
				}
				// Mark the packet as retransmitted.
				crtp_stream_send::packet_retransmitted(packet);

				// Mark the packet as repaired (only if this is the first retransmission).
				if (storageItem->sentTimes == 1)
				{
					crtp_stream_send::packet_repaired(packet);
				}
			}
		}
	}

	void crtp_stream_send::receive_rtcp_receiver_report(RTC::RTCP::ReceiverReport * report)
	{
		// Get the NTP representation of the current timestamp.
		uint64_t nowMs = uv_util::GetTimeMs();
		auto ntp = rtc_time::TimeMs2Ntp(nowMs);

		// Get the compact NTP representation of the current timestamp.
		uint32_t compactNtp = (ntp.seconds & 0x0000FFFF) << 16;

		compactNtp |= (ntp.fractions & 0xFFFF0000) >> 16;
		//<<<<<<< HEAD
				// NTP时间戳的中间32位
		//=======
				//  NTP时间戳的中间32位
		//>>>>>>> 20adb84c0b9b03c2ea608d143e974cef7a4a3e62
		uint32_t lastSr = report->GetLastSenderReport();

		// 记录接收SR的时间与发送SR的时间差
		uint32_t dlsr = report->GetDelaySinceLastSenderReport();

		// RTT in 1/2^16 second fractions.
		uint32_t rtt{ 0 };

		// If no Sender Report was received by the remote endpoint yet, ignore lastSr
		// and dlsr values in the Receiver Report.
		if (lastSr && dlsr && (compactNtp > dlsr + lastSr))
		{
			//INFO_EX_LOG("[lastSr = %u][dlstr = %u][compactNtp = %u]", lastSr, dlsr, compactNtp);
			rtt = compactNtp - dlsr - lastSr;
		}
		else
		{
			//INFO_EX_LOG("[lastSr = %u][dlstr = %u][compactNtp = %u]", lastSr, dlsr, compactNtp);
		}
		// RTT in milliseconds.
		m_rtt = static_cast<float>(rtt >> 16) * 1000;
		m_rtt += (static_cast<float>(rtt & 0x0000FFFF) / 65536) * 1000;

		if (this->m_rtt > 0.0f)
		{
			this->m_has_rtt = true;
		}

		//自接收开始漏包总数，迟到包不算漏包，重传有可以导致负数
//=======

//>>>>>>> 20adb84c0b9b03c2ea608d143e974cef7a4a3e62
		this->m_packets_lost = report->GetTotalLost();

		//上一次报告之后从SSRC_n来包的漏包比列
		this->m_fraction_lost = report->GetFractionLost();
		//NORMAL_EX_LOG("[this->m_packets_lost = %u][this->m_fraction_lost = %u]", this->m_packets_lost, this->m_fraction_lost);
		//INFO_EX_LOG("[packetsLost = %u][fractionLost = %u][rtt = %u]", packetsLost, fractionLost, rtt);
		// Update the score with the received RR.
		UpdateScore(report);
	}

	RTC::RTCP::SenderReport* crtp_stream_send::get_rtcp_sender_report(uint64_t nowMs)
	{
		if (this->m_transmission_counter.GetPacketCount() == 0u)
		{
			return nullptr;
		}

		auto ntp = rtc_time::TimeMs2Ntp(nowMs);
		auto* report = new RTC::RTCP::SenderReport();

		// Calculate TS difference between now and maxPacketMs.
		auto diffMs = nowMs - m_max_packet_ms;
		auto diffTs = diffMs * 90000 / 1000;

		report->SetSsrc(m_params.ssrc);
		report->SetPacketCount(m_transmission_counter.GetPacketCount());
		report->SetOctetCount(m_transmission_counter.GetBytes());
		report->SetNtpSec(ntp.seconds);
		report->SetNtpFrac(ntp.fractions);
		report->SetRtpTs(m_max_packet_ts + diffTs);

		// Update info about last Sender Report.
		m_last_sender_report_ntp_ms = nowMs;
		this->m_last_sender_repor_ts = this->m_max_packet_ts + diffTs;

		return report;
	}

	void crtp_stream_send::receive_key_frame_request(RTC::RTCP::FeedbackPs::MessageType messageType)
	{
		switch (messageType)
		{
		case RTC::RTCP::FeedbackPs::MessageType::PLI:
			m_pli_count++;
			break;

		case RTC::RTCP::FeedbackPs::MessageType::FIR:
			m_fir_count++;
			break;

		default:;
		}
	}

	//void RtpStreamSend::ReceiveKeyFrameRequest(RTC::RTCP::FeedbackPs::MessageType messageType)
	//{
	//	MS_TRACE();

	//	switch (messageType)
	//	{
	//	case RTC::RTCP::FeedbackPs::MessageType::PLI:
	//		this->pliCount++;
	//		break;

	//	case RTC::RTCP::FeedbackPs::MessageType::FIR:
	//		this->firCount++;
	//		break;

	//	default:;
	//	}
	//}

	//void RtpStreamSend::ReceiveRtcpReceiverReport(RTC::RTCP::ReceiverReport* report)
	//{
	//	MS_TRACE();

	//	/* Calculate RTT. */

	//	// Get the NTP representation of the current timestamp.
	//	uint64_t nowMs = DepLibUV::GetTimeMs();
	//	auto ntp = Utils::Time::TimeMs2Ntp(nowMs);

	//	// Get the compact NTP representation of the current timestamp.
	//	uint32_t compactNtp = (ntp.seconds & 0x0000FFFF) << 16;

	//	compactNtp |= (ntp.fractions & 0xFFFF0000) >> 16;
	//	//<<<<<<< HEAD
	//	// NTP时间戳的中间32位
	//	//=======
	//	//  NTP时间戳的中间32位
	//	//>>>>>>> 20adb84c0b9b03c2ea608d143e974cef7a4a3e62
	//	uint32_t lastSr = report->GetLastSenderReport();

	//	// 记录接收SR的时间与发送SR的时间差
	//	uint32_t dlsr = report->GetDelaySinceLastSenderReport();

	//	// RTT in 1/2^16 second fractions.
	//	uint32_t rtt{ 0 };

	//	// If no Sender Report was received by the remote endpoint yet, ignore lastSr
	//	// and dlsr values in the Receiver Report.
	//	if (lastSr && dlsr && (compactNtp > dlsr + lastSr))
	//	{
	//		INFO_EX_LOG("[lastSr = %u][dlstr = %u][compactNtp = %u]", lastSr, dlsr, compactNtp);
	//		rtt = compactNtp - dlsr - lastSr;
	//	}
	//	else
	//	{
	//		INFO_EX_LOG("[lastSr = %u][dlstr = %u][compactNtp = %u]", lastSr, dlsr, compactNtp);
	//	}
	//	// RTT in milliseconds.
	//	this->rtt = static_cast<float>(rtt >> 16) * 1000;
	//	this->rtt += (static_cast<float>(rtt & 0x0000FFFF) / 65536) * 1000;

	//	if (this->rtt > 0.0f)
	//	{
	//		this->hasRtt = true;
	//	}

	//	//自接收开始漏包总数，迟到包不算漏包，重传有可以导致负数
	//	//=======

	//	//>>>>>>> 20adb84c0b9b03c2ea608d143e974cef7a4a3e62
	//	this->packetsLost = report->GetTotalLost();

	//	//上一次报告之后从SSRC_n来包的漏包比列
	//	this->fractionLost = report->GetFractionLost();
	//	INFO_EX_LOG("[packetsLost = %u][fractionLost = %u][rtt = %u]", packetsLost, fractionLost, rtt);
	//	// Update the score with the received RR.
	//	UpdateScore(report);
	//}

	//RTC::RTCP::SenderReport* RtpStreamSend::GetRtcpSenderReport(uint64_t nowMs)
	//{
	//	MS_TRACE();

	//	if (this->transmissionCounter.GetPacketCount() == 0u)
	//		return nullptr;

	//	auto ntp = Utils::Time::TimeMs2Ntp(nowMs);
	//	auto* report = new RTC::RTCP::SenderReport();

	//	// Calculate TS difference between now and maxPacketMs.
	//	auto diffMs = nowMs - this->maxPacketMs;
	//	auto diffTs = diffMs * GetClockRate() / 1000;

	//	report->SetSsrc(GetSsrc());
	//	report->SetPacketCount(this->transmissionCounter.GetPacketCount());
	//	report->SetOctetCount(this->transmissionCounter.GetBytes());
	//	report->SetNtpSec(ntp.seconds);
	//	report->SetNtpFrac(ntp.fractions);
	//	report->SetRtpTs(this->maxPacketTs + diffTs);

	//	// Update info about last Sender Report.
	//	this->lastSenderReportNtpMs = nowMs;
	//	this->lastSenderReporTs = this->maxPacketTs + diffTs;

	//	return report;
	//}



void crtp_stream_send::pause()
{
	_clear_buffer();
}

void crtp_stream_send::resume()
{
}

uint32 crtp_stream_send::get_bitrate(uint64 nowMs)
{
	return m_transmission_counter.GetBitrate(nowMs);
}

void crtp_stream_send::_store_packet(RTC::RtpPacket * packet)
	{
		if (packet->GetSize() > RTC::MtuSize)
		{
			WARNING_EX_LOG( "packet too big [ssrc:%" PRIu32 ", seq:%" PRIu16 ", size:%zu]",
				packet->GetSsrc(),
				packet->GetSequenceNumber(),
				packet->GetSize());

			return;
		}

		uint16 seq = packet->GetSequenceNumber();
		StorageItem* storageItem = m_buffer[seq];

		// Buffer is empty.
		if (m_buffer_size == 0)
		{
			// Take the first storage position.
			storageItem = std::addressof(m_storage[0]);
			m_buffer[seq] = storageItem;

			// Increase buffer size and set start index.
			m_buffer_size++;
			m_buffer_start_idx = seq;
		}
		// The buffer item is already used. Check whether we should replace its
		// storage with the new packet or just ignore it (if duplicated packet).
		else if (storageItem)
		{
			auto* storedPacket = storageItem->packet;

			if (packet->GetTimestamp() == storedPacket->GetTimestamp())
			{
				// 重复包
				WARNING_EX_LOG("[packet seq = %u]", packet->GetSequenceNumber());
				return;
			}

			// Reset the storage item.
			resetStorageItem(storageItem);

			// If this was the item referenced by the buffer start index, move it to
			// the next one.
			if (this->m_buffer_start_idx == seq)
			{
				WARNING_EX_LOG("[buffer start index = %u]",m_buffer_start_idx);
				UpdateBufferStartIdx();
			}
		}
		// Buffer not yet full, add an entry.
		else if (m_buffer_size < m_storage.size())
		{
			// Take the next storage position.
			storageItem = std::addressof(m_storage[m_buffer_size]);
			m_buffer[seq] = storageItem;

			// Increase buffer size.
			m_buffer_size++;
		}
		// Buffer full, remove oldest entry and add new one.
		else
		{
			StorageItem* firstStorageItem = m_buffer[m_buffer_start_idx];
			//WARNING_EX_LOG("clear buffer start index -->>> video [m_buffer_start_idx = %u]", m_buffer_start_idx);
			// Reset the first storage item.
			resetStorageItem(firstStorageItem);

			// Unfill the buffer start item.
			m_buffer[m_buffer_start_idx] = nullptr;

			// Move the buffer start index.
			UpdateBufferStartIdx();

			// Take the freed storage item.
			storageItem = firstStorageItem;
			m_buffer[seq] = storageItem;
		}

		// Clone the packet into the retrieved storage item.
		storageItem->packet = packet->Clone(storageItem->store);
	}
	void crtp_stream_send::_clear_buffer()
	{
		if (this->m_storage.empty())
		{
			return;
		}

		for (uint32_t idx{ 0 }; idx < this->m_buffer.size(); ++idx)
		{
			auto* storageItem = this->m_buffer[idx];

			if (!storageItem)
			{
				cassert(!this->m_buffer[idx], "key should be NULL");

				continue;
			}

			// Reset (free RTP packet) the storage item.
			resetStorageItem(storageItem);

			// Unfill the buffer item.
			this->m_buffer[idx] = nullptr;
		}

		// Reset buffer.
		this->m_buffer_start_idx = 0;
		this->m_buffer_size = 0;
	}

	void crtp_stream_send::UpdateBufferStartIdx()
	{
		uint16_t seq = this->m_buffer_start_idx + 1;

		for (uint32_t idx{ 0 }; idx < this->m_buffer.size(); ++idx, ++seq)
		{
			auto* storageItem = this->m_buffer[seq];

			if (storageItem)
			{
				this->m_buffer_start_idx = seq;

				break;
			}
		}
	}

	void crtp_stream_send::UpdateScore(RTC::RTCP::ReceiverReport * report)
	{
		// Calculate number of packets sent in this interval.
	// 1. 发送总包数
		size_t totalSent = m_transmission_counter.GetPacketCount();
		// 2. 先前时刻发送包数
		size_t sent = totalSent - m_sent_prior_score;
		// 3. 记录当前时刻总发送包数
		this->m_sent_prior_score = totalSent;
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				// Calculate number of packets lost in this interval.
				// 4. 总丢包数
		uint32_t totalLost = report->GetTotalLost() > 0 ? report->GetTotalLost() : 0;
		uint32_t lost;
		//INFO_EX_LOG("[totalSent = %u][sent = %u][totalLost = %u]", totalSent, sent, totalLost);
		// 5. 上一时刻到现在时刻是否有丢包
		// | ...     |        <-->          |
		// |     上一个时刻                  现在
		//
		if (totalLost < this->m_lost_prior_score)
		{
			lost = 0;
		}
		else
		{
			lost = totalLost - this->m_lost_prior_score;
		}

		// 6. 记录当前时刻总丢包数
		this->m_lost_prior_score = totalLost;

		// Calculate number of packets repaired in this interval.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// 7. 记录总修复包 丢包修复总数量
		size_t totalRepaired = this->m_packets_repaired;
		// 8. 上一个时刻到现在修复包的数量
		uint32_t repaired = totalRepaired - this->m_repaired_prior_score;
		// 9. 记录当前时刻总修复包的数量
		this->m_repaired_prior_score = totalRepaired;

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				// Calculate number of packets retransmitted in this interval.
				// 10 . 记录总重新发送包的数量
		auto totatRetransmitted = this->m_packets_retransmitted;
		// 11. 上一个时刻到现在重新发送包的数量
		uint32_t retransmitted = totatRetransmitted - this->m_retransmitted_prior_score;
		//INFO_EX_LOG("[totalRepaired = %u][repaired = %u][packetsRetransmitted = %u]", totalRepaired, repaired, packetsRetransmitted);
		// 12. 记录当前时刻总重新发送包数量
		this->m_retransmitted_prior_score = totatRetransmitted;

		// We didn't send any packet.
		// 13. 判断上一个时刻到现在之间没有发送包
		if (sent == 0)
		{
			//INFO_EX_LOG("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
			 update_score(10);

			return;
		}
		// 14. 上一个时刻到现在丢包数量大于发送包数量 ， 就修改丢包总数为正常发送包数量哈
		if (lost > sent)
		{
			lost = sent;
		}
		// 15. 上一个时刻到现在 修复包数量大于丢包数量， 就修改包为丢包数量哈
		if (repaired > lost)
		{
			repaired = lost;
		}

#if MS_LOG_DEV_LEVEL == 3
		MS_DEBUG_TAG(
			score,
			"[totalSent:%zu, totalLost:%" PRIi32 ", totalRepaired:%zu",
			totalSent,
			totalLost,
			totalRepaired);

		MS_DEBUG_TAG(
			score,
			"fixed values [sent:%zu, lost:%" PRIu32 ", repaired:%" PRIu32 ", retransmitted:%" PRIu32,
			sent,
			lost,
			repaired,
			retransmitted);
#endif
		// 16. 上一个时刻到现在修复包比率 = 修复包/发送包
		auto repairedRatio = static_cast<float>(repaired) / static_cast<float>(sent);
		// 17. 上一个时刻到现在修复包权重 =   [1/(修复包比率 + 1))] ^ 4 =====> ||||||||||||
		auto repairedWeight = std::pow(1 / (repairedRatio + 1), 4);
		//INFO_EX_LOG("== [repairedRatio = %s][repairedWeight = %s]", std::to_string(repairedRatio).c_str(), std::to_string(repairedWeight).c_str());
		cassert(retransmitted >= repaired, "repaired packets cannot be more than retransmitted ones");


		// 18. 上一个时刻到现在 修复包数量大于0 , 修复包权重格式 = repairedWeight  * (修复包/ 重新发送包)
		if (retransmitted > 0)
		{
			repairedWeight *= static_cast<float>(repaired) / retransmitted;
		}

		// 19. 丢包数量 重新估计  = lost -  (修复包 * 重新发送包)  ????
		lost -= repaired * repairedWeight;

		// 20. 上一个时刻到现在已经完成传输包/总发送包比例 =  (发送总包数量 - 发送无效总包数量) / 发送总包数量
		auto deliveredRatio = static_cast<float>(sent - lost) / static_cast<float>(sent);

		// 21. 分数 计算公式 =  函数即四舍五入取偶((有效总包数量 ^ 4) * 10) 
		auto score = static_cast<uint8_t>(std::round(std::pow(deliveredRatio, 4) * 10));





		//////////////////////////////////////////////////////////////////////////////////////////////////////////
		//                                        分析评估算法步骤
		//   1. 目的 评估网络情况是否良好    -----> 就判断掉包数  --> 每个时刻掉包数都是不同所以如何判断？？？？ 每个时刻掉包数呢 准确哈
		//
		//   2. 每个时刻的掉包 修复权重  
		//   
		//   3. 每个时刻从新发送包量 对修复权重影响 
		//
		//   4. 掉包评估数量
		//
		//
		//    ---------------------------------------最终公式----------------------------------------------------   
		//
		//       ① [1/(最近时刻修复包比率 + 1)] ^ 4
		//       ② 判断是否从新发送包情况    如果有从新发送包情况  就需要在调整修复比率  公式就变成 =   ([1/(最近时刻修复包比率 + 1)] ^ 4) * ( 最近时刻修复包 /  最近从新发送包)
		//       ③ 调整掉包数  = 掉包数 - (修复包 * 从新发送包)
		//
		//////////////////////////////////////////////////////////////////////////////////////////////////////////
#if MS_LOG_DEV_LEVEL == 3
		MS_DEBUG_TAG(
			score,
			"[deliveredRatio:%f, repairedRatio:%f, repairedWeight:%f, new lost:%" PRIu32 ", score:%" PRIu8
			"]",
			deliveredRatio,
			repairedRatio,
			repairedWeight,
			lost,
			score);
#endif
		//INFO_EX_LOG("[pdatescore = %u]", score);
		update_score(score);
	}

	// This method looks for the requested RTP packets and inserts them into the
	// RetransmissionContainer vector (and sets to null the next position).
	//
	// If RTX is used the stored packet will be RTX encoded now (if not already
	// encoded in a previous resend).
	void crtp_stream_send::FillRetransmissionContainer(uint16_t seq, uint16_t bitmask)
	{
		//MS_TRACE();

		// Ensure the container's first element is 0.
		RetransmissionContainer[0] = nullptr;

		// If NACK is not supported, exit.
		if (!this->m_params.use_nack)
		{
			WARNING_EX_LOG( "rtx , NACK not supported");

			return;
		}

		// Look for each requested packet.
		uint64_t nowMs = uv_util::GetTimeMs();
		uint16_t rtt = (this->m_rtt != 0u ? this->m_rtt : DefaultRtt);
		uint16_t currentSeq = seq;
		bool requested{ true };
		size_t containerIdx{ 0 };

		// Variables for debugging.
		uint16_t origBitmask = bitmask;
		uint16_t sentBitmask{ 0b0000000000000000 };
		bool isFirstPacket{ true };
		bool firstPacketSent{ false };
		uint8_t bitmaskCounter{ 0 };
		bool tooOldPacketFound{ false };

		while (requested || bitmask != 0)
		{
			bool sent = false;

			if (requested)
			{
				auto* storageItem = this->m_buffer[currentSeq];
				RTC::RtpPacket* packet{ nullptr };
				uint32_t diffMs = 0;

				// Calculate the elapsed time between the max timestampt seen and the
				// requested packet's timestampt (in ms).
				if (storageItem)
				{
					packet = storageItem->packet;

					uint32_t diffTs = this->m_max_packet_ts - packet->GetTimestamp();

					diffMs = diffTs * 1000 / (90000)/*this->m_params.clock_rate*/;
				}
				//NORMAL_EX_LOG("[diffMS = %u]", uv_util::GetTimeUs() - packet->GetTimestamp());
				// Packet not found.
				if (!storageItem)
				{
					// Do nothing.
				}
				// Don't resend the packet if older than MaxRetransmissionDelay ms.
				else if (diffMs > MaxRetransmissionDelay)
				{
					if (!tooOldPacketFound)
					{
						WARNING_EX_LOG( "rtx ignoring retransmission for too old packet "
							"[seq:%" PRIu16 ", max age:%" PRIu32 "ms, packet age:%" PRIu32 "ms]",
							packet->GetSequenceNumber(),
							MaxRetransmissionDelay,
							diffMs);

						tooOldPacketFound = true;
					}
				}
				// Don't resent the packet if it was resent in the last RTT ms.
				// clang-format off
				else if (
					storageItem->resentAtMs != 0u &&
					nowMs - storageItem->resentAtMs <= static_cast<uint64_t>(rtt)
					)
					// clang-format on
				{
					/*DEBUG_EX_LOG( "ignoring retransmission for a packet already resent in the last RTT ms "
						"[seq:%" PRIu16 ", rtt:%" PRIu32 "]",
						packet->GetSequenceNumber(),
						rtt);*/
				}
				// Stored packet is valid for retransmission. Resend it.
				else
				{
					// If we use RTX and the packet has not yet been resent, encode it now.
					if (has_rtx())
					{
						// Increment RTX seq.
						++this->m_rtx_seq;

						if (!storageItem->rtxEncoded)
						{
							packet->RtxEncode(this->m_params.rtx_payload_type, this->m_params.rtx_ssrc, this->m_rtx_seq);

							storageItem->rtxEncoded = true;
						}
						else
						{
							packet->SetSequenceNumber(this->m_rtx_seq);
						}
					}

					// Save when this packet was resent.
					storageItem->resentAtMs = nowMs;

					// Increase the number of times this packet was sent.
					storageItem->sentTimes++;

					// Store the storage item in the container and then increment its index.
					RetransmissionContainer[containerIdx++] = storageItem;

					sent = true;

					if (isFirstPacket)
						firstPacketSent = true;
				}
			}

			requested = (bitmask & 1) != 0;
			bitmask >>= 1;
			++currentSeq;

			if (!isFirstPacket)
			{
				sentBitmask |= (sent ? 1 : 0) << bitmaskCounter;
				++bitmaskCounter;
			}
			else
			{
				isFirstPacket = false;
			}
		}

		// If not all the requested packets was sent, log it.
		if (!firstPacketSent || origBitmask != sentBitmask)
		{
			WARNING_EX_LOG(
				"could not resend all packets [seq:%" PRIu16
				", first:%s, "
				"bitmask:" MS_UINT16_TO_BINARY_PATTERN ", sent bitmask:" MS_UINT16_TO_BINARY_PATTERN "]",
				seq,
				firstPacketSent ? "yes" : "no",
				MS_UINT16_TO_BINARY(origBitmask),
				MS_UINT16_TO_BINARY(sentBitmask));
		}
		else
		{
			/*DEBUG_EX_LOG(
				"all packets resent [seq:%" PRIu16 ", bitmask:" MS_UINT16_TO_BINARY_PATTERN "]",
				seq,
				MS_UINT16_TO_BINARY(origBitmask));*/
		}

		// Set the next container element to null.
		RetransmissionContainer[containerIdx] = nullptr;
	}
}