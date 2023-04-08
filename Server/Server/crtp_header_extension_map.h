///***********************************************************************************************
//created: 		2023-04-08
//
//author:			chensong
//
//purpose:		ccfg
//************************************************************************************************/
//#ifndef _C_RTP_HEADER_EXTENSION_MAP_H_
//#define _C_RTP_HEADER_EXTENSION_MAP_H_
//#include "cnet_type.h"
//#include "crtp_rtcp_define.h"
//#include "clog.h"
//namespace chen {
//	class crtp_header_extension_map
//	{
//	public:
//		static constexpr RTPExtensionType kInvalidType = kRtpExtensionNone;
//		static constexpr int kInvalidId = 0;
//	public:
//		crtp_header_extension_map();
//		~crtp_header_extension_map();
//	public:
//		template <typename Extension>
//		bool Register(int32 id) 
//		{
//			return Register(id, Extension::kId, Extension::kUri);
//		}
//		bool RegisterByType(int32 id, RTPExtensionType type);
//		bool RegisterByUri(int32 id, const std::string& uri);
//
//		bool IsRegistered(RTPExtensionType type) const {
//			return GetId(type) != kInvalidId;
//		}
//		// Return kInvalidType if not found.
//		RTPExtensionType GetType(int32 id) const;
//		// Return kInvalidId if not found.
//		uint8 GetId(RTPExtensionType type) const {
//			RTC_DCHECK_GT(type, kRtpExtensionNone);
//			RTC_DCHECK_LT(type, kRtpExtensionNumberOfExtensions);
//			return m_ids[type];
//		}
//
//		// TODO(danilchap): Remove use of the functions below.
//		int32 Register(RTPExtensionType type, int32 id) {
//			return RegisterByType(id, type) ? 0 : -1;
//		}
//		int32 Deregister(RTPExtensionType type);
//
//		// Corresponds to the SDP attribute extmap-allow-mixed, see RFC8285.
//		// Set to true if it's allowed to mix one- and two-byte RTP header extensions
//		// in the same stream.
//		bool ExtmapAllowMixed() const { return m_extmap_allow_mixed; }
//		void SetExtmapAllowMixed(bool extmap_allow_mixed) {
//			m_extmap_allow_mixed = extmap_allow_mixed;
//		}
//
//	protected:
//	private:
//
//		bool _register(int32 id, RTPExtensionType type, const char *uri);
//
//	private:
//		uint8   m_ids[kRtpExtensionNumberOfExtensions];
//		bool	m_extmap_allow_mixed;
//	};
//
//}
//
//#endif // _C_RTP_HEADER_EXTENSION_MAP_H_