/***********************************************************************************************
created: 		2023-01-30

author:			chensong

purpose:		_C_DTLS_CERTIFICATE_H_


************************************************************************************************/
#include "cdtls_session.h"
#include <srtp2/srtp.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <cassert>
#include <iostream>
#include "clog.h"
#include "cdtls_certificate.h"
#include "cdtls_define.h"
#include "cstr2digit.h"
namespace chen {

	// to avoid dtls negotiate failed, set max fragment size 1200. // dtls mtu 1350
	static const int32 DTLS_FRAGMENT_MAX_SIZE = 1200;

	//  SRTP key len 
	static const int32 SRTP_MASTER_KEY_KEY_LEN = 16;
	static const int32 SRTP_MASTER_KEY_SALT_LEN = 14;


    ///////////////////////////////////////////////////////////////////////////////////
    //                       OpenSSL global function 

	 // Setup the openssl timeout for DTLS packet.
	// @see https://www.openssl.org/docs/man1.1.1/man3/DTLS_set_timer_cb.html
	//
	// Use step timeout for ARQ, [50, 100, 200, 400, 800, 1600, 3200, 6400, 12800, 25600, 51200] in ms,
	// then total timeout is sum([50, 100, 200, 400, 800, 1600, 3200, 6400, 12800, 25600, 51200]) = 102350ms.
	//
	// @remark The connection might be closed for timeout in about 30s by default, which stop the DTLS ARQ.
	inline static uint32 on_dtls_timer_cb(SSL* dtls_ptr, uint32 previous_us)
	{
		cdtls_session * dtsl_session_ptr = static_cast<cdtls_session*>(SSL_get_ex_data(dtls_ptr, 0));

		assert(dtsl_session_ptr);



		// Double the timeout. Note that it can be 0.
		uint32 timeout_us = previous_us * 2;
		// If previous_us is 0, for example, the HelloVerifyRequest, we should response it ASAP.
		// When got ServerHello, we should reset the timer.
		// TODO@chensong 2022-01-30  dtsl client reset timer ---> 
		if (previous_us == 0 || dtsl_session_ptr->should_reset_timer())
		{
			timeout_us = 50 * 1000; // in us
		}

		// Never exceed the max timeout.
		timeout_us = timeout_us > (30 * 1000 * 1000) ? (30 * 1000 * 1000) : timeout_us; //min(timeout_us, (30 * 1000 * 1000)); // in us

		NORMAL_EX_LOG("DTLS: ARQ timer cb timeout=%ums, previous=%ums", timeout_us / 1000, previous_us / 1000);

		/*if (timerUs == 0)
		{
			return 100000;
		}
		else if (timerUs >= 4000000)
		{
			return 4000000;
		}*/
		//else
		return 2 * timeout_us;
	}


