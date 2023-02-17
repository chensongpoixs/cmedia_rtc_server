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
		, m_fingerprints("")
		/*, m_ssl_ctx_ptr(NULL)*/{}
		~cdtls_certificate();
	public:
		bool init();
		void destroy();

	public:

		bool _init_global_ssl_ctx();
		void _destroy_global_ssl_ctx();
	public:
		// dtls_cert
		X509* get_cert();
		// public key
		EVP_PKEY* get_private_key();
		// ECDSA key
		EC_KEY* get_ecdsa_key();
		// certificate fingerprint
		std::string get_fingerprint();

		SSL_CTX *  get_ssl_ctx();
	protected:

	private:
		EC_KEY *					m_eckey_ptr;
		//X509_NAME*					m_cert_name_ptr;
		X509*						m_certificate_ptr;
		EVP_PKEY *					m_private_key_ptr;
		std::string 				m_fingerprints;
		

		///////////////////////global SSL_CTX/////////////////

		SSL_CTX *					m_ssl_ctx_ptr;
	};


	extern cdtls_certificate   g_dtls_certificate;
}

#endif // _C_DTLS_CERTIFICATE_H_