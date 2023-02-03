/***********************************************************************************************
created: 		2023-02-01

author:			chensong

purpose:		 rtc——stun define 
************************************************************************************************/


#ifndef _C_RTC_STUN_DEFINE_H_
#define _C_RTC_STUN_DEFINE_H_
#include "cnet_type.h"

namespace chen {


	// STUN headers are 20 bytes.
	//(len >= 20) &&
		// DOC: https://tools.ietf.org/html/draft-ietf-avtcore-rfc5764-mux-fixes
	//	(data[0] < 3) &&
		// Magic cookie must match.
	// @see: https://tools.ietf.org/html/rfc5389
	// The magic cookie field MUST contain the fixed value 0x2112A442 in network byte order
	// 固定的 魔法数
	const uint8   kStunMagicCookie[] = { 0x21, 0x12, 0xA4, 0x42 } /*0x2112A442*/;
	const uint32  KStunMagicCookie = 0x2112A442;


	enum EStunMessageType
	{
		// see @ https://tools.ietf.org/html/rfc3489#section-11.1	
		EBindingRequest = 0x0001,
		EBindingResponse = 0x0101,
		EBindingErrorResponse = 0x0111,
		ESharedSecretRequest = 0x0002,
		ESharedSecretResponse = 0x0102,
		ESharedSecretErrorResponse = 0x0112,
	};


	enum EStunMessageAttribute
	{
		// see @ https://tools.ietf.org/html/rfc3489#section-11.2
		EMappedAddress		= 0x0001,
		EResponseAddress		= 0x0002,
		EChangeRequest		= 0x0003,
		ESourceAddress		= 0x0004,
		EChangedAddress		= 0x0005,
		EUsername			= 0x0006,
		EPassword			= 0x0007,
		EMessageIntegrity	= 0x0008,
		EErrorCode			= 0x0009,
		EUnknownAttributes	= 0x000A,
		EReflectedFrom		= 0x000B,

		// see @ https://tools.ietf.org/html/rfc5389#section-18.2
		ERealm				= 0x0014,
		ENonce				= 0x0015,
		EXorMappedAddress	= 0x0020,
		ESoftware			= 0x8022,
		EAlternateServer		= 0x8023,
		EFingerprint			= 0x8028,

		EPriority				= 0x0024,
		EUseCandidate			= 0x0025,
		EIceControlled			= 0x8029,
		EIceControlling			= 0x802A,
	};
}

#endif // _C_RTC_STUN_DEFINE_H_