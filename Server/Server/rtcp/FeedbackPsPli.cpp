//#define MS_CLASS "RTC::RTCP::FeedbackPsPli"
// #define MS_LOG_DEV_LEVEL 3

#include "FeedbackPsPli.hpp"
//#include "Logger.hpp"
#include "clog.h"

using namespace chen;

namespace RTC
{
	namespace RTCP
	{
		/* Class methods. */

		FeedbackPsPliPacket* FeedbackPsPliPacket::Parse(const uint8_t* data, size_t len)
		{
			//MS_TRACE();

			if (len < sizeof(CommonHeader) + sizeof(FeedbackPacket::Header))
			{
				WARNING_EX_LOG("rtcp, not enough space for Feedback packet, discarded");

				return nullptr;
			}

			// NOLINTNEXTLINE(llvm-qualified-auto)
			auto* commonHeader = const_cast<CommonHeader*>(reinterpret_cast<const CommonHeader*>(data));

			std::unique_ptr<FeedbackPsPliPacket> packet(new FeedbackPsPliPacket(commonHeader));

			return packet.release();
		}

		void FeedbackPsPliPacket::Dump() const
		{
			//MS_TRACE();

			DEBUG_LOG("<FeedbackPsPliPacket>");
			FeedbackPsPacket::Dump();
			DEBUG_LOG("</FeedbackPsPliPacket>");
		}
	} // namespace RTCP
} // namespace RTC
