/***********************************************************************************************
created: 		2022-05-22

author:			chensong

purpose:		TLSv1.3 协议的学习

原因是WebRTC中有DTSTransport -->
************************************************************************************************/

#include "dtlsv1.x.h"




namespace chen {
	
	///////////////////////////////////////////////////////////////////////////////////////
	enum class CryptoSuite
	{
		NONE = 0,
		AES_CM_128_HMAC_SHA1_80 = 1,
		AES_CM_128_HMAC_SHA1_32,
		AEAD_AES_256_GCM,
		AEAD_AES_128_GCM
	};
	struct SrtpCryptoSuiteMapEntry
	{
		CryptoSuite cryptoSuite;
		const char* name;
	};


	

	////////////////////////////////////////////////////////////////
	std::vector<SrtpCryptoSuiteMapEntry>  srtpCryptoSuites =
	{
		{ CryptoSuite::AEAD_AES_256_GCM, "SRTP_AEAD_AES_256_GCM" },
		{ CryptoSuite::AEAD_AES_128_GCM, "SRTP_AEAD_AES_128_GCM" },
		{ CryptoSuite::AES_CM_128_HMAC_SHA1_80, "SRTP_AES128_CM_SHA1_80" },
		{ CryptoSuite::AES_CM_128_HMAC_SHA1_32, "SRTP_AES128_CM_SHA1_32" }
	};



	std::map<std::string, FingerprintAlgorithm> string2FingerprintAlgorithm =
	{
		{ "sha-1",  FingerprintAlgorithm::SHA1   },
		{ "sha-224",FingerprintAlgorithm::SHA224 },
		{ "sha-256",FingerprintAlgorithm::SHA256 },
		{ "sha-384",FingerprintAlgorithm::SHA384 },
		{ "sha-512",FingerprintAlgorithm::SHA512 }
	};

	///////////////////////////////////////////////////
	



	const char * dtlsCertificateFile = "./certs/fullchain.pem";
	const char * dtlsPrivateKeyFile = "./certs/privkey.pem";


	// clang-format off
	static  int DtlsMtu{ 1350 };
	static constexpr int SslReadBufferSize{ 65536 };
	/* Class variables. */

	thread_local X509*  certificate{ nullptr };
	thread_local EVP_PKEY*  privateKey{ nullptr };
	thread_local SSL_CTX*  sslCtx{ nullptr };
	thread_local uint8_t  sslReadBuffer[SslReadBufferSize];
	thread_local std::vector<Fingerprint>  localFingerprints;



	void dtlsv1x::GenerateCertificateAndPrivateKey()
	{
		 

		int ret{ 0 };
		EC_KEY* ecKey{ nullptr };
		X509_NAME* certName{ nullptr };
		std::string subject = std::string("chensong") + std::to_string(999999);

		// 1. 使用曲线创建关键点。
		ecKey = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);

		if (!ecKey)
		{
			ERROR_EX_LOG("EC_KEY_new_by_curve_name() failed");

			goto error;
		}

		EC_KEY_set_asn1_flag(ecKey, OPENSSL_EC_NAMED_CURVE);

		// NOTE: This can take some time.
		ret = EC_KEY_generate_key(ecKey);

		if (ret == 0)
		{
			ERROR_EX_LOG("EC_KEY_generate_key() failed");

			goto error;
		}

		// 2. 创建私钥对象。
		privateKey = EVP_PKEY_new();

		if (!privateKey)
		{
			ERROR_EX_LOG("EVP_PKEY_new() failed");

			goto error;
		}

		// NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
		ret = EVP_PKEY_assign_EC_KEY( privateKey, ecKey);

		if (ret == 0)
		{
			ERROR_EX_LOG("EVP_PKEY_assign_EC_KEY() failed");

			goto error;
		}

		// The EC key now belongs to the private key, so don't clean it up separately.
		ecKey = nullptr;

		// 3. 创建 X509 证书.
		 certificate = X509_new();

		if (!  certificate)
		{
			ERROR_EX_LOG("X509_new() failed");

			goto error;
		}

		// Set version 3 (note that 0 means version 1). 
		X509_set_version(  certificate, 2);

		// Set serial number (avoid default 0).
		ASN1_INTEGER_set(
			X509_get_serialNumber(  certificate),
			static_cast<uint64_t>(9999999));

		// Set valid period.
		X509_gmtime_adj(X509_get_notBefore(  certificate), -315360000); // -10 years.
		X509_gmtime_adj(X509_get_notAfter( certificate), 315360000);   // 10 years.

		// Set the public key for the certificate using the key.
		ret = X509_set_pubkey( certificate,  privateKey);

