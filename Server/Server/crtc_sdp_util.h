/***********************************************************************************************
created: 		2022-09-04

author:			chensong

purpose:		crtc_sdp_util


************************************************************************************************/

#ifndef _C_RTC_SDP_UTIL_H_
#define _C_RTC_SDP_UTIL_H_
#include <string>
#include <vector>
#include "crtc_sdp_define.h"
#pragma warning (push)
#pragma warning (disable : 4005)
#include <intsafe.h>
#include <stdint.h>
#pragma warning (pop)
namespace chen {
	namespace rtc_sdp_util
	{

		// Remove leading and trailing whitespaces.
		std::string string_trim(const std::string& s);



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
		int cinet_pton(int af, const char* src, void* dst);

		bool is_rtp(const std::string &protocol);
		bool is_dtls_sctp(const std::string & protocol);

		/*template<typename C>
		C parse_content_description(const std::string & message, const MediaType media_type, int32_t mline_index, const std::string & protocol, const std::vector<int>& payload_types, size_t * pos, std::string * content_name, bool* bundle_only, int* msid_signaling)
		{

		}*/
		 
		 

	}
}


#endif // _C_RTC_SDP_UTIL_H_