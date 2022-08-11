//#define MS_CLASS "RTC::RTCP::SenderReport"
// #define MS_LOG_DEV_LEVEL 3

#include "SenderReport.hpp"
//#include "Logger.hpp"
#include <cstring>
#include "clog.h"

using namespace chen;

namespace RTC
{
	namespace RTCP
	{
		/* Class methods. */

		SenderReport* SenderReport::Parse(const uint8_t* data, size_t len)
		{
			//MS_TRACE();

			// Get the header.
			auto* header = const_cast<Header*>(reinterpret_cast<const Header*>(data));

			// Packet size must be >= header size.
			if (len < sizeof(Header))
			{
				WARNING_EX_LOG("rtcp, not enough space for sender report, packet discarded");

				return nullptr;
			}

			return new SenderReport(header);
		}

		/* Instance methods. */

		void SenderReport::Dump() const
		{
			//MS_TRACE();

			DEBUG_LOG("<SenderReport>");
			DEBUG_LOG("  ssrc         : %u" , GetSsrc());
			DEBUG_LOG("  ntp sec      : %u" , GetNtpSec());
			DEBUG_LOG("  ntp frac     : %u" , GetNtpFrac());
			DEBUG_LOG("  rtp ts       : %u" , GetRtpTs());
			DEBUG_LOG("  packet count : %u" , GetPacketCount());
			DEBUG_LOG("  octet count  : %u" , GetOctetCount());
			DEBUG_LOG("</SenderReport>");
		}

		size_t SenderReport::Serialize(uint8_t* buffer)
		{
			//MS_TRACE();

			// Copy the header.
			std::memcpy(buffer, this->header, sizeof(Header));

			return sizeof(Header);
		}

		/* Class methods. */

		SenderReportPacket* SenderReportPacket::Parse(const uint8_t* data, size_t len)
		{
			//MS_TRACE();
			//    Sender report (SR) (RFC 3550).            //     0                   1                   2                   3            //     0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1            //    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+            //    |V=2|P|    RC   |   PT=SR=200   |             length            |            //    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+            //  0 |                         SSRC of sender                        |            //    +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+            //  4 |              NTP timestamp, most significant word             |            //    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+            //  8 |             NTP timestamp, least significant word             |            //    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+            // 12 |                         RTP timestamp                         |            //    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+            // 16 |                     sender's packet count                     |            //    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+            // 20 |                      sender's octet count                     |            // 24 +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
			// Get the header.
			auto* header = const_cast<CommonHeader*>(reinterpret_cast<const CommonHeader*>(data));

			std::unique_ptr<SenderReportPacket> packet(new SenderReportPacket(header));
			size_t offset = sizeof(Packet::CommonHeader);

			SenderReport* report = SenderReport::Parse(data + offset, len - offset);

			if (report)
				packet->AddReport(report);

			return packet.release();
		}

		/* Instance methods. */

		size_t SenderReportPacket::Serialize(uint8_t* buffer)
		{
			//MS_TRACE();

			//MS_ASSERT(this->reports.size() == 1, "invalid number of sender reports");

			size_t offset = Packet::Serialize(buffer);

			// Serialize reports.
			for (auto* report : this->reports)
			{
				offset += report->Serialize(buffer + offset);
			}

			return offset;
		}

		void SenderReportPacket::Dump() const
		{
			//MS_TRACE();

			DEBUG_LOG("<SenderReportPacket>");
			for (auto* report : this->reports)
			{
				report->Dump();
			}
			DEBUG_LOG("</SenderReportPacket>");
		}
	} // namespace RTCP
} // namespace RTC
