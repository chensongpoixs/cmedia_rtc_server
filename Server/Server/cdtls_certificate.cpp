/***********************************************************************************************
created: 		2023-01-18

author:			chensong

purpose:		_C_DTLS_CERTIFICATE_H_


************************************************************************************************/
#include "cdtls_certificate.h"
#include "crandom.h"

#include <cassert>
#include <srtp2/srtp.h>

namespace chen {


//#pragma GCC diagnostic push
//#pragma GCC diagnostic ignored "-Wdeprecated-declarations" 
//#pragma GCC diagnostic pop

	cdtls_certificate   g_dtls_certificate;
	cdtls_certificate::~cdtls_certificate()
	{
	}
	bool cdtls_certificate::init()
	{

		int32 ret = 0;

#if OPENSSL_VERSION_NUMBER < 0x10100000L // v1.1.x
		// Initialize SSL library by registering algorithms
		// The SSL_library_init() and OpenSSL_add_ssl_algorithms() functions were deprecated in OpenSSL 1.1.0 by OPENSSL_init_ssl().
		// @see https://www.openssl.org/docs/man1.1.0/man3/OpenSSL_add_ssl_algorithms.html
		// @see https://web.archive.org/web/20150806185102/http://sctp.fh-muenster.de:80/dtls/dtls_udp_echo.c
		OpenSSL_add_ssl_algorithms();
#else
		// As of version 1.1.0 OpenSSL will automatically allocate all resources that it needs so no explicit
		// initialisation is required. Similarly it will also automatically deinitialise as required.
		// @see https://www.openssl.org/docs/man1.1.0/man3/OPENSSL_init_ssl.html
		//OPENSSL_init_ssl();
#endif

		// Initialize SRTP first.
		//cassert(srtp_init() == 0); //======> [ destroy  ===> srtp_shutdown()];

		X509_NAME * cert_name_ptr = NULL;
		std::string subject = std::string("cmedia_rtc_server") + std::to_string(c_rand.rand(100000, 999999));


		// 1. 使用曲线创建关键点。
		m_eckey_ptr = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);
		if (!m_eckey_ptr)
		{
			WARNING_EX_LOG("EC_KEY_new_by_curve_name() failed");
			return false;
		}


		EC_KEY_set_asn1_flag(m_eckey_ptr, OPENSSL_EC_NAMED_CURVE);

		// NOTE: This can take some time.
		ret = EC_KEY_generate_key(m_eckey_ptr);
		if (ret == 0)
		{
			WARNING_EX_LOG("EC_KEY_generate_key() failed"); 
			return false;
		}

		// 2. 创建私钥对象。
		m_private_key_ptr = EVP_PKEY_new();

		if (!m_private_key_ptr)
		{
			WARNING_EX_LOG("EVP_PKEY_new() failed");

			return false;
		}
		// NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
		ret = EVP_PKEY_assign_EC_KEY(m_private_key_ptr, m_eckey_ptr);

		if (ret == 0)
		{
			WARNING_EX_LOG("EVP_PKEY_assign_EC_KEY() failed");
			return false;
		}


		//////////////////////////////////////////////////////////////////////////

		// The EC key now belongs to the private key, so don't clean it up separately.
		// m_eckey_ptr = nullptr;
		//////////////////////////////////////////////////////////////////////////


		// 3. 创建 X509 证书.
		m_certificate_ptr = X509_new();

		if (!m_certificate_ptr)
		{
			ERROR_EX_LOG("X509_new() failed");
			return false;
		}

		// Set version 3 (note that 0 means version 1). 
		X509_set_version(m_certificate_ptr, 2);

		// Set serial number (avoid default 0).
		ASN1_INTEGER_set( X509_get_serialNumber(m_certificate_ptr), static_cast<uint64_t>(c_rand.rand(100000, 9999999)));

		// Set valid period.
		int32 expire_day = 365;
		const int64 cert_duration = 60 * 60 * 24 * expire_day;

		X509_gmtime_adj(X509_get_notBefore(m_certificate_ptr), 0);  
		X509_gmtime_adj(X509_get_notAfter(m_certificate_ptr), cert_duration);    