	//DTLS状态回调
	// Print the information of SSL, DTLS alert as such.
	inline static void on_ssl_info(const SSL* dtls_ptr, int32 where, int32 ret)
	{
		//DEBUG_EX_LOG("[where = %d][ret = %d]", where, ret);
		//static_cast<dtlsv1x*>(SSL_get_ex_data(ssl, 0))->OnSslInfo(where, ret);
		cdtls_session * dtsl_session_ptr = static_cast<cdtls_session*>(SSL_get_ex_data(dtls_ptr, 0));

		assert(dtsl_session_ptr);

		const char* method;
		int32 w = where & ~SSL_ST_MASK;
		if (w & SSL_ST_CONNECT)
		{
			method = "SSL_connect";
		}
		else if (w & SSL_ST_ACCEPT) 
		{
			method = "SSL_accept";
		}
		else 
		{
			method = "undefined";
		}


		int32 r1 = SSL_get_error(dtls_ptr, ret);
		if (where & SSL_CB_LOOP) 
		{
			NORMAL_EX_LOG("DTLS: method=%s state=%s(%s), where=%d, ret=%d, r1=%d", method, SSL_state_string(dtls_ptr),
				SSL_state_string_long(dtls_ptr), where, ret, r1);
		}
		else if (where & SSL_CB_ALERT) 
		{
			method = (where & SSL_CB_READ) ? "read" : "write";

			// @see https://www.openssl.org/docs/man1.0.2/man3/SSL_alert_type_string_long.html
			std::string alert_type = SSL_alert_type_string_long(ret);
			std::string alert_desc = SSL_alert_desc_string(ret);

			if (alert_type == "warning" && alert_desc == "CN")
			{
				WARNING_EX_LOG("DTLS: SSL3 alert method=%s type=%s, desc=%s(%s), where=%d, ret=%d, r1=%d", method, alert_type.c_str(),
					alert_desc.c_str(), SSL_alert_desc_string_long(ret), where, ret, r1);
			}
			else 
			{
				ERROR_EX_LOG("DTLS: SSL3 alert method=%s type=%s, desc=%s(%s), where=%d, ret=%d, r1=%d", method, alert_type.c_str(),
					alert_desc.c_str(), SSL_alert_desc_string_long(ret), where, ret, r1);
			}

			// Notify the DTLS to handle the ALERT message, which maybe means media connection disconnect.
			// TODO@chensong 2023-01-30  密钥协商失败的处理
			//dtsl_session_ptr->callback_by_ssl(alert_type, alert_desc);
		}
		else if (where & SSL_CB_EXIT) 
		{
			if (ret == 0) 
			{
				WARNING_EX_LOG("DTLS: Fail method=%s state=%s(%s), where=%d, ret=%d, r1=%d", method, SSL_state_string(dtls_ptr),
					SSL_state_string_long(dtls_ptr), where, ret, r1);
			}
			else if (ret < 0) 
			{
				if (r1 != SSL_ERROR_NONE && r1 != SSL_ERROR_WANT_READ && r1 != SSL_ERROR_WANT_WRITE)
				{
					ERROR_EX_LOG("DTLS: Error method=%s state=%s(%s), where=%d, ret=%d, r1=%d", method, SSL_state_string(dtls_ptr),
						SSL_state_string_long(dtls_ptr), where, ret, r1);
				}
				else 
				{
					NORMAL_EX_LOG("DTLS: Error method=%s state=%s(%s), where=%d, ret=%d, r1=%d", method, SSL_state_string(dtls_ptr),
						SSL_state_string_long(dtls_ptr), where, ret, r1);
				}
			}
		}
		else if ((where & SSL_CB_HANDSHAKE_START) != 0)
		{
			NORMAL_EX_LOG("DTLS : method = %s handshake start", method);
		}
		else if ((where & SSL_CB_HANDSHAKE_DONE) != 0)
		{
			NORMAL_EX_LOG("DTLS : method = %s handshake done", method);

			//this->handshakeDoneNow = true;
		}
		else
		{
			//dtls  shutdown 
			//NORMAL_EX_LOG("DTLS : method = %s, un ", method);
			DEBUG_EX_LOG("DTLS: method=%s state=%s(%s), where=%d, ret=%d, r1=%d", method, SSL_state_string(dtls_ptr),
				SSL_state_string_long(dtls_ptr), where, ret, r1);
		}

		// check ssl status 
		int32 status = SSL_get_shutdown(dtls_ptr);
		if (SSL_RECEIVED_SHUTDOWN & status)
		{
			WARNING_EX_LOG("DTLS: shutdown !!!");
		}

		// NOTE: checking SSL_get_shutdown(this->ssl) & SSL_RECEIVED_SHUTDOWN here upon
		// receipt of a close alert does not work (the flag is set after this callback).
	}


