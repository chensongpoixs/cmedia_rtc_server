///***********************************************************************************************
//created: 		2023-04-08
//
//author:			chensong
//
//purpose:		ccfg
//************************************************************************************************/
//#include "crtp_header_extension_map.h"
//namespace chen {
//	namespace {
//
//		struct ExtensionInfo {
//			RTPExtensionType type;
//			const char* uri;
//		};
//
//		template <typename Extension>
//		  ExtensionInfo CreateExtensionInfo() {
//			return { Extension::kId, Extension::kUri };
//		}
//
//		  ExtensionInfo kExtensions[] = {
//			CreateExtensionInfo<TransmissionOffset>(),
//			CreateExtensionInfo<AudioLevel>(),
//			CreateExtensionInfo<AbsoluteSendTime>(),
//			CreateExtensionInfo<VideoOrientation>(),
//			CreateExtensionInfo<TransportSequenceNumber>(),
//			CreateExtensionInfo<TransportSequenceNumberV2>(),
//			CreateExtensionInfo<PlayoutDelayLimits>(),
//			CreateExtensionInfo<VideoContentTypeExtension>(),
//			CreateExtensionInfo<VideoTimingExtension>(),
//			CreateExtensionInfo<FrameMarkingExtension>(),
//			CreateExtensionInfo<RtpStreamId>(),
//			CreateExtensionInfo<RepairedRtpStreamId>(),
//			CreateExtensionInfo<RtpMid>(),
//			CreateExtensionInfo<RtpGenericFrameDescriptorExtension00>(),
//			CreateExtensionInfo<RtpGenericFrameDescriptorExtension01>(),
//			CreateExtensionInfo<ColorSpaceExtension>(),
//		};
//
//		// Because of kRtpExtensionNone, NumberOfExtension is 1 bigger than the actual
//		// number of known extensions.
//		static_assert(arraysize(kExtensions) ==
//			static_cast<int>(kRtpExtensionNumberOfExtensions) - 1,
//			"kExtensions expect to list all known extensions");
//
//	}  // namespace
//
//	crtp_header_extension_map::crtp_header_extension_map()
//	{
//	}
//	crtp_header_extension_map::~crtp_header_extension_map()
//	{
//	}
//	bool crtp_header_extension_map::RegisterByType(int32 id, RTPExtensionType type)
//	{
//		for (const ExtensionInfo& extension : kExtensions)
//		{
//			if (type == extension.type)
//			{
//				return _register(id, extension.type, extension.uri);
//			}
//		}
//		RTC_NOTREACHED();
//		return false;
//	}
//	bool crtp_header_extension_map::_register(int32 id, RTPExtensionType type, const char * uri)
//	{
//		return false;
//	}
//}