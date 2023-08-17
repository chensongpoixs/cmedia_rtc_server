
#ifndef _C_RTC_QUEUE_H_
#define _C_RTC_QUEUE_H_
#include <iostream>
#include <cstdbool>
#include <cstdint>
#include "cnet_type.h"
#include "crtc_transport.h"
namespace chen {

	struct crtc_rtp_packet
	{
		// Cloned packet.
		RTC::RtpPacket* packet{ nullptr };
		// Memory to hold the cloned packet (with extra space for RTX encoding).
		uint8  store[RTC::MtuSize + 100];
		// Last time this packet was resent.
		uint64  resentAtMs{ 0u };
		// Number of times this packet was resent.
		uint8  sentTimes{ 0u };
		// Whether the packet has been already RTX encoded.
		bool rtxEncoded{ false };
		std::shared_ptr < crtc_rtp_packet > next{ NULL };
	};
	class cdtls_session;
	class ctransport_tuple;
	class cice_server;
	//cudp_socket * socket, const uint8_t * data, size_t len, const sockaddr * remoteAddr


	struct cqueue_packet {
		cudp_socket* socket;
		uint8     data[65535];
		size_t   len;
		struct sockaddr remoteAddr;
		std::shared_ptr< cqueue_packet> next{ NULL };
	};
	class crtc_rtp_queue
	{
	private:
		typedef std::mutex								clock_type;
		typedef std::lock_guard<clock_type>				clock_guard;
	public:
		explicit crtc_rtp_queue()
			: m_head_ptr(nullptr)
		, m_tail_ptr(NULL){}
		~crtc_rtp_queue() {}
		void enqueue(std::shared_ptr<crtc_rtp_packet>p)
		{
			clock_guard guard(m_lock);
			if (!m_head_ptr)
			{
				m_head_ptr = m_tail_ptr = p;
			}
			else
			{
				m_head_ptr->next = (p);
				m_head_ptr = p;
			}
		}
		std::shared_ptr< crtc_rtp_packet> dequeue()
		{
			clock_guard guard(m_lock);
			if (!m_tail_ptr)
			{
				return NULL;
			}
			std::shared_ptr<crtc_rtp_packet> ptr = m_tail_ptr;
			m_tail_ptr = m_head_ptr = NULL;
			return ptr;
		}
		//}
		void clear() 
		{
			clock_guard guard(m_lock);
			std::shared_ptr<crtc_rtp_packet> msg_ptr = NULL;
			while (m_tail_ptr)
			{
				msg_ptr = m_tail_ptr->next;
				//m_tail_ptr->free_me();
				m_tail_ptr = msg_ptr;
			}
			m_head_ptr = m_tail_ptr = NULL;
		}
		bool empty() const { return m_head_ptr == NULL; }
	private:
		crtc_rtp_queue(const crtc_rtp_queue&);
		crtc_rtp_queue& operator=(const crtc_rtp_queue&);
	private:
		std::shared_ptr< crtc_rtp_packet>			m_head_ptr;
		std::shared_ptr< crtc_rtp_packet>			m_tail_ptr;
		clock_type		m_lock; 
	};

	class crtc_queue
	{
	private:
		typedef std::mutex								clock_type;
		typedef std::lock_guard<clock_type>				clock_guard;
	public:
		crtc_queue() : m_head_ptr(NULL)
			, m_tail_ptr(NULL) {}
		~crtc_queue() {}
		void enqueue(std::shared_ptr<cqueue_packet>p)
		{
			clock_guard guard(m_lock);
			if (!m_head_ptr)
			{
				m_head_ptr = m_tail_ptr = p;
			}
			else
			{
				m_head_ptr->next = (p);
				m_head_ptr = p;
			}
		}
		std::shared_ptr< cqueue_packet> dequeue()
		{
			clock_guard guard(m_lock);
			if (!m_tail_ptr)
			{
				return NULL;
			}
			std::shared_ptr<cqueue_packet> ptr = m_tail_ptr;
			m_tail_ptr = m_head_ptr = NULL;
			return ptr;
		}
		//}
		void clear()
		{
			clock_guard guard(m_lock);
			std::shared_ptr<cqueue_packet> msg_ptr = NULL;
			while (m_tail_ptr)
			{
				msg_ptr = m_tail_ptr->next;
				//m_tail_ptr->free_me();
				m_tail_ptr = msg_ptr;
			}
			m_head_ptr = m_tail_ptr = NULL;
		}
		bool empty() const { return m_head_ptr == NULL; }
	private:
		crtc_queue(const crtc_queue&);
		crtc_queue& operator=(const crtc_queue&);
	private:
		std::shared_ptr< cqueue_packet>			m_head_ptr;
		std::shared_ptr< cqueue_packet>			m_tail_ptr;
		clock_type		m_lock;
	};


}

#endif //_C_RTC_QUEUE_H_