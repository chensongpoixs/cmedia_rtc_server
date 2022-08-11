//#define MS_CLASS "RTC::RTCP::XR"
// #define MS_LOG_DEV_LEVEL 3

//#include "Logger.hpp"
//#include "Utils.hpp"
#include "XrDelaySinceLastRr.hpp"
#include "XrReceiverReferenceTime.hpp"
#include "clog.h"
#include "crtc_util.h"

using namespace chen;

namespace RTC
{
	namespace RTCP
	{
		/* Class methods. */

		ExtendedReportBlock* ExtendedReportBlock::Parse(const uint8_t* data, size_t len)
		{
			//MS_TRACE();

			// Get the header.
			auto* header = const_cast<CommonHeader*>(reinterpret_cast<const CommonHeader*>(data));

			// Ensure there is space for the common header and the SSRC of packet sender.
			if (len < sizeof(CommonHeader))
			{
				WARNING_EX_LOG("rtcp, not enough space for a extended report block, report discarded");

				return nullptr;
			}

			switch (ExtendedReportBlock::Type(header->blockType))
			{
				case RTC::RTCP::ExtendedReportBlock::Type::RRT:
				{
					return ReceiverReferenceTime::Parse(data, len);
				}

				case RTC::RTCP::ExtendedReportBlock::Type::DLRR:
				{
					return DelaySinceLastRr::Parse(data, len);
				}

				default:
				{
					DEBUG_EX_LOG("rtcp, unknown RTCP XR block type [blockType:%hhu]", header->blockType);

					break;
				}
			}

			return nullptr;
		}

		/* Instance methods. */

		/* Class methods. */

		ExtendedReportPacket* ExtendedReportPacket::Parse(const uint8_t* data, size_t len)
		{
			//MS_TRACE();

			// Get the header.
			auto* header = const_cast<CommonHeader*>(reinterpret_cast<const CommonHeader*>(data));

			// Ensure there is space for the common header and the SSRC of packet sender.
			if (len < sizeof(CommonHeader) + 4u)
			{
				WARNING_EX_LOG("rtcp, not enough space for a extended report packet, packet discarded");

				return nullptr;
			}

			std::unique_ptr<ExtendedReportPacket> packet(new ExtendedReportPacket(header));

			uint32_t ssrc = rtc_byte::get4bytes(reinterpret_cast<uint8_t*>(header), sizeof(CommonHeader));

			packet->SetSsrc(ssrc);

			auto offset = sizeof(Packet::CommonHeader) + 4u /* ssrc */;

			while (len > offset)
			{
				ExtendedReportBlock* report = ExtendedReportBlock::Parse(data + offset, len - offset);

				if (report)
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

		size_t ExtendedReportPacket::Serialize(uint8_t* buffer)
		{
			//MS_TRACE();

			size_t offset = Packet::Serialize(buffer);

			// Copy the SSRC.
			rtc_byte::set4bytes(buffer, sizeof(CommonHeader), this->ssrc);
			offset += 4u /*ssrc*/;

			// Serialize reports.
			for (auto* report : this->reports)
			{
				offset += report->Serialize(buffer + offset);
			}

			return offset;
		}

		void ExtendedReportPacket::Dump() const
		{
			//MS_TRACE();

			DEBUG_LOG("<ExtendedReportPacket>");
			DEBUG_LOG("  ssrc: %u"  , this->ssrc);
			for (auto* report : this->reports)
			{
				report->Dump();
			}
			DEBUG_LOG("</ExtendedReportPacket>");
		}
	} // namespace RTCP
} // namespace RTC