	// The return value of verify_callback controls the strategy of the further verification process. If verify_callback
	// returns 0, the verification process is immediately stopped with "verification failed" state. If SSL_VERIFY_PEER is
	// set, a verification failure alert is sent to the peer and the TLS/SSL handshake is terminated. If verify_callback
	// returns 1, the verification process is continued. If verify_callback always returns 1, the TLS/SSL handshake will
	// not be terminated with respect to verification failures and the connection will be established. The calling process
	// can however retrieve the error code of the last verification error using SSL_get_verify_result(3) or by maintaining
	// its own error storage managed by verify_callback.
	// @see https://www.openssl.org/docs/man1.0.2/man3/SSL_CTX_set_verify.html
	inline static int32 on_ssl_verify_callback(int32 preverify_ok, X509_STORE_CTX* ctx_ptr)
	{

		//DEBUG_EX_LOG("OpenSSL callbacks");
		// Always valid since DTLS certificates are self-signed.
		 // Always OK, we don't check the certificate of client,
		// because we allow client self-sign certificate.
		return 1;
	}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
	SSL_CTX*  build_dtls_ctx()
	{
		SSL_CTX * dtls_ptr = NULL; 

		// Both DTLS 1.0 and 1.2 (requires OpenSSL >= 1.1.0).
		dtls_ptr = SSL_CTX_new(DTLS_method());
		if (!dtls_ptr)
		{
			WARNING_EX_LOG("OpenSSL SSL_CTX_new failed !!!");
			return NULL;
		}
		// Enable ECDH ciphers.
		// DOC: http://en.wikibooks.org/wiki/OpenSSL/Diffie-Hellman_parameters
		// NOTE: https://code.google.com/p/chromium/issues/detail?id=406458
		// NOTE: https://bugs.ruby-lang.org/issues/12324

		// For OpenSSL >= 1.0.2. EC支持的曲线函数
		SSL_CTX_set_ecdh_auto(dtls_ptr, 1);

		// Setup DTLS context.
		if (true)
		{
			// 1. Set ciphers.
			// We use "ALL", while you can use "DEFAULT" means "ALL:!EXPORT:!LOW:!aNULL:!eNULL:!SSLv2"
			// @see https://www.openssl.org/docs/man1.0.2/man1/ciphers.html

			assert(SSL_CTX_set_cipher_list(dtls_ptr, "DEFAULT:!NULL:!aNULL:!SHA256:!SHA384:!aECDH:!AESGCM+AES256:!aPSK") == 1);


			// 2. Setup the certificate.
			assert(SSL_CTX_use_certificate(dtls_ptr, g_dtls_certificate.get_cert()) == 1);
			assert(SSL_CTX_use_PrivateKey(dtls_ptr, g_dtls_certificate.get_public_key()) == 1);


			// 3. check SSL 
			// @see https://www.openssl.org/docs/man1.0.2/man3/SSL_CTX_check_private_key.html
			assert(SSL_CTX_check_private_key(dtls_ptr) == 1);

			// 4. Set options.
			// @see https://www.openssl.org/docs/man1.0.2/man3/SSL_CTX_set_options.html
			//// set dtls fragment
			// @see https://stackoverflow.com/questions/62413602/openssl-server-packets-get-fragmented-into-270-bytes-per-packet
			//SSL_CTX_set_options(dtls_ptr, SSL_OP_CIPHER_SERVER_PREFERENCE | SSL_OP_NO_TICKET | SSL_OP_SINGLE_ECDH_USE | SSL_OP_NO_QUERY_MTU);

			// 5. Don't use sessions cache.
			// @see https://www.openssl.org/docs/man1.0.2/man3/SSL_CTX_set_session_cache_mode.html 
			// SSL_SESS_CACHE_OFF : 不会对客户端或服务器进行会话缓存
			//SSL_CTX_set_session_cache_mode(dtls_ptr, SSL_SESS_CACHE_OFF);


			// 6. Require certificate from peer. 证书的客户端与服务端验证根据的流程规定
			// Server will send Certificate Request.
			// @see https://www.openssl.org/docs/man1.0.2/man3/SSL_CTX_set_verify.html
			// TODO: FIXME: Config it, default to off to make the packet smaller.
			// 其中 SSL_VERIFY_FAIL_IF_NO_PEER_CERT和SSL_VERIFY_CLIENT_ONCE 
			/* SSL_VERIFY_FAIL_IF_NO_PEER_CERT

				服务器模式：如果客户端未返回证书，则 TLS/SSL 握手将立即终止，并显示“握手失败”警报。此标志必须与SSL_VERIFY_PEER一起使用。

				客户端模式：忽略

			SSL_VERIFY_CLIENT_ONCE

				服务器模式：仅在初始 TLS/SSL 握手时请求客户端证书。在重新协商的情况下，不要再次要求提供客户端证书。此标志必须与SSL_VERIFY_PEER一起使用

				客户端模式：忽略

				总结： SSL_VERIFY_CLIENT_ONCE 在SSL_VERIFY_FAIL_IF_NO_PEER_CERT在进行了优化 作用已经有的证书就不需要再次验证了

			*/ 
			SSL_CTX_set_verify(dtls_ptr, SSL_VERIFY_PEER | SSL_VERIFY_CLIENT_ONCE /*SSL_VERIFY_FAIL_IF_NO_PEER_CERT*/ , on_ssl_verify_callback);

			// 7. The depth count is "level 0:peer certificate", "level 1: CA certificate",
		   // "level 2: higher level CA certificate", and so on.
		   // @see https://www.openssl.org/docs/man1.0.2/man3/SSL_CTX_set_verify.html
			SSL_CTX_set_verify_depth(dtls_ptr, 4);


			// 8. Read always as much into the buffer as possible.
			// NOTE: This is the default for DTLS, but a bug in non latest OpenSSL
			// versions makes this call required.
			// Whether we should read as many input bytes as possible (for non-blocking reads) or not.
			// @see https://www.openssl.org/docs/man1.0.2/man3/SSL_CTX_set_read_ahead.html
			SSL_CTX_set_read_ahead(dtls_ptr, 1);

			// 9. [SRTP]  NOTE: This function returns 0 on success.
			// TODO: Maybe we can use SRTP-GCM in future.
			// @see https://bugs.chromium.org/p/chromium/issues/detail?id=713701
			// @see https://groups.google.com/forum/#!topic/discuss-webrtc/PvCbWSetVAQ
			// @remark Only support SRTP_AES128_CM_SHA1_80, please read ssl/d1_srtp.c
			assert( SSL_CTX_set_tlsext_use_srtp(dtls_ptr, "SRTP_AES128_CM_SHA1_80") == 0);
		}


		return  dtls_ptr;
	}

#pragma GCC diagnostic pop
    //////////////////////////////////////////////////////////////////////////////////




