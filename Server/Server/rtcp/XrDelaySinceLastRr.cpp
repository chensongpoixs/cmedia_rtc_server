//#define MS_CLASS "RTC::RTCP::XrDelaySinceLastRr"
// #define MS_LOG_DEV_LEVEL 3

#include "XrDelaySinceLastRr.hpp"
//#include "Logger.hpp"
#include <cstring>
#include "clog.h"

using namespace chen;


namespace RTC
{
	namespace RTCP
	{
		/* Class methods. */

		DelaySinceLastRr::SsrcInfo* DelaySinceLastRr::SsrcInfo::Parse(const uint8_t* data, size_t len)
		{
			//MS_TRACE();

			// Ensure there is space for the common header and the SSRC of packet sender.
			if (len < sizeof(Body))
			{
				WARNING_EX_LOG("rtcp, not enough space for a extended DSLR sub-block, sub-block discarded");

				return nullptr;
			}

			// Get the header.
			auto* body = const_cast<SsrcInfo::Body*>(reinterpret_cast<const SsrcInfo::Body*>(data));

			auto* ssrcInfo = new DelaySinceLastRr::SsrcInfo(body);

			return ssrcInfo;
		}

		/* Instance methods. */

		void DelaySinceLastRr::SsrcInfo::Dump() const
		{
			//MS_TRACE();

			DEBUG_LOG("  <SsrcInfo>");
			DEBUG_LOG("    ssrc : %u"  , GetSsrc());
			DEBUG_LOG("    lrr  : %u"  , GetLastReceiverReport());
			DEBUG_LOG("    dlrr : %u"  , GetDelaySinceLastReceiverReport());
			DEBUG_LOG("  <SsrcInfo>");
		}

		size_t DelaySinceLastRr::SsrcInfo::Serialize(uint8_t* buffer)
		{
			//MS_TRACE();

			// Copy the body.
			std::memcpy(buffer, this->body, sizeof(DelaySinceLastRr::SsrcInfo::Body));

			return sizeof(DelaySinceLastRr::SsrcInfo::Body);
		}

		/* Class methods. */

		DelaySinceLastRr* DelaySinceLastRr::Parse(const uint8_t* data, size_t len)
		{
			//MS_TRACE();

			auto* header = const_cast<ExtendedReportBlock::CommonHeader*>(
			  reinterpret_cast<const ExtendedReportBlock::CommonHeader*>(data));
			std::unique_ptr<DelaySinceLastRr> report(new DelaySinceLastRr(header));
			size_t offset{ sizeof(ExtendedReportBlock::CommonHeader) };
			uint16_t reportLength = ntohs(header->length) * 4;

			while (len > offset && reportLength >= sizeof(DelaySinceLastRr::SsrcInfo::Body))
			{
				DelaySinceLastRr::SsrcInfo* ssrcInfo =
				  DelaySinceLastRr::SsrcInfo::Parse(data + offset, len - offset);

				if (ssrcInfo)
				{
					report->AddSsrcInfo(ssrcInfo);
					offset += ssrcInfo->GetSize();
				}
				else
				{
					return report.release();
				}

				offset += ssrcInfo->GetSize();
				reportLength -= sizeof(DelaySinceLastRr::SsrcInfo::Body);
			}

			return report.release();
		}

		/* Instance methods. */

		size_t DelaySinceLastRr::Serialize(uint8_t* buffer)
		{
			//MS_TRACE();

			size_t length = static_cast<uint16_t>((sizeof(SsrcInfo::Body) * this->ssrcInfos.size() / 4));

			// Fill the common header.
			this->header->blockType = static_cast<uint8_t>(this->type);
			this->header->reserved  = 0;
			this->header->length    = uint16_t{ htons(length) };

			std::memcpy(buffer, this->header, sizeof(ExtendedReportBlock::CommonHeader));

			size_t offset{ sizeof(ExtendedReportBlock::CommonHeader) };

			// Serialize sub-blocks.
			for (auto* ssrcInfo : this->ssrcInfos)
			{
				offset += ssrcInfo->Serialize(buffer + offset);
			}

			return offset;
		}

		void DelaySinceLastRr::Dump() const
		{
			//MS_TRACE();

			DEBUG_LOG("<DelaySinceLastRr>");
			DEBUG_LOG("  block type : %hhu" , (uint8_t)this->type);
			DEBUG_LOG("  reserved   : 0");
			DEBUG_LOG( "  length     : %hu"  ,
			  static_cast<uint16_t>((sizeof(SsrcInfo::Body) * this->ssrcInfos.size() / 4)));
			for (auto* ssrcInfo : this->ssrcInfos)
			{
				ssrcInfo->Dump();
			}
			DEBUG_LOG("</DelaySinceLastRr>");
		}
	} // namespace RTCP
} // namespace RTC
