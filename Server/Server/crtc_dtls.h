/***********************************************************************************************
created: 		2023-02-17

author:			chensong

purpose:		_C_DTLS_ _H_


************************************************************************************************/

#ifndef _C_RTC_DTLS_H_
#define _C_RTC_DTLS_H_
#include "cnet_type.h"
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include "crtc_sdp.h"
#include "cmedia_desc.h"
//#include "crtc_transport.h"
#include <stdio.h>
#include <stdlib.h>
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
#include "cdtls_define.h"
#include "csrtp.h"
#include "csrtp_session.h"
#include "csrtp_define.h"
namespace chen {
	class crtc_transportlinster;
	 class crtc_dtls 
	 {
	 public:
		 explicit crtc_dtls(crtc_transportlinster * callback)
		 : m_callback_ptr(callback)
		 , m_ssl_ptr(NULL)
		 , m_ssl_bio_read_network_ptr(NULL)
		 , m_ssl_bio_write_network_ptr(NULL)
		 , m_dtls_status(EDtlsStateInit)
		 , m_handshake_done_for_us(false)
		 , m_role("")
		 , m_remote_cert(""){}
		virtual  ~crtc_dtls();

	 public:

		 /*static bool init_global_ssl_ctx();
		 static void destroy_global_ssl_ctx();*/

		////////////////////////global SSL_CTX //////////////////////////////////
		//static SSL_CTX		*				g_ssl_ctx_ptr;
	 public:



		 bool init();
		 void update(uint32 uDeltaTime);
		 void destroy();



		 
	 public:
		 bool start_active_handshake(const std::string & type);

		 void process_dtls_data(const uint8* data, int32 len);
	 public:

		 void send_pending_outgoing_dtlsdata();

		 inline bool set_timeout();

	 public:
		 inline bool process_handshake();
	 public:

		 // callbacks fired by OpenSSL events 

		 void on_ssl_info(int32 where , int32 ret);

	 private:
		 inline bool _check_status(int32 return_code);
		 inline bool _check_remote_fingerprintf();
		 inline ECRYPTO_SUITE _get_netotiated_srtp_crypto_suite();
		 void _extract_srtp_keys(ECRYPTO_SUITE srtpCryptoSuite);
	 protected:
	 private:
		 crtc_transportlinster	*			m_callback_ptr;
		 SSL*								m_ssl_ptr;
		 BIO *								m_ssl_bio_read_network_ptr;
		 BIO *								m_ssl_bio_write_network_ptr;
		 EDtlsState							m_dtls_status;

		 // Whether the handshake is done, for us only.
		// @remark For us only, means peer maybe not done, we also need to handle the DTLS packet.
		 bool								m_handshake_done_for_us;
		 std::string						m_role;
		 std::string						m_remote_cert;

	 };
}


#endif // _C_DTLS_TEST_H_