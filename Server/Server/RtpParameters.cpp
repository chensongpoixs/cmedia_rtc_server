//#define MS_CLASS "RTC::RtpParameters"
//// #define MS_LOG_DEV_LEVEL 3
//
//#include "Logger.hpp"
//#include "MediaSoupErrors.hpp"
#include "RtpDictionaries.hpp"
#include <unordered_set>
#include "clog.h"
using namespace chen;
namespace RTC
{
	/* Class variables. */

	// clang-format off
	std::unordered_map<std::string, RtpParameters::Type> RtpParameters::string2Type =
	{
		{ "none",      RtpParameters::Type::NONE      },
		{ "simple",    RtpParameters::Type::SIMPLE    },
		{ "simulcast", RtpParameters::Type::SIMULCAST },
		{ "svc",       RtpParameters::Type::SVC       },
		{ "pipe",      RtpParameters::Type::PIPE      }
	};
	std::map<RtpParameters::Type, std::string> RtpParameters::type2String =
	{
		{ RtpParameters::Type::NONE,      "none"      },
		{ RtpParameters::Type::SIMPLE,    "simple"    },
		{ RtpParameters::Type::SIMULCAST, "simulcast" },
		{ RtpParameters::Type::SVC,       "svc"       },
		{ RtpParameters::Type::PIPE,      "pipe"      }
	};
	// clang-format on

	/* Class methods. */

	RtpParameters::Type RtpParameters::GetType(const RtpParameters& rtpParameters)
	{
		//MS_TRACE();
		// 视频是否是SVC与SIMULCAST
		/*if (rtpParameters.encodings.size() == 1)
		{
			auto& encoding = rtpParameters.encodings[0];

			if (encoding.spatialLayers > 1 || encoding.temporalLayers > 1)
				return RtpParameters::Type::SVC;
			else
				return RtpParameters::Type::SIMPLE;
		}
		else if (rtpParameters.encodings.size() > 1)
		{
			return RtpParameters::Type::SIMULCAST;
		}*/

		return RtpParameters::Type::NONE;
	}

	//RtpParameters::Type RtpParameters::GetType(std::string& str)
	//{
	////	MS_TRACE();

	//	/*auto it = RtpParameters::string2Type.find(str);

	//	if (it == RtpParameters::string2Type.end())
	//	{
	//		ERROR_EX_LOG("invalid RtpParameters type [type:%s]", str.c_str());
	//	}*/

	//	return SIMPLE;// it->second;
	//}

	RtpParameters::Type RtpParameters::GetType(std::string&& str)
	{
		//MS_TRACE();

		auto it = RtpParameters::string2Type.find(str);

		if (it == RtpParameters::string2Type.end())
		{
			ERROR_EX_LOG("invalid RtpParameters type [type:%s]", str.c_str());
		}

		return it->second;
	}

	std::string& RtpParameters::GetTypeString(RtpParameters::Type type)
	{
	//	MS_TRACE();

		return RtpParameters::type2String.at(type);
	}

	/* Instance methods. */

