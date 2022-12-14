//#define MS_CLASS "RTC::RTCP::FeedbackPsRemb"
// #define MS_LOG_DEV_LEVEL 3

#include "FeedbackPsRemb.hpp"
//#include "Logger.hpp"
//#include "Utils.hpp"
#include <cstring>
#include "cnet_type.h"
#include "clog.h"
#include "crtc_util.h"
using namespace chen;

namespace RTC
{
	namespace RTCP
	{
		/* Class variables. */

		uint32_t FeedbackPsRembPacket::uniqueIdentifier{ 0x52454D42 };

		/* Class methods. */

		FeedbackPsRembPacket* FeedbackPsRembPacket::Parse(const uint8_t* data, size_t len)
		{
			//MS_TRACE();

			// Check that there is space for the REMB unique identifier and basic fields.
			// NOTE: Feedback.cpp already checked that there is space for CommonHeader and
			// Feedback Header.
			if (len < sizeof(CommonHeader) + sizeof(FeedbackPacket::Header) + 8u)
			{
				WARNING_EX_LOG("rtcp, not enough space for Feedback packet, discarded");

				return nullptr;
			}

			// NOLINTNEXTLINE(llvm-qualified-auto)
			auto* commonHeader = const_cast<CommonHeader*>(reinterpret_cast<const CommonHeader*>(data));

			std::unique_ptr<FeedbackPsRembPacket> packet(new FeedbackPsRembPacket(commonHeader, len));

			if (!packet->IsCorrect())
			{
				return nullptr;
			}

			return packet.release();
		}

		FeedbackPsRembPacket::FeedbackPsRembPacket(CommonHeader* commonHeader, size_t availableLen)
		  : FeedbackPsAfbPacket(commonHeader, FeedbackPsAfbPacket::Application::REMB)
		{
			size_t len = static_cast<size_t>(ntohs(commonHeader->length) + 1) * 4;

			if (len > availableLen)
			{
				WARNING_EX_LOG("rtcp, packet announced length exceeds the available buffer length, discarded");

				this->isCorrect = false;

				return;
			}

			// Make data point to the 4 bytes that must containt the "REMB" identifier.
			auto* data = reinterpret_cast<uint8_t*>(commonHeader) + sizeof(CommonHeader) +
			             sizeof(FeedbackPacket::Header);
			size_t numSsrcs = data[4];

			// Ensure there is space for the the announced number of SSRC feedbacks.
			if (len != sizeof(CommonHeader) + sizeof(FeedbackPacket::Header) + 8u + (numSsrcs * 4u))
			{
				WARNING_EX_LOG("rtcp, invalid payload size (%zu bytes) for the given number of ssrcs (%zu)", len, numSsrcs);

				this->isCorrect = false;

				return;
			}

			// Verify the "REMB" unique identifier.
			if (rtc_byte::get4bytes(data, 0) != FeedbackPsRembPacket::uniqueIdentifier)
			{
				WARNING_EX_LOG("rtcp, invalid unique indentifier in REMB packet");

				this->isCorrect = false;

				return;
			}

			uint8_t exponent = data[5] >> 2;
			uint64_t mantissa = (static_cast<uint32_t>(data[5] & 0x03) << 16) | rtc_byte::get2bytes(data, 6);

			this->bitrate = (mantissa << exponent);

			if ((this->bitrate >> exponent) != mantissa)
			{
				WARNING_EX_LOG("rtcp, invalid REMB bitrate value: %" PRIu64 " *2^%u", mantissa, exponent);

				this->isCorrect = false;

				return;
			}

			// Make index point to the first SSRC feedback item.
			size_t index{ 8 };

			this->ssrcs.reserve(numSsrcs);

			for (size_t n{ 0 }; n < numSsrcs; ++n)
			{
				this->ssrcs.push_back(rtc_byte::get4bytes(data, index));
				index += 4u;
			}
		}

		size_t FeedbackPsRembPacket::Serialize(uint8_t* buffer)
		{
			//MS_TRACE();

			size_t offset     = FeedbackPsPacket::Serialize(buffer);
			uint64_t mantissa = this->bitrate;
			uint8_t exponent{ 0u };

			while (mantissa > 0x3FFFF /* max mantissa (18 bits) */)
			{
				mantissa >>= 1;
				++exponent;
			}

			rtc_byte::set4bytes(buffer, offset, FeedbackPsRembPacket::uniqueIdentifier);
			offset += sizeof(FeedbackPsRembPacket::uniqueIdentifier);

			buffer[offset] = this->ssrcs.size();
			offset += 1;

			buffer[offset] = (exponent << 2) | (mantissa >> 16);
			offset += 1;

			rtc_byte::set2bytes(buffer, offset, mantissa & 0xFFFF);
			offset += 2;

			for (auto ssrc : this->ssrcs)
			{
				rtc_byte::set4bytes(buffer, offset, ssrc);
				offset += 4u;
			}

			return offset;
		}

		void FeedbackPsRembPacket::Dump() const
		{
			//MS_TRACE();

			DEBUG_LOG("<FeedbackPsRembPacket>");
			FeedbackPsPacket::Dump();
			DEBUG_LOG("  bitrate (bps) : %lli"  , this->bitrate);
			for (auto ssrc : this->ssrcs)
			{
				DEBUG_LOG("  ssrc          : %u"  , ssrc);
			}
			DEBUG_LOG("</FeedbackPsRembPacket>");
		}
	} // namespace RTCP
} // namespace RTC
