/***********************************************************************************************
created: 		2022-08-12

author:			chensong

purpose:		cudp_socket_handler
************************************************************************************************/


#include "cudp_socket_handler.h"
#include "clog.h"
#include "cuv_ip.h"

namespace chen {



	/* Static. */

	static constexpr size_t ReadBufferSize{ 65536 };
	thread_local static uint8_t ReadBuffer[ReadBufferSize];

	/* Static methods for UV callbacks. */

	inline static void onAlloc(uv_handle_t* handle, size_t suggestedSize, uv_buf_t* buf)
	{
		cudp_socket_handler* socket_ptr = static_cast<cudp_socket_handler*>(handle->data);

		if (socket_ptr)
		{
			socket_ptr->OnUvRecvAlloc(suggestedSize, buf);
		}
	}

	inline static void onRecv( uv_udp_t* handle, ssize_t nread, const uv_buf_t* buf, const struct sockaddr* addr, unsigned int flags)
	{
		cudp_socket_handler* socket_ptr = static_cast<cudp_socket_handler*>(handle->data);

		if (socket_ptr)
		{
			socket_ptr->OnUvRecv(nread, buf, addr, flags);
		}
	}

	inline static void onSend(uv_udp_send_t* req, int status)
	{
		cudp_socket_handler::UvSendData* sendData = static_cast<cudp_socket_handler::UvSendData*>(req->data);
		uv_udp_t* handle = req->handle;
		cudp_socket_handler* socket_ptr = static_cast<cudp_socket_handler*>(handle->data);
		cudp_socket_handler::onSendCallback* cb = sendData->cb;

		if (socket_ptr)
		{
			socket_ptr->OnUvSend(status, cb);
		}

		// Delete the UvSendData struct (it will delete the store and cb too).
		delete sendData;
	}

	inline static void onClose(uv_handle_t* handle)
	{
		delete handle;
	}


	 cudp_socket_handler::cudp_socket_handler(uv_udp_t * uvHandle)
		: m_local_addr()
		, m_local_ip("")
		, m_local_port(0u)
		, m_uvHandle(uvHandle)
		, m_closed(false)
		, m_recvBytes(0u)
		, m_sentBytes(0u)
	{
		 int32 err;
		 m_uvHandle->data = static_cast<void*>(this);

		 err = uv_udp_recv_start(m_uvHandle, static_cast<uv_alloc_cb>(onAlloc), static_cast<uv_udp_recv_cb>(onRecv));

		 if (err != 0)
		 {
			 uv_close(reinterpret_cast<uv_handle_t*>(m_uvHandle), static_cast<uv_close_cb>(onClose));

			 ERROR_EX_LOG("uv_udp_recv_start() failed: %s", uv_strerror(err));
		 }

		 // Set local address.
		 if (!SetLocalAddress())
		 {
			 uv_close(reinterpret_cast<uv_handle_t*>(m_uvHandle), static_cast<uv_close_cb>(onClose));

			 ERROR_EX_LOG("error setting local IP and port");
		 }
	}

