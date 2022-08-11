/***********************************************************************************************
created: 		2022-08-11

author:			chensong

purpose:		tiemr
************************************************************************************************/


#include "ctimer.h"
#include "cuv_util.h"

namespace chen {

	/* Static methods for UV callbacks. */

	inline static void onTimer(uv_timer_t* handle)
	{
		static_cast<ctimer*>(handle->data)->OnUvTimer();
	}

	inline static void onClose(uv_handle_t* handle)
	{
		delete handle;
	}


	ctimer::ctimer()
		: m_uvHandle(NULL)
		, m_closed(false)
		, m_timeout(0u)
		, m_repeat(0u)
	{
	}
	ctimer::~ctimer()
	{
	}
	bool ctimer::init()
	{
		m_uvHandle = new uv_timer_t;
		if (!m_uvHandle)
		{
			WARNING_EX_LOG(" uv timer alloc filed !!!");
			return false;
		}

		m_uvHandle->data = static_cast<void*>(this);

		int32 err = uv_timer_init(uv_util::get_loop(), m_uvHandle);

		if (0 != err)
		{
			delete m_uvHandle;
			m_uvHandle = NULL;

			ERROR_EX_LOG("uv_timer_init() failed: %s", uv_strerror(err));
			return false;
		}

		return true;
	}
	void ctimer::destroy()
	{
		if (!m_closed)
		{
			Close();
		}
	}
	void ctimer::Start(uint64_t timeout, uint64_t repeat)
	{
		if (m_closed)
		{
			ERROR_EX_LOG("timer closeed ");
			return;
		}

		m_timeout = timeout;
		m_repeat = repeat;

		if (0 != uv_is_active(reinterpret_cast<uv_handle_t*>(m_uvHandle)))
		{
			Stop();
		}


		int32 err = uv_timer_start(m_uvHandle, static_cast<uv_timer_cb>(onTimer), timeout, repeat);

		if (0 != err)
		{
			ERROR_EX_LOG("uv_timer_start() failed: %s", uv_strerror(err));
		}
	}
	void ctimer::Stop()
	{
		if (m_closed)
		{
			ERROR_EX_LOG("closed");
			return;
		}
		
		int32 err = uv_timer_stop(m_uvHandle);
		if (0 != err)
		{
			ERROR_EX_LOG("uv_timer_stop() failed: %s", uv_strerror(err));
		}
	}
	void ctimer::Close()
	{
		if (m_closed)
		{
			return;
		}
		m_closed = true;
		uv_close(reinterpret_cast<uv_handle_t*>(m_uvHandle), static_cast<uv_close_cb>(onClose));
	}
	void ctimer::Reset()
	{
		if (m_closed)
		{
			ERROR_EX_LOG("closed");
			return;
		}

		if (uv_is_active(reinterpret_cast<uv_handle_t*>(m_uvHandle)) == 0)
		{
			return;
		}

		if (m_repeat == 0u)
		{
			return;
		}

		int32 err = uv_timer_start(m_uvHandle, static_cast<uv_timer_cb>(onTimer), m_repeat, m_repeat);

		if (err != 0)
		{
			ERROR_EX_LOG("uv_timer_start() failed: %s", uv_strerror(err));
		}
	}
	void ctimer::Restart()
	{
		if (m_closed)
		{
			ERROR_EX_LOG("closed");
		}

		if (uv_is_active(reinterpret_cast<uv_handle_t*>(m_uvHandle)) != 0)
		{
			Stop();
		}

		int32 err = uv_timer_start(m_uvHandle, static_cast<uv_timer_cb>(onTimer), m_timeout, m_repeat);

		if (0 != err)
		{
			ERROR_EX_LOG("uv_timer_start() failed: %s", uv_strerror(err));
		}
	}
	void ctimer::OnUvTimer()
	{
		// Callback TODO@chensong 20220811 
		//OnTimer(this);
		OnTimer();
	}
}