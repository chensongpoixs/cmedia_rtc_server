/***********************************************************************************************
created: 		2019-03-02

author:			chensong

purpose:		log
输赢不重要，答案对你们有什么意义才重要。

光阴者，百代之过客也，唯有奋力奔跑，方能生风起时，是时代造英雄，英雄存在于时代。或许世人道你轻狂，可你本就年少啊。 看护好，自己的理想和激情。


我可能会遇到很多的人，听他们讲好2多的故事，我来写成故事或编成歌，用我学来的各种乐器演奏它。
然后还可能在一个国家遇到一个心仪我的姑娘，她可能会被我帅气的外表捕获，又会被我深邃的内涵吸引，在某个下雨的夜晚，她会全身淋透然后要在我狭小的住处换身上的湿衣服。
3小时候后她告诉我她其实是这个国家的公主，她愿意向父皇求婚。我不得已告诉她我是穿越而来的男主角，我始终要回到自己的世界。
然后我的身影慢慢消失，我看到她眼里的泪水，心里却没有任何痛苦，我才知道，原来我的心被丢掉了，我游历全世界的原因，就是要找回自己的本心。
于是我开始有意寻找各种各样失去心的人，我变成一块砖头，一颗树，一滴水，一朵白云，去听大家为什么会失去自己的本心。
我发现，刚出生的宝宝，本心还在，慢慢的，他们的本心就会消失，收到了各种黑暗之光的侵蚀。
从一次争论，到嫉妒和悲愤，还有委屈和痛苦，我看到一只只无形的手，把他们的本心扯碎，蒙蔽，偷走，再也回不到主人都身边。
我叫他本心猎手。他可能是和宇宙同在的级别 但是我并不害怕，我仔细回忆自己平淡的一生 寻找本心猎手的痕迹。
沿着自己的回忆，一个个的场景忽闪而过，最后发现，我的本心，在我写代码的时候，会回来。
安静，淡然，代码就是我的一切，写代码就是我本心回归的最好方式，我还没找到本心猎手，但我相信，顺着这个线索，我一定能顺藤摸瓜，把他揪出来。
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