		if (ret == 0)
		{
			ERROR_EX_LOG("X509_set_pubkey() failed");

			goto error;
		}

		// Set certificate fields.
		certName = X509_get_subject_name(  certificate);

		if (!certName)
		{
			ERROR_EX_LOG("X509_get_subject_name() failed");

			goto error;
		}

		X509_NAME_add_entry_by_txt(
			certName, "O", MBSTRING_ASC, reinterpret_cast<const uint8_t*>(subject.c_str()), -1, -1, 0);
		X509_NAME_add_entry_by_txt(
			certName, "CN", MBSTRING_ASC, reinterpret_cast<const uint8_t*>(subject.c_str()), -1, -1, 0);

		// It is self-signed so set the issuer name to be the same as the subject.
		ret = X509_set_issuer_name(  certificate, certName);

		if (ret == 0)
		{
			ERROR_EX_LOG("X509_set_issuer_name() failed");

			goto error;
		}

		// Sign the certificate with its own private key.
		ret = X509_sign( certificate,  privateKey, EVP_sha1());

		if (ret == 0)
		{
			ERROR_EX_LOG("X509_sign() failed");

			goto error;
		}

		return;

	error:

		if (ecKey)
		{
			EC_KEY_free(ecKey);
		}

		if (privateKey)
		{
			EVP_PKEY_free(privateKey); // NOTE: This also frees the EC key.
		}

		if (certificate)
		{
			X509_free(certificate);
		}

