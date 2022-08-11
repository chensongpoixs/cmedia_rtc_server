//#define MS_CLASS "RTC::RTCP::FeedbackRtpEcn"
// #define MS_LOG_DEV_LEVEL 3

#include "FeedbackRtpEcn.hpp"
//#include "Logger.hpp"
#include <cstring>
#include "clog.h"
using namespace chen;
namespace RTC
{
	namespace RTCP
	{
		size_t FeedbackRtpEcnItem::Serialize(uint8_t* buffer)
		{
			//MS_TRACE();

			// Add minimum header.
			std::memcpy(buffer, this->header, sizeof(Header));

			return sizeof(Header);
		}

		void FeedbackRtpEcnItem::Dump() const
		{
			//MS_TRACE();

			DEBUG_LOG("<FeedbackRtpEcnItem>");
			DEBUG_LOG("  sequence number    : %u"  , this->GetSequenceNumber());
			DEBUG_LOG("  ect0 counter       : %u"  , this->GetEct0Counter());
			DEBUG_LOG("  ect1 counter       : %u"  , this->GetEct1Counter());
			DEBUG_LOG("  ecn ce counter     : %hu"  , this->GetEcnCeCounter());
			DEBUG_LOG("  not ect counter    : %hu"  , this->GetNotEctCounter());
			DEBUG_LOG("  lost packets       : %hu"  , this->GetLostPackets());
			DEBUG_LOG("  duplicated packets : %hu"  , this->GetDuplicatedPackets());
			DEBUG_LOG("</FeedbackRtpEcnItem>");
		}
	} // namespace RTCP
} // namespace RTC
