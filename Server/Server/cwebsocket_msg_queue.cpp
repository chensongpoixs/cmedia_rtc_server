/***********************************************************************************************
created: 		2019-12-27

author:			chensong

purpose:		cwebsocket_msg_queue
************************************************************************************************/

#include "cwebsocket_msg_queue.h"

namespace chen {
	cwebsocket_msg_queue::cwebsocket_msg_queue()
		: m_head_ptr (NULL)
		, m_tail_ptr (NULL)
	{
	}
	cwebsocket_msg_queue::~cwebsocket_msg_queue()
	{
	}
	void cwebsocket_msg_queue::enqueue(cwebsocket_msg * p)
	{
		clock_guard guard(m_lock);
		if (!m_head_ptr)
		{
			m_head_ptr = m_tail_ptr = p;
		}
		else
		{
			m_head_ptr->set_next_msg(p);
			m_head_ptr = p;
		}
	}
	cwebsocket_msg * cwebsocket_msg_queue::dequeue()
	{
		clock_guard guard(m_lock);
		if (!m_tail_ptr)
		{
			return NULL;
		}
		cwebsocket_msg * ptr = m_tail_ptr;
		m_tail_ptr = m_head_ptr = NULL;
		return ptr;
	}
	void cwebsocket_msg_queue::clear()
	{
		clock_guard guard(m_lock);
		cwebsocket_msg* msg_ptr = NULL;
		while (m_tail_ptr)
		{
			msg_ptr = m_tail_ptr->get_next_msg();
			//m_tail_ptr->free_me();

			delete m_tail_ptr;
			m_tail_ptr = NULL;
			m_tail_ptr = msg_ptr;
		}
		m_head_ptr = m_tail_ptr =  NULL;
	}
	
	void cwebsocket_msg::set_buffer(unsigned char * buffer, uint32_t data_size)
	{
		if (m_buffer_ptr)
		{
			delete[] m_buffer_ptr;
			m_buffer_ptr = NULL;
		}
		m_buffer_ptr = new unsigned char[data_size];
		if (!m_buffer_ptr)
		{
			// error info alloc failed !!!
			return;
		}
		memcpy(m_buffer_ptr, buffer, data_size);
		m_data_size = data_size;
	}

	void cwebsocket_msg::free()
	{
		if (m_buffer_ptr)
		{
			delete[] m_buffer_ptr;
			m_buffer_ptr = NULL;
		}
	}

} // !namespace chen 