	cdtls_session::~cdtls_session()
	{
	}

	bool cdtls_session::init()
	{
		m_dtls_ctx_ptr = build_dtls_ctx();

		if ((m_dtls_ptr = SSL_new(m_dtls_ctx_ptr)) == NULL)
		{
			WARNING_EX_LOG("OpenSSL: SSL_new failed !!!");
			return false;
		}
		// Set this as custom data.
		// openssl中提供保持数据的的api
		//   1、使用SSL_set_ex_data(SSL *s, int idx, void *arg)  是根据小标使用保持数据的 
		//       s  :  第一个参数openssl的变量
		//       idx:  第二个参数是保持数据的位置索引 
		//       arg:  第三个参数的保持数据的参数
		//   2、使用  void *SSL_get_ex_data(const SSL *s, int idx) 函数是根据下标索引取openssl中保持数据
		//       s     :    openssl的变量
		//       idx   :    openssl中保持数据的下标索引
		//      return ：   返回openssl中小标保持数据 
		SSL_set_ex_data(m_dtls_ptr, 0, static_cast<void*>(this));
		SSL_set_info_callback(m_dtls_ptr, on_ssl_info);


		// set dtls fragment
	// @see https://stackoverflow.com/questions/62413602/openssl-server-packets-get-fragmented-into-270-bytes-per-packet
		SSL_set_options(m_dtls_ptr, SSL_OP_NO_QUERY_MTU);
		SSL_set_mtu(m_dtls_ptr, DTLS_FRAGMENT_MAX_SIZE);
		DTLS_set_link_mtu(m_dtls_ptr, DTLS_FRAGMENT_MAX_SIZE);



		// Set callback handler for setting DTLS timer interval.
		//DTLS_set_timer_cb(this->ssl, onSslDtlsTimer);
		// @see https://linux.die.net/man/3/openssl_version_number
		//                MM NN FF PP S
		// 0x1010102fL = 0x1 01 01 02 fL            // 1.1.1b release
		//   MM(major) = 0x1                        // 1.*
		//     NN(minor) = 0x01                     // 1.1.*
		//          FF(fix) = 0x01                  // 1.1.1*
		//     PP(patch) = 'a' + 0x02 - 1 = 'b'     // 1.1.1b *
		//              S(status) = 0xf = release   // 1.1.1b release
		// @note Status 0 for development, 1 to e for betas 1 to 14, and f for release.
	#if OPENSSL_VERSION_NUMBER >= 0x1010102fL // 1.1.1b
		DTLS_set_timer_cb(m_dtls_ptr, on_dtls_timer_cb);
	#endif


		if ((m_bio_in_ptr = BIO_new(BIO_s_mem())) == NULL) 
		{
			WARNING_EX_LOG("OpenSSL:  BIO_new in failed  !!!");
			return false;
			 
		}

		if ((m_bio_out_ptr = BIO_new(BIO_s_mem())) == NULL) 
		{
			BIO_free(m_bio_in_ptr);
			WARNING_EX_LOG("OpenSSL:  BIO_new out failed  !!!");
			return false;
		}

		SSL_set_bio(m_dtls_ptr, m_bio_in_ptr, m_bio_out_ptr);
		 

		return true;
	}

