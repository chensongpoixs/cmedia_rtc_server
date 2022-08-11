//#define MS_CLASS "RTC::RTCP::FeedbackPsSli"
// #define MS_LOG_DEV_LEVEL 3

#include "FeedbackPsSli.hpp"
//#include "Logger.hpp"
#include <cstring>
#include "clog.h"

using namespace chen;

namespace RTC
{
	namespace RTCP
	{
		/* Instance methods. */

		FeedbackPsSliItem::FeedbackPsSliItem(Header* header)
		{
			//MS_TRACE();

			this->header = header;

			auto compact = uint32_t{ ntohl(header->compact) };

			this->first     = compact >> 19;           /* first 13 bits */
			this->number    = (compact >> 6) & 0x1fff; /* next  13 bits */
			this->pictureId = compact & 0x3f;          /* last   6 bits */
		}

		size_t FeedbackPsSliItem::Serialize(uint8_t* buffer)
		{
			uint32_t compact = (this->first << 19) | (this->number << 6) | this->pictureId;
			auto* header     = reinterpret_cast<Header*>(buffer);

			header->compact = uint32_t{ htonl(compact) };
			std::memcpy(buffer, header, sizeof(Header));

			return sizeof(Header);
		}

		void FeedbackPsSliItem::Dump() const
		{
			//MS_TRACE();

			DEBUG_LOG("<FeedbackPsSliItem>");
			DEBUG_LOG("  first      : %hu"  , this->first);
			DEBUG_LOG("  number     : %hu"  , this->number);
			DEBUG_LOG("  picture id : %hhu"  , this->pictureId);
			DEBUG_LOG("</FeedbackPsSliItem>");
		}
	} // namespace RTCP
} // namespace RTC
