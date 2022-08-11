//#define MS_CLASS "RTC::RTCP::FeedbackPsPsLei"
// #define MS_LOG_DEV_LEVEL 3

#include "FeedbackPsLei.hpp"
//#include "Logger.hpp"
#include <cstring>
#include "clog.h"

using namespace chen;

namespace RTC
{
	namespace RTCP
	{
		/* Instance methods. */
		FeedbackPsLeiItem::FeedbackPsLeiItem(uint32_t ssrc)
		{
			//MS_TRACE();

			this->raw          = new uint8_t[sizeof(Header)];
			this->header       = reinterpret_cast<Header*>(this->raw);
			this->header->ssrc = uint32_t{ htonl(ssrc) };
		}

		size_t FeedbackPsLeiItem::Serialize(uint8_t* buffer)
		{
			//MS_TRACE();

			// Add minimum header.
			std::memcpy(buffer, this->header, sizeof(Header));

			return sizeof(Header);
		}

		void FeedbackPsLeiItem::Dump() const
		{
			//MS_TRACE();

			DEBUG_LOG("<FeedbackPsLeiItem>");
			DEBUG_LOG("  ssrc : %u"  , this->GetSsrc());
			DEBUG_LOG("</FeedbackPsLeiItem>");
		}
	} // namespace RTCP
} // namespace RTC
