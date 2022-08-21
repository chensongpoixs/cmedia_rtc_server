﻿//#define MS_CLASS "RTC::RtpCodecParameters"
// #define MS_LOG_DEV_LEVEL 3

//#include "Logger.hpp"
//#include "MediaSoupErrors.hpp"
//#include "Utils.hpp"
#include "RtpDictionaries.hpp"
#include "clog.h"
#include "crtc_util.h"
using namespace chen;
namespace RTC
{
	/* Instance methods. */

	RtpCodecParameters::RtpCodecParameters(Json::Value& data)
	{
		//MS_TRACE();

		/*if (!data.is_object())
		{
			ERROR_EX_LOG("data is not an object");
		}*/

		/*auto jsonMimeTypeIt     = data.find("mimeType");
		auto jsonPayloadTypeIt  = data.find("payloadType");
		auto jsonClockRateIt    = data.find("clockRate");
		auto jsonChannelsIt     = data.find("channels");
		auto jsonParametersIt   = data.find("parameters");
		auto jsonRtcpFeedbackIt = data.find("rtcpFeedback");*/

		// mimeType is mandatory.
		if (!data.isMember("mimeType") || !data["mimeType"].isString()/*jsonMimeTypeIt == data.end() || !jsonMimeTypeIt->is_string()*/)
		{
			ERROR_EX_LOG("missing mimeType");
		}

		// Set MIME field.
		// This may throw.
		this->mimeType.SetMimeType(data["mimeType"].asCString()/* jsonMimeTypeIt->get<std::string>()*/);

		// payloadType is mandatory.
		// clang-format off
		if ( !data.isMember("payloadType") || !data["payloadType"].isUInt()
			/*jsonPayloadTypeIt == data.end() ||
			!Utils::Json::IsPositiveInteger(*jsonPayloadTypeIt)*/
		)
		// clang-format on
		{
			ERROR_EX_LOG("missing payloadType");
		}

		this->payloadType = data["payloadType"].asInt(); /*jsonPayloadTypeIt->get<uint8_t>()*/;

		// clockRate is mandatory.
		// clang-format off
		if ( !data.isMember("clockRate") || !data["clockRate"].isUInt()
			/*jsonClockRateIt == data.end() ||
			!Utils::Json::IsPositiveInteger(*jsonClockRateIt)*/
		)
		// clang-format on
		{
			ERROR_EX_LOG("missing clockRate");
		}

		this->clockRate = data["clockRate"].asUInt();/*jsonClockRateIt->get<uint32_t>()*/;

		// channels is optional.
		// clang-format off
		if (! data.isMember("channels") || !data["channels"].isUInt()
			/*jsonChannelsIt != data.end() &&
			Utils::Json::IsPositiveInteger(*jsonChannelsIt)*/
		)
		// clang-format on
		{
			this->channels = data["channels"].asUInt();/*jsonChannelsIt->get<uint8_t>();*/
		}

		// parameters is optional.
		if (!data.isMember("parameters") || !data["parameters"].isObject() /*jsonParametersIt != data.end() && jsonParametersIt->is_object()*/)
		{
			this->parameters.Set(data["parameters"]);
		}

		// rtcpFeedback is optional.
		if (!data.isMember("rtcpFeedback") || !data["rtcpFeedback"].isArray() /*jsonRtcpFeedbackIt != data.end() && jsonRtcpFeedbackIt->is_array()*/)
		{
			this->rtcpFeedback.reserve(data["rtcpFeedback"].asLargestUInt()/*jsonRtcpFeedbackIt->size()*/);

			//for (auto& entry : *jsonRtcpFeedbackIt)
			for (size_t i = 0; i < data["rtcpFeedback"].asLargestUInt(); ++i)
			{
				// This may throw due the constructor of RTC::RtcpFeedback.
				this->rtcpFeedback.emplace_back(RtcpFeedback(data["rtcpFeedback"]));
			}
		}

		// Check codec.
		CheckCodec();
	}

	//void RtpCodecParameters::FillJson(json& jsonObject) const
	//{
	//	MS_TRACE();

	//	// Add mimeType.
	//	jsonObject["mimeType"] = this->mimeType.ToString();

	//	// Add payloadType.
	//	jsonObject["payloadType"] = this->payloadType;

	//	// Add clockRate.
	//	jsonObject["clockRate"] = this->clockRate;

	//	// Add channels.
	//	if (this->channels > 1)
	//		jsonObject["channels"] = this->channels;

	//	// Add parameters.
	//	this->parameters.FillJson(jsonObject["parameters"]);

	//	// Add rtcpFeedback.
	//	jsonObject["rtcpFeedback"] = json::array();
	//	auto jsonRtcpFeedbackIt    = jsonObject.find("rtcpFeedback");

	//	for (size_t i{ 0 }; i < this->rtcpFeedback.size(); ++i)
	//	{
	//		jsonRtcpFeedbackIt->emplace_back(json::value_t::object);

	//		auto& jsonEntry = (*jsonRtcpFeedbackIt)[i];
	//		auto& fb        = this->rtcpFeedback[i];

	//		fb.FillJson(jsonEntry);
	//	}
	//}

	inline void RtpCodecParameters::CheckCodec()
	{
	//	MS_TRACE();

		static std::string aptString{ "apt" };

		// Check per MIME parameters and set default values.
		switch (this->mimeType.subtype)
		{
			case RTC::RtpCodecMimeType::Subtype::RTX:
			{
				// A RTX codec must have 'apt' parameter.
				if (!this->parameters.HasPositiveInteger(aptString))
				{
					ERROR_EX_LOG("missing apt parameter in RTX codec");
				}

				break;
			}

			default:;
		}
	}
} // namespace RTC