	bool RtpParameters::parse(Json::Value& data)
	{
		if (!data.isMember("codecs"))
		{
			WARNING_EX_LOG("rtpParameters not find `codecs` failed (data = %s) !!!", data.toStyledString().c_str());
			return false;
		}

		if (!data.isMember("headerExtensions"))
		{
			WARNING_EX_LOG("rtpParameters not find `headerExtensions` failed (data = %s) !!!", data.toStyledString().c_str());
			return false;
		}

		if (!data.isMember("encodings"))
		{
			WARNING_EX_LOG("rtpParameters not find `encodings` failed (data = %s) !!!", data.toStyledString().c_str());
			return false;
		}

		if (!data.isMember("rtcp"))
		{
			WARNING_EX_LOG("rtpParameters not find `rtcp` failed (data = %s) !!!", data.toStyledString().c_str());
			return false;
		}

		if (!data.isMember("mid"))
		{
			WARNING_EX_LOG("rtpParameters not find `mid` failed (data = %s) !!!", data.toStyledString().c_str());
			return false;
		}



		this->mid = data["mid"].asCString();
		 
		this->codecs.reserve(data["codecs"].size()/*jsonCodecsIt->size()*/);
		for (auto iter : data["codecs"])
		{
			RtpCodecParameters rtp_code;
			rtp_code.parse(iter);
			codecs.push_back(rtp_code);
		}
		//for (auto& entry : *jsonCodecsIt)
		//{
		//	// This may throw due the constructor of RTC::RtpCodecParameters.
		//	this->codecs.emplace_back(entry);
		//}

		if (this->codecs.empty())
		{
			ERROR_EX_LOG("empty codecs");
			return false;
		}

		// encodings is mandatory.
		if (!data.isMember("encodings") || !data["encodings"].isArray() /*jsonEncodingsIt == data.end() || !jsonEncodingsIt->is_array()*/)
		{
			ERROR_EX_LOG("missing encodings");
			return false;
		}
		const Json::Value & rtpParameter_encodings = data["encodings"];
		this->encodings.reserve(rtpParameter_encodings.size());
		for (auto iter : rtpParameter_encodings)
		{
			RtpEncodingParameters rtp_encoding;
			rtp_encoding.parse(iter);
			this->encodings.emplace_back(rtp_encoding);
		}
		//for (auto& entry : *jsonEncodingsIt)
		//{
		//	// This may throw due the constructor of RTC::RtpEncodingParameters.
		//	this->encodings.emplace_back(entry);
		//}

		if (this->encodings.empty())
		{
			ERROR_EX_LOG("empty encodings");
		}

		// headerExtensions is optional.
		if (data.isMember("headerExtensions") &&data["headerExtensions"].isArray()/*jsonHeaderExtensionsIt != data.end() && jsonHeaderExtensionsIt->is_array()*/)
		{
			const Json::Value & headerExtensionsValue = data["headerExtensions"];
			this->headerExtensions.reserve(headerExtensionsValue.size());

			for (auto iter : headerExtensionsValue)
			{
				// This may throw due the constructor of RTC::RtpHeaderExtensionParameters.
				RtpHeaderExtensionParameters rtp_header_extension_parameters;
				rtp_header_extension_parameters.parse(iter);
				this->headerExtensions.emplace_back(rtp_header_extension_parameters);
			}
		}

		// rtcp is optional.
		if (data.isMember("rtcp") || data["rtcp"].isObject() /*jsonRtcpIt != data.end() && jsonRtcpIt->is_object()*/)
		{
			// This may throw.
			RTC::RtcpParameters rtcp_parameters;
			rtcp_parameters.parse(data["rtcp"]);
			this->rtcp    = rtcp_parameters;
			this->hasRtcp = true;
		}

		// Validate RTP parameters.
		ValidateCodecs();
		ValidateEncodings();
		return true;
	}

	//void RtpParameters::FillJson(json& jsonObject) const
	//{
	//	MS_TRACE();

	//	// Add mid.
	//	if (!this->mid.empty())
	//		jsonObject["mid"] = this->mid;

	//	// Add codecs.
	//	jsonObject["codecs"] = json::array();
	//	auto jsonCodecsIt    = jsonObject.find("codecs");

	//	for (size_t i{ 0 }; i < this->codecs.size(); ++i)
	//	{
	//		jsonCodecsIt->emplace_back(json::value_t::object);

	//		auto& jsonEntry = (*jsonCodecsIt)[i];
	//		auto& codec     = this->codecs[i];

	//		codec.FillJson(jsonEntry);
	//	}

	//	// Add encodings.
	//	jsonObject["encodings"] = json::array();
	//	auto jsonEncodingsIt    = jsonObject.find("encodings");

	//	for (size_t i{ 0 }; i < this->encodings.size(); ++i)
	//	{
	//		jsonEncodingsIt->emplace_back(json::value_t::object);

	//		auto& jsonEntry = (*jsonEncodingsIt)[i];
	//		auto& encoding  = this->encodings[i];

	//		encoding.FillJson(jsonEntry);
	//	}

	//	// Add headerExtensions.
	//	jsonObject["headerExtensions"] = json::array();
	//	auto jsonHeaderExtensionsIt    = jsonObject.find("headerExtensions");

	//	for (size_t i{ 0 }; i < this->headerExtensions.size(); ++i)
	//	{
	//		jsonHeaderExtensionsIt->emplace_back(json::value_t::object);

	//		auto& jsonEntry       = (*jsonHeaderExtensionsIt)[i];
	//		auto& headerExtension = this->headerExtensions[i];

	//		headerExtension.FillJson(jsonEntry);
	//	}

	//	// Add rtcp.
	//	if (this->hasRtcp)
	//		this->rtcp.FillJson(jsonObject["rtcp"]);
	//	else
	//		jsonObject["rtcp"] = json::object();
	//}

RtpParameters::RtpParameters()
{
//	mid = "0";
	/*std::string mid;
	std::vector<RtpCodecParameters> codecs;
	std::vector<RtpEncodingParameters> encodings;
	std::vector<RtpHeaderExtensionParameters> headerExtensions;
	RtcpParameters rtcp;
	bool hasRtcp{ false };*/
}

const RTC::RtpCodecParameters* RtpParameters::GetCodecForEncoding(RtpEncodingParameters& encoding) const
	{
		//MS_TRACE();

		uint8_t payloadType = encoding.codecPayloadType;
		auto it             = this->codecs.begin();

		for (; it != this->codecs.end(); ++it)
		{
			auto& codec = *it;

			if (codec.payloadType == payloadType)
			{
				return std::addressof(codec);
			}
		}

		// This should never happen.
		if (it == this->codecs.end())
		{
			ERROR_EX_LOG("no valid codec payload type for the given encoding");
		}

		return nullptr;
	}

