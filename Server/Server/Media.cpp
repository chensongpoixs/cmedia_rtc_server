//#define MS_CLASS "RTC::Media"
//// #define MS_LOG_DEV_LEVEL 3
//
//#include "Logger.hpp"
//#include "MediaSoupErrors.hpp"
//#include "Utils.hpp"
#include "RtpDictionaries.hpp"
#include "clog.h"
using namespace chen;
namespace RTC
{
	/* Class variables. */

	// clang-format off
	std::unordered_map<std::string, Media::Kind> Media::string2Kind =
	{
		{ "",      Media::Kind::ALL   },
		{ "audio", Media::Kind::AUDIO },
		{ "video", Media::Kind::VIDEO }
	};
	std::map<Media::Kind, std::string> Media::kind2String =
	{
		{ Media::Kind::ALL,   ""      },
		{ Media::Kind::AUDIO, "audio" },
		{ Media::Kind::VIDEO, "video" }
	};
	// clang-format on

	/* Class methods. */

	Media::Kind Media::GetKind(std::string& str)
	{
		//MS_TRACE();

		// Force lowcase kind.
		std::transform(str.begin(), str.end(), str.begin(), ::tolower);
		//Utils::String::ToLowerCase(str);

		auto it = Media::string2Kind.find(str);

		if (it == Media::string2Kind.end())
		{
			ERROR_EX_LOG("invalid media kind [kind:%s]", str.c_str());
		}

		return it->second;
	}

	Media::Kind Media::GetKind(std::string&& str)
	{
		//MS_TRACE();

		// Force lowcase kind.
		std::transform(str.begin(), str.end(), str.begin(), ::tolower);
		//Utils::String::ToLowerCase(str);

		auto it = Media::string2Kind.find(str);

		if (it == Media::string2Kind.end())
		{
			ERROR_EX_LOG("invalid media kind [kind:%s]", str.c_str());
		}
			//MS_THROW_TYPE_ERROR("invalid media kind [kind:%s]", str.c_str());

		return it->second;
	}

	const std::string& Media::GetString(Media::Kind kind)
	{
		//MS_TRACE();

		return Media::kind2String.at(kind);
	}
} // namespace RTC
