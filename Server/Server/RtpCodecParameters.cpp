//#define MS_CLASS "RTC::RtpCodecParameters"
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

	bool RtpCodecParameters::parse(Json::Value& data)
	{
		//MS_TRACE();
		if (!data.isMember("mimeType"))
		{
			WARNING_EX_LOG("RtpCodecParameters not find `mimeType` failed (data = %s) !!!", data.toStyledString().c_str());
			return false;
		}
		if (!data.isMember("payloadType"))
		{
			WARNING_EX_LOG("RtpCodecParameters not find `payloadType` failed (data = %s) !!!", data.toStyledString().c_str());
			return false;
		}
		if (!data.isMember("clockRate"))
		{
			WARNING_EX_LOG("RtpCodecParameters not find `clockRate` failed (data = %s) !!!", data.toStyledString().c_str());
			return false;
		}
		if (data.isMember("channels"))
		{
			WARNING_EX_LOG("RtpCodecParameters not find `channels` failed (data = %s) !!!", data.toStyledString().c_str());
			//return false;
		}
		if (!data.isMember("parameters"))
		{
			WARNING_EX_LOG("RtpCodecParameters not find `parameters` failed (data = %s)!!!", data.toStyledString().c_str());
			return false;
		}
		if (!data.isMember("rtcpFeedback"))
		{
			WARNING_EX_LOG("RtpCodecParameters not find `rtcpFeedback` failed (data = %s) !!!", data.toStyledString().c_str());
			return false;
		}
		 

	 

		// Set MIME field.
		// This may throw.
		this->mimeType.SetMimeType(data["mimeType"].asCString() );

		 

		this->payloadType = data["payloadType"].asInt(); /*jsonPayloadTypeIt->get<uint8_t>()*/;

		

		this->clockRate = data["clockRate"].asUInt();

		// channels is optional.
		// clang-format off
		if ( data.isMember("channels") &&data["channels"].isUInt()
			/*jsonChannelsIt != data.end() &&
			Utils::Json::IsPositiveInteger(*jsonChannelsIt)*/
		)
		// clang-format on
		{
			this->channels = data["channels"].asUInt();/*jsonChannelsIt->get<uint8_t>();*/
		}

		// parameters is optional.
		if ( data.isMember("parameters") &&data["parameters"].isObject() /*jsonParametersIt != data.end() && jsonParametersIt->is_object()*/)
		{
			this->parameters.Set(data["parameters"]);
		}
		//std::string rss = data.toStyledString();
		// rtcpFeedback is optional.
		if ( data.isMember("rtcpFeedback") && data["rtcpFeedback"].isArray() /*jsonRtcpFeedbackIt != data.end() && jsonRtcpFeedbackIt->is_array()*/)
		{
			this->rtcpFeedbacks.reserve(data["rtcpFeedback"].size()/*jsonRtcpFeedbackIt->size()*/);

			//for (auto& entry : *jsonRtcpFeedbackIt)
			const Json::Value& rtcpFeebackValue = data["rtcpFeedback"];
			for (auto it : rtcpFeebackValue)
			{
				// This may throw due the constructor of RTC::RtcpFeedback.
				RtcpFeedback rtcp_feed_back;
				rtcp_feed_back.parse(it);
				this->rtcpFeedbacks.emplace_back(rtcp_feed_back);
			}
		}

		// Check codec.
		CheckCodec();
		return true;
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

RtpCodecParameters::RtpCodecParameters()
	:  mimeType() // default video/H264
	, payloadType(0u)
	, clockRate(0u)
	, parameters() // default H264 
	, rtcpFeedbacks()
{
	//payloadType = 101; // H264 payloadType 
	//clockRate = 90000;

	//// rtcpfeedback default 
	//// 服务质量Qos RTCPfeedback
	//RtcpFeedback rtcpfeedback;

	//rtcpfeedback.type = "goog-remb";
	//rtcpFeedbacks.push_back(rtcpfeedback);

	//rtcpfeedback.type = "transport-cc";
	//rtcpFeedbacks.push_back(rtcpfeedback);

	//rtcpfeedback.type = "ccm";
	//rtcpfeedback.parameter = "fir";
	//rtcpFeedbacks.push_back(rtcpfeedback);

	//rtcpfeedback.type = "nack";
	//rtcpfeedback.parameter = "";
	//rtcpFeedbacks.push_back(rtcpfeedback);


	//rtcpfeedback.type = "nack";
	//rtcpfeedback.parameter = "pli";
	//rtcpFeedbacks.push_back(rtcpfeedback);
}
//bool RtpCodecParameters::parse(Json::Value & data)
//{
//
//	return true;
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
