/***********************************************************************************************
created: 		2022-08-12

author:			chensong

purpose:		csrtp_session


************************************************************************************************/

#ifndef _C_SRTP_SESSION_H_
#define _C_SRTP_SESSION_H_

#include "cnet_type.h"
#include <srtp2/srtp.h>

namespace chen {
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

	class csrtp_session
	{
	public:
		csrtp_session(EType type, ECRYPTO_SUITE cryptoSuite, uint8_t* key, size_t keyLen);
		~csrtp_session();


	public:
		static void init();
		static void destroy();
		static bool IsError(srtp_err_status_t code)
		{
			return (code != srtp_err_status_ok);
		}
		static const char* GetErrorString(srtp_err_status_t code);
	public:
		bool EncryptRtp(const uint8_t** data, size_t* len);
		bool DecryptSrtp(uint8_t* data, size_t* len);
		bool EncryptRtcp(const uint8_t** data, size_t* len);
		bool DecryptSrtcp(uint8_t* data, size_t* len);
		void RemoveStream(uint32_t ssrc)
		{
			srtp_remove_stream(m_session , uint32_t{ htonl(ssrc) });
		}
	private:
		srtp_t					m_session;
	};
}

#endif // _C_SRTP_SESSION_H_