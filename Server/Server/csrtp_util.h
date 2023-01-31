/***********************************************************************************************
created: 		2023-01-31

author:			chensong

purpose:		csrtp


************************************************************************************************/

#ifndef _C_SRTP_UTIL_H_
#define _C_SRTP_UTIL_H_

#include "cnet_type.h"
#include <srtp2/srtp.h>

namespace chen {

	const char * get_srtp_error_desc(srtp_err_status_t code);
}


#endif // _C_SRTP_UTIL_H_