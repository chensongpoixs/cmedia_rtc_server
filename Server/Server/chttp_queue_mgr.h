/********************************************************************
created:	2021-11-24

author:		chensong

purpose:	cdecoderProxy

*********************************************************************/

#ifndef  _C_HTTP_QUEUE_MGR_H_
#define  _C_HTTP_QUEUE_MGR_H_

#include <cstdint>
#include "cmessage_handler.h"
#include "clocation.h"
#include <chrono>
#include <list>
#include <string.h>

#include <algorithm>
#include <list>
#include <memory>
#include <queue>
#include <vector>
#include <thread>
#include "cconstructor_magic.h"
#include<condition_variable>
#include <atomic>
namespace chen {


	struct Message;
	class MessageQueue;

	const uint32_t MQID_ANY = static_cast<uint32_t>(-1);
	const uint32_t MQID_DISPOSE = static_cast<uint32_t>(-2);

	// No destructor
	class MessageData {
	public:
		MessageData() {}
		virtual ~MessageData() {}
	};
	struct Message {
		Message()
			: phandler(nullptr), message_id(0), pdata(nullptr), cur_time() {}
		inline bool Match(MessageHandler* handler, uint32_t id) const {
			return (handler == nullptr || handler == phandler) &&
				(id == MQID_ANY || id == message_id);
		}
		Location posted_from;
		MessageHandler* phandler;
		uint32_t message_id;
		MessageData* pdata;
		std::chrono::steady_clock::time_point cur_time;
	};

	typedef std::list<Message> MessageList;
	class chttp_queue_mgr
	{
		_DISALLOW_COPY_AND_ASSIGN(chttp_queue_mgr)
	private: 
		typedef std::atomic_bool						catomic_bool;
		typedef std::lock_guard<std::mutex>				clock_guard;
		typedef std::mutex								cmutex;
	public:
		static const int kForever = -1;
	public:
		explicit chttp_queue_mgr();
		virtual ~chttp_queue_mgr();


		bool init();
		void update();
		void destroy();
	public:
			void Post(const Location& posted_from,
				MessageHandler* phandler,
				uint32_t id = 0,
				MessageData* pdata = nullptr,
				bool time_sensitive = true);

			void Dispatch(Message* pmsg);
	protected:
	private:
	private: 
		cmutex			m_lock;
		MessageList		m_list; 
	};
	extern chttp_queue_mgr g_http_queue_mgr;
}

#endif // _C_HTTP_QUEUE_MGR_H_
