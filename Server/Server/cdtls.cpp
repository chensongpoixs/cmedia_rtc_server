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

namespace chen {
	cdtls::~cdtls()
	{
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