	const RTC::RtpCodecParameters* RtpParameters::GetRtxCodecForEncoding(RtpEncodingParameters& encoding) const
	{
		//MS_TRACE();

		static const std::string AptString{ "apt" };

		uint8_t payloadType = encoding.codecPayloadType;

		for (const auto& codec : this->codecs)
		{
			if (codec.mimeType.IsFeatureCodec() && codec.parameters.GetInteger(AptString) == payloadType)
			{
				return std::addressof(codec);
			}
		}

		return nullptr;
	}

	void RtpParameters::ValidateCodecs()
	{
		//MS_TRACE();

		static const std::string AptString{ "apt" };

		std::unordered_set<uint8_t> payloadTypes;

		for (auto& codec : this->codecs)
		{
			if (payloadTypes.find(codec.payloadType) != payloadTypes.end())
			{
				ERROR_EX_LOG("duplicated payloadType");
			}

			payloadTypes.insert(codec.payloadType);

			switch (codec.mimeType.subtype)
			{
				// A RTX codec must have 'apt' parameter pointing to a non RTX codec.
				case RTC::RtpCodecMimeType::Subtype::RTX:
				{
					// NOTE: RtpCodecParameters already asserted that there is apt parameter.
					int32_t apt = codec.parameters.GetInteger(AptString);
					auto it     = this->codecs.begin();

					for (; it != this->codecs.end(); ++it)
					{
						auto codec = *it;

						if (static_cast<int32_t>(codec.payloadType) == apt)
						{
							if (codec.mimeType.subtype == RTC::RtpCodecMimeType::Subtype::RTX)
							{
								ERROR_EX_LOG("apt in RTX codec points to a RTX codec");
							}
							else if (codec.mimeType.subtype == RTC::RtpCodecMimeType::Subtype::ULPFEC)
							{
								ERROR_EX_LOG("apt in RTX codec points to a ULPFEC codec");
							}
							else if (codec.mimeType.subtype == RTC::RtpCodecMimeType::Subtype::FLEXFEC)
							{
								ERROR_EX_LOG("apt in RTX codec points to a FLEXFEC codec");
							}
							else
								break;
						}
					}

					if (it == this->codecs.end())
					{
						ERROR_EX_LOG("apt in RTX codec points to a non existing codec");
					}

					break;
				}

				default:;
			}
		}
	}

	void RtpParameters::ValidateEncodings()
	{
		uint8_t firstMediaPayloadType{ 0 };

		{
			auto it = this->codecs.begin();

			for (; it != this->codecs.end(); ++it)
			{
				auto& codec = *it;

				// Must be a media codec.
				if (codec.mimeType.IsMediaCodec())
				{
					firstMediaPayloadType = codec.payloadType;

					break;
				}
			}

			if (it == this->codecs.end())
			{
				ERROR_EX_LOG("no media codecs found");
			}
		}

		// Iterate all the encodings, set the first payloadType in all of them with
		// codecPayloadType unset, and check that others point to a media codec.
		//
		// Also, don't allow multiple SVC spatial layers into an encoding if there
		// are more than one encoding (simulcast).
		for (auto& encoding : this->encodings)
		{
			if (encoding.spatialLayers > 1 && this->encodings.size() > 1)
			{
				ERROR_EX_LOG(
				  "cannot use both simulcast and encodings with multiple SVC spatial layers");
			}

			if (!encoding.hasCodecPayloadType)
			{
				encoding.codecPayloadType    = firstMediaPayloadType;
				encoding.hasCodecPayloadType = true;
			}
			else
			{
				auto it = this->codecs.begin();

				for (; it != this->codecs.end(); ++it)
				{
					auto codec = *it;

					if (codec.payloadType == encoding.codecPayloadType)
					{
						// Must be a media codec.
						if (codec.mimeType.IsMediaCodec())
							break;

						ERROR_EX_LOG("invalid codecPayloadType");
					}
				}

				if (it == this->codecs.end())
				{
					ERROR_EX_LOG("unknown codecPayloadType");
				}
			}
		}
	}
} // namespace RTC
