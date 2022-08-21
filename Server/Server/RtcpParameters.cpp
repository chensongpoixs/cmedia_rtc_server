//#define MS_CLASS "RTC::RtcpParameters"
// #define MS_LOG_DEV_LEVEL 3

//#include "Logger.hpp"
//#include "MediaSoupErrors.hpp"
//#include "Utils.hpp"
#include "RtpDictionaries.hpp"

#include "crtc_util.h"
#include "clog.h"

namespace RTC
{
	/* Instance methods. */
	using namespace chen;

	RtcpParameters::RtcpParameters(Json::Value& data)
	{
		//MS_TRACE();

		if (!data.isObject())
		{
			ERROR_EX_LOG("data is not an object");
		}

		/*auto jsonCnameIt       = data.find("cname");
		auto jsonSsrcIt        = data.find("ssrc");
		auto jsonRedicedSizeIt = data.find("reducedSize");*/

		// cname is optional.
		if (data.isMember("cname") && data["cname"].isString()/*jsonCnameIt != data.end() && jsonCnameIt->is_string()*/)
			this->cname = data["cname"].asCString();// jsonCnameIt->get<std::string>();

		// ssrc is optional.
		// clang-format off
		if ( data.isMember("ssrc") && data["ssrc"].isUInt()
			/*jsonSsrcIt != data.end() &&
			Utils::Json::IsPositiveInteger(*jsonSsrcIt)*/
		)
		// clang-format on
		{
			this->ssrc = data["ssrc"].asUInt();////jsonSsrcIt->get<uint32_t>();
		}

		// reducedSize is optional.
		if (data.isMember("reducedSize") && data["reducedSize"].isBool() /*jsonRedicedSizeIt != data.end() && jsonRedicedSizeIt->is_boolean()*/)
			this->reducedSize = data["reducedSize"].asBool(); //jsonRedicedSizeIt->get<bool>();
	}

	//void RtcpParameters::FillJson(json& jsonObject) const
	//{
	//	MS_TRACE();

	//	// Add cname.
	//	if (!this->cname.empty())
	//		jsonObject["cname"] = this->cname;

	//	// Add ssrc.
	//	if (this->ssrc != 0u)
	//		jsonObject["ssrc"] = this->ssrc;

	//	// Add reducedSize.
	//	jsonObject["reducedSize"] = this->reducedSize;
	//}
} // namespace RTC
