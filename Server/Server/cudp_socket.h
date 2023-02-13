/***********************************************************************************************
created: 		2022-08-12

author:			chensong

purpose:		cudp_socket 
************************************************************************************************/


#ifndef _C_UDP_SOCKET_H_
#define _C_UDP_SOCKET_H_

#include "cudp_socket_handler.h"

namespace chen {
	class cudp_socket : public cudp_socket_handler
	{
	public:
		class Listener
		{
		public:
		//	virtual ~Listener() = default;

		public:
			virtual void OnUdpSocketPacketReceived(
				cudp_socket* socket, const uint8_t* data, size_t len, const struct sockaddr* remoteAddr) = 0;
		};
	public:
		cudp_socket(Listener* listener, std::string& ip );
		cudp_socket(Listener* listener, std::string& ip, uint16_t port);
		~cudp_socket();

	public:
		/* Pure virtual methods inherited from ::UdpSocketHandler. */
		void UserOnUdpDatagramReceived(const uint8_t* data, size_t len, const struct sockaddr* addr)  ;
	private:
		// Passed by argument.
		Listener*			m_listener;// { nullptr };
		bool				m_fixedPort;// { false };
	};
		
}

#endif // _C_UDP_SOCKET_H_