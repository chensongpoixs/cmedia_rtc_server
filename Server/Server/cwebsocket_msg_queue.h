/***********************************************************************************************
created: 		2019-12-27

author:			chensong

purpose:		cnet_msg_queue
************************************************************************************************/
#ifndef _C_MSG_WEBSOCKET_QUEUE_H_
#define _C_MSG_WEBSOCKET_QUEUE_H_
#include <mutex>
#include "cnet_define.h"
namespace chen {
	class cwebsocket_msg
	{
	public:
		cwebsocket_msg()
			: m_session_id(0)
			, m_next_msg_ptr(NULL)
			, m_msg_id(0)
			, m_buffer_ptr(NULL)
			, m_data_size(0)
		{}
		~cwebsocket_msg(){}
	public:
		cwebsocket_msg * get_next_msg() const { return m_next_msg_ptr; }
		void set_next_msg(cwebsocket_msg * ptr) { m_next_msg_ptr = ptr; }
		const uint16_t get_msg_id() const { return m_msg_id; }
		void set_msg_id(uint16_t msg_id) { m_msg_id = msg_id; }
		const uint64_t get_session_id() const { return m_session_id; }
		void set_session_id(uint64_t value) { m_session_id = value; }
		void set_buffer(unsigned char  * buffer, uint32_t data_size);
		unsigned char  *   get_buffer() const { return m_buffer_ptr; }
		uint32_t    get_data_size()   { return m_data_size; }
		void free();
	private:
		cwebsocket_msg(const cwebsocket_msg&);
		cwebsocket_msg& operator=(const cwebsocket_msg&);
	private:

		uint64_t						m_session_id;
		cwebsocket_msg*					m_next_msg_ptr;
		uint16_t							m_msg_id;
		unsigned char  * 							m_buffer_ptr;
		uint32_t						m_data_size;
	};
	 
	class cwebsocket_msg_queue
	{
	private:
		typedef std::mutex								clock_type;
		typedef std::lock_guard<clock_type>				clock_guard;
	public:
		cwebsocket_msg_queue();
		~cwebsocket_msg_queue();
		void enqueue(cwebsocket_msg*p);
		cwebsocket_msg* dequeue();
		void clear();
	private:
		cwebsocket_msg_queue(const cwebsocket_msg_queue&);
		cwebsocket_msg_queue& operator=(const cwebsocket_msg_queue&);
	private:
		cwebsocket_msg*			m_head_ptr;
		cwebsocket_msg*			m_tail_ptr;
		clock_type		m_lock;
	};
}//!namespace chen


#endif // !#define _C_MSG_WEBSOCKET_QUEUE_H_