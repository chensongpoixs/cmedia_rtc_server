/***********************************************************************************************
created: 		2023-01-31

author:			chensong

purpose:		csrtp

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
		policy.window_size = 8192; // 8192; // 1024
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

		return true;
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
	bool csrtp::unprotect_rtp(void * packet, int32 * nb_plaintext)
	{
		int32 err = 0;

		// If DTLS/SRTP is not ready, fail.
		if (!m_recv_ctx_ptr) 
		{
			WARNING_EX_LOG("recv ctx ptr not ready");
			//return ESrtpUnProject;
			return false;
		}

		srtp_err_status_t r0 = srtp_err_status_ok;
		if ((r0 = srtp_unprotect(m_recv_ctx_ptr, packet, nb_plaintext)) != srtp_err_status_ok)
		{
			WARNING_EX_LOG("rtp unprotect r0=%u, [len = %u]", r0, *nb_plaintext);
			//return ESrtpUnProject; 
			return false;
		}
		return true;
		return err;
	}
	bool csrtp::unprotect_rtcp(void * packet, int32 * nb_plaintext)
	{
		int32  err = 0;
		// If DTLS/SRTP is not ready, fail.
		if (!m_recv_ctx_ptr) 
		{
			WARNING_EX_LOG("recv ctx ptr not ready");
			//return ESrtpUnProject;
			return false;
		}

		srtp_err_status_t r0 = srtp_err_status_ok;
		if ((r0 = srtp_unprotect_rtcp(m_recv_ctx_ptr, packet, nb_plaintext)) != srtp_err_status_ok) 
		{
			WARNING_EX_LOG("rtp unprotect r0=%u", r0);
			//return ESrtpUnProject;
			return false;
		}
		return true;
		//return err;
	}
}