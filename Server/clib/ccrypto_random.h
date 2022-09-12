/***********************************************************************************************
created: 		2022-08-11

author:			chensong

purpose:		ccrypto_random
************************************************************************************************/


#ifndef _C_CRYPTO_RANDOM_H_
#define _C_CRYPTO_RANDOM_H_
//#include "cnet_type.h"
#include "csingleton.h"
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <string>
namespace chen {
	class ccrypto_random
	{
	public:
		ccrypto_random();
		~ccrypto_random();

	public:
		bool init();

		void destroy();


	public:

		uint32_t GetRandomUInt(uint32_t min, uint32_t max);
		const std::string GetRandomString(size_t len);
		uint32_t GetCRC32(const uint8_t* data, size_t size);

		const uint8_t* GetHmacSha1(const std::string& key, const uint8_t* data, size_t len);
	private:
		  uint32_t					m_seed;
		  HMAC_CTX*					m_hmacSha1Ctx;
		  uint8_t					m_hmacSha1Buffer[SHA_DIGEST_LENGTH];
		 
	};


}
#define s_crypto_random    chen::csingleton<chen::ccrypto_random>::get_instance()
#endif // _C_CRYPTO_RANDOM_H_