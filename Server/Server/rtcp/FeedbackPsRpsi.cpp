//#define MS_CLASS "RTC::RTCP::FeedbackPsRpsi"
// #define MS_LOG_DEV_LEVEL 3

#include "FeedbackPsRpsi.hpp"
//#include "Logger.hpp"
#include <cstring>
#include "clog.h"

using namespace chen;
namespace RTC
{
	namespace RTCP
	{
		/* Instance methods. */

		FeedbackPsRpsiItem::FeedbackPsRpsiItem(Header* header)
		{
			//MS_TRACE();

			this->header = header;

			// Calculate bitString length.
			if (this->header->paddingBits % 8 != 0)
			{
				WARNING_EX_LOG("rtcp, invalid Rpsi packet with fractional padding bytes value");

				isCorrect = false;
			}

			size_t paddingBytes = this->header->paddingBits / 8;

			if (paddingBytes > FeedbackPsRpsiItem::maxBitStringSize)
			{
				WARNING_EX_LOG("rtcp, invalid Rpsi packet with too many padding bytes");

				isCorrect = false;
			}

			this->length = FeedbackPsRpsiItem::maxBitStringSize - paddingBytes;
		}

		FeedbackPsRpsiItem::FeedbackPsRpsiItem(uint8_t payloadType, uint8_t* bitString, size_t length)
		{
			//MS_TRACE();

			//MS_ASSERT(payloadType <= 0x7f, "rpsi payload type exceeds the maximum value");
			//MS_ASSERT(
			//  length <= FeedbackPsRpsiItem::maxBitStringSize,
			//  "rpsi bit string length exceeds the maximum value");

			this->raw    = new uint8_t[sizeof(Header)];
			this->header = reinterpret_cast<Header*>(this->raw);

			// 32 bits padding.
			// TODO@chensong 202208011  
			// .cpp(55): error C4146: 一元负运算符应用于无符号类型，结果仍为无符号类型  --> 解决办法——关闭SDL检查
			size_t padding = (-length) & 3;

			this->header->paddingBits = padding * 8;
			this->header->zero        = 0;
			std::memcpy(this->header->bitString, bitString, length);

			// Fill padding.
			for (size_t i{ 0 }; i < padding; ++i)
			{
				this->raw[sizeof(Header) + i - 1] = 0;
			}
		}

		size_t FeedbackPsRpsiItem::Serialize(uint8_t* buffer)
		{
			//MS_TRACE();

			std::memcpy(buffer, this->header, sizeof(Header));

			return sizeof(Header);
		}

		void FeedbackPsRpsiItem::Dump() const
		{
			//MS_TRACE();

			DEBUG_LOG("<FeedbackPsRpsiItem>");
			DEBUG_LOG("  padding bits : %hhu"  , this->header->paddingBits);
			DEBUG_LOG("  payload type : %hhu"  , this->GetPayloadType());
			DEBUG_LOG("  length       : %zu", this->GetLength());
			DEBUG_LOG("</FeedbackPsRpsiItem>");
		}
	} // namespace RTCP
} // namespace RTC
