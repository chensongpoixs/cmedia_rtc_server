/***********************************************************************************************
created: 		2023-01-16

author:			chensong

purpose:		_C_SRTP_SESSION_TEST_H_


************************************************************************************************/
#include "csrtp_session.h"
#include "csrtp_session_test.h"

namespace chen {
	void srtp_session_test()
	{
		// global init
		csrtp_session::init();

		ECRYPTO_SUITE   srtp_crypto_suite;
		/*csrtp_session send_session(EOUTBOUND, );
		csrtp_session recv_session(EINBOUND, );*/

		// global destroy
		csrtp_session::destroy();

	}
}