	void cdtls_session::destroy()
	{
		if (!m_handshake_done_for_us)
		{
			WARNING_EX_LOG("DTLS: Hang, done=%u,   arq=%u", m_handshake_done_for_us,  m_nn_arq_packets);
		}

		if (m_dtls_ctx_ptr)
		{
			SSL_CTX_free(m_dtls_ctx_ptr);
			m_dtls_ctx_ptr = NULL;
		}
		if (m_dtls_ptr)
		{
			//////////////////////////////////////////////////
			/// this function will free bio_in and bio_out ///
			//////////////////////////////////////////////////
			SSL_free(m_dtls_ptr);
			m_dtls_ptr = NULL;
		}
	}

	int32 cdtls_session::on_dtls(char * data, int32 nb_data)
	{
		int32 err = 0;
		if ((err = do_on_dtls(data, nb_data)) != 0) 
		{
			WARNING_EX_LOG("on_dtls size=%u, data=[%s]", nb_data, str2hex(data, nb_data, 32).c_str());
			return err;
		}
		return err;
	}

	int32 cdtls_session::do_on_dtls(char * data, int32 nb_data)
	{
		int32 err = 0;
		// When already done, only for us, we still got message from client,
	    // it might be our response is lost, or application data.
		if (m_handshake_done_for_us) 
		{
			NORMAL_EX_LOG("DTLS: After done, got %d bytes", nb_data);
		}

		int r0 = 0;
		// TODO: FIXME: Why reset it before writing?
		if ((r0 = BIO_reset(m_bio_in_ptr)) != 1) 
		{
			WARNING_EX_LOG("BIO_reset io_in r0=%d", r0);
			return EOpenSslBIOReset; 
		}
		if ((r0 = BIO_reset(m_bio_out_ptr)) != 1) 
		{
			WARNING_EX_LOG("BIO_reset io out r0=%d", r0);
			return EOpenSslBOUTReset;
		}

		// Trace the detail of DTLS packet.
		state_trace((uint8*)data, nb_data, true, r0, SSL_ERROR_NONE, false);

		if ((r0 = BIO_write(m_bio_in_ptr, data, nb_data)) <= 0) 
		{
			// TODO: 0 or -1 maybe block, use BIO_should_retry to check.
			WARNING_EX_LOG("BIO_write io in r0=%d", r0);
			return EOpenSslBIOWrite; 
		}

		// Always do handshake, even the handshake is done, because the last DTLS packet maybe dropped,
		// so we thought the DTLS is done, but client need us to retransmit the last packet.
		if ((err = do_handshake()) != 0) 
		{
			WARNING_EX_LOG("do handshake failed !!!");

			return err; 
		}

		// If there is data in bio_in, read it to let SSL consume it.
		// @remark Limit the max loop, to avoid the dead loop.
		for (int32 i = 0; i < 1024 && BIO_ctrl_pending(m_bio_in_ptr) > 0; i++) 
		{
			char buf[8092];
			int32 r0 = SSL_read(m_dtls_ptr, buf, sizeof(buf));
			int32 r1 = SSL_get_error(m_dtls_ptr, r0);

			if (r0 <= 0) 
			{
				// SSL_ERROR_ZERO_RETURN
				//
				// The TLS/SSL connection has been closed. If the protocol version is SSL 3.0 or higher,
				// this result code is returned only if a closure alert has occurred in the protocol,
				// i.e. if the connection has been closed cleanly.
				// @see https://www.openssl.org/docs/man1.1.0/man3/SSL_get_error.html
				// @remark Already close, never read again, because padding always exsists.
				if (r1 != SSL_ERROR_WANT_READ && r1 != SSL_ERROR_WANT_WRITE) 
				{
					break;
				}

				// We got data in memory, which can not read by SSL_read, generally, it's handshake data.
				uint8_t* data = NULL;
				int size = BIO_get_mem_data(m_bio_out_ptr, (char**)&data);

				// Logging when got SSL original data.
				state_trace((uint8_t*)data, size, false, r0, r1, false);
				// TODO@chensong 2022-01-30  DTSL 数据回写到应用层
				if (size > 0 /*&& (err = callback_->write_dtls_data(data, size)) != 0*/) 
				{
					/*return srs_error_wrap(err, "dtls send size=%u, data=[%s]", size,
						srs_string_dumps_hex((char*)data, size, 32).c_str());*/

					WARNING_EX_LOG("dtls send size=%u, data=[%s]", size, str2hex((char*)data, size, 32).c_str());
					return err;
				}
				continue;
			}

			NORMAL_EX_LOG("DTLS: read r0=%d, r1=%d, padding=%d, done=%d, data=[%s]",
				r0, r1, BIO_ctrl_pending(m_bio_in_ptr), m_handshake_done_for_us, str2hex(buf, r0, 32).c_str());

			// webrtc ---> datachannel 
			/*if ((err = callback_->on_dtls_application_data(buf, r0)) != 0)
			{

				WARNING_EX_LOG("on DTLS data, done=%d, r1=%d, size=%u, data=[%s]", m_handshake_done_for_us,
					r1, r0, str2hex(buf, r0, 32).c_str());
				return err;
			}*/
		}

		return err;
	}

