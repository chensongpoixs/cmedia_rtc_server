//#define MS_CLASS "RTC::RTCP::ReceiverReport"
// #define MS_LOG_DEV_LEVEL 3

#include "ReceiverReport.hpp"
//#include "Logger.hpp"
//#include "Utils.hpp"
#include <cstring>
#include "crtc_util.h"
#include "clog.h"
using namespace chen;

namespace RTC
{
	namespace RTCP
	{
		/* Class methods. */

		ReceiverReport* ReceiverReport::Parse(const uint8_t* data, size_t len)
		{
			//MS_TRACE();

			// Get the header.
			auto* header = const_cast<Header*>(reinterpret_cast<const Header*>(data));

			// Packet size must be >= header size.
			if (len < sizeof(Header))
			{
				WARNING_EX_LOG("rtcp, not enough space for receiver report, packet discarded");

				return nullptr;
			}

			return new ReceiverReport(header);
		}

		/* Instance methods. */

		void ReceiverReport::Dump() const
		{
			//MS_TRACE();

			DEBUG_LOG("<ReceiverReport>");
			DEBUG_LOG("  ssrc          : %u"  , GetSsrc());
			DEBUG_LOG("  fraction lost : %hhu"  , GetFractionLost());
			DEBUG_LOG("  total lost    : %u"  , GetTotalLost());
			DEBUG_LOG("  last seq      : %u"  , GetLastSeq());
			DEBUG_LOG("  jitter        : %u"  , GetJitter());
			DEBUG_LOG("  lsr           : %u"  , GetLastSenderReport());
			DEBUG_LOG("  dlsr          : %u"  , GetDelaySinceLastSenderReport());
			DEBUG_LOG("</ReceiverReport>");
		}

		size_t ReceiverReport::Serialize(uint8_t* buffer)
		{
			//MS_TRACE();

			// Copy the header.
			std::memcpy(buffer, this->header, sizeof(Header));

			return sizeof(Header);
		}

		/* Class methods. */

		/**
		 * ReceiverReportPacket::Parse()
		 * @param  data   - Points to the begining of the incoming RTCP packet.
		 * @param  len    - Total length of the packet.
		 * @param  offset - points to the first Receiver Report in the incoming packet.
		 */
		ReceiverReportPacket* ReceiverReportPacket::Parse(const uint8_t* data, size_t len, size_t offset)
		{
			//MS_TRACE();
			// RTCP receiver report (RFC 3550).			//			//   0                   1                   2                   3			//   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1			//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+			//  |V=2|P|    RC   |   PT=RR=201   |             length            |			//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+			//  |                     SSRC of packet sender                     |			//  +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+			//  |                         report block(s)                       |			//  |                            ....                               |
			// Get the header.
			auto* header = const_cast<CommonHeader*>(reinterpret_cast<const CommonHeader*>(data));

			// Ensure there is space for the common header and the SSRC of packet sender.
			if (len < sizeof(CommonHeader) + 4u /* ssrc */)
			{
				WARNING_EX_LOG("rtcp, not enough space for receiver report packet, packet discarded");

				return nullptr;
			}

			std::unique_ptr<ReceiverReportPacket> packet(new ReceiverReportPacket(header));

			uint32_t ssrc = rtc_byte::get4bytes(reinterpret_cast<uint8_t*>(header), sizeof(CommonHeader));

			packet->SetSsrc(ssrc);

			if (offset == 0)
			{
				offset = sizeof(Packet::CommonHeader) + 4u /* ssrc */;
			}
			// RC ===> report block个数
			uint8_t count = header->count;

			while ((count-- != 0u) && (len > offset))
			{
//<<<<<<< HEAD
				// RR的反馈信息
//=======
				// 可能有多个源 count是源的个数反馈 信息都是要收集的哈
//>>>>>>> 20adb84c0b9b03c2ea608d143e974cef7a4a3e62
				ReceiverReport* report = ReceiverReport::Parse(data + offset, len - offset);

				if (report != nullptr)
				{
					packet->AddReport(report);
					offset += report->GetSize();
				}
				else
				{
					return packet.release();
				}
			}

			return packet.release();
		}

		/* Instance methods. */

		size_t ReceiverReportPacket::Serialize(uint8_t* buffer)
		{
			//MS_TRACE();

			size_t offset = Packet::Serialize(buffer);

			// Copy the SSRC.
			rtc_byte::set4bytes(buffer, sizeof(Packet::CommonHeader), this->ssrc);
			offset += 4u;

			// Serialize reports.
			for (auto* report : this->reports)
			{
				offset += report->Serialize(buffer + offset);
			}

			return offset;
		}

		void ReceiverReportPacket::Dump() const
		{
			//MS_TRACE();

			DEBUG_LOG("<ReceiverReportPacket>");
			DEBUG_LOG("  ssrc: %u" , this->ssrc);
			for (auto* report : this->reports)
			{
				report->Dump();
			}
			DEBUG_LOG("</ReceiverReportPacket>");
		}
	} // namespace RTCP
} // namespace RTC
