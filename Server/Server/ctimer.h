/***********************************************************************************************
created: 		2022-08-11

author:			chensong

purpose:		tiemr
************************************************************************************************/


#ifndef _C_TIMER_H_
#define _C_TIMER_H_
#include "clog.h"
#include "cnet_type.h"



namespace chen {



	class ctimer
	{
	public:
		class Listener
		{
		public:
			virtual ~Listener() = default;
		public :
			virtual void OnTimer(ctimer * timer) = 0;
		};
	public:
		ctimer(Listener * listener);
		virtual ~ctimer();
	public:
	//	bool init();
		
		//void destroy();
		void Start(uint64_t timeout, uint64_t repeat = 0);
		void Stop();
		void Close();
		void Reset();
		void Restart();
		uint64_t GetTimeout() const
		{
			return  m_timeout;
		}
		uint64_t GetRepeat() const
		{
			return  m_repeat;
		}
		bool IsActive() const
		{
			return uv_is_active(reinterpret_cast<uv_handle_t*>(m_uvHandle)) != 0;
		}

		
	public:
		/* Callbacks fired by UV events. */
		void OnUvTimer();
	public: 
	 	//virtual void OnTimer(ctimer * timer) = 0;
	private:
		ctimer(const ctimer&);
		ctimer& operator=(const ctimer&)  ;
		

	private:
		Listener	*		m_listener;
		// Allocated by this.
		uv_timer_t*			m_uvHandle/*{ nullptr }*/;
		// Others.
		bool				m_closed/*{ false }*/;
		uint64_t			m_timeout/*{ 0u }*/;
		uint64_t			m_repeat/*{ 0u }*/;

	};
}

#endif // _C_TIMER_H_