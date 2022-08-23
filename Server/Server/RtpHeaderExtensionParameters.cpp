//#define MS_CLASS "RTC::RtpHeaderExtensionParameters"
// #define MS_LOG_DEV_LEVEL 3
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
	RtpHeaderExtensionParameters::RtpHeaderExtensionParameters(Json::Value& data)
	{
		//MS_TRACE();

		if (!data.isObject())
		{
			ERROR_EX_LOG("data is not an object");
		}

		/*auto jsonUriIt        = data.find("uri");
		auto jsonIdIt         = data.find("id");
		auto jsonEncryptIt    = data.find("encrypt");
		auto jsonParametersIt = data.find("parameters");*/

		// uri is mandatory.
		if (!data.isMember("uri") || !data["uri"].isString() /*jsonUriIt == data.end() || !jsonUriIt->is_string()*/)
		{
			ERROR_EX_LOG("missing uri");
			return;
		}

		this->uri = data["uri"].asCString(); //jsonUriIt->get<std::string>();

		if (this->uri.empty())
		{
			ERROR_EX_LOG("empty uri");
			return;
		}

		// Get the type.
		this->type = RTC::RtpHeaderExtensionUri::GetType(this->uri);

		// id is mandatory.
		if (!data.isMember("id") || !data["id"].isUInt()/*jsonIdIt == data.end() || !Utils::Json::IsPositiveInteger(*jsonIdIt)*/)
		{
			ERROR_EX_LOG("missing id");
			return;
		}

		this->id = data["id"].asUInt(); //jsonIdIt->get<uint8_t>();

		// Don't allow id 0.
		if (this->id == 0u)
		{
			ERROR_EX_LOG("invalid id 0");
			return;
		}

		// encrypt is optional.
		if ( data.isMember("encrypt")&&data["encrypt"].isBool()/*jsonEncryptIt != data.end() && jsonEncryptIt->is_boolean()*/)
		{
			this->encrypt = data["encrypt"].asBool();//jsonEncryptIt->get<bool>();
		}

		// parameters is optional.
		if ( data.isMember("parameters") &&data["parameters"].isObject()/*jsonParametersIt != data.end() && jsonParametersIt->is_object()*/)
		{
			this->parameters.Set(data["parameters"]);
		}
	}

	//void RtpHeaderExtensionParameters::FillJson(json& jsonObject) const
	//{
	//	MS_TRACE();

	//	// Add uri.
	//	jsonObject["uri"] = this->uri;

	//	// Add id.
	//	jsonObject["id"] = this->id;

	//	// Add encrypt.
	//	jsonObject["encrypt"] = this->encrypt;

	//	// Add parameters.
	//	this->parameters.FillJson(jsonObject["parameters"]);
	//}
} // namespace RTC
