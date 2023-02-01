/***********************************************************************************************
created: 		2022-08-12

author:			chensong

purpose:		cudp_socket_handler
************************************************************************************************/


#ifndef _C_UDP_SOCKET_HANDLER_H_
#define _C_UDP_SOCKET_HANDLER_H_

//#include "cnet_type.h"

#include <uv.h>
#include <functional>
#include <cstring>
#include <ws2def.h>
#include <string>

namespace chen {
	class cudp_socket_handler
	{
	public:
		typedef std::function<void(bool sent)>  onSendCallback;
		/* Struct for the data field of uv_req_t when sending a datagram. */
		struct UvSendData
		{
			explicit UvSendData(size_t storeSize)
			{
				this->store = new uint8_t[storeSize];
			}

			// Disable copy constructor because of the dynamically allocated data (store).
			UvSendData(const UvSendData&) = delete;

			~UvSendData()
			{
				delete[] this->store;
				delete this->cb;
			}

			uv_udp_send_t req;
			uint8_t* store{ nullptr };
			onSendCallback* cb{ nullptr };
		};
	public:
		explicit cudp_socket_handler(uv_udp_t* uvHandle);
		virtual ~cudp_socket_handler();

	public:
		void Close();
		virtual void Dump() const;
		void Send( const uint8_t* data, size_t len, const struct sockaddr* addr,  onSendCallback* cb);
		const struct sockaddr* GetLocalAddress() const
		{
			return reinterpret_cast<const struct sockaddr*>(&m_local_addr);
		}
		int GetLocalFamily() const
		{
			return reinterpret_cast<const struct sockaddr*>(&m_local_addr)->sa_family;
		}
		const std::string& GetLocalIp() const
		{
			return m_local_ip;
		}
		uint16_t GetLocalPort() const
		{
			return m_local_port;
		}
		size_t GetRecvBytes() const
		{
			return m_recvBytes;
		}
		size_t GetSentBytes() const
		{
			return m_sentBytes;
		}
	public:
		/* Callbacks fired by UV events. */
		void OnUvRecvAlloc(size_t suggestedSize, uv_buf_t* buf);
		void OnUvRecv(ssize_t nread, const uv_buf_t* buf, const struct sockaddr* addr, unsigned int flags);
		void OnUvSend(int status, onSendCallback* cb);
	protected:
		/* Pure virtual methods that must be implemented by the subclass. */
		virtual void UserOnUdpDatagramReceived(
			const uint8_t* data, size_t len, const struct sockaddr* addr) = 0;

	private:
		bool SetLocalAddress();

	private:
		cudp_socket_handler(const cudp_socket_handler&);
		cudp_socket_handler& operator=(const cudp_socket_handler&)   ;
		
	protected: 
		//struct socketaddr_storage		m_local_addr ;
		struct sockaddr_storage			m_local_addr;
		std::string						m_local_ip;
		uint16							m_local_port;
	private:
		// Allocated by this (may be passed by argument).
		uv_udp_t*						m_uvHandle;// { nullptr };
		// Others.
		bool							m_closed;// { false };
		size_t							m_recvBytes;// { 0u };
		size_t							m_sentBytes;// { 0u };
	};
}

#endif // _C_UDP_SOCKET_HANDLER_H_