	int32 cdtls_session::do_handshake()
	{
		int32 err = 0;
		// Done for use, ignore handshake packets. If need to ARQ the handshake packets,
	// we should use SSL_read to handle it.
		if (m_handshake_done_for_us)
		{
			return err;
		}

		// Do handshake and get the result.
		int32 r0 = SSL_do_handshake(m_dtls_ptr);
		int32 r1 = SSL_get_error(m_dtls_ptr, r0);

		// Fatal SSL error, for example, no available suite when peer is DTLS 1.0 while we are DTLS 1.2.
		if (r0 < 0 && (r1 != SSL_ERROR_NONE && r1 != SSL_ERROR_WANT_READ && r1 != SSL_ERROR_WANT_WRITE)) 
		{
			WARNING_EX_LOG("handshake r0=%d, r1=%d", r0, r1);
			return EOpenSslRTCDTLS;  
		}

		// OK, Handshake is done, note that it maybe done many times.
		if (r1 == SSL_ERROR_NONE) 
		{
			m_handshake_done_for_us = true;
		}

		// The data to send out to peer.
		uint8* data = NULL;
		int32 size = BIO_get_mem_data(m_bio_in_ptr, (char**)&data);

		// Logging when got SSL original data.
		state_trace((uint8*)data, size, false, r0, r1, false);

		// Callback for the final output data, before send-out.
		if ((err = on_final_out_data(data, size)) != 0) 
		{
			WARNING_EX_LOG("handler");
			return err;
			 
		}
		// 
		if (size > 0 /*&& (err = callback_->write_dtls_data(data, size)) != 0*/) 
		{
			WARNING_EX_LOG(  "dtls send size=%u, data=[%s]", size, str2hex((char*)data, size, 32).c_str());
			return err;
		}

		if (m_handshake_done_for_us) 
		{
			if (((err = on_handshake_done()) != 0)) 
			{
				WARNING_EX_LOG("done");
				return err;
			}
		}

		return err;
	}

	void cdtls_session::state_trace(uint8  * data, int32 length, bool incoming, int32 r0, int32 r1, bool arq)
	{
		// change_cipher_spec(20), alert(21), handshake(22), application_data(23)
		// @see https://tools.ietf.org/html/rfc2246#section-6.2.1
		uint8 content_type = 0;
		if (length >= 1)
		{
			content_type = (uint8_t)data[0];
		}

		uint16 size = 0;
		if (length >= 13) 
		{
			size = uint16(data[11]) << 8 | uint16(data[12]);
		}

		uint8 handshake_type = 0;
		if (length >= 14) 
		{
			handshake_type = (uint8)data[13];
		}

		NORMAL_EX_LOG("DTLS: State %s %s, done=%u, arq=%u/%u, r0=%d, r1=%d, len=%u, cnt=%u, size=%u, hs=%u",
			(is_dtls_client() ? "Active" : "Passive"), (incoming ? "RECV" : "SEND"), m_handshake_done_for_us, arq,
			m_nn_arq_packets, r0, r1, length, content_type, size, handshake_type);
	}

