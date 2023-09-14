/********************************************************************
created:	2021-11-24

author:		chensong

purpose:	cdecoderProxy

*********************************************************************/
#include "chttp_queue_mgr.h"

namespace chen {
	static const int kMaxMsgLatency = 150;                // 150 ms
	static const int kSlowDispatchLoggingThreshold = 50;  // 50 ms
	chttp_queue_mgr g_http_queue_mgr;
	static std::chrono::steady_clock::time_point get_timemillis()
	{
		return std::chrono::steady_clock::now();
	}
	chttp_queue_mgr::chttp_queue_mgr()
	{
	}
	chttp_queue_mgr::~chttp_queue_mgr()
	{
	}
	bool chttp_queue_mgr::init()
	{
		return true;
	}
	void chttp_queue_mgr::update()
	{
		MessageList temp_message;
		//while (!m_stoped)
		{
			// 
			/*if (m_list.size() <= 0)
			{
				std::unique_lock<std::mutex> lock(m_lock);

				m_condition.wait(lock, [this]() {return m_stoped.load() || m_list.size() > 0; });
			}*/

			{
				clock_guard lock(m_lock);
				temp_message = m_list;
				m_list.clear();
			}
			for (MessageList::iterator iter = temp_message.begin(); iter != temp_message.end(); ++iter)
			{
				Dispatch(&*iter);
			}
			temp_message.clear();
		}
	}
	void chttp_queue_mgr::destroy()
	{

	}
 
	void chttp_queue_mgr::Post(const Location& posted_from,
		MessageHandler* phandler,
		uint32_t id,
		MessageData* pdata,
		bool time_sensitive)
	{
		clock_guard lock(m_lock);
		Message msg;
		msg.posted_from = posted_from;
		msg.phandler = phandler;
		msg.message_id = id;
		msg.pdata = pdata;
		msg.cur_time = get_timemillis();
		m_list.push_back(msg);

	}
	
	void chttp_queue_mgr::Dispatch(Message * pmsg)
	{
		std::chrono::steady_clock::time_point start_time = get_timemillis();
		std::chrono::steady_clock::duration diff = start_time - pmsg->cur_time;
		std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(diff);
		/*if (ms.count() > (kMaxMsgLatency - kSlowDispatchLoggingThreshold))
		{
			WARNING_EX_LOG("work   Message took  diff = %lu ms to dispatch. Posted from: = %s",  ms.count(), pmsg->posted_from.ToString().c_str());
		}*/
		pmsg->phandler->OnMessage(pmsg);
		std::chrono::steady_clock::time_point end_time = get_timemillis();
		diff = end_time - start_time;
		ms = std::chrono::duration_cast<std::chrono::milliseconds>(diff);
		/*if (ms.count() > kSlowDispatchLoggingThreshold)
		{
			WARNING_EX_LOG("thread_id = %p , Message took  diff = %lu ms to dispatch. Posted from: = %s", m_thread_id, ms.count(), pmsg->posted_from.ToString().c_str());
		}*/
	}
}