		ERROR_EX_LOG("DTLS certificate and private key generation failed");
	}

	// 读取公钥和私钥
	void dtlsv1x::ReadCertificateAndPrivateKeyFromFiles()
	{
		FILE* file{ nullptr };
		DEBUG_EX_LOG("");
		file = fopen(dtlsCertificateFile, "r");

		if (!file)
		{
			ERROR_EX_LOG("error reading DTLS certificate file: %s", std::strerror(errno));

			return;
		}
		DEBUG_EX_LOG("");
		certificate = PEM_read_X509(file, nullptr, nullptr, nullptr);

		if (!certificate)
		{
			ERROR_EX_LOG("PEM_read_X509() failed");

			return;
		}
		DEBUG_EX_LOG("");
		fclose(file);

		file = fopen(dtlsPrivateKeyFile, "r");

		if (!file)
		{
			ERROR_EX_LOG("error reading DTLS private key file: %s", std::strerror(errno));

			return;
		}

		privateKey = PEM_read_PrivateKey(file, nullptr, nullptr, nullptr);

		if (!privateKey)
		{
			ERROR_EX_LOG("PEM_read_PrivateKey() failed");

			return;
		}

		fclose(file);
	}

	//DTLS状态回调
	inline static void onSslInfo(const SSL* ssl, int where, int ret)
	{
		//DEBUG_EX_LOG("[where = %d][ret = %d]", where, ret);
		static_cast<dtlsv1x*>(SSL_get_ex_data(ssl, 0))->OnSslInfo(where, ret);
	}

	/* Static methods for OpenSSL callbacks. */

	inline static int onSslCertificateVerify(int /*preverifyOk*/, X509_STORE_CTX* /*ctx*/)
	{

		//DEBUG_EX_LOG("OpenSSL callbacks");
		// Always valid since DTLS certificates are self-signed.
		return 1;
	}

	void  dtlsv1x::CreateSslCtx()
	{


		std::string dtlsSrtpCryptoSuites;
		int ret;

		/* Set the global DTLS context. */

		// Both DTLS 1.0 and 1.2 (requires OpenSSL >= 1.1.0).
		sslCtx = SSL_CTX_new(DTLS_method());

		if (!sslCtx)
		{
			ERROR_EX_LOG("SSL_CTX_new() failed");

			goto error;;
		}

		ret = SSL_CTX_use_certificate(sslCtx, certificate);

		if (ret == 0)
		{
			ERROR_EX_LOG("SSL_CTX_use_certificate() failed");

			goto error;;
		}

		ret = SSL_CTX_use_PrivateKey(sslCtx, privateKey);

		if (ret == 0)
		{
			ERROR_EX_LOG("SSL_CTX_use_PrivateKey() failed");

			goto error;;
		}

		ret = SSL_CTX_check_private_key(sslCtx);

		if (ret == 0)
		{
			ERROR_EX_LOG("SSL_CTX_check_private_key() failed");

			goto error;;
		}

		// Set options.
		SSL_CTX_set_options(
			sslCtx,
			SSL_OP_CIPHER_SERVER_PREFERENCE | SSL_OP_NO_TICKET | SSL_OP_SINGLE_ECDH_USE |
			SSL_OP_NO_QUERY_MTU);

		// Don't use sessions cache.
		SSL_CTX_set_session_cache_mode(sslCtx, SSL_SESS_CACHE_OFF);

		// Read always as much into the buffer as possible.
		// NOTE: This is the default for DTLS, but a bug in non latest OpenSSL
		// versions makes this call required.
		SSL_CTX_set_read_ahead(sslCtx, 1);

		SSL_CTX_set_verify_depth(sslCtx, 4);

		// Require certificate from peer.
		SSL_CTX_set_verify(
			sslCtx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, onSslCertificateVerify);

		// Set SSL info callback.
		SSL_CTX_set_info_callback(sslCtx, onSslInfo);

		// Set ciphers.
		ret = SSL_CTX_set_cipher_list(
			sslCtx, "DEFAULT:!NULL:!aNULL:!SHA256:!SHA384:!aECDH:!AESGCM+AES256:!aPSK");

		if (ret == 0)
		{
			ERROR_EX_LOG("SSL_CTX_set_cipher_list() failed");

			goto error;;
		}

		// Enable ECDH ciphers.
		// DOC: http://en.wikibooks.org/wiki/OpenSSL/Diffie-Hellman_parameters
		// NOTE: https://code.google.com/p/chromium/issues/detail?id=406458
		// NOTE: https://bugs.ruby-lang.org/issues/12324

		// For OpenSSL >= 1.0.2.
		SSL_CTX_set_ecdh_auto( sslCtx, 1);

		// Set the "use_srtp" DTLS extension.
		for (auto it = srtpCryptoSuites.begin(); it != srtpCryptoSuites.end(); ++it)
		{
			if (it != srtpCryptoSuites.begin())
			{
				dtlsSrtpCryptoSuites += ":";
			}

			SrtpCryptoSuiteMapEntry* cryptoSuiteEntry = std::addressof(*it);
			dtlsSrtpCryptoSuites += cryptoSuiteEntry->name;
		}

		DEBUG_EX_LOG("setting SRTP cryptoSuites for DTLS: %s", dtlsSrtpCryptoSuites.c_str());

		// NOTE: This function returns 0 on success.
		ret = SSL_CTX_set_tlsext_use_srtp(sslCtx, dtlsSrtpCryptoSuites.c_str());

		if (ret != 0)
		{
			ERROR_EX_LOG(
				"SSL_CTX_set_tlsext_use_srtp() failed when entering '%s'", dtlsSrtpCryptoSuites.c_str());
			ERROR_EX_LOG("SSL_CTX_set_tlsext_use_srtp() failed");

			goto error;
		}

		return;

	error:

		if (sslCtx)
		{
			SSL_CTX_free(sslCtx);
			sslCtx = nullptr;
		}

		ERROR_EX_LOG("SSL context creation failed");
	}

	static FingerprintAlgorithm GetFingerprintAlgorithm(const std::string& fingerprint)
	{
		auto it = string2FingerprintAlgorithm.find(fingerprint);

		if (it != string2FingerprintAlgorithm.end())
		{
			return it->second;
		}
		 
		return  FingerprintAlgorithm::NONE;
		 
	}
	void dtlsv1x::GenerateFingerprints()
	{


		for (auto& kv : string2FingerprintAlgorithm)
		{
			const std::string& algorithmString = kv.first;
			FingerprintAlgorithm algorithm = kv.second;
			uint8_t binaryFingerprint[EVP_MAX_MD_SIZE];
			unsigned int size{ 0 };
			char hexFingerprint[(EVP_MAX_MD_SIZE * 3) + 1];
			const EVP_MD* hashFunction = NULL;;
			int ret;

			switch (algorithm)
			{
				case FingerprintAlgorithm::SHA1:
				{
					hashFunction = EVP_sha1();
					break;
				}

				case FingerprintAlgorithm::SHA224:
				{
					hashFunction = EVP_sha224();
					break;
				}

				case FingerprintAlgorithm::SHA256:
				{
					hashFunction = EVP_sha256();
					break;
				}

				case FingerprintAlgorithm::SHA384:
				{
					hashFunction = EVP_sha384();
					break;
				}

				case FingerprintAlgorithm::SHA512:
				{
					hashFunction = EVP_sha512();
					break;
				}

				default:
				{
					ERROR_EX_LOG("unknown algorithm");
				}
			}

			ret = X509_digest(certificate, hashFunction, binaryFingerprint, &size);

			//(void *)hashFunction;
			if (ret == 0)
			{
				ERROR_EX_LOG("X509_digest() failed");
				ERROR_EX_LOG("Fingerprints generation failed");
			}

			// Convert to hexadecimal format in uppercase with colons.
			for (unsigned int i{ 0 }; i < size; ++i)
			{
				std::sprintf(hexFingerprint + (i * 3), "%.2X:", binaryFingerprint[i]);
			}
			hexFingerprint[(size * 3) - 1] = '\0';

			DEBUG_EX_LOG("%-7s fingerprint: %s", algorithmString.c_str(), hexFingerprint);

			// Store it in the vector.
			Fingerprint fingerprint;

			fingerprint.algorithm = GetFingerprintAlgorithm(algorithmString);
			fingerprint.value = hexFingerprint;

			localFingerprints.push_back(fingerprint);
		}
	}

	void dtlsv1x::Run(Role localRole)
	{
		Role previousLocalRole = this->localRole;

		if (localRole == previousLocalRole)
		{
			ERROR_EX_LOG("[server_name = %s]same local DTLS role provided, doing nothing", m_server_name.c_str());

			return;
		}

		// If the previous local DTLS role was 'client' or 'server' do reset.
		if (previousLocalRole == Role::CLIENT || previousLocalRole == Role::SERVER)
		{
			DEBUG_EX_LOG("[server_name = %s]resetting DTLS due to local role change", m_server_name.c_str());

			Reset();
		}

		// Update local role.
		this->localRole = localRole;

		// Set state and notify the listener.
		this->state = DtlsState::CONNECTING;
		 

		switch (this->localRole)
		{
			case Role::CLIENT:
			{
				DEBUG_EX_LOG("running [role:client]");

				///  ????????????????????????????? dtls ???? 交换协商的流程

				SSL_set_connect_state(this->ssl);
				SSL_do_handshake(this->ssl);
				SendPendingOutgoingDtlsData();


				break;
			}

			case Role::SERVER:
			{
				DEBUG_EX_LOG("running [role:server]");

				SSL_set_accept_state(this->ssl);
				SSL_do_handshake(this->ssl);

				break;
			}

			default:
			{
				ERROR_EX_LOG("[server_name = %s]invalid local DTLS role", m_server_name.c_str());
			}

		}
	}

	void dtlsv1x::ProcessDtlsData(const uint8_t * data, size_t len)
	{
		DEBUG_EX_LOG("[server_name = %s] ", m_server_name.c_str());
		dtsl_data temp_data;
		temp_data.data = new uint8_t[len];
		memcpy(temp_data.data, data, len);
		temp_data.size = len;
		{
			std::lock_guard<std::mutex> locak(m_mutex);

			m_quene.push_back(temp_data);
		}
	}


	void dtlsv1x::Reset()
	{
		int ret;

		if (!IsRunning())
		{
			DEBUG_EX_LOG("");
			return;
		}

		WARNING_EX_LOG( "[server_name = %s]resetting DTLS transport", m_server_name.c_str());

		// Stop the DTLS timer.
		//this->timer->Stop();

		// We need to reset the SSL instance so we need to "shutdown" it, but we
		// don't want to send a Close Alert to the peer, so just don't call
		// SendPendingOutgoingDTLSData().
		SSL_shutdown(this->ssl);

		this->localRole = Role::NONE;
		this->state = DtlsState::NEW;
		this->handshakeDone = false;
		this->handshakeDoneNow = false;

		// Reset SSL status.
		// NOTE: For this to properly work, SSL_shutdown() must be called before.
		// NOTE: This may fail if not enough DTLS handshake data has been received,
		// but we don't care so just clear the error queue.
		ret = SSL_clear(this->ssl);

		if (ret == 0)
		{
			ERR_clear_error();
		}
	}

	bool dtlsv1x::CheckStatus(int returnCode)
	{
		int err;
		bool wasHandshakeDone = this->handshakeDone;

		err = SSL_get_error(this->ssl, returnCode);

		switch (err)
		{
		case SSL_ERROR_NONE:
			break;

		case SSL_ERROR_SSL:
			ERROR_EX_LOG("[server_name = %s]SSL status: SSL_ERROR_SSL", m_server_name.c_str());
			break;

		case SSL_ERROR_WANT_READ:
			break;

		case SSL_ERROR_WANT_WRITE:
			WARNING_EX_LOG(  "[server_name = %s]SSL status: SSL_ERROR_WANT_WRITE", m_server_name.c_str());
			break;

		case SSL_ERROR_WANT_X509_LOOKUP:
			DEBUG_EX_LOG(  "[server_name = %s]SSL status: SSL_ERROR_WANT_X509_LOOKUP", m_server_name.c_str());
			break;

		case SSL_ERROR_SYSCALL:
			ERROR_EX_LOG("[server_name = %s]SSL status: SSL_ERROR_SYSCALL", m_server_name.c_str());
			break;

		case SSL_ERROR_ZERO_RETURN:
			break;

		case SSL_ERROR_WANT_CONNECT:
			WARNING_EX_LOG( "[server_name = %s]SSL status: SSL_ERROR_WANT_CONNECT", m_server_name.c_str());
			break;

		case SSL_ERROR_WANT_ACCEPT:
			WARNING_EX_LOG( "[server_name = %s]SSL status: SSL_ERROR_WANT_ACCEPT", m_server_name.c_str());
			break;

		default:
			WARNING_EX_LOG(  "[server_name = %s]SSL status: unknown error", m_server_name.c_str());
		}
		DEBUG_EX_LOG("[server_name = %s]", m_server_name.c_str());
		// Check if the handshake (or re-handshake) has been done right now.
		if (this->handshakeDoneNow)
		{
			this->handshakeDoneNow = false;
			this->handshakeDone = true;

			// Stop the timer.
			//this->timer->Stop();
			DEBUG_EX_LOG("[server_name = %s][wasHandshakeDone = %u][remoteFingerprint.algorithm = %u]", m_server_name.c_str(), wasHandshakeDone, remoteFingerprint.algorithm);
			// Process the handshake just once (ignore if DTLS renegotiation).
			if (!wasHandshakeDone && this->remoteFingerprint.algorithm != FingerprintAlgorithm::NONE)
			{
				DEBUG_EX_LOG("[server_name = %s]", m_server_name.c_str());
				return ProcessHandshake();
			}

			return true;
		}
		// Check if the peer sent close alert or a fatal error happened.
		else if (((SSL_get_shutdown(this->ssl) & SSL_RECEIVED_SHUTDOWN) != 0) || err == SSL_ERROR_SSL || err == SSL_ERROR_SYSCALL)
		{
			if (this->state == DtlsState::CONNECTED)
			{
				DEBUG_EX_LOG(  "[server_name = %s]disconnected", m_server_name.c_str());

				Reset();

				// Set state and notify the listener.
				this->state = DtlsState::CLOSED; 
			}
			else
			{
				WARNING_EX_LOG(  "[server_name = %s]connection failed", m_server_name.c_str());

				Reset();
 
				this->state = DtlsState::FAILED;
				 
			}

			return false;
		}

		DEBUG_EX_LOG("[server_name = %s]", m_server_name.c_str());
		 return true;
		 
	}

	bool dtlsv1x::ProcessHandshake()
	{
		DEBUG_EX_LOG("[server_name = %s]", m_server_name.c_str());
		// Validate the remote fingerprint.
		if (!CheckRemoteFingerprint())
		{
			Reset();

			// Set state and notify the listener.
			this->state = DtlsState::FAILED;
			 

			return false;
		}
		DEBUG_EX_LOG("[server_name = %s]", m_server_name.c_str());
		 
		return true;
	 
		 
	}

	bool dtlsv1x::CheckRemoteFingerprint()
	{
		X509* certificate;
		uint8_t binaryFingerprint[EVP_MAX_MD_SIZE];
		unsigned int size{ 0 };
		char hexFingerprint[(EVP_MAX_MD_SIZE * 3) + 1];
		const EVP_MD* hashFunction = NULL;
		int ret;
		DEBUG_EX_LOG("[server_name = %s]", m_server_name.c_str());
		certificate = SSL_get_peer_certificate(this->ssl);

		if (!certificate)
		{
			WARNING_EX_LOG(  "[server_name = %s]no certificate was provided by the peer", m_server_name.c_str());

			return false;
		}
		DEBUG_EX_LOG("[server_name = %s]", m_server_name.c_str());
		switch (this->remoteFingerprint.algorithm)
		{
			case FingerprintAlgorithm::SHA1:
			{
				hashFunction = EVP_sha1();
				break;

			}
			case FingerprintAlgorithm::SHA224:
			{
				hashFunction = EVP_sha224();
				break;
			}

			case FingerprintAlgorithm::SHA256:
			{
				hashFunction = EVP_sha256();
				break;
			}

			case FingerprintAlgorithm::SHA384:
			{
				hashFunction = EVP_sha384();
				break;
			}

			case FingerprintAlgorithm::SHA512:
			{
				hashFunction = EVP_sha512();
				break;
			}

			default:
			{
				ERROR_EX_LOG("[server_name = %s]unknown algorithm", m_server_name.c_str());
			}
		}

		// Compare the remote fingerprint with the value given via signaling.
		ret = X509_digest(certificate, hashFunction, binaryFingerprint, &size);
		DEBUG_EX_LOG("[server_name = %s]", m_server_name.c_str());
		//(void *)hashFunction;
		if (ret == 0)
		{
			ERROR_EX_LOG("[server_name = %s]X509_digest() failed", m_server_name.c_str());

			X509_free(certificate);

			return false;
		}

		// Convert to hexadecimal format in uppercase with colons.
		for (unsigned int i{ 0 }; i < size; ++i)
		{
			std::sprintf(hexFingerprint + (i * 3), "%.2X:", binaryFingerprint[i]);
		}
		hexFingerprint[(size * 3) - 1] = '\0';

		if (this->remoteFingerprint.value != hexFingerprint)
		{
			WARNING_EX_LOG( 
				"[server_name = %s]fingerprint in the remote certificate (%s) does not match the announced one (%s)", m_server_name.c_str(),
				hexFingerprint,
				this->remoteFingerprint.value.c_str());

			X509_free(certificate);

			return false;
		}

		DEBUG_EX_LOG(  "[server_name = %s]valid remote fingerprint", m_server_name.c_str());

		// Get the remote certificate in PEM format.

		BIO* bio = BIO_new(BIO_s_mem());

		// Ensure the underlying BUF_MEM structure is also freed.
		// NOTE: Avoid stupid "warning: value computed is not used [-Wunused-value]" since
		// BIO_set_close() always returns 1.
		(void)BIO_set_close(bio, BIO_CLOSE);

		ret = PEM_write_bio_X509(bio, certificate);

		if (ret != 1)
		{
			ERROR_EX_LOG("[server_name = %s]PEM_write_bio_X509() failed", m_server_name.c_str());

			X509_free(certificate);
			BIO_free(bio);

			return false;
		}

		BUF_MEM* mem;

		BIO_get_mem_ptr(bio, &mem); // NOLINT[cppcoreguidelines-pro-type-cstyle-cast]

		if (!mem || !mem->data || mem->length == 0u)
		{
			ERROR_EX_LOG("[server_name = %s]BIO_get_mem_ptr() failed", m_server_name.c_str());

			X509_free(certificate);
			BIO_free(bio);

			return false;
		}
		DEBUG_EX_LOG("[server_name = %s]", m_server_name.c_str());
		this->remoteCert = std::string(mem->data, mem->length);

		X509_free(certificate);
		BIO_free(bio);

		return true;
	}

	void dtlsv1x::show() const
	{
		std::string state{ "new" };
		std::string role{ "none " };
		switch (this->state)
		{
		case DtlsState::CONNECTING:
			state = "connecting";
			break;
		case DtlsState::CONNECTED:
			state = "connected";
			break;
		case DtlsState::FAILED:
			state = "failed";
			break;
		case DtlsState::CLOSED:
			state = "closed";
			break;
		default:;
		}

		switch (this->localRole)
		{
		case Role::AUTO:
			role = "auto";
			break;
		case Role::SERVER:
			role = "server";
			break;
		case Role::CLIENT:
			role = "client";
			break;
		default:;
		}

		DEBUG_EX_LOG("[server_name = %s]<dtlsv1x>", m_server_name.c_str());
		DEBUG_EX_LOG("  state           : %s", state.c_str());
		DEBUG_EX_LOG("  role            : %s", role.c_str());
		DEBUG_EX_LOG("  handshake done: : %s", this->handshakeDone ? "yes" : "no");
		DEBUG_EX_LOG("</dtlsv1x>");
	}

		void dtlsv1x::SetListener(Listener * ptr)
		{
			this->listener = ptr;
		}

		void dtlsv1x::startup()
		{
			m_thread = std::thread(&dtlsv1x::_work_thread, this);
		}

		void dtlsv1x::OnSslInfo(int where, int ret)
		{
			int w = where & -SSL_ST_MASK;
			const char* role;

			if ((w & SSL_ST_CONNECT) != 0)
				role = "client";
			else if ((w & SSL_ST_ACCEPT) != 0)
				role = "server";
			else
				role = "undefined";

			if ((where & SSL_CB_LOOP) != 0)
			{
				DEBUG_EX_LOG(  "[server_name = %s][role:%s, action:'%s']", m_server_name.c_str(), role, SSL_state_string_long(this->ssl));
			}
			else if ((where & SSL_CB_ALERT) != 0)
			{
				const char* alertType;

				switch (*SSL_alert_type_string(ret))
				{
				case 'W':
					alertType = "warning";
					break;

				case 'F':
					alertType = "fatal";
					break;

				default:
					alertType = "undefined";
				}

				if ((where & SSL_CB_READ) != 0)
				{
					WARNING_EX_LOG( "[server_name = %s]received DTLS %s alert: %s", m_server_name.c_str(), alertType, SSL_alert_desc_string_long(ret));
				}
				else if ((where & SSL_CB_WRITE) != 0)
				{
					DEBUG_EX_LOG(  "[server_name = %s]sending DTLS %s alert: %s", m_server_name.c_str(), alertType, SSL_alert_desc_string_long(ret));
				}
				else
				{
					DEBUG_EX_LOG(  "[server_name = %s]DTLS %s alert: %s", m_server_name.c_str(), alertType, SSL_alert_desc_string_long(ret));
				}
			}
			else if ((where & SSL_CB_EXIT) != 0)
			{
				if (ret == 0)
				{
					DEBUG_EX_LOG("[server_name = %s][role:%s, failed:'%s']", m_server_name.c_str(), role, SSL_state_string_long(this->ssl));
				}
				else if (ret < 0)
				{
					DEBUG_EX_LOG("[server_name = %s]role: %s, waiting:'%s']", m_server_name.c_str(), role, SSL_state_string_long(this->ssl));
				}
			}
			else if ((where & SSL_CB_HANDSHAKE_START) != 0)
			{
				DEBUG_EX_LOG(  "[server_name = %s]DTLS handshake start", m_server_name.c_str());
			}
			else if ((where & SSL_CB_HANDSHAKE_DONE) != 0)
			{
				DEBUG_EX_LOG( "[server_name = %s]DTLS handshake done", m_server_name.c_str());

				this->handshakeDoneNow = true;
			}

			// NOTE: checking SSL_get_shutdown(this->ssl) & SSL_RECEIVED_SHUTDOWN here upon
			// receipt of a close alert does not work (the flag is set after this callback).
		}

		

	inline static unsigned int onSslDtlsTimer(SSL* /*ssl*/, unsigned int timerUs)
	{
		if (timerUs == 0)
		{
			return 100000;
		}
		else if (timerUs >= 4000000)
		{
			return 4000000;
		}
		//else
		return 2 * timerUs;
	}

	dtlsv1x::dtlsv1x(const char * server_name/*Listener *listener*/)
		: listener(NULL)
		, ssl(NULL)
		, sslBioFromNetwork(NULL)
		, sslBioToNetwork(NULL)
		, state(DtlsState::NEW)
		, localRole(Role::NONE)
		, remoteFingerprint()
		, handshakeDone(false)
		, handshakeDoneNow(false)
		, remoteCert()
		, m_server_name(server_name)
	{
		/* Set SSL. */

		this->ssl = SSL_new( sslCtx);

		if (!this->ssl)
		{
			ERROR_EX_LOG("[server_name = %s]SSL_new() failed", m_server_name.c_str());

			goto error;
		}

		// Set this as custom data.
		SSL_set_ex_data(this->ssl, 0, static_cast<void*>(this));

		this->sslBioFromNetwork = BIO_new(BIO_s_mem());

		if (!this->sslBioFromNetwork)
		{
			ERROR_EX_LOG("[server_name = %s]BIO_new() failed", m_server_name.c_str());

			SSL_free(this->ssl);

			goto error;
		}

		this->sslBioToNetwork = BIO_new(BIO_s_mem());

		if (!this->sslBioToNetwork)
		{
			ERROR_EX_LOG("[server_name = %s]BIO_new() failed", m_server_name.c_str());

			BIO_free(this->sslBioFromNetwork);
			SSL_free(this->ssl);

			goto error;
		}

		SSL_set_bio(this->ssl, this->sslBioFromNetwork, this->sslBioToNetwork);

		// Set the MTU so that we don't send packets that are too large with no fragmentation.
		SSL_set_mtu(this->ssl, DtlsMtu);
		DTLS_set_link_mtu(this->ssl, DtlsMtu);

		// Set callback handler for setting DTLS timer interval.
		//DTLS_set_timer_cb(this->ssl, onSslDtlsTimer);

		// Set the DTLS timer.
		//this->timer = new Timer(this);

		return;

	error:

		// NOTE: At this point SSL_set_bio() was not called so we must free BIOs as
		// well.
		if (this->sslBioFromNetwork)
		{
			BIO_free(this->sslBioFromNetwork);
		}

		if (this->sslBioToNetwork)
		{
			BIO_free(this->sslBioToNetwork);
		}

		if (this->ssl)
		{
			SSL_free(this->ssl);
		}

		// NOTE: If this is not catched by the caller the program will abort, but
		// this should never happen.
		ERROR_EX_LOG("[server_name = %s]DtlsTransport instance creation failed", m_server_name.c_str());
	}
	bool dtlsv1x::IsRunning() const
	{
		switch (this->state)
		{
			case DtlsState::NEW:
			{
				return false;
			}
			case DtlsState::CONNECTING:
			case DtlsState::CONNECTED:
			{
				return true;
			}
			case DtlsState::FAILED:
			case DtlsState::CLOSED:
			{
				return false;
			}
		}

		// Make GCC 4.9 happy.
		return false;
	}

	void dtlsv1x::SendPendingOutgoingDtlsData()
	{
		if (BIO_eof(this->sslBioToNetwork))
		{
			DEBUG_EX_LOG("[server_name = %s]", m_server_name.c_str());
			return;
		}

		int64_t read;
		char* data{ nullptr };

		read = BIO_get_mem_data(this->sslBioToNetwork, &data); // NOLINT

		if (read <= 0)
		{
			WARNING_EX_LOG("[server_name = %s]", m_server_name.c_str());
			return;
		}

		DEBUG_EX_LOG("[server_name = %s]%u bytes of DTLS data ready to sent to the peer", m_server_name.c_str(), read);

		// Notify the listener.
		this->listener->ProcessDtlsData(  reinterpret_cast<uint8_t*>(data), static_cast<size_t>(read));

		// Clear the BIO buffer.
		// NOTE: the (void) avoids the -Wunused-value warning.
		(void)BIO_reset(this->sslBioToNetwork);
	}

	void dtlsv1x::_work_thread()
	{
		while (true)
		{
			if (m_quene.empty())
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
			else
			{
				while (!m_quene.empty())
				{
					DEBUG_EX_LOG("quene size = %u", m_quene.size());
					dtsl_data temp_data;
					{
						std::lock_guard<std::mutex> locak(m_mutex);
						temp_data = m_quene.front();
						m_quene.pop_front();
					}
					std::this_thread::sleep_for(std::chrono::seconds(1));
					_process_data(temp_data.data, temp_data.size);
					delete[] temp_data.data;
					temp_data.data = NULL;
					temp_data.size = 0;
				}
			}
		}
	}

	void dtlsv1x::_process_data(const uint8_t * data, size_t len)
	{
		int written;
		int read;

		if (!IsRunning())
		{
			ERROR_EX_LOG("[server_name = %s]cannot process data while not running", m_server_name.c_str());

			return;
		}
		DEBUG_EX_LOG("[server_name = %s]", m_server_name.c_str());
		// Write the received DTLS data into the sslBioFromNetwork.
		written =
			BIO_write(this->sslBioFromNetwork, static_cast<const void*>(data ), static_cast<int>(len));
		DEBUG_EX_LOG("[server_name = %s]", m_server_name.c_str());
		if (written != static_cast<int>(len))
		{
			WARNING_EX_LOG(
				"[server_name = %s]OpenSSL BIO_write() wrote less (%zu bytes) than given data (%zu bytes)", m_server_name.c_str(),
				static_cast<size_t>(written),
				len);
		}
		DEBUG_EX_LOG("[server_name = %s]", m_server_name.c_str());
		// Must call SSL_read() to process received DTLS data.
		read = SSL_read(this->ssl, static_cast<void*>(sslReadBuffer), SslReadBufferSize);
		DEBUG_EX_LOG("[server_name = %s]", m_server_name.c_str());
		// Send data if it's ready.
		SendPendingOutgoingDtlsData();
		DEBUG_EX_LOG("[server_name = %s]", m_server_name.c_str());
		// Check SSL status and return if it is bad/closed.
		if (!CheckStatus(read))
		{
			return;
		}
		DEBUG_EX_LOG("[server_name = %s]", m_server_name.c_str());
		// Set/update the DTLS timeout.
	/*	if (!SetTimeout())
			return;*/

			// Application data received. Notify to the listener.
		if (read > 0)
		{
			// It is allowed to receive DTLS data even before validating remote fingerprint.
			if (!this->handshakeDone)
			{
				WARNING_EX_LOG("[server_name = %s]ignoring application data received while DTLS handshake not done", m_server_name.c_str());

				return;
			}
			DEBUG_EX_LOG("[server_name = %s]SctpAssociation", m_server_name.c_str());
			// Notify the listener.
			/*this->listener->OnDtlsTransportApplicationDataReceived(
				this, (uint8_t*) sslReadBuffer, static_cast<size_t>(read));*/
		}
		DEBUG_EX_LOG("[server_name = %s]", m_server_name.c_str());
	}


	/*dtlsv1x::~dtlsv1x()
	{
	}*/
}