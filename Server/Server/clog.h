/***********************************************************************************************
created: 		2019-03-02

author:			chensong

purpose:		log
************************************************************************************************/


#ifndef _C_LOG_H_
#define _C_LOG_H_
#include "cnet_type.h"
#include <cstdio>
#include <cstdlib>
#include "cnoncopyable.h"
#include "casync_log.h"
#include "clog_define.h"
#include <uv.h>
///>uv_a.lib(util.obj) : error LNK2019: 无法解析的外部符号 __imp_GetUserProfileDirectoryW，该符号在函数 uv__getpwuid_r 中被引用 
#pragma comment(lib,"ws2_32.lib")
#pragma comment (lib,"Advapi32.lib")
#pragma comment (lib,"Iphlpapi.lib")
#pragma comment(lib, "Psapi.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "userenv.lib")
 
namespace chen {

	class clog : private cnoncopyable
	{
	private:
		enum { EBuf_Size = 1024 };
	public:

		explicit clog();
		explicit clog(ELogLevelType level);
		explicit clog(ELogLevelType level, const char* func, int line);
		~clog();
	public:
		static bool init(const std::string& path, const std::string& name, const std::string& ext = ".log"
			, ELogNameType name_type = ELogName_DateTime, bool mod_append = false, bool show_screen = true);
		static void fix_log(ELogLevelType level, const void* p, int len);
		static void var_log(ELogLevelType level, const char* format, ...);

		static void set_level(ELogLevelType level);
		static void destroy();
	public:
		clog& operator<<(bool);
		clog& operator<<(char);

		clog& operator<<(signed char);
		clog& operator<<(unsigned char);

		clog& operator<<(signed short);
		clog& operator<<(unsigned short);

		clog& operator<<(signed int);
		clog& operator<<(unsigned int);

		clog& operator<<(signed long);
		clog& operator<<(unsigned long);

		clog& operator<<(signed long long);
		clog& operator<<(unsigned long long);

		clog& operator<<(const char *);
		clog& operator<<(const std::string&);

		clog& operator<<(float);
		clog& operator<<(double);
	private:
		char m_data[EBuf_Size];
		int  m_len;
		ELogLevelType m_level;

	};


#define LOG clog

#define SLOG LOG()



#if defined(_MSC_VER)
#define FUNCTION __FUNCTION__

#elif defined(__GNUC__)
#define FUNCTION __PRETTY_FUNCTION__

#else
#pragma error "unknow platform!!!"

#endif
 


