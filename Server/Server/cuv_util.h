/***********************************************************************************************
created: 		2022-08-11

author:			chensong

purpose:		cuv_util
************************************************************************************************/


#ifndef _C_UV_UTIL_H_
#define _C_UV_UTIL_H_

#include "clog.h"
namespace chen {
	namespace uv_util 
	{
		bool init();
		void destroy();

		void print_version();

		void run_loop();


		uv_loop_t* get_loop();

		uint64_t GetTimeMs();

		uint64_t GetTimeUs();

		uint64_t GetTimeNs();

		int64_t GetTimeMsInt64();

		int64_t GetTimeUsInt64();
	}
}

#endif // _C_UV_UTIL_H_