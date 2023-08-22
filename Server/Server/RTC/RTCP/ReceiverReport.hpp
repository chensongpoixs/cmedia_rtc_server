﻿#ifndef MS_RTC_RTCP_RECEIVER_REPORT_HPP
#define MS_RTC_RTCP_RECEIVER_REPORT_HPP

#include "common.hpp"
#include "Utils.hpp"
#include "RTC/RTCP/Packet.hpp"
#include <vector>

namespace RTC
{
	namespace RTCP
	{
		class ReceiverReport
		{
		public:
			/* Struct for RTCP receiver report. */
			struct Header
			{
				uint32_t ssrc;//32位，接收到的每个媒体源
				uint32_t fractionLost : 8;//8位，上一次报告之后从SSRC_n来包的漏包比列
				uint32_t totalLost : 24;//24位，自接收开始漏包总数，迟到包不算漏包，重传有可以导致负数
				uint32_t lastSeq; //--? 低16位表式收到的最大seq，高16位表式seq循环次数 
				uint32_t jitter;//RTP包到达时间间隔的统计方差
				uint32_t lsr;//32位， NTP时间戳的中间32位
				uint32_t dlsr;//记录接收SR的时间与发送SR的时间差
			};

		public:
			static ReceiverReport* Parse(const uint8_t* data, size_t len);

		public:
			// Locally generated Report. Holds the data internally.
			ReceiverReport()
			{
				this->header = reinterpret_cast<Header*>(this->raw);
			}
			// Parsed Report. Points to an external data.
			explicit ReceiverReport(Header* header) : header(header)
			{
			}
			explicit ReceiverReport(ReceiverReport* report) : header(report->header)
			{
			}

			void Dump() const;
			size_t Serialize(uint8_t* buffer);
			size_t GetSize() const
			{
				return sizeof(Header);
			}
			uint32_t GetSsrc() const
			{
				return uint32_t{ ntohl(this->header->ssrc) };
			}
			void SetSsrc(uint32_t ssrc)
			{
				this->header->ssrc = uint32_t{ htonl(ssrc) };
			}
			uint8_t GetFractionLost() const
			{
				return uint8_t{ Utils::Byte::Get1Byte((uint8_t*)this->header, 4) };
			}
			void SetFractionLost(uint8_t fractionLost)
			{
				Utils::Byte::Set1Byte((uint8_t*)this->header, 4, fractionLost);
			}
			int32_t GetTotalLost() const
			{
				auto value = uint32_t{ Utils::Byte::Get3Bytes((uint8_t*)this->header, 5) };

				// Possitive value.
				if (((value >> 23) & 1) == 0)
					return value;

				// Negative value.
				if (value != 0x0800000)
					value &= ~(1 << 23);

				return -value;
			}
			void SetTotalLost(int32_t totalLost)
			{
				// Get the limit value for possitive and negative totalLost.
				int32_t clamp = (totalLost >= 0) ? totalLost > 0x07FFFFF ? 0x07FFFFF : totalLost
				                                 : -totalLost > 0x0800000 ? 0x0800000 : -totalLost;

				uint32_t value = (totalLost >= 0) ? (clamp & 0x07FFFFF) : (clamp | 0x0800000);

				Utils::Byte::Set3Bytes(reinterpret_cast<uint8_t*>(this->header), 5, value);
			}
			uint32_t GetLastSeq() const
			{
				return uint32_t{ ntohl(this->header->lastSeq) };
			}
			void SetLastSeq(uint32_t lastSeq)
			{
				this->header->lastSeq = uint32_t{ htonl(lastSeq) };
			}
			uint32_t GetJitter() const
			{
				return uint32_t{ ntohl(this->header->jitter) };
			}
			void SetJitter(uint32_t jitter)
			{
				this->header->jitter = uint32_t{ htonl(jitter) };
			}
			uint32_t GetLastSenderReport() const
			{
				return uint32_t{ ntohl(this->header->lsr) };
			}
			void SetLastSenderReport(uint32_t lsr)
			{
				this->header->lsr = uint32_t{ htonl(lsr) };
			}
			uint32_t GetDelaySinceLastSenderReport() const
			{
				return uint32_t{ ntohl(this->header->dlsr) };
			}
			void SetDelaySinceLastSenderReport(uint32_t dlsr)
			{
				this->header->dlsr = uint32_t{ htonl(dlsr) };
			}

		private:
			Header* header{ nullptr };
			uint8_t raw[sizeof(Header)]{ 0u };
		};

		class ReceiverReportPacket : public Packet
		{
		public:
			using Iterator = std::vector<ReceiverReport*>::iterator;

		public:
			static ReceiverReportPacket* Parse(const uint8_t* data, size_t len, size_t offset = 0);

		public:
			ReceiverReportPacket() : Packet(Type::RR)
			{
			}
			explicit ReceiverReportPacket(CommonHeader* commonHeader) : Packet(commonHeader)
			{
			}
			~ReceiverReportPacket() override
			{
				for (auto* report : this->reports)
				{
					delete report;
				}
			}

			uint32_t GetSsrc() const
			{
				return this->ssrc;
			}
			void SetSsrc(uint32_t ssrc)
			{
				this->ssrc = ssrc;
			}
			void AddReport(ReceiverReport* report)
			{
				this->reports.push_back(report);
			}
			Iterator Begin()
			{
				return this->reports.begin();
			}
			Iterator End()
			{
				return this->reports.end();
			}

			/* Pure virtual methods inherited from Packet. */
		public:
			void Dump() const override;
			size_t Serialize(uint8_t* buffer) override;
			// NOTE: We need to force this since when we parse a SenderReportPacket that
			// contains receive report blocks we also generate a second ReceiverReportPacket
			// from same data and len, so parent Packet::GetType() would return
			// this->type which would be SR instead of RR.
			Type GetType() const override
			{
				return Type::RR;
			}
			size_t GetCount() const override
			{
				return this->reports.size();
			}
			size_t GetSize() const override
			{
				size_t size = sizeof(Packet::CommonHeader) + 4u /* this->ssrc */;

				for (auto* report : reports)
				{
					size += report->GetSize();
				}

				return size;
			}

		private:
			// SSRC of packet sender.
			uint32_t ssrc{ 0u };
			std::vector<ReceiverReport*> reports;
		};
	} // namespace RTCP
} // namespace RTC

#endif