	// Usage:
	//   MS_DEBUG_DEV("Leading text "MS_UINT16_TO_BINARY_PATTERN, MS_UINT16_TO_BINARY(value));
#define MS_UINT16_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c"
#define MS_UINT16_TO_BINARY(value) \
	((value & 0x8000) ? '1' : '0'), \
	((value & 0x4000) ? '1' : '0'), \
	((value & 0x2000) ? '1' : '0'), \
	((value & 0x1000) ? '1' : '0'), \
	((value & 0x800) ? '1' : '0'), \
	((value & 0x400) ? '1' : '0'), \
	((value & 0x200) ? '1' : '0'), \
	((value & 0x100) ? '1' : '0'), \
	((value & 0x80) ? '1' : '0'), \
	((value & 0x40) ? '1' : '0'), \
	((value & 0x20) ? '1' : '0'), \
	((value & 0x10) ? '1' : '0'), \
	((value & 0x08) ? '1' : '0'), \
	((value & 0x04) ? '1' : '0'), \
	((value & 0x02) ? '1' : '0'), \
	((value & 0x01) ? '1' : '0')


#define LOG clog


//标准log 有时间前缀
#define LOG_SYSTEM LOG(ELogLevel_System)
#define LOG_FATAL  LOG(ELogLevel_Fatal, FUNCTION, __LINE__)
#define LOG_ERROR  LOG(ELogLevel_Error, FUNCTION, __LINE__)
#define LOG_WARN   LOG(ELogLevel_Warn, FUNCTION, __LINE__)
#define LOG_INFO   LOG(ELogLevel_Info)
#define LOG_DEBUG  LOG(ELogLevel_Debug)

#define VAR_LOG LOG::var_log

#define NORMAL_LOG(format, ...)		VAR_LOG(ELogLevel_Info, format, ##__VA_ARGS__)
#define ERROR_LOG(format, ...)		VAR_LOG(ELogLevel_Error, format, ##__VA_ARGS__)
#define WARNING_LOG(format, ...)	VAR_LOG(ELogLevel_Warn, format, ##__VA_ARGS__)
#define SYSTEM_LOG(format, ...)		VAR_LOG(ELogLevel_System, format, ##__VA_ARGS__)
#define DEBUG_LOG(format, ...)		VAR_LOG(ELogLevel_Debug, format, ##__VA_ARGS__)

#define NORMAL_EX_LOG(format, ...)	NORMAL_LOG("[%s][%d]" format, FUNCTION, __LINE__, ##__VA_ARGS__)
#define DEBUG_EX_LOG(format, ...)	DEBUG_LOG("[%s][%d]" format, FUNCTION, __LINE__, ##__VA_ARGS__)
#define WARNING_EX_LOG(format, ...)	WARNING_LOG("[%s][%d]" format, FUNCTION, __LINE__, ##__VA_ARGS__)

#define ERROR_EX_LOG(format, ...)	ERROR_LOG("[%s][%d]" format, FUNCTION, __LINE__, ##__VA_ARGS__)


#define cassert(expression )  \
		if (!(!!(expression))) \
		{ \
			ERROR_EX_LOG("assert : %s   " , #expression  ); \
			assert(expression); \
		}

#define cassert_desc(expression, desc, ...)  \
		if (!(!!(expression))) \
		{ \
			ERROR_EX_LOG(desc, ##__VA_ARGS__); \
			ERROR_EX_LOG(" %s  , failed assert '%s'" , #expression, desc, ##__VA_ARGS__ ); \
			assert(expression); \
		}

#define RTC_UNREACHABLE_CODE_HIT false
#define RTC_NOTREACHED() cassert(RTC_UNREACHABLE_CODE_HIT)

// Helper macro for binary operators.
// Don't use this macro directly in your code, use RTC_CHECK_EQ et al below.
#define RTC_CHECK_OP(name, op, val1, val2)                               \
   cassert(((val1) op (val2)));

#define RTC_DCHECK(condition) cassert(condition)
#define RTC_CHECK_EQ(val1, val2) RTC_CHECK_OP(Eq, ==, val1, val2)
#define RTC_CHECK_NE(val1, val2) RTC_CHECK_OP(Ne, !=, val1, val2)
#define RTC_CHECK_LE(val1, val2) RTC_CHECK_OP(Le, <=, val1, val2)
#define RTC_CHECK_LT(val1, val2) RTC_CHECK_OP(Lt, <, val1, val2)
#define RTC_CHECK_GE(val1, val2) RTC_CHECK_OP(Ge, >=, val1, val2)
#define RTC_CHECK_GT(val1, val2) RTC_CHECK_OP(Gt, >, val1, val2)
	///////////////////////////////////////
#define RTC_DCHECK_EQ(v1, v2) RTC_CHECK_EQ(v1, v2)
#define RTC_DCHECK_NE(v1, v2) RTC_CHECK_NE(v1, v2)
#define RTC_DCHECK_LE(v1, v2) RTC_CHECK_LE(v1, v2)
#define RTC_DCHECK_LT(v1, v2) RTC_CHECK_LT(v1, v2)
#define RTC_DCHECK_GE(v1, v2) RTC_CHECK_GE(v1, v2)
#define RTC_DCHECK_GT(v1, v2) RTC_CHECK_GT(v1, v2)
} // namespace chen

#endif //!#define _C_LOG_H_