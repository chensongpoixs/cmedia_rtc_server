//#define MS_CLASS "RTC::RtpEncodingParameters"
//// #define MS_LOG_DEV_LEVEL 3
//
//#include "Logger.hpp"
//#include "MediaSoupErrors.hpp"
//#include "Utils.hpp"
#include "RtpDictionaries.hpp"
#include <exception>
#include <regex>
#include "clog.h"
namespace RTC
{
	using namespace chen;
	/* Instance methods. */

	//RtpEncodingParameters::RtpEncodingParameters(Json::Value& data)
	//{
	//	//MS_TRACE();

	//	if (!data.isObject())
	//	{
	//		ERROR_EX_LOG("data is not an object");
	//	}

	//	/*auto jsonSsrcIt             = data.find("ssrc");
	//	auto jsonRidIt              = data.find("rid");
	//	auto jsonCodecPayloadTypeIt = data.find("codecPayloadType");
	//	auto jsonRtxIt              = data.find("rtx");
	//	auto jsonMaxBitrateIt       = data.find("maxBitrate");
	//	auto jsonMaxFramerateIt     = data.find("maxFramerate");
	//	auto jsonDtxIt              = data.find("dtx");
	//	auto jsonScalabilityModeIt  = data.find("scalabilityMode");*/

	//	// ssrc is optional.
	//	// clang-format off
	//	if (  data.isMember("ssrc")&&data["ssrc"].isUInt()
	//		/*jsonSsrcIt != data.end() &&
	//		Utils::Json::IsPositiveInteger(*jsonSsrcIt)*/
	//	)
	//	// clang-format on
	//	{
	//		this->ssrc = data["ssrc"].asUInt(); //jsonSsrcIt->get<uint32_t>();
	//	}

	//	// rid is optional.
	//	if ( data.isMember("rid") &&data["rid"].isString() /*jsonRidIt != data.end() && jsonRidIt->is_string()*/)
	//	{
	//		this->rid = data["rid"].asCString();////jsonRidIt->get<std::string>();
	//	}

	//	// codecPayloadType is optional.
	//	// clang-format off
	//	if (  data.isMember("codecPayloadType") &&data["codecPayloadType"].isUInt()
	//		/*jsonCodecPayloadTypeIt != data.end() &&
	//		Utils::Json::IsPositiveInteger(*jsonCodecPayloadTypeIt)*/
	//	)
	//	// clang-format on
	//	{
	//		this->codecPayloadType = data["codecPayloadType"].asUInt();//jsonCodecPayloadTypeIt->get<uint8_t>();
	//		this->hasCodecPayloadType = true;
	//	}

	//	// rtx is optional.
	//	// This may throw.
	//	if ( data.isMember("rtx") &&data["rtx"].isObject() /*jsonRtxIt != data.end() && jsonRtxIt->is_object()*/)
	//	{
	//		this->rtx    = RtpRtxParameters(data["rtx"]);
	//		this->hasRtx = true;
	//	}

	//	// maxBitrate is optional.
	//	// clang-format off
	//	if (  data.isMember("maxBitrate") &&data["maxBitrate"].isUInt()
	//		/*jsonMaxBitrateIt != data.end() &&
	//		Utils::Json::IsPositiveInteger(*jsonMaxBitrateIt)*/
	//	)
	//	// clang-format on
	//	{
	//		this->maxBitrate = data["maxBitrate"].asUInt();//jsonMaxBitrateIt->get<uint32_t>();
	//	}

	//	// maxFramerate is optional.
	//	if (!data.isMember("maxFramerate") || !data["maxFramerate"].isDouble() /*jsonMaxFramerateIt != data.end() && jsonMaxFramerateIt->is_number()*/)
	//	{
	//		this->maxFramerate = data["maxFramerate"].asDouble();//jsonMaxFramerateIt->get<double>();
	//	}
	//	// dtx is optional.
	//	if (!data.isMember("dtx") || !data["dtx"].isBool()/*jsonDtxIt != data.end() && jsonDtxIt->is_boolean()*/)
	//	{
	//		this->dtx = data["dtx"].asBool();//// jsonDtxIt->get<bool>();
	//	}

	//	// scalabilityMode is optional.
	//	if (!data.isMember("scalabilityMode") || ! data["scalabilityMode"].isString()/*jsonScalabilityModeIt != data.end() && jsonScalabilityModeIt->is_string()*/)
	//	{
	//		std::string scalabilityMode = data["scalabilityMode"].asCString();//jsonScalabilityModeIt->get<std::string>();

	//		static const std::regex ScalabilityModeRegex(
	//		  "^[LS]([1-9]\\d{0,1})T([1-9]\\d{0,1})(_KEY)?.*", std::regex_constants::ECMAScript);

	//		std::smatch match;

	//		std::regex_match(scalabilityMode, match, ScalabilityModeRegex);

	//		if (!match.empty())
	//		{
	//			this->scalabilityMode = scalabilityMode;

	//			try
	//			{
	//				this->spatialLayers  = std::stoul(match[1].str());
	//				this->temporalLayers = std::stoul(match[2].str());
	//				this->ksvc           = match.size() >= 4 && match[3].str() == "_KEY";
	//			}
	//			catch (std::exception& e)
	//			{
	//				ERROR_EX_LOG("invalid scalabilityMode: %s", e.what());
	//			}
	//		}
	//	}
	//}

	//void RtpEncodingParameters::FillJson(json& jsonObject) const
	//{
	//	MS_TRACE();

	//	// Force it to be an object even if no key/values are added below.
	//	jsonObject = json::object();

	//	// Add ssrc.
	//	if (this->ssrc != 0u)
	//		jsonObject["ssrc"] = this->ssrc;

	//	// Add rid.
	//	if (!this->rid.empty())
	//		jsonObject["rid"] = this->rid;

	//	// Add codecPayloadType.
	//	if (this->hasCodecPayloadType)
	//		jsonObject["codecPayloadType"] = this->codecPayloadType;

	//	// Add rtx.
	//	if (this->hasRtx)
	//		this->rtx.FillJson(jsonObject["rtx"]);

	//	// Add maxBitrate.
	//	if (this->maxBitrate != 0u)
	//		jsonObject["maxBitrate"] = this->maxBitrate;

	//	// Add maxFramerate.
	//	if (this->maxFramerate > 0)
	//		jsonObject["maxFramerate"] = this->maxFramerate;

	//	// Add dtx.
	//	if (this->dtx)
	//		jsonObject["dtx"] = this->dtx;

	//	// Add scalabilityMode.
	//	if (!this->scalabilityMode.empty())
	//	{
	//		jsonObject["scalabilityMode"] = this->scalabilityMode;
	//		jsonObject["spatialLayers"]   = this->spatialLayers;
	//		jsonObject["temporalLayers"]  = this->temporalLayers;
	//		jsonObject["ksvc"]            = this->ksvc;
	//	}
	//}
} // namespace RTC
