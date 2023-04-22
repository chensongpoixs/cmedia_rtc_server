/***********************************************************************************************
created: 		2022-08-11

author:			chensong

purpose:		cuv_util
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
#include "cuv_util.h"
#include <chrono>
#include <ctime>
#include <ctime>
#include <string>
#include <chrono>
#include <sstream>
#include <iomanip>

#if defined(WIN32)
// clang-format off
// clang formatting would put <windows.h> last,
// which leads to compilation failure.
#include <windows.h>
#include <mmsystem.h>
#include <sys/timeb.h>
#pragma comment(lib, "winmm.lib")
// clang-format on
#endif

namespace chen {
	namespace uv_util
	{

		thread_local static uv_loop_t* g_uv_loop = NULL;;

		inline static void onClose(uv_handle_t* handle)
		{
			delete handle;
		}
		inline static void onWalk(uv_handle_t* handle, void* /*arg*/)
		{
			// Must use MS_ERROR_STD since at this point the Channel is already closed.
			ERROR_EX_LOG(
				"alive UV handle found (this shouldn't happen) [type:%s, active:%d, closing:%d, has_ref:%d]",
				uv_handle_type_name(handle->type),
				uv_is_active(handle),
				uv_is_closing(handle),
				uv_has_ref(handle));

			if (!uv_is_closing(handle))
			{
				uv_close(handle, onClose);
			}
		}

		bool init()
		{
			g_uv_loop = new uv_loop_t;

			if (!g_uv_loop)
			{
				WARNING_EX_LOG("g_uv_loop alloc failed !!!");
				return false;
			}
			int32 err = uv_loop_init(g_uv_loop);

			if (0 != err)
			{
				WARNING_EX_LOG("uv_loop_init  failed !!!");
				return false;
			}
			return true;
		}
		void destroy()
		{

			int err;
			if (!g_uv_loop)
			{
				WARNING_EX_LOG("uv loop ptr == null !!!");
				return;
			}
			uv_stop(g_uv_loop);
			uv_walk(g_uv_loop, onWalk, nullptr);

			while (true)
			{
				err = uv_loop_close(g_uv_loop);

				if (err != UV_EBUSY)
				{
					break;
				}

				uv_run(g_uv_loop, UV_RUN_NOWAIT);
			}

			if (err != 0)
			{
				ERROR_EX_LOG("failed to close libuv loop: %s", uv_err_name(err));
			}

			delete g_uv_loop;
			g_uv_loop = NULL;
		}
		void print_version()
		{
			DEBUG_EX_LOG( "libuv version: \"%s\"", uv_version_string());
		}
		void run_loop()
		{
			// This should never happen.
			//MS_ASSERT(DepLibUV::loop != nullptr, "loop unset");
			if (!g_uv_loop)
			{
				ERROR_EX_LOG("g_uv_loop == NULL  !!!");
				return;
			}
			int32 ret = uv_run(g_uv_loop, UV_RUN_DEFAULT);
			if (0 != ret)
			{
				ERROR_EX_LOG("uv_run() returned %s", uv_err_name(ret));
			}
			//MS_ASSERT(ret == 0, "uv_run() returned %s", uv_err_name(ret));
		}
		uv_loop_t * get_loop()
		{
			return g_uv_loop;
		}
		uint64_t GetTimeMs()
		{

			//static const int64_t kNumMillisecsPerSec = INT64_C(1000);
			//static const int64_t kNumMicrosecsPerSec = INT64_C(1000000);
			//static const int64_t kNumNanosecsPerSec = INT64_C(1000000000);

			//static const int64_t kNumMicrosecsPerMillisec =
			//	kNumMicrosecsPerSec / kNumMillisecsPerSec;
			//static const int64_t kNumNanosecsPerMillisec =
			//	kNumNanosecsPerSec / kNumMillisecsPerSec;
			//static const int64_t kNumNanosecsPerMicrosec =
			//	kNumNanosecsPerSec / kNumMicrosecsPerSec;
			//int64_t ticks;
			//static volatile LONG last_timegettime = 0;
			//static volatile int64_t num_wrap_timegettime = 0;
			//volatile LONG* last_timegettime_ptr = &last_timegettime;
			//DWORD now = timeGetTime();//winmm.lib
			//// winmm.lib
			//// Atomically update the last gotten time
			//DWORD old = InterlockedExchange(last_timegettime_ptr, now);
			//if (now < old) {
			//	// If now is earlier than old, there may have been a race between threads.
			//	// 0x0fffffff ~3.1 days, the code will not take that long to execute
			//	// so it must have been a wrap around.
			//	if (old > 0xf0000000 && now < 0x0fffffff) {
			//		num_wrap_timegettime++;
			//	}
			//}
			//ticks = now + (num_wrap_timegettime << 32);
			//// TODO(deadbeef): Calculate with nanosecond precision. Otherwise, we're
			//// just wasting a multiply and divide when doing Time() on Windows.
			//ticks = ticks * kNumNanosecsPerMillisec;
			//return ticks / kNumNanosecsPerMillisec;//GetTickCount();
			return static_cast<uint64_t>(uv_hrtime() / 1000000u);
			//auto now = std::chrono::system_clock::now();
			/*	return std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count()
					- std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count() * 1000 ;
		*/		//return static_cast<uint64_t>(std::chrono::system_clock::now().time_since_epoch/*uv_hrtime() / 1000000u*/);
		}
		uint64_t GetTimeUs()
		{
			//return std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()) % 1000000u/*uv_hrtime() / 1000u*/;
			return static_cast<uint64_t>(uv_hrtime() / 1000u);
		}
		uint64_t GetTimeNs()
		{
			return uv_hrtime();
		}
		int64_t GetTimeMsInt64()
		{
			return static_cast<int64_t>(GetTimeMs());
		}
		int64_t GetTimeUsInt64()
		{
			return static_cast<int64_t>(GetTimeUs());
		}
	}
}