﻿/***********************************************************************************************
created: 		2023-01-31

author:			chensong

purpose:		csrtp


************************************************************************************************/
#include "csrtp_util.h"
#include <vector>

namespace chen
{
	// clang-format off
	static const  std::vector<const char*> g_srtp_global_errors =
	{
		// From 0 (srtp_err_status_ok) to 24 (srtp_err_status_pfkey_err).
		"success (srtp_err_status_ok)",
		"unspecified failure (srtp_err_status_fail)",
		"unsupported parameter (srtp_err_status_bad_param)",
		"couldn't allocate memory (srtp_err_status_alloc_fail)",
		"couldn't deallocate memory (srtp_err_status_dealloc_fail)",
		"couldn't initialize (srtp_err_status_init_fail)",
		"can’t process as much data as requested (srtp_err_status_terminus)",
		"authentication failure (srtp_err_status_auth_fail)",
		"cipher failure (srtp_err_status_cipher_fail)",
		"replay check failed (bad index) (srtp_err_status_replay_fail)",
		"replay check failed (index too old) (srtp_err_status_replay_old)",
		"algorithm failed test routine (srtp_err_status_algo_fail)",
		"unsupported operation (srtp_err_status_no_such_op)",
		"no appropriate context found (srtp_err_status_no_ctx)",
		"unable to perform desired validation (srtp_err_status_cant_check)",
		"can’t use key any more (srtp_err_status_key_expired)",
		"error in use of socket (srtp_err_status_socket_err)",
		"error in use POSIX signals (srtp_err_status_signal_err)",
		"nonce check failed (srtp_err_status_nonce_bad)",
		"couldn’t read data (srtp_err_status_read_fail)",
		"couldn’t write data (srtp_err_status_write_fail)",
		"error parsing data (srtp_err_status_parse_err)",
		"error encoding data (srtp_err_status_encode_err)",
		"error while using semaphores (srtp_err_status_semaphore_err)",
		"error while using pfkey (srtp_err_status_pfkey_err)"
	};
	const char * get_srtp_error_desc(srtp_err_status_t code)
	{
		return g_srtp_global_errors.at(code);
	}
}