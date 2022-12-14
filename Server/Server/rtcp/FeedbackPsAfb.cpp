//#define MS_CLASS "RTC::RTCP::FeedbackPsAfb"
// #define MS_LOG_DEV_LEVEL 3

#include "FeedbackPsAfb.hpp"
//#include "Logger.hpp"
//#include "Utils.hpp"
#include "FeedbackPsRemb.hpp"
#include <cstring>

#include "clog.h"
#include "crtc_util.h"

using namespace chen;


namespace RTC
{
	namespace RTCP
	{
		/* Class methods. */

		FeedbackPsAfbPacket* FeedbackPsAfbPacket::Parse(const uint8_t* data, size_t len)
		{
			//MS_TRACE();

			if (len < sizeof(CommonHeader) + sizeof(FeedbackPacket::Header))
			{
				WARNING_EX_LOG("rtcp, not enough space for Feedback packet, discarded");

				return nullptr;
			}

			// NOLINTNEXTLINE(llvm-qualified-auto)
			auto* commonHeader = const_cast<CommonHeader*>(reinterpret_cast<const CommonHeader*>(data));

			std::unique_ptr<FeedbackPsAfbPacket> packet;

			constexpr size_t Offset = sizeof(CommonHeader) + sizeof(FeedbackPacket::Header);

			// clang-format off
			if (
				len >= sizeof(CommonHeader) + sizeof(FeedbackPacket::Header) + 4 &&
				rtc_byte:: get4bytes(data, Offset) == FeedbackPsRembPacket::uniqueIdentifier
			)
			// clang-format on
			{
				packet.reset(FeedbackPsRembPacket::Parse(data, len));
			}
			else
			{
				packet.reset(new FeedbackPsAfbPacket(commonHeader));
			}

			return packet.release();
		}

		size_t FeedbackPsAfbPacket::Serialize(uint8_t* buffer)
		{
		//	MS_TRACE();

			size_t offset = FeedbackPsPacket::Serialize(buffer);

			// Copy the content.
			std::memcpy(buffer + offset, this->data, this->size);

			return offset + this->size;
		}

		void FeedbackPsAfbPacket::Dump() const
		{
		//	MS_TRACE();

			DEBUG_LOG("<FeedbackPsAfbPacket>");
			FeedbackPsPacket::Dump();
			DEBUG_LOG("</FeedbackPsAfbPacket>");
		}
	} // namespace RTCP
} // namespace RTC
