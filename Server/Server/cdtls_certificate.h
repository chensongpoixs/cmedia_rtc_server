/***********************************************************************************************
created: 		2023-01-18

author:			chensong

purpose:		_C_DTLS_CERTIFICATE_H_


************************************************************************************************/

#ifndef _C_DTLS_CERTIFICATE_H_
#define _C_DTLS_CERTIFICATE_H_
#include "cnet_type.h"
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include "crtc_sdp.h"
#include "cmedia_desc.h"
#include "crandom.h"
#include <openssl/crypto.h>
#include <openssl/rand.h>
#include <iostream>
#include <mutex>
#include <openssl/asn1.h>
#include <openssl/bn.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>


namespace chen {
	class cdtls_certificate
	{
	public:
		cdtls_certificate()
		: m_eckey_ptr(NULL)
		//, m_cert_name_ptr(NULL)
		, m_certificate_ptr(NULL)
		, m_private_key_ptr(NULL)
		, m_fingerprints(""){}
		~cdtls_certificate();
	public:
		bool init();
		void destroy();
	protected:
	private:
		EC_KEY *					m_eckey_ptr;
		//X509_NAME*					m_cert_name_ptr;
		X509*						m_certificate_ptr;
		EVP_PKEY *					m_private_key_ptr;
		std::string 				m_fingerprints;
	};
}

#endif // _C_DTLS_CERTIFICATE_H_