	int32 cdtls_session::get_srtp_key(std::string & recv_key, std::string & send_key)
	{
		int32 err = 0;
		unsigned char material[SRTP_MASTER_KEY_LEN * 2] = { 0 };  // client(SRTP_MASTER_KEY_KEY_LEN + SRTP_MASTER_KEY_SALT_LEN) + server
		static const std::string dtls_srtp_lable = "EXTRACTOR-dtls_srtp";
		if (!SSL_export_keying_material(m_dtls_ptr, material, sizeof(material), dtls_srtp_lable.c_str(), dtls_srtp_lable.size(), NULL, 0, 0))
		{
			WARNING_EX_LOG("SSL export key r0=%lu", ERR_get_error());
			return EOpenSslRTCSRTPINIT;  
		}

		size_t offset = 0;

		std::string client_master_key(reinterpret_cast<char*>(material), SRTP_MASTER_KEY_KEY_LEN);
		offset += SRTP_MASTER_KEY_KEY_LEN;
		std::string server_master_key(reinterpret_cast<char*>(material + offset), SRTP_MASTER_KEY_KEY_LEN);
		offset += SRTP_MASTER_KEY_KEY_LEN;
		std::string client_master_salt(reinterpret_cast<char*>(material + offset), SRTP_MASTER_KEY_SALT_LEN);
		offset += SRTP_MASTER_KEY_SALT_LEN;
		std::string server_master_salt(reinterpret_cast<char*>(material + offset), SRTP_MASTER_KEY_SALT_LEN);

		if (is_dtls_client()) 
		{
			recv_key = server_master_key + server_master_salt;
			send_key = client_master_key + client_master_salt;
		}
		else
		{
			recv_key = client_master_key + client_master_salt;
			send_key = server_master_key + server_master_salt;
		}
		return err;
	}

	void cdtls_session::callback_by_ssl(std::string type, std::string desc)
	{
		// global delete SSL 
		// int32 err = 0;
		/*if ((err = callback_->on_dtls_alert(type, desc)) != 0)
		{
			 warn2(TAG_DTLS_ALERT, "DTLS: handler alert err %s",  error_desc(err).c_str());
			 freep(err);
		}*/
	}

	cdtls_server::~cdtls_server()
	{
	}

	cdtls_client::~cdtls_client()
	{
	}

	bool cdtls_client::init()
	{
		if (!cdtls_session::init())
		{
			WARNING_EX_LOG("dtls session init failed !!!");
			return false;
		}


		// Dtls setup active, as client role.
		SSL_set_connect_state(m_dtls_ptr);
		SSL_set_max_send_fragment(m_dtls_ptr, DTLS_FRAGMENT_MAX_SIZE);

		return true;
	}

	void cdtls_client::update(uint32 uDeltaTime)
	{
		// Limit the max retry for ARQ, to avoid infinite loop.
	// Note that we set the timeout to [50, 100, 200, 400, 800, 1600, 3200, 6400, 12800, 25600, 51200] in ms,
	// but the actual timeout is limit to 1s:
	//      50ms, 100ms, 200ms, 400ms, 800ms, (1000ms,600ms), (200ms,1000ms,1000ms,1000ms),
	//      (400ms,1000ms,1000ms,1000ms,1000ms,1000ms,1000ms), ...
	// So when the max ARQ limit to 12 times, the max loop is about 103.
	// @remark We change the max sleep to 100ms, so we limit about (103*10)/2=500.
		const int max_loop = 512;

		int32 arq_count = 0;
		for (int32 i = 0; arq_count < m_arq_max_retry && i < max_loop; i++) 
		{
			// We ignore any error for ARQ thread.
			/*if ((err = trd->pull()) != 0) {
				srs_freep(err);
				return err;
			}*/

			// If done, should stop ARQ.
			if (m_handshake_done_for_us)
			{
				return;// err;
			}

			// For DTLS client ARQ, the state should be specified.
			if (m_state  != EDtlsStateClientHello && m_state != EDtlsStateClientCertificate)
			{
				return;// err;
			}

			// If there is a timeout in progress, it sets *out to the time remaining
			// and returns one. Otherwise, it returns zero.
			int32 r0 = 0; timeval to = { 0 };
			if ((r0 = DTLSv1_get_timeout(m_dtls_ptr, &to)) == 0)
			{
				//// The time unit in ms, for example 100 * SRS_UTIME_MILLISECONDS means 100ms.
				//#define  UTIME_MILLISECONDS 1000
				// No timeout, for example?, wait for a default 50ms.
				//std::this_thread::sleep_for(std::chrono::milliseconds(TICK_TIME - uDelta))
				std::this_thread::sleep_for(std::chrono::microseconds(50 * 1000));
				//usleep(50 * UTIME_MILLISECONDS);
				continue;
			}
			uint32 timeout = to.tv_sec + to.tv_usec;

			// There is timeout to wait, so we should wait, because there is no packet in openssl.
			if (timeout > 0) {
				// Never wait too long, because we might need to retransmit other messages.
				// For example, we have transmit 2 ClientHello as [50ms, 100ms] then we sleep(200ms),
				// during this we reset the openssl timer to 50ms and need to retransmit Certificate,
				// we still need to wait 200ms not 50ms.
				timeout = (100 * 1000) > timeout ?  timeout:(100 * 1000);// min(, timeout);
				timeout = (50 * 1000) > timeout ?  (50 * 1000): timeout ;///max(50 * 1000, timeout);
				 //usleep(timeout);
				std::this_thread::sleep_for(std::chrono::microseconds(timeout));
				continue;
			}

			// The timeout is 0, so there must be a ARQ packet to transmit in openssl.
			r0 = BIO_reset(m_bio_out_ptr); 
			int32 r1 = SSL_get_error(m_dtls_ptr, r0);
			if (r0 != 1)
			{
				WARNING_EX_LOG("BIO_reset r0=%d, r1=%d", r0, r1);
				return;
				//return  error_new( OpenSslBIOReset, "BIO_reset r0=%d, r1=%d", r0, r1);
			}

			// DTLSv1_handle_timeout is called when a DTLS handshake timeout expires. If no timeout
			// had expired, it returns 0. Otherwise, it retransmits the previous flight of handshake
			// messages and returns 1. If too many timeouts had expired without progress or an error
			// occurs, it returns -1.
			r0 = DTLSv1_handle_timeout(m_dtls_ptr);
			r1 = SSL_get_error(m_dtls_ptr, r0);
			if (r0 == 0)
			{
				continue; // No timeout had expired.
			}
			if (r0 != 1)
			{
				WARNING_EX_LOG("ARQ r0=%d, r1=%d", r0, r1);
				return;
				//return  error_new(ERROR_RTC_DTLS, "ARQ r0=%d, r1=%d", r0, r1);
			}

			// The data to send out to peer.
			uint8 * data = NULL;
			int32 size = BIO_get_mem_data(m_bio_in_ptr, (char**)&data);

			arq_count++;
			m_nn_arq_packets++;
			state_trace((uint8_t*)data, size, false, r0, r1, true);

			if (size > 0 /*&& (err = callback_->write_dtls_data(data, size)) != 0*/) 
			{
				WARNING_EX_LOG("dtls send size = %u, data = [%s]", size,
					str2hex((char*)data, size, 32).c_str());
				 //return;
			}
		}
	}

