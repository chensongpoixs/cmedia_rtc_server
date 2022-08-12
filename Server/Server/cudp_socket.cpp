/***********************************************************************************************
created: 		2022-08-12

author:			chensong

purpose:		cudp_socket
************************************************************************************************/


#include "cudp_socket.h"
#include "PortManager.hpp"
#include "clog.h"

namespace chen {
	 cudp_socket::cudp_socket(Listener * listener, std::string & ip)
		 : cudp_socket_handler(RTC::PortManager::BindUdp(ip))
		 , m_listener(listener)
		 , m_fixedPort(false)
	{
	}

	 cudp_socket::cudp_socket(Listener * listener, std::string & ip, uint16_t port)
		 :  cudp_socket_handler(RTC::PortManager::BindUdp(ip, port))
		 , m_listener(listener)
		 , m_fixedPort(true)
	{
	}

	 cudp_socket::~cudp_socket()
	{
		 if (!m_fixedPort)
		 {
			 RTC::PortManager::UnbindUdp(m_local_ip, m_local_port);
		 }
	}

	void  cudp_socket::UserOnUdpDatagramReceived(const uint8_t * data, size_t len, const sockaddr * addr)
	{
		if (!m_listener)
		{
			ERROR_EX_LOG("no listener set");

			return;
		}
		// ----> 
		// Notify the reader.
		m_listener->OnUdpSocketPacketReceived(this, data, len, addr);
	}
}
