//#define MS_CLASS "RTC::RTCP::FeedbackRtpNack"
// #define MS_LOG_DEV_LEVEL 3

#include "FeedbackRtpNack.hpp"
//#include "Logger.hpp"
#include <bitset> // std::bitset()
#include <cstring>

#include "clog.h"
using namespace chen;

namespace RTC
{
	namespace RTCP
	{
		/* Instance methods. */
		FeedbackRtpNackItem::FeedbackRtpNackItem(uint16_t packetId, uint16_t lostPacketBitmask)
		{
			this->raw    = new uint8_t[sizeof(Header)];
			this->header = reinterpret_cast<Header*>(this->raw);

			this->header->packetId          = uint16_t{ htons(packetId) };
			this->header->lostPacketBitmask = uint16_t{ htons(lostPacketBitmask) };
		}

		size_t FeedbackRtpNackItem::Serialize(uint8_t* buffer)
		{
			//MS_TRACE();

			// Add minimum header.
			std::memcpy(buffer, this->header, sizeof(Header));

			return sizeof(Header);
		}

		void FeedbackRtpNackItem::Dump() const
		{
			//MS_TRACE();

			std::bitset<16> nackBitset(this->GetLostPacketBitmask());

			DEBUG_LOG("<FeedbackRtpNackItem>");
			DEBUG_LOG("  pid : %hu"  , this->GetPacketId());
			DEBUG_LOG("  bpl : %s", nackBitset.to_string().c_str());
			DEBUG_LOG("</FeedbackRtpNackItem>");
		}
	} // namespace RTCP
} // namespace RTC
