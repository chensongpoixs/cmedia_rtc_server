/***********************************************************************************************
created: 		2022-12-21

author:			chensong

purpose:		media_error


************************************************************************************************/

#ifndef _C_MEDIA_ERROR_H_
#define _C_MEDIA_ERROR_H_


namespace chen {
	
	enum  EMediaErrorDecode
	{
		EMediaRtcSdpInvalidSsrc = 5000, // invaliad ssrc
		EMediaRtcSdpInvalidSemantics, // semantics //
		EMediaRtcSdpInvalidH264ParamProfileLevelId, // no h264 param: profile-level-id
		EMediaRtcSdpInvalidH264ParamPacketizationMode, // no h264 param: packetization-mode
		EMediaRtcSdpInvalidH264ParamLevelAsymmetryAllowed, // no h264 param: level-asymmetry-allowed
		EMediaRtcSdpInvalidSessionAttributeFingerprintAlgo,
		EMediaRtcSdpInvalidSessionAttributeFingerprint,
		EMediaRtcSdpInvalidAttrRtpmapPayloadType,
		EMediaRtcSdpInvalidAttrRtpmap,
		EMediaRtcSdpInvalidAttrRtcpfbPayloadType,
		EMediaRtcSdpInvalidAttrFmtpPayloadType,
		EMediaRtcSdpInvalidAttrFmtpSpecificParam,
		EMediaRtcSdpInvalidAttrParseMid,
		EMediaRtcSdpInvalidAttrParseMsid,
		EMediaRtcSdpInvalidAttrParseSsrc,
		EMediaRtcSdpInvalidAttrParseSsrcAttr,
		EMediaRtcSdpInvalidAttrParseSsrcGroupSemantics,
		EMediaRtcSdpInvalidAttrParseSsrcGroupSsrcId,
		EMediaRtcSdpInvalidAttrParseExtmapId,
		EMediaRtcSdpInvalidAttrParseExtmapUri,
		EMediaRtcSdpInvalidOParseUsername,
		EMediaRtcSdpInvalidOParseSessionId,
		EMediaRtcSdpInvalidOParseSessionVersion,
		EMediaRtcSdpInvalidOParseNetType,
		EMediaRtcSdpInvalidOParseAddrType,
		EMediaRtcSdpInvalidOParseUnicastAddress,
		EMediaRtcSdpInvalidVersion,
		EMediaRtcSdpInvalidTimingStartTime,
		EMediaRtcSdpInvalidTimingEndTime,
		EMediaRtcSdpInvalidAttributeMsidSemantic,
		EMediaRtcSdpInvalidGb28181Ssrc,
		EMediaRtcSdpInvalidGb28181SsrcLabel,
		EMediaRtcSdpInvalidMediaMedia,
		EMediaRtcSdpInvalidMediaPort,
		EMediaRtcSdpInvalidMediaProto,
		EMediaRtcSdpInvalidAttrGroupPolicy,
		EMediaRtcSdpEncodeSessionInfoFailed,
		EMediaRtcSdpEncodeMediaPayloadFailed,
		EMediaRtcSdpEncodeSsrcFailed,
		EMediaRtcSdpEncodeMediaDescription,
		EMediaRtcSdpUpdateMsidFailed, 
		EMediaRtcSdpInvalidSdpLineFailed, 
		
		//////////////////////////////////////////////////////
		
		
	};
}

#endif/// _C_MEDIA_ERROR_H_

