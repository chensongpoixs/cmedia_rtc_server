//#define MS_CLASS "RTC::RTCP::XrReceiverReferenceTime"
// #define MS_LOG_DEV_LEVEL 3

#include "XrReceiverReferenceTime.hpp"
//#include "Logger.hpp"
#include <cstring>
#include "clog.h"
using namespace chen;
namespace RTC
{
	namespace RTCP
	{
		/* Class methods. */

		ReceiverReferenceTime* ReceiverReferenceTime::Parse(const uint8_t* data, size_t len)
		{
			//MS_TRACE();

			// Ensure there is space for the common header and the body.
			if (len < sizeof(ExtendedReportBlock::CommonHeader) + sizeof(ReceiverReferenceTime::Body))
			{
				WARNING_EX_LOG("rtcp, not enough space for a extended RRT block, block discarded");

				return nullptr;
			}

			// Get the header.
			auto* header = const_cast<ExtendedReportBlock::CommonHeader*>(
			  reinterpret_cast<const ExtendedReportBlock::CommonHeader*>(data));

			return new ReceiverReferenceTime(header);
		}

		void ReceiverReferenceTime::Dump() const
		{
			//MS_TRACE();

			DEBUG_LOG("<ReceiverReferenceTime>");
			DEBUG_LOG("  block type : %hhu" , static_cast<uint8_t>(this->type));
			DEBUG_LOG("  reserved   : 0");
			DEBUG_LOG("  length     : 2");
			DEBUG_LOG("  ntp sec    : %u"  , GetNtpSec());
			DEBUG_LOG("  ntp frac   : %u"  , GetNtpFrac());
			DEBUG_LOG("</ReceiverReferenceTime>");
		}

		size_t ReceiverReferenceTime::Serialize(uint8_t* buffer)
		{
			//MS_TRACE();

			// Fill the common header.
			this->header->blockType = static_cast<uint8_t>(this->type);
			this->header->reserved  = 0;
			this->header->length    = htons(2);

			std::memcpy(buffer, this->header, sizeof(ExtendedReportBlock::CommonHeader));

			size_t offset{ sizeof(ExtendedReportBlock::CommonHeader) };

			// Copy the body.
			std::memcpy(buffer + offset, this->body, sizeof(ReceiverReferenceTime::Body));

			offset += sizeof(ReceiverReferenceTime::Body);

			return offset;
		}
	} // namespace RTCP
} // namespace RTC