	 cudp_socket_handler::~cudp_socket_handler()
	{
		 if (!m_closed)
		 {
			 Close();
		 }
	}
	 void cudp_socket_handler::Close()
	 {
		 if (m_closed)
		 {
			 return;
		 }

		 m_closed = true;

		 // Tell the UV handle that the UdpSocketHandler has been closed.
		 m_uvHandle->data = nullptr;

		 // Don't read more.
		 int err = uv_udp_recv_stop(m_uvHandle);

		 if (err != 0)
		 {
			 ERROR_EX_LOG("uv_udp_recv_stop() failed: %s", uv_strerror(err));
		 }

		 uv_close(reinterpret_cast<uv_handle_t*>(m_uvHandle), static_cast<uv_close_cb>(onClose));
	 }
	 void cudp_socket_handler::Dump() const
	 {
		 DEBUG_LOG("<UdpSocketHandler>");
		 DEBUG_LOG("  localIp   : %s", m_local_ip.c_str());
		 DEBUG_LOG("  localPort : %hu" , static_cast<uint16_t>(m_local_port));
		 DEBUG_LOG("  closed    : %s", !m_closed ? "open" : "closed");
		 DEBUG_LOG("</UdpSocketHandler>");
	 }
	 void cudp_socket_handler::Send(const uint8_t * data, size_t len, const sockaddr * addr, onSendCallback * cb)
	 {
		 if (m_closed)
		 {
			 if (cb)
			 {
				 (*cb)(false);
				 delete cb;
			 }

			 return;
		 }

		 if (len == 0)
		 {
			 if (cb)
			 {
				 (*cb)(false);
				 delete cb;
			 }

			 return;
		 }

		 // First try uv_udp_try_send(). In case it can not directly send the datagram
		 // then build a uv_req_t and use uv_udp_send().

		 uv_buf_t buffer = uv_buf_init(reinterpret_cast<char*>(const_cast<uint8_t*>(data)), len);
		 int sent = uv_udp_try_send(m_uvHandle, &buffer, 1, addr);

		 // Entire datagram was sent. Done.
		 if (sent == static_cast<int>(len))
		 {
			 // Update sent bytes.
			 m_sentBytes += sent;

			 if (cb)
			 {
				 (*cb)(true);
				 delete cb;
			 }

			 return;
		 }
		 else if (sent >= 0)
		 {
			 WARNING_EX_LOG("datagram truncated (just %d of %zu bytes were sent)", sent, len);

			 // Update sent bytes.
			 m_sentBytes += sent;

			 if (cb)
			 {
				 (*cb)(false);
				 delete cb;
			 }

			 return;
		 }
		 // Any error but legit EAGAIN. Use uv_udp_send().
		 else if (sent != UV_EAGAIN)
		 {
			 WARNING_EX_LOG("uv_udp_try_send() failed, trying uv_udp_send(): %s", uv_strerror(sent));
		 }

		 auto* sendData = new UvSendData(len);

		 sendData->req.data = static_cast<void*>(sendData);
		 std::memcpy(sendData->store, data, len);
		 sendData->cb = cb;

		 buffer = uv_buf_init(reinterpret_cast<char*>(sendData->store), len);

		 int err = uv_udp_send(
			 &sendData->req, m_uvHandle, &buffer, 1, addr, static_cast<uv_udp_send_cb>(onSend));

		 if (err != 0)
		 {
			 // NOTE: uv_udp_send() returns error if a wrong INET family is given
			 // (IPv6 destination on a IPv4 binded socket), so be ready.
			 WARNING_EX_LOG("uv_udp_send() failed: %s", uv_strerror(err));

			 if (cb)
				 (*cb)(false);

			 // Delete the UvSendData struct (it will delete the store and cb too).
			 delete sendData;
		 }
		 else
		 {
			 // Update sent bytes.
			 m_sentBytes += len;
		 }
	 }
	 void cudp_socket_handler::OnUvRecvAlloc(size_t suggestedSize, uv_buf_t * buf)
	 {
		 // Tell UV to write into the static buffer.
		 buf->base = reinterpret_cast<char*>(ReadBuffer);
		 // Give UV all the buffer space.
		 buf->len = ReadBufferSize;
	 }
	 void cudp_socket_handler::OnUvRecv(ssize_t nread, const uv_buf_t * buf, const sockaddr * addr, unsigned int flags)
	 {
		 // NOTE: Ignore if there is nothing to read or if it was an empty datagram.
		 if (nread == 0)
		 {
			 return;
		 }

		 // Check flags.
		 if ((flags & UV_UDP_PARTIAL) != 0u)
		 {
			 ERROR_EX_LOG("received datagram was truncated due to insufficient buffer, ignoring it");

			 return;
		 }

		 // Data received.
		 if (nread > 0)
		 {
			 // Update received bytes.
			 m_recvBytes += nread;

			 // Notify the subclass.
			 UserOnUdpDatagramReceived(reinterpret_cast<uint8_t*>(buf->base), nread, addr);
		 }
		 // Some error.
		 else
		 {
			 DEBUG_EX_LOG("read error: %s", uv_strerror(nread));
		 }
	 }
	 void cudp_socket_handler::OnUvSend(int status, onSendCallback * cb)
	 {
		 // NOTE: Do not delete cb here since it will be delete in onSend() above.

		 if (status == 0)
		 {
			 if (cb)
			 {
				 (*cb)(true);
			 }
		 }
		 else
		 {
#if MS_LOG_DEV_LEVEL == 3
			 MS_DEBUG_DEV("send error: %s", uv_strerror(status));
#endif

			 if (cb)
			 {
				 (*cb)(false);
			 }
		 }
	 }
	 bool cudp_socket_handler::SetLocalAddress()
	 {
		 int32 err;
		 int32 len = sizeof(m_local_addr);

		 err =
			 uv_udp_getsockname(m_uvHandle, reinterpret_cast<struct sockaddr*>(&m_local_addr), &len);

		 if (err != 0)
		 {
			 ERROR_EX_LOG("uv_udp_getsockname() failed: %s", uv_strerror(err));

			 return false;
		 }

		 int family;

		 uv_ip::GetAddressInfo(
			 reinterpret_cast<const struct sockaddr*>(&m_local_addr), family, m_local_ip, m_local_port);

		 return true;
	 }
}
