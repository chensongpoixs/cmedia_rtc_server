#ifndef MS_RTC_RTCP_SENDER_REPORT_HPP
#define MS_RTC_RTCP_SENDER_REPORT_HPP

#include "common.hpp"
#include "RTC/RTCP/Packet.hpp"
#include <vector>

namespace RTC
{
	namespace RTCP
	{
		class SenderReport
		{
		public:
			/* Struct for RTCP sender report. */
			struct Header
			{
				uint32_t ssrc;          // Դ
				uint32_t ntpSec;        // ����ʱ����� ���ڲ�ͬԴ֮���ͬ�� 
				uint32_t ntpFrac;
				uint32_t rtpTs;        // ����ʱ�������RTP��ʱ���һ��
				uint32_t packetCount;  //�ܷ��Ͱ���
				uint32_t octetCount;  // �ܷ��͵�������
			};

		public:
			static SenderReport* Parse(const uint8_t* data, size_t len);

		public:
			// Locally generated Report. Holds the data internally.
			SenderReport()
			{
				this->header = reinterpret_cast<Header*>(this->raw);
			}
			// Parsed Report. Points to an external data.
			explicit SenderReport(Header* header) : header(header)
			{
			}
			explicit SenderReport(SenderReport* report) : header(report->header)
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
			uint32_t GetNtpSec() const
			{
				return uint32_t{ ntohl(this->header->ntpSec) };
			}
			void SetNtpSec(uint32_t ntpSec)
			{
				this->header->ntpSec = uint32_t{ htonl(ntpSec) };
			}
			uint32_t GetNtpFrac() const
			{
				return uint32_t{ ntohl(this->header->ntpFrac) };
			}
			void SetNtpFrac(uint32_t ntpFrac)
			{
				this->header->ntpFrac = uint32_t{ htonl(ntpFrac) };
			}
			uint32_t GetRtpTs() const
			{
				return uint32_t{ ntohl(this->header->rtpTs) };
			}
			void SetRtpTs(uint32_t rtpTs)
			{
				this->header->rtpTs = uint32_t{ htonl(rtpTs) };
			}
			uint32_t GetPacketCount() const
			{
				return uint32_t{ ntohl(this->header->packetCount) };
			}
			void SetPacketCount(uint32_t packetCount)
			{
				this->header->packetCount = uint32_t{ htonl(packetCount) };
			}
			uint32_t GetOctetCount() const
			{
				return uint32_t{ ntohl(this->header->octetCount) };
			}
			void SetOctetCount(uint32_t octetCount)
			{
				this->header->octetCount = uint32_t{ htonl(octetCount) };
			}

		private:
			Header* header{ nullptr };
			uint8_t raw[sizeof(Header)]{ 0 };
		};

		class SenderReportPacket : public Packet
		{
		public:
			using Iterator = std::vector<SenderReport*>::iterator;

		public:
			static SenderReportPacket* Parse(const uint8_t* data, size_t len);

		public:
			SenderReportPacket() : Packet(Type::SR)
			{
			}
			explicit SenderReportPacket(CommonHeader* commonHeader) : Packet(commonHeader)
			{
			}
			~SenderReportPacket() override
			{
				for (auto* report : this->reports)
				{
					delete report;
				}
			}

			void AddReport(SenderReport* report)
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
			size_t GetCount() const override
			{
				return 0;
			}
			size_t GetSize() const override
			{
				size_t size = sizeof(Packet::CommonHeader);

				for (auto* report : this->reports)
				{
					size += report->GetSize();
				}

				return size;
			}

		private:
			std::vector<SenderReport*> reports;
		};
	} // namespace RTCP
} // namespace RTC

#endif
