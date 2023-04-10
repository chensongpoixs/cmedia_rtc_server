/***********************************************************************************************
created: 		2022-08-10

author:			chensong

purpose:		room

Copyright boost
************************************************************************************************/
#include "crtp_stream_send.h"
#include "crandom.h"
namespace chen {
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
			
		}
		// Increase transmission counter.
		//this->transmissionCounter.Update(packet);
		return true;
	}

	//void RtpStreamSend::ReceiveNack(RTC::RTCP::FeedbackRtpNackPacket* nackPacket)
	//{
	//	MS_TRACE();

	//	this->nackCount++;

	//	for (auto it = nackPacket->Begin(); it != nackPacket->End(); ++it)
	//	{
	//		RTC::RTCP::FeedbackRtpNackItem* item = *it;

	//		this->nackPacketCount += item->CountRequestedPackets();

	//		FillRetransmissionContainer(item->GetPacketId(), item->GetLostPacketBitmask());

	//		for (auto* storageItem : RetransmissionContainer)
	//		{
	//			if (!storageItem)
	//				break;

	//			// Note that this is an already RTX encoded packet if RTX is used
	//			// (FillRetransmissionContainer() did it).
	//			auto* packet = storageItem->packet;

	//			// Retransmit the packet.
	//			static_cast<RTC::RtpStreamSend::Listener*>(this->listener)
	//				->OnRtpStreamRetransmitRtpPacket(this, packet);

	//			// Mark the packet as retransmitted.
	//			RTC::RtpStream::PacketRetransmitted(packet);

	//			// Mark the packet as repaired (only if this is the first retransmission).
	//			if (storageItem->sentTimes == 1)
	//				RTC::RtpStream::PacketRepaired(packet);
	//		}
	//	}
	//}

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
				return;
			}

			// Reset the storage item.
			resetStorageItem(storageItem);

			// If this was the item referenced by the buffer start index, move it to
			// the next one.
			//if (this->bufferStartIdx == seq)
				//UpdateBufferStartIdx();
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

			// Reset the first storage item.
			resetStorageItem(firstStorageItem);

			// Unfill the buffer start item.
			m_buffer[m_buffer_start_idx] = nullptr;

			// Move the buffer start index.
			//UpdateBufferStartIdx();

			// Take the freed storage item.
			storageItem = firstStorageItem;
			m_buffer[seq] = storageItem;
		}

		// Clone the packet into the retrieved storage item.
		storageItem->packet = packet->Clone(storageItem->store);
	}
	void crtp_stream_send::_clear_buffer()
	{
	}
}