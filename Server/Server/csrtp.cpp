/***********************************************************************************************
created: 		2023-01-31

author:			chensong

purpose:		csrtp


************************************************************************************************/
#include "csrtp.h"
#include <string.h>

#include "csrtp_util.h"

#include "clog.h"
#include "csrtp_define.h"

namespace chen {
	csrtp::~csrtp()
	{
	}
	bool csrtp::init(const std::string & recv_key, const std::string & send_key)
	{
		int32 err = 0;
		srtp_policy_t policy;
		std::memset(&policy, 0, sizeof(srtp_policy_t));

		// TODO: Maybe we can use SRTP-GCM in future.
		// @see https://bugs.chromium.org/p/chromium/issues/detail?id=713701
		// @see https://groups.google.com/forum/#!topic/discuss-webrtc/PvCbWSetVAQ
		srtp_crypto_policy_set_aes_cm_128_hmac_sha1_80(&policy.rtp);
		srtp_crypto_policy_set_aes_cm_128_hmac_sha1_80(&policy.rtcp);

		policy.ssrc.value = 0;
		// TODO: adjust window_size
		policy.window_size = 8192;
		policy.allow_repeat_tx = 1;
		policy.next = NULL;

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// init recv context
		policy.ssrc.type = ssrc_any_inbound; 
		m_recv_key_ptr = static_cast<uint8*>(::malloc(sizeof(uint8) * recv_key.size()));
		memcpy(m_recv_key_ptr, recv_key.data(), recv_key.size());
		policy.key = m_recv_key_ptr;

		srtp_err_status_t r0 = srtp_err_status_ok;
		if ((r0 = srtp_create(&m_recv_ctx_ptr, &policy)) != srtp_err_status_ok)
		{  
			WARNING_EX_LOG("srtp create recv ctx [r0=%u] [des = %s]failed !!! ", r0, get_srtp_error_desc(r0));
			return  false; // error_new(ERROR_RTC_SRTP_INIT, "srtp create r0=%u", r0);
		}
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// init send context 
		policy.ssrc.type = ssrc_any_outbound; 
		m_send_key_ptr = static_cast<uint8*>(::malloc(sizeof(uint8) * send_key.size()));
		memcpy(m_send_key_ptr, send_key.data(), send_key.size()); 
		policy.key = m_send_key_ptr;

		if ((r0 = srtp_create(&m_send_ctx_ptr, &policy)) != srtp_err_status_ok) 
		{
			WARNING_EX_LOG("srtp create send ctx [r0=%u] [des = %s]failed !!! ", r0, get_srtp_error_desc(r0));
			return  false;
		}

		return err;
	}
	void csrtp::destroy()
	{
		if (m_recv_ctx_ptr)
		{
			srtp_dealloc(m_recv_ctx_ptr);
			m_recv_ctx_ptr = NULL;
		}
		if (m_send_ctx_ptr)
		{
			srtp_dealloc(m_send_ctx_ptr);
			m_send_ctx_ptr = NULL;
		}


		if (m_recv_key_ptr)
		{
			::free(m_recv_key_ptr);
			m_recv_key_ptr = NULL;
		}
		if (m_send_key_ptr)
		{
			::free(m_send_key_ptr);
			m_send_key_ptr = NULL;
		}
	}
	int32 csrtp::protect_rtp(void * packet, int32 * nb_cipher)
	{
		int32 err = 0;
		// If DTLS/SRTP is not ready, fail.
		if (!m_send_ctx_ptr)
		{
			WARNING_EX_LOG("send ctx ptr not ready");
			return ESrtpProject; 
		}

		srtp_err_status_t r0 = srtp_err_status_ok;
		if ((r0 = srtp_protect(m_send_ctx_ptr, packet, nb_cipher)) != srtp_err_status_ok) 
		{
			WARNING_EX_LOG("rtp protect r0=%u", r0);
			return ESrtpProject;
			 
		}

		return err;
	}
	int32 csrtp::protect_rtcp(void * packet, int32 * nb_cipher)
	{
		int32 err = 0;
		// If DTLS/SRTP is not ready, fail.
		if (!m_send_ctx_ptr) 
		{
			WARNING_EX_LOG("send ctx ptr not ready");
			return ESrtpProject;
		}

		srtp_err_status_t r0 = srtp_err_status_ok;
		if ((r0 = srtp_protect_rtcp(m_send_ctx_ptr, packet, nb_cipher)) != srtp_err_status_ok) 
		{
			WARNING_EX_LOG("rtcp protect r0=%u", r0);
			return ESrtpProject;
		}

		return err;
	}
	int32 csrtp::unprotect_rtp(void * packet, int32 * nb_plaintext)
	{
		int32 err = 0;

		// If DTLS/SRTP is not ready, fail.
		if (!m_recv_ctx_ptr) 
		{
			WARNING_EX_LOG("recv ctx ptr not ready");
			return ESrtpUnProject;
			 
		}

		srtp_err_status_t r0 = srtp_err_status_ok;
		if ((r0 = srtp_unprotect(m_recv_ctx_ptr, packet, nb_plaintext)) != srtp_err_status_ok)
		{
			WARNING_EX_LOG("rtp unprotect r0=%u", r0);
			return ESrtpUnProject; 
		}

		return err;
	}
	int32 csrtp::unprotect_rtcp(void * packet, int32 * nb_plaintext)
	{
		int32  err = 0;
		// If DTLS/SRTP is not ready, fail.
		if (!m_recv_ctx_ptr) 
		{
			WARNING_EX_LOG("recv ctx ptr not ready");
			return ESrtpUnProject;
		}

		srtp_err_status_t r0 = srtp_err_status_ok;
		if ((r0 = srtp_unprotect_rtcp(m_recv_ctx_ptr, packet, nb_plaintext)) != srtp_err_status_ok) 
		{
			WARNING_EX_LOG("rtp unprotect r0=%u", r0);
			return ESrtpUnProject;
		}

		return err;
	}
}