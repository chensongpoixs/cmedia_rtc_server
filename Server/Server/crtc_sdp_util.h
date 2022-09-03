/***********************************************************************************************
created: 		2022-09-04

author:			chensong

purpose:		crtc_sdp_util


************************************************************************************************/

#ifndef _C_RTC_SDP_UTIL_H_
#define _C_RTC_SDP_UTIL_H_
#include <string>
#include <vector>

namespace chen {
	namespace rtc_sdp_util
	{
		size_t split(const std::string& source,
			char delimiter,
			std::vector<std::string>* fields);

		// Extract the first token from source as separated by delimiter, with
		// duplicates of delimiter ignored. Return false if the delimiter could not be
		// found, otherwise return true.
		bool tokenize_first(const std::string& source,
			const char delimiter,
			std::string* token,
			std::string* rest);
		int inet_pton(int af, const char* src, void* dst);

		bool is_rtp(const std::string &protocol);
		bool is_dtls_sctp(const std::string & protocol);
	}
}


#endif // _C_RTC_SDP_UTIL_H_