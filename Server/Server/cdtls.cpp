/***********************************************************************************************
created: 		2023-01-17

author:			chensong

purpose:		_C_DTLS_IMPL_H_


************************************************************************************************/
#include "cdtls.h"
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
#include "clog.h"
#include "cdtls_session.h"
namespace chen {
	cdtls::~cdtls()
	{
	}

	bool cdtls::init(const std::string  & role, crtc_transportlinster * callback)
	{
		if (role == "active")
		{
			m_dtls_session_ptr = new cdtls_client(callback);
		}
		else 
		{
			m_dtls_session_ptr = new cdtls_server(callback);
		}

		return m_dtls_session_ptr->init( );
	}

	void cdtls::destroy()
	{
		if (m_dtls_session_ptr)
		{
			m_dtls_session_ptr->destroy();
			delete m_dtls_session_ptr;
			m_dtls_session_ptr = NULL;
		}
	}

	int32 cdtls::start_active_handshake()
	{
		if (m_dtls_session_ptr)
		{
			return m_dtls_session_ptr->start_active_handshake();
		}
		WARNING_EX_LOG("m_dtls_session_ptr == NULL failed !!!");
		return -1;
	}

	int32 cdtls::on_dtls(char * data, int32 nb_data)
	{
		if (m_dtls_session_ptr)
		{
			return m_dtls_session_ptr->on_dtls(data, nb_data);
		}
		WARNING_EX_LOG("m_dtls_session_ptr == NULL failed !!!");
		return -1;
	}

	chen::int32 cdtls::get_srtp_key(std::string& recv_key, std::string& send_key)
	{
		if (m_dtls_session_ptr)
		{
			return m_dtls_session_ptr->get_srtp_key(recv_key, send_key);
		}
		WARNING_EX_LOG("m_dtls_session_ptr == NULL failed !!!");
		return -1;
	}

	//int32 cdtls::init()
	//{
	//	DEBUG_EX_LOG("Openssl version: %s", OpenSSL_version(OPENSSL_VERSION));
	//	//RAND_poll();
	//	 
	//	return true;
	//}
	//int32 cdtls::on_dtls(char * data, int32 nb_data)
	//{
	//	return int32();
	//}
	/*void cdtls::destory()
	{
	}*/
}