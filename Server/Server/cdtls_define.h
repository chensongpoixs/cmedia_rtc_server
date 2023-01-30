/***********************************************************************************************
created: 		2023-01-30

author:			chensong

purpose:		_C_DTLS_CERTIFICATE_H_


************************************************************************************************/

#ifndef _C_DTLS_DEFINE_H_
#define _C_DTLS_DEFINE_H_
#include "cnet_type.h"

namespace chen {



	enum EOPENSSL
	{
		EOpenSslBIOReset =  100,
		EOpenSslBOUTReset,
		EOpenSslBIOWrite,
		EOpenSslRTCDTLS,
		EOpenSslRTCSRTPINIT,
	};


	// The state for DTLS client.
	enum EDtlsState
	{
		EDtlsStateInit, // Start.
		EDtlsStateClientHello, // Should start ARQ thread.
		EDtlsStateServerHello, // We are in the first ARQ state.
		EDtlsStateClientCertificate, // Should start ARQ thread again.
		EDtlsStateServerDone, // We are in the second ARQ state.
		EDtlsStateClientDone, // Done.
	};
}

#endif // _C_DTLS_DEFINE_H_