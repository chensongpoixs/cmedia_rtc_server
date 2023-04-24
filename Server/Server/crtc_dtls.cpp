/***********************************************************************************************
created: 		2023-02-17

author:			chensong

purpose:		_C_DTLS_ _H_
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
#include "crtc_dtls.h"
#include "clog.h"
#include "cdtls_certificate.h"
#include <openssl/asn1.h>
#include <openssl/bn.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/bio.h>
#include "crtc_transport.h"
namespace chen {
	static const std::vector< SrtpCryptoSuiteMapEntry> g_srtpCryptoSuites =
	{
		{ ECRYPTO_AEAD_AES_256_GCM, "SRTP_AEAD_AES_256_GCM" },
	{ ECRYPTO_AEAD_AES_128_GCM, "SRTP_AEAD_AES_128_GCM" },
	{ ECRYPTO_AES_CM_128_HMAC_SHA1_80, "SRTP_AES128_CM_SHA1_80" },
	{ ECRYPTO_AES_CM_128_HMAC_SHA1_32, "SRTP_AES128_CM_SHA1_32" }
	};
	static const int32  g_ssl_read_buffer_size = 65536;
	static uint8        g_ssl_read_buffer[g_ssl_read_buffer_size] = {0};
	inline static void on_ssl_info(const SSL* ssl, int32 where, int32 ret)
	{
		DEBUG_EX_LOG("[where = %d][ret = %d]", where, ret);
		 static_cast<crtc_dtls*>(SSL_get_ex_data(ssl, 0))->on_ssl_info(where, ret);
	
	}

	inline static uint32 on_ssl_dtls_timer(SSL* /*ssl*/, uint32 timerUs)
	{
		if (timerUs == 0)
		{
			return 100000;
		}
		else if (timerUs >= 4000000)
		{
			return 4000000;
		}
		else
		{
			return 2 * timerUs;
		}
		return 2 * timerUs;
	}
	/* Static methods for OpenSSL callbacks. */

	inline static int32 on_ssl_certificate_verify(int32 /*preverifyOk*/, X509_STORE_CTX* /*ctx*/)
	{
		 
		// Always valid since DTLS certificates are self-signed.
		return 1;
	}
	//////////////////////////////////////////////////////////////////////////
	bool   cdtls_certificate::_init_global_ssl_ctx()
	{

		/* Set the global DTLS context. */
		// Both DTLS 1.0 and 1.2 (requires OpenSSL >= 1.1.0).
		m_ssl_ctx_ptr = SSL_CTX_new(DTLS_method());

		if (!m_ssl_ctx_ptr)
		{
			WARNING_EX_LOG("SSL_CTX_new(DTLS_method()) failed !!!");
			return false;
		}

		// Enable ECDH ciphers.
		// DOC: http://en.wikibooks.org/wiki/OpenSSL/Diffie-Hellman_parameters
		// NOTE: https://code.google.com/p/chromium/issues/detail?id=406458
		// NOTE: https://bugs.ruby-lang.org/issues/12324

		// For OpenSSL >= 1.0.2. //EC支持的曲线函数
		SSL_CTX_set_ecdh_auto(m_ssl_ctx_ptr, 1);
		// Setup DTLS context.
		//if (true) {
		//// We use "ALL", while you can use "DEFAULT" means "ALL:!EXPORT:!LOW:!aNULL:!eNULL:!SSLv2"
		//// @see https://www.openssl.org/docs/man1.0.2/man1/ciphers.html
		//ret = SSL_CTX_set_cipher_list(sslCtx, "ALL")  ;
		//if (ret == 0)
		//{
		//	ERROR_EX_LOG("SSL_CTX_set_cipher_list() failed");
		//	goto error;;
		//}


		if (true)
		{
			// Set ciphers.
			// We use "ALL", while you can use "DEFAULT" means "ALL:!EXPORT:!LOW:!aNULL:!eNULL:!SSLv2"
			// @see https://www.openssl.org/docs/man1.0.2/man1/ciphers.html
			int32 ret = SSL_CTX_set_cipher_list(m_ssl_ctx_ptr, "DEFAULT:!NULL:!aNULL:!SHA256:!SHA384:!aECDH:!AESGCM+AES256:!aPSK");

			if (ret == 0)
			{
				WARNING_EX_LOG("SSL_CTX_set_cipher_list() failed");
				return false;
			}


			// Setup the certificate.
			ret = SSL_CTX_use_certificate(m_ssl_ctx_ptr, g_dtls_certificate.get_cert());

			if (ret == 0)
			{
				WARNING_EX_LOG("SSL_CTX_use_certificate() failed");
				return false;
			}

			ret = SSL_CTX_use_PrivateKey(m_ssl_ctx_ptr, g_dtls_certificate.get_private_key());

			if (ret == 0)
			{
				WARNING_EX_LOG("SSL_CTX_use_PrivateKey() failed");

				return false;
			}

			ret = SSL_CTX_check_private_key(m_ssl_ctx_ptr);

			if (ret == 0)
			{
				WARNING_EX_LOG("SSL_CTX_check_private_key() failed");

				return false;
			}

			// Set options.
			// @see https://www.openssl.org/docs/man1.0.2/man3/SSL_CTX_set_options.html
			//// set dtls fragment
			// @see https://stackoverflow.com/questions/62413602/openssl-server-packets-get-fragmented-into-270-bytes-per-packet
			SSL_CTX_set_options(m_ssl_ctx_ptr, SSL_OP_CIPHER_SERVER_PREFERENCE | SSL_OP_NO_TICKET | SSL_OP_SINGLE_ECDH_USE | SSL_OP_NO_QUERY_MTU);

			// Don't use sessions cache.
			// @see https://www.openssl.org/docs/man1.0.2/man3/SSL_CTX_set_session_cache_mode.html 
			// SSL_SESS_CACHE_OFF : 不会对客户端或服务器进行会话缓存
			SSL_CTX_set_session_cache_mode(m_ssl_ctx_ptr, SSL_SESS_CACHE_OFF);


			// Require certificate from peer.
			// Server will send Certificate Request.
			// @see https://www.openssl.org/docs/man1.0.2/man3/SSL_CTX_set_verify.html
			// TODO: FIXME: Config it, default to off to make the packet smaller.
			// 其中 SSL_VERIFY_FAIL_IF_NO_PEER_CERT和SSL_VERIFY_CLIENT_ONCE 
			/*
			SSL_VERIFY_FAIL_IF_NO_PEER_CERT

				服务器模式：如果客户端未返回证书，则 TLS/SSL 握手将立即终止，并显示“握手失败”警报。此标志必须与SSL_VERIFY_PEER一起使用。

				客户端模式：忽略

			SSL_VERIFY_CLIENT_ONCE

				服务器模式：仅在初始 TLS/SSL 握手时请求客户端证书。在重新协商的情况下，不要再次要求提供客户端证书。此标志必须与SSL_VERIFY_PEER一起使用

				客户端模式：忽略

				总结： SSL_VERIFY_CLIENT_ONCE 在SSL_VERIFY_FAIL_IF_NO_PEER_CERT在进行了优化 作用已经有的证书就不需要再次验证了

			*/
			// 设置对等证书验证参数
			SSL_CTX_set_verify(m_ssl_ctx_ptr, SSL_VERIFY_PEER | /*SSL_VERIFY_FAIL_IF_NO_PEER_CERT*/ SSL_VERIFY_CLIENT_ONCE, on_ssl_certificate_verify);


			// The depth count is "level 0:peer certificate", "level 1: CA certificate",
		   // "level 2: higher level CA certificate", and so on.
		   // @see https://www.openssl.org/docs/man1.0.2/man3/SSL_CTX_set_verify.html
			SSL_CTX_set_verify_depth(m_ssl_ctx_ptr, 4);



			// Read always as much into the buffer as possible.
			// NOTE: This is the default for DTLS, but a bug in non latest OpenSSL
			// versions makes this call required.
			// Whether we should read as many input bytes as possible (for non-blocking reads) or not.
			// @see https://www.openssl.org/docs/man1.0.2/man3/SSL_CTX_set_read_ahead.html
			SSL_CTX_set_read_ahead(m_ssl_ctx_ptr, 1);

			// Set SSL info callback.
			SSL_CTX_set_info_callback(m_ssl_ctx_ptr, on_ssl_info);



			////////////////////////////////////////////////////////////////
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
			std::vector<SrtpCryptoSuiteMapEntry>  srtpCryptoSuites =
			{
				{ CryptoSuite::AEAD_AES_256_GCM, "SRTP_AEAD_AES_256_GCM" },
				{ CryptoSuite::AEAD_AES_128_GCM, "SRTP_AEAD_AES_128_GCM" },
				{ CryptoSuite::AES_CM_128_HMAC_SHA1_80, "SRTP_AES128_CM_SHA1_80" },
				{ CryptoSuite::AES_CM_128_HMAC_SHA1_32, "SRTP_AES128_CM_SHA1_32" }
			};

			// Set the "use_srtp" DTLS extension.
			std::string dtlsSrtpCryptoSuites;
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
			// TODO: Maybe we can use SRTP-GCM in future.
			// @see https://bugs.chromium.org/p/chromium/issues/detail?id=713701
			// @see https://groups.google.com/forum/#!topic/discuss-webrtc/PvCbWSetVAQ
			// @remark Only support SRTP_AES128_CM_SHA1_80, please read ssl/d1_srtp.c
			ret = SSL_CTX_set_tlsext_use_srtp(m_ssl_ctx_ptr, dtlsSrtpCryptoSuites.c_str());

			if (ret != 0)
			{
				WARNING_EX_LOG(
					"SSL_CTX_set_tlsext_use_srtp() failed when entering '%s'", dtlsSrtpCryptoSuites.c_str());
				WARNING_EX_LOG("SSL_CTX_set_tlsext_use_srtp() failed");

				return false;
			}
		}



		return true;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////


	crtc_dtls::~crtc_dtls()
	{
	}

	

	/*void crtc_dtls::destroy_global_ssl_ctx()
	{
	}*/

	bool crtc_dtls::init()
	{
		m_ssl_ptr = SSL_new(g_dtls_certificate.get_ssl_ctx());
		if (!m_ssl_ptr)
		{
			WARNING_EX_LOG("SSL_new failed !!!");
			return false;
		}

		// Set this as custom data.
		SSL_set_ex_data(m_ssl_ptr, 0, static_cast<void*>(this));

		m_ssl_bio_read_network_ptr = BIO_new(BIO_s_mem());
		if (!m_ssl_bio_read_network_ptr)
		{
			WARNING_EX_LOG("BIO_new read network failed !!!");
			return false;
		}
		m_ssl_bio_write_network_ptr = BIO_new(BIO_s_mem());
		if (!m_ssl_bio_write_network_ptr)
		{
			WARNING_EX_LOG("m_ssl_bio_write_network_ptr BIO_new failed !!! ");
			return false;
		}
		SSL_set_bio(m_ssl_ptr, m_ssl_bio_read_network_ptr, m_ssl_bio_write_network_ptr);

		// Set the MTU so that we don't send packets that are too large with no fragmentation.
		SSL_set_mtu(m_ssl_ptr, DTLSMTU);
		DTLS_set_link_mtu(m_ssl_ptr, DTLSMTU);


		// Set callback handler for setting DTLS timer interval.
		DTLS_set_timer_cb(m_ssl_ptr, on_ssl_dtls_timer);

		return true;
	}

	void crtc_dtls::update(uint32 uDeltaTime)
	{
		// Workaround for https://github.com/openssl/openssl/issues/7998.
		if (m_handshake_done/*m_handshake_done_for_us*//*this->handshakeDone*/)
		{
			NORMAL_EX_LOG("handshake is done so return");

			return;
		}

		DTLSv1_handle_timeout(m_ssl_ptr);

		// If required, send DTLS data.
		send_pending_outgoing_dtlsdata();

		// Set the DTLS timer again.
		set_timeout();
	}

	void crtc_dtls::destroy()
	{
		// NOTE: At this point SSL_set_bio() was not called so we must free BIOs as
		// well.
		if (m_ssl_ptr)
		{
			if (m_handshake_done/*m_handshake_done_for_us*/)
			{
				SSL_shutdown(m_ssl_ptr);
				send_pending_outgoing_dtlsdata();
			}
			
			SSL_free(m_ssl_ptr);
			m_ssl_ptr = NULL;
		}
		if (m_ssl_bio_read_network_ptr)
		{
			//BIO_free(m_ssl_bio_read_network_ptr);
			m_ssl_bio_read_network_ptr = NULL;
		}
		if (m_ssl_bio_write_network_ptr)
		{
			//BIO_free(m_ssl_bio_write_network_ptr);
			m_ssl_bio_write_network_ptr = NULL;
		}

		
	}

	bool crtc_dtls::start_active_handshake(const std::string & type)
	{
		NORMAL_EX_LOG("role:%s", type);
		m_role = type;
		if (type == "server")
		{
			SSL_set_accept_state(m_ssl_ptr);
			SSL_do_handshake(m_ssl_ptr);
			
		}
		else if (type == "client")
		{
			SSL_set_connect_state(m_ssl_ptr);
			SSL_do_handshake(m_ssl_ptr);
			send_pending_outgoing_dtlsdata();
			set_timeout();
		}
		else
		{
			WARNING_EX_LOG("invalid local DTLS role  type= %s, error", type.c_str());
		}

		return true;
	}

	void crtc_dtls::process_dtls_data(const uint8 * data, int32 len)
	{

		int32 written = 0;
		int32 read = 0;
		
		// Write the received DTLS data into the sslBioFromNetwork.
		written = BIO_write(m_ssl_bio_read_network_ptr, static_cast<const void*>(data), static_cast<int>(len));

		if (written != static_cast<int>(len))
		{
			WARNING_EX_LOG( "OpenSSL BIO_write() wrote less (%zu bytes) than given data (%zu bytes)",
				static_cast<size_t>(written),
				len);
		}

		// Must call SSL_read() to process received DTLS data.
		read = SSL_read(m_ssl_ptr, static_cast<void*>(g_ssl_read_buffer), g_ssl_read_buffer_size);

		// Send data if it's ready.
		send_pending_outgoing_dtlsdata();

		// Check SSL status and return if it is bad/closed.
		 if (!_check_status(read))
		{
			return;
		} 

		// Set/update the DTLS timeout.
		if (!set_timeout())
		{
			return;
		}

		// Application data received. Notify to the listener.
		if (read > 0)
		{
			// It is allowed to receive DTLS data even before validating remote fingerprint.
			if (!m_handshake_done /*!this->handshakeDone*/)
			{
				WARNING_EX_LOG( "ignoring application data received while DTLS handshake not done");

				return;
			}

			// Notify the listener.
			m_callback_ptr->on_dtls_application_data(  g_ssl_read_buffer, static_cast<size_t>(read));
		}

	}

	void crtc_dtls::send_application_data(const uint8* data, size_t len)
	{
		if (!m_handshake_done)
		{
			WARNING_EX_LOG("cannot send application data while DTLS is not fully connected");
			return;
		}
		if (len == 0)
		{
			WARNING_EX_LOG( "ignoring 0 length data");

			return;
		}

		int written;

		written = SSL_write(this->m_ssl_ptr, static_cast<const void*>(data), static_cast<int>(len));

		if (written < 0)
		{
			WARNING_EX_LOG("SSL_write() failed");

			if (!_check_status(written))
			{
				return;
			}
		}
		else if (written != static_cast<int>(len))
		{
			WARNING_EX_LOG(  "OpenSSL SSL_write() wrote less (%d bytes) than given data (%zu bytes)", written, len);
		}

		// Send data.
		send_pending_outgoing_dtlsdata();
	}

	void crtc_dtls::send_pending_outgoing_dtlsdata()
	{
		if (BIO_eof(m_ssl_bio_write_network_ptr))
		{
			return;
		}

		int64 read;
		char *data = NULL;;
		read = BIO_get_mem_data(m_ssl_bio_write_network_ptr , &data);
		if (read <= 0)
		{
			return;
		}

		//NORMAL_EX_LOG("%" PRIu64 " bytes of DTLS data ready to sent to the peer", read);

		m_callback_ptr->write_dtls_data(reinterpret_cast<uint8*>(data), static_cast<size_t>(read));
		// Clear the BIO buffer.
		// NOTE: the (void) avoids the -Wunused-value warning.
		(void)BIO_reset(m_ssl_bio_write_network_ptr);

	}

	bool crtc_dtls::set_timeout()
	{
		int64_t ret;
		uv_timeval_t dtlsTimeout{ 0, 0 };
		uint64_t timeoutMs;

		// NOTE: If ret == 0 then ignore the value in dtlsTimeout.
		// NOTE: No DTLSv_1_2_get_timeout() or DTLS_get_timeout() in OpenSSL 1.1.0-dev.
		ret = DTLSv1_get_timeout(m_ssl_ptr, static_cast<void*>(&dtlsTimeout)); // NOLINT

		if (ret == 0)
		{
			return true;
		}

		timeoutMs = (dtlsTimeout.tv_sec * static_cast<uint64_t>(1000)) + (dtlsTimeout.tv_usec / 1000);

		if (timeoutMs == 0)
		{
			return true;
		}
		else if (timeoutMs < 30000)
		{
			NORMAL_EX_LOG("DTLS timer set in %" PRIu64 "ms", timeoutMs);

			//this->timer->Start(timeoutMs);

			return true;
		}
		// NOTE: Don't start the timer again if the timeout is greater than 30 seconds.
		else
		{
			WARNING_EX_LOG( "DTLS timeout too high (%" PRIu64 "ms), resetting DLTS", timeoutMs);

		//	Reset();

			// Set state and notify the listener.
			//this->state = DtlsState::FAILED;
			//this->listener->OnDtlsTransportFailed(this);

			return false;
		}
	}

	bool crtc_dtls::process_handshake()
	{
		if (!_check_remote_fingerprintf())
		{
			WARNING_EX_LOG("check remote fingerprintf failed !!!");
			return false;
		}
		// Get the negotiated SRTP crypto suite.
		ECRYPTO_SUITE srtpCryptoSuite = _get_netotiated_srtp_crypto_suite();
		if (srtpCryptoSuite != ECRYPTO_NONE)
		{
			// Extract the SRTP keys (will notify the listener with them).
			_extract_srtp_keys(srtpCryptoSuite);
			return true;
		}

		// NOTE: We assume that "use_srtp" DTLS extension is required even if
		// there is no audio/video.
		WARNING_EX_LOG("dtls, srtp,  SRTP crypto suite not negotiated");

		//Reset();

		// Set state and notify the listener.
		//this->state = DtlsState::FAILED;
		//this->listener->OnDtlsTransportFailed(this);
		return false;
	}

	void crtc_dtls::on_ssl_info(int32 where, int32 ret)
	{
		int w = where & -SSL_ST_MASK;
		const char* role;

		if ((w & SSL_ST_CONNECT) != 0)
		{
			role = "client";
		}
		else if ((w & SSL_ST_ACCEPT) != 0)
		{
			role = "server";
		}
		else
		{
			role = "undefined";
		}

		if ((where & SSL_CB_LOOP) != 0)
		{
			NORMAL_EX_LOG("[role:%s, action:'%s']", role, SSL_state_string_long(m_ssl_ptr));
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
				WARNING_EX_LOG("received DTLS %s alert: %s", alertType, SSL_alert_desc_string_long(ret));
			}
			else if ((where & SSL_CB_WRITE) != 0)
			{
				WARNING_EX_LOG( "sending DTLS %s alert: %s", alertType, SSL_alert_desc_string_long(ret));
			}
			else
			{
				WARNING_EX_LOG(  "DTLS %s alert: %s", alertType, SSL_alert_desc_string_long(ret));
			}
		}
		else if ((where & SSL_CB_EXIT) != 0)
		{
			if (ret == 0)
			{
				NORMAL_EX_LOG( "[role:%s, failed:'%s']",  role,  SSL_state_string_long(m_ssl_ptr));
			}
			else if (ret < 0)
			{
				NORMAL_EX_LOG("role: %s, waiting:'%s']", role, SSL_state_string_long(m_ssl_ptr));
			}
		}
		else if ((where & SSL_CB_HANDSHAKE_START) != 0)
		{
			NORMAL_EX_LOG( "DTLS handshake start");
		}
		else if ((where & SSL_CB_HANDSHAKE_DONE) != 0)
		{
			NORMAL_EX_LOG( "DTLS handshake done");

			//this->handshakeDoneNow = true;
			m_handshake_done_for_us = true;
			
		}

		// NOTE: checking SSL_get_shutdown(this->ssl) & SSL_RECEIVED_SHUTDOWN here upon
		// receipt of a close alert does not work (the flag is set after this callback).
	}
	 
	bool crtc_dtls::_check_status(int32 returnCode)
	{
		int err;
		//bool wasHandshakeDone = this->handshakeDone;

		err = SSL_get_error(m_ssl_ptr, returnCode);

		switch (err)
		{
		case SSL_ERROR_NONE:
			break;

		case SSL_ERROR_SSL:
			ERROR_EX_LOG("SSL status: SSL_ERROR_SSL");
			break;

		case SSL_ERROR_WANT_READ:
			break;

		case SSL_ERROR_WANT_WRITE:
			WARNING_EX_LOG( "SSL status: SSL_ERROR_WANT_WRITE");
			break;

		case SSL_ERROR_WANT_X509_LOOKUP:
			NORMAL_EX_LOG( "SSL status: SSL_ERROR_WANT_X509_LOOKUP");
			break;

		case SSL_ERROR_SYSCALL:
			ERROR_EX_LOG("SSL status: SSL_ERROR_SYSCALL");
			break;

		case SSL_ERROR_ZERO_RETURN:
			break;

		case SSL_ERROR_WANT_CONNECT:
			WARNING_EX_LOG( "SSL status: SSL_ERROR_WANT_CONNECT");
			break;

		case SSL_ERROR_WANT_ACCEPT:
			WARNING_EX_LOG(  "SSL status: SSL_ERROR_WANT_ACCEPT");
			break;

		default:
			WARNING_EX_LOG(  "SSL status: unknown error");
		}
		if (m_handshake_done_for_us)
		{
			m_handshake_done_for_us = false;
			m_handshake_done = true;
			return process_handshake();
		}
		// Check if the handshake (or re-handshake) has been done right now.
		//if (this->handshakeDoneNow)
		//{
		//	this->handshakeDoneNow = false;
		//	this->handshakeDone = true;

		//	// Stop the timer.
		//	this->timer->Stop();

		//	// Process the handshake just once (ignore if DTLS renegotiation).
		//	if (!wasHandshakeDone && this->remoteFingerprint.algorithm != FingerprintAlgorithm::NONE)
		//		return ProcessHandshake();

		//	return true;
		//}
		//// Check if the peer sent close alert or a fatal error happened.
		//else if (((SSL_get_shutdown(this->ssl) & SSL_RECEIVED_SHUTDOWN) != 0) || err == SSL_ERROR_SSL || err == SSL_ERROR_SYSCALL)
		//{
		//	if (this->state == DtlsState::CONNECTED)
		//	{
		//		NORMAL_EX_LOG( "disconnected");

		//		//Reset();

		//		// Set state and notify the listener.
		//		//this->state = DtlsState::CLOSED;
		//		//this->listener->OnDtlsTransportClosed(this);
		//	}
		//	else
		//	{
		//		WARNING_EX_LOG( "connection failed");

		//		//Reset();

		//		// Set state and notify the listener.
		//		//this->state = DtlsState::FAILED;
		//		//this->listener->OnDtlsTransportFailed(this);
		//	}

		//	return false;
		//}
		//else
		//{
		//	return true;
		//}
		return true;
	}

	bool crtc_dtls::_check_remote_fingerprintf()
	{

		//

		X509* certificate;
		uint8_t binaryFingerprint[EVP_MAX_MD_SIZE];
		unsigned int size{ 0 };
		char hexFingerprint[(EVP_MAX_MD_SIZE * 3) + 1] = {0};
		const EVP_MD* hashFunction;
		int ret;

		certificate = SSL_get_peer_certificate(m_ssl_ptr);

		if (!certificate)
		{
			WARNING_EX_LOG("no certificate was provided by the peer");

			return false;
		}
		hashFunction = EVP_sha256();
		/*switch (this->remoteFingerprint.algorithm)
		{
		case FingerprintAlgorithm::SHA1:
			hashFunction = EVP_sha1();
			break;

		case FingerprintAlgorithm::SHA224:
			hashFunction = EVP_sha224();
			break;

		case FingerprintAlgorithm::SHA256:
			hashFunction = EVP_sha256();
			break;

		case FingerprintAlgorithm::SHA384:
			hashFunction = EVP_sha384();
			break;

		case FingerprintAlgorithm::SHA512:
			hashFunction = EVP_sha512();
			break;

		default:
			MS_ABORT("unknown algorithm");
		}*/

		// Compare the remote fingerprint with the value given via signaling.
		ret = X509_digest(certificate, hashFunction, binaryFingerprint, &size);

		if (ret == 0)
		{
			ERROR_EX_LOG("X509_digest() failed");

			X509_free(certificate);

			return false;
		}

		// Convert to hexadecimal format in uppercase with colons.
		for (unsigned int i{ 0 }; i < size; ++i)
		{
			std::sprintf(hexFingerprint + (i * 3), "%.2X:", binaryFingerprint[i]);
		}
		hexFingerprint[(size * 3) - 1] = '\0';

		// TODO@chensong 2023-02-18 验证客户都的sdp中密钥是否一致

		/*if (this->remoteFingerprint.value != hexFingerprint)
		{
			MS_WARN_TAG(
				dtls,
				"fingerprint in the remote certificate (%s) does not match the announced one (%s)",
				hexFingerprint,
				this->remoteFingerprint.value.c_str());

			X509_free(certificate);

			return false;
		}*/

		NORMAL_EX_LOG("valid remote fingerprint [hexFingerprint = %s]", hexFingerprint );

		// Get the remote certificate in PEM format.

		BIO* bio = BIO_new(BIO_s_mem());

		// Ensure the underlying BUF_MEM structure is also freed.
		// NOTE: Avoid stupid "warning: value computed is not used [-Wunused-value]" since
		// BIO_set_close() always returns 1.
		(void)BIO_set_close(bio, BIO_CLOSE);

		ret = PEM_write_bio_X509(bio, certificate);

		if (ret != 1)
		{
			ERROR_EX_LOG("PEM_write_bio_X509() failed");

			X509_free(certificate);
			BIO_free(bio);

			return false;
		}

		BUF_MEM* mem;

		BIO_get_mem_ptr(bio, &mem); // NOLINT[cppcoreguidelines-pro-type-cstyle-cast]

		if (!mem || !mem->data || mem->length == 0u)
		{
			ERROR_EX_LOG("BIO_get_mem_ptr() failed");

			X509_free(certificate);
			BIO_free(bio);

			return false;
		}

		m_remote_cert = std::string(mem->data, mem->length);

		X509_free(certificate);
		BIO_free(bio);

		return true;
	}

	inline ECRYPTO_SUITE crtc_dtls::_get_netotiated_srtp_crypto_suite()
	{

		ECRYPTO_SUITE negotiatedSrtpCryptoSuite = ECRYPTO_NONE;
		// Ensure that the SRTP crypto suite has been negotiated.
		// NOTE: This is a OpenSSL type.
		SRTP_PROTECTION_PROFILE* sslSrtpCryptoSuite = SSL_get_selected_srtp_profile(m_ssl_ptr);


		if (!sslSrtpCryptoSuite)
		{
			return negotiatedSrtpCryptoSuite;
		}


		// Get the negotiated SRTP crypto suite.
		for (const  SrtpCryptoSuiteMapEntry& srtpCryptoSuite : g_srtpCryptoSuites)
		{
			//SrtpCryptoSuiteMapEntry* cryptoSuiteEntry = std::addressof(srtpCryptoSuite);

			if (std::strcmp(sslSrtpCryptoSuite->name, srtpCryptoSuite.name) == 0)
			{
				DEBUG_EX_LOG( "chosen SRTP crypto suite: %s", srtpCryptoSuite.name);

				negotiatedSrtpCryptoSuite = srtpCryptoSuite.cryptoSuite;
			}
		}
		cassert_desc(negotiatedSrtpCryptoSuite != ECRYPTO_NONE,
			"chosen SRTP crypto suite is not an available one");
		return negotiatedSrtpCryptoSuite;
	}

	void crtc_dtls::_extract_srtp_keys(ECRYPTO_SUITE srtpCryptoSuite)
	{
		size_t srtpKeyLength{ 0 };
		size_t srtpSaltLength{ 0 };
		size_t srtpMasterLength{ 0 };

		switch (srtpCryptoSuite)
		{//ECRYPTO_AES_CM_128_HMAC_SHA1_80
			case ECRYPTO_AES_CM_128_HMAC_SHA1_80:
			case ECRYPTO_AES_CM_128_HMAC_SHA1_32:
			{
				srtpKeyLength = SrtpMasterKeyLength;
				srtpSaltLength = SrtpMasterSaltLength;
				srtpMasterLength = SrtpMasterLength;

				break;
			}

			case  ECRYPTO_AEAD_AES_256_GCM:
			{
				srtpKeyLength = SrtpAesGcm256MasterKeyLength;
				srtpSaltLength = SrtpAesGcm256MasterSaltLength;
				srtpMasterLength = SrtpAesGcm256MasterLength;

				break;
			}

			case ECRYPTO_AEAD_AES_128_GCM:
			{
				srtpKeyLength = SrtpAesGcm128MasterKeyLength;
				srtpSaltLength = SrtpAesGcm128MasterSaltLength;
				srtpMasterLength = SrtpAesGcm128MasterLength;

				break;
			}

			default:
			{
				ERROR_EX_LOG("unknown SRTP crypto suite");
			}
		}

		auto* srtpMaterial = new uint8_t[srtpMasterLength * 2];
		uint8_t* srtpLocalKey{ nullptr };
		uint8_t* srtpLocalSalt{ nullptr };
		uint8_t* srtpRemoteKey{ nullptr };
		uint8_t* srtpRemoteSalt{ nullptr };
		auto* srtpLocalMasterKey = new uint8_t[srtpMasterLength];
		auto* srtpRemoteMasterKey = new uint8_t[srtpMasterLength];
		int ret;

		ret = SSL_export_keying_material(
			m_ssl_ptr, srtpMaterial, srtpMasterLength * 2, "EXTRACTOR-dtls_srtp", 19, nullptr, 0, 0);

		cassert_desc(ret != 0, "SSL_export_keying_material() failed");
		if (m_role == "server")
		{
			srtpRemoteKey = srtpMaterial;
			srtpLocalKey = srtpRemoteKey + srtpKeyLength;
			srtpRemoteSalt = srtpLocalKey + srtpKeyLength;
			srtpLocalSalt = srtpRemoteSalt + srtpSaltLength;
		}
		else if (m_role == "client")
		{
			srtpLocalKey = srtpMaterial;
			srtpRemoteKey = srtpLocalKey + srtpKeyLength;
			srtpLocalSalt = srtpRemoteKey + srtpKeyLength;
			srtpRemoteSalt = srtpLocalSalt + srtpSaltLength;
		}
		else
		{
			ERROR_EX_LOG("no DTLS role set");
		}
		//switch (m_role)
		//{
		//	case /*Role::SERVER*/"server":
		//	{
		//		srtpRemoteKey = srtpMaterial;
		//		srtpLocalKey = srtpRemoteKey + srtpKeyLength;
		//		srtpRemoteSalt = srtpLocalKey + srtpKeyLength;
		//		srtpLocalSalt = srtpRemoteSalt + srtpSaltLength;

		//		break;
		//	}

		//	case Role::CLIENT:
		//	{
		//		srtpLocalKey = srtpMaterial;
		//		srtpRemoteKey = srtpLocalKey + srtpKeyLength;
		//		srtpLocalSalt = srtpRemoteKey + srtpKeyLength;
		//		srtpRemoteSalt = srtpLocalSalt + srtpSaltLength;

		//		break;
		//	}

		//	default:
		//	{
		//		MS_ABORT("no DTLS role set");
		//	}
		//}

		// Create the SRTP local master key.
		std::memcpy(srtpLocalMasterKey, srtpLocalKey, srtpKeyLength);
		std::memcpy(srtpLocalMasterKey + srtpKeyLength, srtpLocalSalt, srtpSaltLength);
		// Create the SRTP remote master key.
		std::memcpy(srtpRemoteMasterKey, srtpRemoteKey, srtpKeyLength);
		std::memcpy(srtpRemoteMasterKey + srtpKeyLength, srtpRemoteSalt, srtpSaltLength);
		NORMAL_EX_LOG("");
		// Set state and notify the listener.
		//this->state = DtlsState::CONNECTED;
		m_callback_ptr->on_dtls_transport_connected( 
			srtpCryptoSuite,
			srtpLocalMasterKey,
			srtpMasterLength,
			srtpRemoteMasterKey,
			srtpMasterLength);

		delete[] srtpMaterial;
		delete[] srtpLocalMasterKey;
		delete[] srtpRemoteMasterKey;
	}

}