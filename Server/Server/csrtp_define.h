/***********************************************************************************************
created: 		2023-01-31

author:			chensong

purpose:		csrtp


************************************************************************************************/

#ifndef _C_SRTP_DEFINE_H_
#define _C_SRTP_DEFINE_H_

#include "cnet_type.h"
#include <srtp2/srtp.h>
#include <vector>
namespace chen {
	enum ESRTPINFO
	{
		ESrtpProject = 500,  
		ESrtpUnProject,
	};

	enum   ECRYPTO_SUITE
	{
		ECRYPTO_NONE = 0,
		ECRYPTO_AES_CM_128_HMAC_SHA1_80 = 1,
		ECRYPTO_AES_CM_128_HMAC_SHA1_32,
		ECRYPTO_AEAD_AES_256_GCM,
		ECRYPTO_AEAD_AES_128_GCM
	};
	enum   EType
	{
		EINBOUND = 1,
		EOUTBOUND
	};

	struct SrtpCryptoSuiteMapEntry
	{
		ECRYPTO_SUITE cryptoSuite;
		const char* name;
	};
	// AES-HMAC: http://tools.ietf.org/html/rfc3711
	static constexpr size_t SrtpMasterKeyLength{ 16 };
	static constexpr size_t SrtpMasterSaltLength{ 14 };
	static constexpr size_t SrtpMasterLength{ SrtpMasterKeyLength + SrtpMasterSaltLength };
	// AES-GCM: http://tools.ietf.org/html/rfc7714
	static constexpr size_t SrtpAesGcm256MasterKeyLength{ 32 };
	static constexpr size_t SrtpAesGcm256MasterSaltLength{ 12 };
	static constexpr size_t SrtpAesGcm256MasterLength{ SrtpAesGcm256MasterKeyLength + SrtpAesGcm256MasterSaltLength };
	static constexpr size_t SrtpAesGcm128MasterKeyLength{ 16 };
	static constexpr size_t SrtpAesGcm128MasterSaltLength{ 12 };
	static constexpr size_t SrtpAesGcm128MasterLength{ SrtpAesGcm128MasterKeyLength + SrtpAesGcm128MasterSaltLength };
	// clang-format on

	
}


#endif // _C_SRTP_DEFINE_H_