	void cdtls_client::destroy()
	{

	}

	int32 cdtls_client::start_active_handshake()
	{
		int32 err = 0;
		if ((err = do_handshake()) != 0) 
		{
			WARNING_EX_LOG("start handshake failed !!!");
			return err;
			 
		}

		if ((err = start_arq()) != 0) 
		{
			WARNING_EX_LOG("start arq failed !!!");
			return err; 
		}

		return err;
	}

	bool cdtls_client::should_reset_timer()
	{
		bool v = m_reset_timer;
		m_reset_timer = false;
		return v;
	}
	// Note that only handshake sending packets drives the state, neither ARQ nor the
	// final-packets(after handshake done) drives it.
	int32 cdtls_client::on_final_out_data(uint8 * data, int size)
	{
		int32 err = 0;
		// If we are sending client hello, change from init to new state.
		if (m_state == EDtlsStateInit && size > 14 && data[0] == 22 && data[13] == 1) 
		{
			m_state = EDtlsStateClientHello;
			return err;
		}

		// If we are sending certificate, change from SrsDtlsStateClientHello to new state.
		if (m_state == EDtlsStateClientHello && size > 14 && data[0] == 22 && data[13] == 11) 
		{
			m_state = EDtlsStateClientCertificate;

			// When we send out the certificate, we should reset the timer.
			m_reset_timer  = true;
			NORMAL_EX_LOG("DTLS: Reset the timer for ServerHello");
			return err;
		}

		return err;
	}

	int32 cdtls_client::on_handshake_done()
	{
		int32 err = 0;
		// Ignore if done.
		if (m_state == EDtlsStateClientDone)
		{
			return err;
		}

		// Change to done state.
		m_state = EDtlsStateClientDone;

		// When handshake done, stop the ARQ.
		stop_arq();

		// Notify connection the DTLS is done.
		//if (((err = callback_->on_dtls_handshake_done()) != 0))
		{
			WARNING_EX_LOG("dtls done !!!");
			//return  error_wrap(err, "dtls done");
		}

		return err;
	}

	bool cdtls_client::is_dtls_client()
	{
		return true;
	}

	int32 cdtls_client::start_arq()
	{
		return int32();
	}

	void cdtls_client::stop_arq()
	{
	}

}