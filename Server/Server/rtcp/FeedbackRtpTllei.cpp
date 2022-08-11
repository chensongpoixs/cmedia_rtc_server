//#define MS_CLASS "RTC::RTCP::FeedbackRtpTllei"
// #define MS_LOG_DEV_LEVEL 3

#include "FeedbackRtpTllei.hpp"
//#include "Logger.hpp"
#include <cstring>

#include "clog.h"

using namespace chen;

namespace RTC
{
	namespace RTCP
	{
		/* Instance methods. */
		FeedbackRtpTlleiItem::FeedbackRtpTlleiItem(uint16_t packetId, uint16_t lostPacketBitmask)
		{
			this->raw    = new uint8_t[sizeof(Header)];
			this->header = reinterpret_cast<Header*>(this->raw);

			this->header->packetId          = uint16_t{ htons(packetId) };
			this->header->lostPacketBitmask = uint16_t{ htons(lostPacketBitmask) };
		}

		size_t FeedbackRtpTlleiItem::Serialize(uint8_t* buffer)
		{
			//MS_TRACE();

			// Add minimum header.
			std::memcpy(buffer, this->header, sizeof(Header));

			return sizeof(Header);
		}

		void FeedbackRtpTlleiItem::Dump() const
		{
			//MS_TRACE();

			DEBUG_LOG("<FeedbackRtpTlleiItem>");
			DEBUG_LOG("  pid: %hu"  , this->GetPacketId());
			DEBUG_LOG("  bpl: %hu"  , this->GetLostPacketBitmask());
			DEBUG_LOG("</FeedbackRtpTlleiItem>");
		}
	} // namespace RTCP
} // namespace RTC
