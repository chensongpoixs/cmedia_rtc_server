/***********************************************************************************************
created: 		2023-01-31

author:			chensong

purpose:		csrtp 


************************************************************************************************/

#ifndef _C_SRTP_H_
#define _C_SRTP_H_

#include "cnet_type.h"
#include <srtp2/srtp.h>

namespace chen {


	class csrtp 
	{
	public:
		explicit csrtp()
		: m_recv_key_ptr(NULL)
		, m_send_key_ptr(NULL)
		, m_recv_ctx_ptr(NULL)
		, m_send_ctx_ptr(NULL){}
		~csrtp();


	public:
	public:
		// Intialize srtp context with recv_key and send_key.
		bool init(const std::string &recv_key, const std::string &send_key);

		void destroy();


	public:
		int32 protect_rtp(void* packet, int32* nb_cipher);
		int32 protect_rtcp(void* packet, int32* nb_cipher);
		bool unprotect_rtp(void* packet, int32* nb_plaintext);
		bool unprotect_rtcp(void* packet, int32* nb_plaintext);
	protected:
	private:


		uint8	*			m_recv_key_ptr;
		uint8*				m_send_key_ptr;


		srtp_t				m_recv_ctx_ptr;
		srtp_t				m_send_ctx_ptr;




	};

}

#endif // _C_SRTP_H_