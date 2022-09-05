//#define MS_CLASS "RTC::RtpRtxParameters"
//// #define MS_LOG_DEV_LEVEL 3
//
//#include "Logger.hpp"
//#include "MediaSoupErrors.hpp"
//#include "Utils.hpp"
#include "RtpDictionaries.hpp"
#include "clog.h"
namespace RTC
{

	/* Instance methods. */
	using namespace chen;
	RtpRtxParameters::RtpRtxParameters(Json::Value& data)
	{
		//MS_TRACE();

		if (!data.isObject())
		{
			ERROR_EX_LOG("data is not an object");
		}

		//auto jsonSsrcIt = data.find("ssrc");

		// ssrc is optional.
		// clang-format off
		if ( data.isMember("ssrc") &&data["ssrc"].isUInt()
			/*jsonSsrcIt != data.end() &&
			Utils::Json::IsPositiveInteger(*jsonSsrcIt)*/
		)
		// clang-format on
		{
			this->ssrc = data["ssrc"].asUInt();//jsonSsrcIt->get<uint32_t>();
		}
	}

	//void RtpRtxParameters::FillJson(json& jsonObject) const
	//{
	//	MS_TRACE();

	//	// Force it to be an object even if no key/values are added below.
	//	jsonObject = json::object();

	//	// Add ssrc (optional).
	//	if (this->ssrc != 0u)
	//		jsonObject["ssrc"] = this->ssrc;
	//}
} // namespace RTC
