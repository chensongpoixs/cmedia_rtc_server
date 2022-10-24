//#define MS_CLASS "RTC::RtcpFeedback"
// #define MS_LOG_DEV_LEVEL 3

//#include "Logger.hpp"
//#include "MediaSoupErrors.hpp"
#include "RtpDictionaries.hpp"
#include <json/json.h>
#include "clog.h"

using namespace chen;


namespace RTC
{
	/* Instance methods. */

	bool RtcpFeedback::parse(Json::Value& data)
	{
		//MS_TRACE();

		/*if (!data.isObject())
		{
			ERROR_EX_LOG("data is not an object");
		}*/

		/*auto jsonTypeIt      = data.find("type");
		auto jsonParameterIt = data.find("parameter");*/

		// type is mandatory.
		if (!data.isMember("type") || !data["type"].isString()/*jsonTypeIt == data.end() || !jsonTypeIt->is_string()*/)
		{
			ERROR_EX_LOG("missing type");
			return false;
		}

		this->type = data["type"].asCString() /*jsonTypeIt->get<std::string>()*/;

		// parameter is optional.
		if ( data.isMember("parameter") &&data["parameter"].isString()/*jsonParameterIt != data.end() && jsonParameterIt->is_string()*/)
		{
			this->parameter = data["parameter"].asCString(); //jsonParameterIt->get<std::string>();
		}
		return true;
	}

	//void RtcpFeedback::FillJson(json& jsonObject) const
	//{
	//	MS_TRACE();

	//	// Add type.
	//	jsonObject["type"] = this->type;

	//	// Add parameter (optional).
	//	if (this->parameter.length() > 0)
	//		jsonObject["parameter"] = this->parameter;
	//}
} // namespace RTC