		// Set the public key for the certificate using the key.
		ret = X509_set_pubkey(m_certificate_ptr, m_private_key_ptr);

		if (ret == 0)
		{
			WARNING_EX_LOG("X509_set_pubkey() failed");
			return false;
		}
		// Set certificate fields.
		cert_name_ptr = X509_get_subject_name(m_certificate_ptr);

		if (!cert_name_ptr)
		{
			WARNING_EX_LOG("X509_get_subject_name() failed");

			return false;
		}
		

		 X509_NAME_add_entry_by_txt(
		 	cert_name_ptr, "O", MBSTRING_ASC, reinterpret_cast<const uint8_t*>(subject.c_str()), -1, -1, 0);
		X509_NAME_add_entry_by_txt(
			cert_name_ptr, "CN", MBSTRING_ASC, reinterpret_cast<const uint8_t*>(subject.c_str()), -1, -1, 0);

		// It is self-signed so set the issuer name to be the same as the subject.
		ret = X509_set_issuer_name(m_certificate_ptr, cert_name_ptr);
		
		 if (ret == 0)
		{
			WARNING_EX_LOG("X509_set_issuer_name() failed");
			X509_NAME_free(cert_name_ptr);
			cert_name_ptr = NULL;
			return false;
		} 

		 
		
		// Sign the certificate with its own private key.
		ret = X509_sign(m_certificate_ptr  , m_private_key_ptr, EVP_sha1());

		if (ret == 0)
		{
			WARNING_EX_LOG("X509_sign() failed");
			X509_NAME_free(cert_name_ptr);
			cert_name_ptr = NULL;
			return false;
		}
	//	X509_NAME_free(cert_name_ptr);
		//cert_name_ptr = NULL;



		// Show DTLS fingerprint
		if (true) 
		{
			char fp[100] = { 0 };
			char *p = fp;
			unsigned char md[EVP_MAX_MD_SIZE] = {0};
			unsigned int n = 0;

			// TODO: FIXME: Unused variable.
			/*int r = */X509_digest(m_certificate_ptr, EVP_sha256(), md, &n);

			for (unsigned int i = 0; i < n; i++, ++p)
			{
				sprintf(p, "%02X", md[i]);
				p += 2;

				if (i < (n - 1))
				{
					*p = ':';
				}
				else
				{
					*p = '\0';
				}
			}

			m_fingerprints .assign(fp, strlen(fp));
			NORMAL_EX_LOG("fingerprint=%s", m_fingerprints.c_str());
		}


		
		

		return _init_global_ssl_ctx();



		//return true;
	}
	void cdtls_certificate::destroy()
	{
		
		if (m_eckey_ptr)
		{
			EC_KEY_free(m_eckey_ptr);
			m_eckey_ptr = NULL;
		}

		if (m_private_key_ptr)
		{
			EVP_PKEY_free(m_private_key_ptr);
			m_private_key_ptr = NULL;
		}
		 
		if (m_certificate_ptr)
		{
			X509_free(m_certificate_ptr);
			m_certificate_ptr = NULL;
		}
		m_fingerprints.clear();
		_destroy_global_ssl_ctx();
	}
	
	void cdtls_certificate::_destroy_global_ssl_ctx()
	{
		if (m_ssl_ctx_ptr)
		{
			SSL_CTX_free(m_ssl_ctx_ptr);
			m_ssl_ctx_ptr = NULL;
		}
	}
	X509 * cdtls_certificate::get_cert()
	{
		return m_certificate_ptr;
	}
	EVP_PKEY * cdtls_certificate::get_private_key()
	{
		return m_private_key_ptr;
	}
	EC_KEY * cdtls_certificate::get_ecdsa_key()
	{
		return m_eckey_ptr;
	}
	std::string cdtls_certificate::get_fingerprint()
	{
		return m_fingerprints;
	}
	SSL_CTX * cdtls_certificate::get_ssl_ctx()
	{
		return m_ssl_ctx_ptr;
	}
}