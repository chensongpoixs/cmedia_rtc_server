/*

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

#include "RtpStreamSend.hpp"
//#include "Logger.hpp"
//#include "Utils.hpp"
#include "SeqManager.hpp"
#include "clog.h"
#include "crandom.h"
#include "FeedbackRtpNack.hpp"
#include "ReceiverReport.hpp"
#include "SenderReport.hpp"
namespace RTC
{
	/* Static. */

	// 17: 16 bit mask + the initial sequence number.
	static constexpr size_t MaxRequestedPackets{ 17 };
	thread_local static std::vector<RTC::RtpStreamSend::StorageItem*> RetransmissionContainer(
	  MaxRequestedPackets + 1);
	// Don't retransmit packets older than this (ms).
	static constexpr uint32_t MaxRetransmissionDelay{ 2000 };
	static constexpr uint32_t DefaultRtt{ 100 };

	static void resetStorageItem(RTC::RtpStreamSend::StorageItem* storageItem)
	{
		using namespace chen;
		cassert(storageItem, "storageItem cannot be nullptr");

		delete storageItem->packet;

		storageItem->packet     = nullptr;
		storageItem->resentAtMs = 0;
		storageItem->sentTimes  = 0;
		storageItem->rtxEncoded = false;
	}

	/* Instance methods. */

	RtpStreamSend::RtpStreamSend(
	  RTC::RtpStreamSend::Listener* listener, RTC::RtpStream::Params& params, size_t bufferSize)
	  : RTC::RtpStream::RtpStream(listener, params, 10), buffer(bufferSize > 0 ? 65536 : 0, nullptr),
	    storage(bufferSize)
	{
		//MS_TRACE();
	}

	RtpStreamSend::~RtpStreamSend()
	{
		//MS_TRACE();

		// Clear the RTP buffer.
		ClearBuffer();
	}

	//void RtpStreamSend::FillJsonStats(json& jsonObject)
	//{
	//	//MS_TRACE();

	//	uint64_t nowMs = chen::uv_util::GetTimeMs();

	//	RTC::RtpStream::FillJsonStats(jsonObject);

	//	jsonObject["type"]        = "outbound-rtp";
	//	jsonObject["packetCount"] = this->transmissionCounter.GetPacketCount();
	//	jsonObject["byteCount"]   = this->transmissionCounter.GetBytes();
	//	jsonObject["bitrate"]     = this->transmissionCounter.GetBitrate(nowMs);
	//}

	void RtpStreamSend::SetRtx(uint8_t payloadType, uint32_t ssrc)
	{
		//MS_TRACE();

		RTC::RtpStream::SetRtx(payloadType, ssrc);

		this->rtxSeq = c_rand.rand(0u, 0xFFFF);//Utils::Crypto::GetRandomUInt(0u, 0xFFFF);
	}

	bool RtpStreamSend::ReceivePacket(RTC::RtpPacket* packet)
	{
		 

		// Call the parent method.
		if (!RtpStream::ReceivePacket(packet))
		{
			return false;
		}

		// If bufferSize was given, store the packet into the buffer.
		if (!this->storage.empty())
		{
			StorePacket(packet);
		}

		// Increase transmission counter.
		this->transmissionCounter.Update(packet);

		return true;
	}

	void RtpStreamSend::ReceiveNack(RTC::RTCP::FeedbackRtpNackPacket* nackPacket)
	{
		 
		this->nackCount++;

		for (auto it = nackPacket->Begin(); it != nackPacket->End(); ++it)
		{
			RTC::RTCP::FeedbackRtpNackItem* item = *it;

			this->nackPacketCount += item->CountRequestedPackets();

			FillRetransmissionContainer(item->GetPacketId(), item->GetLostPacketBitmask());

			for (auto* storageItem : RetransmissionContainer)
			{
				if (!storageItem)
					break;

				// Note that this is an already RTX encoded packet if RTX is used
				// (FillRetransmissionContainer() did it).
				auto* packet = storageItem->packet;

				// Retransmit the packet.
				static_cast<RTC::RtpStreamSend::Listener*>(this->listener)
				  ->OnRtpStreamRetransmitRtpPacket(this, packet);

				// Mark the packet as retransmitted.
				RTC::RtpStream::PacketRetransmitted(packet);

				// Mark the packet as repaired (only if this is the first retransmission).
				if (storageItem->sentTimes == 1)
					RTC::RtpStream::PacketRepaired(packet);
			}
		}
	}

	void RtpStreamSend::ReceiveKeyFrameRequest(RTC::RTCP::FeedbackPs::MessageType messageType)
	{
		 
		switch (messageType)
		{
			case RTC::RTCP::FeedbackPs::MessageType::PLI:
				this->pliCount++;
				break;

			case RTC::RTCP::FeedbackPs::MessageType::FIR:
				this->firCount++;
				break;

			default:;
		}
	}

	void RtpStreamSend::ReceiveRtcpReceiverReport(RTC::RTCP::ReceiverReport* report)
	{
		 
		
		/* Calculate RTT. */

		// Get the NTP representation of the current timestamp.
		uint64_t nowMs = uv_util::GetTimeMs();
		auto ntp       = rtc_time::TimeMs2Ntp(nowMs);

		// Get the compact NTP representation of the current timestamp.
		uint32_t compactNtp = (ntp.seconds & 0x0000FFFF) << 16;

		compactNtp |= (ntp.fractions & 0xFFFF0000) >> 16;
//<<<<<<< HEAD
		// NTPÊ±¼ä´ÁµÄÖÐ¼ä32Î»
//=======
		//  NTPÊ±¼ä´ÁµÄÖÐ¼ä32Î»
//>>>>>>> 20adb84c0b9b03c2ea608d143e974cef7a4a3e62
		uint32_t lastSr = report->GetLastSenderReport();
		
		// ¼ÇÂ¼½ÓÊÕSRµÄÊ±¼äÓë·¢ËÍSRµÄÊ±¼ä²î
		uint32_t dlsr   = report->GetDelaySinceLastSenderReport();
		
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
		this->rtt = static_cast<float>(rtt >> 16) * 1000;
		this->rtt += (static_cast<float>(rtt & 0x0000FFFF) / 65536) * 1000;

		if (this->rtt > 0.0f)
		{
			this->hasRtt = true;
		}

		//×Ô½ÓÊÕ¿ªÊ¼Â©°ü×ÜÊý£¬³Ùµ½°ü²»ËãÂ©°ü£¬ÖØ´«ÓÐ¿ÉÒÔµ¼ÖÂ¸ºÊý
//=======

//>>>>>>> 20adb84c0b9b03c2ea608d143e974cef7a4a3e62
		this->packetsLost  = report->GetTotalLost();

		//ÉÏÒ»´Î±¨¸æÖ®ºó´ÓSSRC_nÀ´°üµÄÂ©°ü±ÈÁÐ
		this->fractionLost = report->GetFractionLost();
		//INFO_EX_LOG("[packetsLost = %u][fractionLost = %u][rtt = %u]", packetsLost, fractionLost, rtt);
		// Update the score with the received RR.
		UpdateScore(report);
	}

	RTC::RTCP::SenderReport* RtpStreamSend::GetRtcpSenderReport(uint64_t nowMs)
	{
		 

		if (this->transmissionCounter.GetPacketCount() == 0u)
			return nullptr;

		auto ntp     = rtc_time::TimeMs2Ntp(nowMs);
		auto* report = new RTC::RTCP::SenderReport();

		// Calculate TS difference between now and maxPacketMs.
		auto diffMs = nowMs - this->maxPacketMs;
		auto diffTs = diffMs * GetClockRate() / 1000;

		report->SetSsrc(GetSsrc());
		report->SetPacketCount(this->transmissionCounter.GetPacketCount());
		report->SetOctetCount(this->transmissionCounter.GetBytes());
		report->SetNtpSec(ntp.seconds);
		report->SetNtpFrac(ntp.fractions);
		report->SetRtpTs(this->maxPacketTs + diffTs);

		// Update info about last Sender Report.
		this->lastSenderReportNtpMs = nowMs;
		this->lastSenderReporTs     = this->maxPacketTs + diffTs;

		return report;
	}

	/*RTC::RTCP::SdesChunk* RtpStreamSend::GetRtcpSdesChunk()
	{
		MS_TRACE();

		const auto& cname = GetCname();
		auto* sdesChunk   = new RTC::RTCP::SdesChunk(GetSsrc());
		auto* sdesItem =
		  new RTC::RTCP::SdesItem(RTC::RTCP::SdesItem::Type::CNAME, cname.size(), cname.c_str());

		sdesChunk->AddItem(sdesItem);

		return sdesChunk;
	}
*/
	void RtpStreamSend::Pause()
	{
		 
		ClearBuffer();
	}

	void RtpStreamSend::Resume()
	{
		 
	}

	uint32_t RtpStreamSend::GetBitrate(
	  uint64_t /*nowMs*/, uint8_t /*spatialLayer*/, uint8_t /*temporalLayer*/)
	{
		 

		cassert("invalid method call");
		return 0;
	}

	uint32_t RtpStreamSend::GetSpatialLayerBitrate(uint64_t /*nowMs*/, uint8_t /*spatialLayer*/)
	{
		 
		cassert("invalid method call");
		return 0;
	}

	uint32_t RtpStreamSend::GetLayerBitrate(
	  uint64_t /*nowMs*/, uint8_t /*spatialLayer*/, uint8_t /*temporalLayer*/)
	{
		 
		cassert("invalid method call");
		return 0;
	}

	void RtpStreamSend::StorePacket(RTC::RtpPacket* packet)
	{
		 

		if (packet->GetSize() > RTC::MtuSize)
		{
			WARNING_EX_LOG( "rtp packet too big [ssrc:%" PRIu32 ", seq:%" PRIu16 ", size:%zu]",
			  packet->GetSsrc(),
			  packet->GetSequenceNumber(),
			  packet->GetSize());

			return;
		}

		auto seq          = packet->GetSequenceNumber();
		auto* storageItem = this->buffer[seq];

		// Buffer is empty.
		if (this->bufferSize == 0)
		{
			// Take the first storage position.
			storageItem       = std::addressof(this->storage[0]);
			this->buffer[seq] = storageItem;

			// Increase buffer size and set start index.
			this->bufferSize++;
			this->bufferStartIdx = seq;
		}
		// The buffer item is already used. Check whether we should replace its
		// storage with the new packet or just ignore it (if duplicated packet).
		else if (storageItem)
		{
			auto* storedPacket = storageItem->packet;

			if (packet->GetTimestamp() == storedPacket->GetTimestamp())
				return;

			// Reset the storage item.
			resetStorageItem(storageItem);

			// If this was the item referenced by the buffer start index, move it to
			// the next one.
			if (this->bufferStartIdx == seq)
				UpdateBufferStartIdx();
		}
		// Buffer not yet full, add an entry.
		else if (this->bufferSize < this->storage.size())
		{
			// Take the next storage position.
			storageItem       = std::addressof(this->storage[this->bufferSize]);
			this->buffer[seq] = storageItem;

			// Increase buffer size.
			this->bufferSize++;
		}
		// Buffer full, remove oldest entry and add new one.
		else
		{
			auto* firstStorageItem = this->buffer[this->bufferStartIdx];

			// Reset the first storage item.
			resetStorageItem(firstStorageItem);

			// Unfill the buffer start item.
			this->buffer[this->bufferStartIdx] = nullptr;

			// Move the buffer start index.
			UpdateBufferStartIdx();

			// Take the freed storage item.
			storageItem       = firstStorageItem;
			this->buffer[seq] = storageItem;
		}

		// Clone the packet into the retrieved storage item.
		storageItem->packet = packet->Clone(storageItem->store);
	}

	void RtpStreamSend::ClearBuffer()
	{
		 
		if (this->storage.empty())
			return;

		for (uint32_t idx{ 0 }; idx < this->buffer.size(); ++idx)
		{
			auto* storageItem = this->buffer[idx];

			if (!storageItem)
			{
				cassert(!this->buffer[idx], "key should be NULL");

				continue;
			}

			// Reset (free RTP packet) the storage item.
			resetStorageItem(storageItem);

			// Unfill the buffer item.
			this->buffer[idx] = nullptr;
		}

		// Reset buffer.
		this->bufferStartIdx = 0;
		this->bufferSize     = 0;
	}

	/**
	 * Iterates the buffer starting from the current start idx + 1 until next
	 * used one. It takes into account that the buffer is circular.
	 */
	inline void RtpStreamSend::UpdateBufferStartIdx()
	{
		uint16_t seq = this->bufferStartIdx + 1;

		for (uint32_t idx{ 0 }; idx < this->buffer.size(); ++idx, ++seq)
		{
			auto* storageItem = this->buffer[seq];

			if (storageItem)
			{
				this->bufferStartIdx = seq;

				break;
			}
		}
	}

	// This method looks for the requested RTP packets and inserts them into the
	// RetransmissionContainer vector (and sets to null the next position).
	//
	// If RTX is used the stored packet will be RTX encoded now (if not already
	// encoded in a previous resend).
	void RtpStreamSend::FillRetransmissionContainer(uint16_t seq, uint16_t bitmask)
	{
		 
		// Ensure the container's first element is 0.
		RetransmissionContainer[0] = nullptr;

		// If NACK is not supported, exit.
		if (!this->params.useNack)
		{
			WARNING_EX_LOG("rtx, NACK not supported");

			return;
		}

		// Look for each requested packet.
		uint64_t nowMs      = uv_util::GetTimeMs();
		uint16_t rtt        = (this->rtt != 0u ? this->rtt : DefaultRtt);
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
				auto* storageItem = this->buffer[currentSeq];
				RTC::RtpPacket* packet{ nullptr };
				uint32_t diffMs;

				// Calculate the elapsed time between the max timestampt seen and the
				// requested packet's timestampt (in ms).
				if (storageItem)
				{
					packet = storageItem->packet;

					uint32_t diffTs = this->maxPacketTs - packet->GetTimestamp();

					diffMs = diffTs * 1000 / this->params.clockRate;
				}

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
					DEBUG_EX_LOG( "rtx ignoring retransmission for a packet already resent in the last RTT ms "
					  "[seq:%" PRIu16 ", rtt:%" PRIu32 "]",
					  packet->GetSequenceNumber(),
					  rtt);
				}
				// Stored packet is valid for retransmission. Resend it.
				else
				{
					// If we use RTX and the packet has not yet been resent, encode it now.
					if (HasRtx())
					{
						// Increment RTX seq.
						++this->rtxSeq;

						if (!storageItem->rtxEncoded)
						{
							packet->RtxEncode(this->params.rtxPayloadType, this->params.rtxSsrc, this->rtxSeq);

							storageItem->rtxEncoded = true;
						}
						else
						{
							packet->SetSequenceNumber(this->rtxSeq);
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
			DEBUG_EX_LOG(
			  "all packets resent [seq:%" PRIu16 ", bitmask:" MS_UINT16_TO_BINARY_PATTERN "]",
			  seq,
			  MS_UINT16_TO_BINARY(origBitmask));
		}

		// Set the next container element to null.
		RetransmissionContainer[containerIdx] = nullptr;
	}

	void RtpStreamSend::UpdateScore(RTC::RTCP::ReceiverReport* report)
	{
		 

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Calculate number of packets sent in this interval.
		// 1. ·¢ËÍ×Ü°üÊý
		size_t totalSent = this->transmissionCounter.GetPacketCount();
		// 2. ÏÈÇ°Ê±¿Ì·¢ËÍ°üÊý
		size_t sent      = totalSent - this->sentPriorScore;
		// 3. ¼ÇÂ¼µ±Ç°Ê±¿Ì×Ü·¢ËÍ°üÊý
		this->sentPriorScore = totalSent;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Calculate number of packets lost in this interval.
		// 4. ×Ü¶ª°üÊý
		uint32_t totalLost = report->GetTotalLost() > 0 ? report->GetTotalLost() : 0;
		uint32_t lost;
		//INFO_EX_LOG("[totalSent = %u][sent = %u][totalLost = %u]", totalSent, sent, totalLost);
		// 5. ÉÏÒ»Ê±¿Ìµ½ÏÖÔÚÊ±¿ÌÊÇ·ñÓÐ¶ª°ü
		// | ...     |        <-->          |
		// |     ÉÏÒ»¸öÊ±¿Ì                  ÏÖÔÚ
		//
		if (totalLost < this->lostPriorScore)
		{
			lost = 0;
		}
		else
		{
			lost = totalLost - this->lostPriorScore;
		}

		// 6. ¼ÇÂ¼µ±Ç°Ê±¿Ì×Ü¶ª°üÊý
		this->lostPriorScore = totalLost;

		// Calculate number of packets repaired in this interval.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// 7. ¼ÇÂ¼×ÜÐÞ¸´°ü ¶ª°üÐÞ¸´×ÜÊýÁ¿
		size_t totalRepaired = this->packetsRepaired;
		// 8. ÉÏÒ»¸öÊ±¿Ìµ½ÏÖÔÚÐÞ¸´°üµÄÊýÁ¿
		uint32_t repaired  = totalRepaired - this->repairedPriorScore;
		// 9. ¼ÇÂ¼µ±Ç°Ê±¿Ì×ÜÐÞ¸´°üµÄÊýÁ¿
		this->repairedPriorScore = totalRepaired;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Calculate number of packets retransmitted in this interval.
		// 10 . ¼ÇÂ¼×ÜÖØÐÂ·¢ËÍ°üµÄÊýÁ¿
		auto totatRetransmitted = this->packetsRetransmitted;
		// 11. ÉÏÒ»¸öÊ±¿Ìµ½ÏÖÔÚÖØÐÂ·¢ËÍ°üµÄÊýÁ¿
		uint32_t retransmitted  = totatRetransmitted - this->retransmittedPriorScore;
		//INFO_EX_LOG("[totalRepaired = %u][repaired = %u][packetsRetransmitted = %u]", totalRepaired, repaired, packetsRetransmitted);
		// 12. ¼ÇÂ¼µ±Ç°Ê±¿Ì×ÜÖØÐÂ·¢ËÍ°üÊýÁ¿
		this->retransmittedPriorScore = totatRetransmitted;

		// We didn't send any packet.
		// 13. ÅÐ¶ÏÉÏÒ»¸öÊ±¿Ìµ½ÏÖÔÚÖ®¼äÃ»ÓÐ·¢ËÍ°ü
		if (sent == 0)
		{
			//INFO_EX_LOG("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
			RTC::RtpStream::UpdateScore(10);

			return;
		}
		// 14. ÉÏÒ»¸öÊ±¿Ìµ½ÏÖÔÚ¶ª°üÊýÁ¿´óÓÚ·¢ËÍ°üÊýÁ¿ £¬ ¾ÍÐÞ¸Ä¶ª°ü×ÜÊýÎªÕý³£·¢ËÍ°üÊýÁ¿¹þ
		if (lost > sent)
		{
			lost = sent;
		}
		// 15. ÉÏÒ»¸öÊ±¿Ìµ½ÏÖÔÚ ÐÞ¸´°üÊýÁ¿´óÓÚ¶ª°üÊýÁ¿£¬ ¾ÍÐÞ¸Ä°üÎª¶ª°üÊýÁ¿¹þ
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
		// 16. ÉÏÒ»¸öÊ±¿Ìµ½ÏÖÔÚÐÞ¸´°ü±ÈÂÊ = ÐÞ¸´°ü/·¢ËÍ°ü
		auto repairedRatio  = static_cast<float>(repaired) / static_cast<float>(sent);
		// 17. ÉÏÒ»¸öÊ±¿Ìµ½ÏÖÔÚÐÞ¸´°üÈ¨ÖØ =   [1/(ÐÞ¸´°ü±ÈÂÊ + 1))] ^ 4 =====> ||||||||||||
		auto repairedWeight = std::pow(1 / (repairedRatio + 1), 4);
		//INFO_EX_LOG("== [repairedRatio = %s][repairedWeight = %s]", std::to_string(repairedRatio).c_str(), std::to_string(repairedWeight).c_str());
		cassert(retransmitted >= repaired, "repaired packets cannot be more than retransmitted ones");


		// 18. ÉÏÒ»¸öÊ±¿Ìµ½ÏÖÔÚ ÐÞ¸´°üÊýÁ¿´óÓÚ0 , ÐÞ¸´°üÈ¨ÖØ¸ñÊ½ = repairedWeight  * (ÐÞ¸´°ü/ ÖØÐÂ·¢ËÍ°ü)
		if (retransmitted > 0)
		{
			repairedWeight *= static_cast<float>(repaired) / retransmitted;
		}

		// 19. ¶ª°üÊýÁ¿ ÖØÐÂ¹À¼Æ  = lost -  (ÐÞ¸´°ü * ÖØÐÂ·¢ËÍ°ü)  ????
		lost -= repaired * repairedWeight;

		// 20. ÉÏÒ»¸öÊ±¿Ìµ½ÏÖÔÚÒÑ¾­Íê³É´«Êä°ü/×Ü·¢ËÍ°ü±ÈÀý =  (·¢ËÍ×Ü°üÊýÁ¿ - ·¢ËÍÎÞÐ§×Ü°üÊýÁ¿) / ·¢ËÍ×Ü°üÊýÁ¿
		auto deliveredRatio = static_cast<float>(sent - lost) / static_cast<float>(sent);

		// 21. ·ÖÊý ¼ÆËã¹«Ê½ =  º¯Êý¼´ËÄÉáÎåÈëÈ¡Å¼((ÓÐÐ§×Ü°üÊýÁ¿ ^ 4) * 10) 
		auto score          = static_cast<uint8_t>(std::round(std::pow(deliveredRatio, 4) * 10));





		//////////////////////////////////////////////////////////////////////////////////////////////////////////
		//                                        ·ÖÎöÆÀ¹ÀËã·¨²½Öè
		//   1. Ä¿µÄ ÆÀ¹ÀÍøÂçÇé¿öÊÇ·ñÁ¼ºÃ    -----> ¾ÍÅÐ¶Ïµô°üÊý  --> Ã¿¸öÊ±¿Ìµô°üÊý¶¼ÊÇ²»Í¬ËùÒÔÈçºÎÅÐ¶Ï£¿£¿£¿£¿ Ã¿¸öÊ±¿Ìµô°üÊýÄØ ×¼È·¹þ
		//
		//   2. Ã¿¸öÊ±¿ÌµÄµô°ü ÐÞ¸´È¨ÖØ  
		//   
		//   3. Ã¿¸öÊ±¿Ì´ÓÐÂ·¢ËÍ°üÁ¿ ¶ÔÐÞ¸´È¨ÖØÓ°Ïì 
		//
		//   4. µô°üÆÀ¹ÀÊýÁ¿
		//
		//
		//    ---------------------------------------×îÖÕ¹«Ê½----------------------------------------------------   
		//
		//       ¢Ù [1/(×î½üÊ±¿ÌÐÞ¸´°ü±ÈÂÊ + 1)] ^ 4
		//       ¢Ú ÅÐ¶ÏÊÇ·ñ´ÓÐÂ·¢ËÍ°üÇé¿ö    Èç¹ûÓÐ´ÓÐÂ·¢ËÍ°üÇé¿ö  ¾ÍÐèÒªÔÚµ÷ÕûÐÞ¸´±ÈÂÊ  ¹«Ê½¾Í±ä³É =   ([1/(×î½üÊ±¿ÌÐÞ¸´°ü±ÈÂÊ + 1)] ^ 4) * ( ×î½üÊ±¿ÌÐÞ¸´°ü /  ×î½ü´ÓÐÂ·¢ËÍ°ü)
		//       ¢Û µ÷Õûµô°üÊý  = µô°üÊý - (ÐÞ¸´°ü * ´ÓÐÂ·¢ËÍ°ü)
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
		RtpStream::UpdateScore(score);
	}
} // namespace RTC
