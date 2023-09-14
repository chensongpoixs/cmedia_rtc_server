/********************************************************************
created:	2021-11-24

author:		chensong

purpose:	Location

*********************************************************************/

#ifndef  _EVENT_H_
#define  _EVENT_H_

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <atomic>
#include "clog.h"
namespace chen {

class Event {
private:
	typedef std::lock_guard<std::mutex>		lock_guard;
	typedef std::condition_variable			condition;
 public:
  static const int kForever = -1;

  Event()
  {
	  m_run.store(true);
  }
  Event(bool manual_reset, bool initially_signaled)
  {
	  m_run.store(true);
  }
  Event(const Event&) = delete;
  Event& operator=(const Event&) = delete;
  ~Event()
  {
	  if (m_run.load())
	  {
		  m_run.store(false);
		 
	  }

	  m_condition.notify_one();
	 
  }

  void Set()
  {
	  {
	  
		  //std::lock_guard<std::mutex> lock(m_lock);
		  m_run.store(false);
	  }
	  //m_condition.notify_one();
	  m_condition.notify_one();
  }
  void Reset()
  {
	  {
		  std::lock_guard<std::mutex> lock(m_lock);
		  m_run.store(false);
	  }
	  //m_cond.notify_one();
	  m_condition.notify_one();
  }

  // Waits for the event to become signaled, but logs a warning if it takes more
  // than `warn_after_ms` milliseconds, and gives up completely if it takes more
  // than `give_up_after_ms` milliseconds. (If `warn_after_ms >=
  // give_up_after_ms`, no warning will be logged.) Either or both may be
  // `kForever`, which means wait indefinitely.
  //
  // Returns true if the event was signaled, false if there was a timeout or
  // some other error.
  bool Wait(int give_up_after_ms, int warn_after_ms)
  {
	 
	  while (m_run.load())
	  {
		  std::unique_lock<std::mutex> lock(m_lock);
		  
		  m_condition.wait(lock, [this]() {return !m_run.load(); });
	  }
	  NORMAL_EX_LOG("m_run = %d", m_run.load());
	  return true;
  }

  // Waits with the given timeout and a reasonable default warning timeout.
  bool Wait(int give_up_after_ms) 
  {
	  m_run.store(true);
    return Wait(give_up_after_ms,
                give_up_after_ms == kForever ? 100 : kForever);
  }

 private:
	 std::atomic<bool>		   m_run;
	 std::mutex				   m_lock;
	 condition				   m_condition;
};



}  // namespace chen

#endif  // RTC_BASE_EVENT_H_
