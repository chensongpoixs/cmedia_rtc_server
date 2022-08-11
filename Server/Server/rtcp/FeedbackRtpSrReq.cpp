//#define MS_CLASS "RTC::RTCP::FeedbackRtpSrReq"
// #define MS_LOG_DEV_LEVEL 3

#include "FeedbackRtpSrReq.hpp"
//#include "Logger.hpp"
#include "clog.h"

using namespace chen;

namespace RTC
{
	namespace RTCP
	{
		/* Class methods. */

		FeedbackRtpSrReqPacket* FeedbackRtpSrReqPacket::Parse(const uint8_t* data, size_t len)
		{
		//	MS_TRACE();

			if (len < sizeof(CommonHeader) + sizeof(FeedbackPacket::Header))
			{
				WARNING_EX_LOG("rtcp, not enough space for Feedback packet, discarded");

				return nullptr;
			}

			auto* commonHeader = reinterpret_cast<CommonHeader*>(const_cast<uint8_t*>(data));

			return new FeedbackRtpSrReqPacket(commonHeader);
		}

		void FeedbackRtpSrReqPacket::Dump() const
		{
			//MS_TRACE();

			DEBUG_LOG("<FeedbackRtpSrReqPacket>");
			FeedbackRtpPacket::Dump();
			DEBUG_LOG("</FeedbackRtpSrReqPacket>");
		}
	} // namespace RTCP
} // namespace RTC
