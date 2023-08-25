/***********************************************************************************************
created: 		2023-01-18

author:			chensong

purpose:		_C_DTLS_CERTIFICATE_H_

输赢不重要，答案对你们有什么意义才重要。

光阴者，百代之过客也，唯有奋力奔跑，方能生风起时，是时代造英雄，英雄存在于时代。或许世人道你轻狂，可你本就年少啊。 看护好，自己的理想和激情。


我可能会遇到很多的人，听他们讲好2多的故事，我来写成故事或编成歌，用我学来的各种乐器演奏它。
然后还可能在一个国家遇到一个心仪我的姑娘，她可能会被我帅气的外表捕获，又会被我深邃的内涵吸引，在某个下雨的夜晚，她会全身淋透然后要在我狭小的住处换身上的湿衣服。
3小时候后她告诉我她其实是这个国家的公主，她愿意向父皇求婚。我不得已告诉她我是穿越而来的男主角，我始终要回到自己的世界。
然后我的身影慢慢消失，我看到她眼里的泪水，心里却没有任何痛苦，我才知道，原来我的心被丢掉了，我游历全世界的原因，就是要找回自己的本心。
于是我开始有意寻找各种各样失去心的人，我变成一块砖头，一颗树，一滴水，一朵白云，去听大家为什么会失去自己的本心。
我发现，刚出生的宝宝，本心还在，慢慢的，他们的本心就会消失，收到了各种黑暗之光的侵蚀。
从一次争论，到嫉妒和悲愤，还有委屈和痛苦，我看到一只只无形的手，把他们的本心扯碎，蒙蔽，偷走，再也回不到主人都身边。
我叫他本心猎手。他可能是和宇宙同在的级别 但是我并不害怕，我仔细回忆自己平淡的一生 寻找本心猎手的痕迹。
沿着自己的回忆，一个个的场景忽闪而过，最后发现，我的本心，在我写代码的时候，会回来。
安静，淡然，代码就是我的一切，写代码就是我本心回归的最好方式，我还没找到本心猎手，但我相信，顺着这个线索，我一定能顺藤摸瓜，把他揪出来。
************************************************************************************************/
#include "cdtls_certificate.h"
#include "crandom.h"

#include <cassert>
#include <srtp.h>

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