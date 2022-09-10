/***********************************************************************************************
created: 		2022-08-11

author:			chensong

purpose:		cuv_util
************************************************************************************************/
#include "cuv_util.h"


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
			return static_cast<uint64_t>(uv_hrtime() / 1000000u);
		}
		uint64_t GetTimeUs()
		{
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