/***********************************************************************************************
created: 		2022-08-12

author:			chensong

purpose:		cudp_socket_handler
输赢不重要，答案对你们有什么意义才重要。

光阴者，百代之过客也，唯有奋力奔跑，方能生风起时，是时代造英雄，英雄存在于时代。或许世人道你轻狂，可你本就年少啊。 看护好，自己的理想和激情。


我可能会遇到很多的人，听他们讲好2多的故事，我来写成故事或编成歌，用我学来的各种乐器演奏它。
然后还可能在一个国家遇到一个心仪我的姑娘，她可能会被我帅气的外表捕获，又会被我深邃的内涵吸引，在某个下雨的夜晚，她会全身淋透然后要在我狭小的住处换身上的湿衣服。
3小时候后她告诉我她其实是这个国家的公主，她愿意向父皇求婚。我不得已告诉她我是穿越而来的男主角，我始终要回到自己的世界。
然后我的身影慢慢消失，我看到她眼里的泪水，心里却没有任何痛苦，我才知道，原来我的心被丢掉了，我游历全世界的原因，就是要找回自己的本心。
于是我开始有意寻找各种各样失去心的人，我变成一块砖头，一颗树，一滴水，一朵白云，去听大家为什么会失去自己的本心。
我发现，刚出生的宝宝，本心还在，慢慢的，他们的本心就会消失，收到了各种黑暗之光的侵蚀。
从一次争论，到嫉妒和悲愤，还有委屈和痛苦，我看到一只只无形的手，把他们的本心扯碎，蒙蔽，偷走，再也回不到主人都身边。
我叫他本心猎手。他可能是和宇宙同在的级别 但是我并不害怕，我仔细回忆自己平淡的一生 寻找本心猎手的痕迹。
沿着自己的回忆，一个个的场景忽闪而过，最后发现，我的本心，在我写代码的时候，会回来。
安静，淡然，代码就是我的一切，写代码就是我本心回归的最好方式，我还没找到本心猎手，但我相信，顺着这个线索，我一定能顺藤摸瓜，把他揪出来。
************************************************************************************************/


#include "cudp_socket_handler.h"
#include "clog.h"
#include "cuv_ip.h"
#include "cglobal_rtc_config.h"
#include "cglobal_rtc_port.h"
#include "csocket_util.h"
namespace chen {



	/* Static. */

	static constexpr size_t ReadBufferSize{ (65536 * 64) };
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
		, m_socket(0)
		, m_ReadBuffer_ptr(NULL)
		, m_send_queue()
	{              
		 m_ReadBuffer_ptr = new uint8[ReadBufferSize];
		 
		 uint32 port = g_global_rtc_port.get_new_port();
		 m_socket = socket(AF_INET, SOCK_DGRAM, 0);
		 if (m_socket == INVALID_SOCKET)
		 {
			int  err = WSAGetLastError();
		//	 QUEUE_REMOVE(&handle->handle_queue);
			// return uv_translate_sys_error(err);
		 }

		 while  (!socket_util::bind(m_socket, "0.0.0.0", port))
		 {
			 WARNING_EX_LOG("[udp]bind [port = %u] failed !!!", port);
			 g_global_rtc_port.brack_port(port);
			 port = g_global_rtc_port.get_new_port();
		 }
		 socket_util::setnonblock(m_socket);
		 int value = 0;
		 int len = sizeof(int);
		 getsockopt(m_socket, SOL_SOCKET, SO_SNDBUF, (char*)&value, &len);
		 len = sizeof(int);
		 getsockopt(m_socket, SOL_SOCKET, SO_RCVBUF, (char*)&value, &len);
		 value = 1024 * 64;
		 len = sizeof(int);

		 setsockopt(m_socket, SOL_SOCKET, SO_SNDBUF, (const char*)&value, len);
//		 len = sizeof(int);
		 setsockopt(m_socket, SOL_SOCKET, SO_RCVBUF, (const char*)&value, len);
		 m_thread = std::thread(&cudp_socket_handler::_work_pthread, this);
		//   int buffer_size = 1024 * 64 * 64;
		 //uv_send_buffer_size((uv_handle_t*)m_uvHandle, &buffer_size);
		// buffer_size = 1024 * 64 * 64;
		// uv_recv_buffer_size((uv_handle_t*)m_uvHandle, &buffer_size);
		// int32 err;
		// m_uvHandle->data = static_cast<void*>(this);
		//
		// err = uv_udp_recv_start(m_uvHandle, static_cast<uv_alloc_cb>(onAlloc), static_cast<uv_udp_recv_cb>(onRecv));
		//
		// if (err != 0)
		// {
		//	 uv_close(reinterpret_cast<uv_handle_t*>(m_uvHandle), static_cast<uv_close_cb>(onClose));
		//
		//	 ERROR_EX_LOG("uv_udp_recv_start() failed: %s", uv_strerror(err));
		// }
		//
		// // Set local address.
		 if (!SetLocalAddress())
		 {
			// uv_close(reinterpret_cast<uv_handle_t*>(m_uvHandle), static_cast<uv_close_cb>(onClose));
		
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
		 if (m_thread.joinable())
		 {
			 m_thread.join();
		 }
		 //

		 socket_util::close(m_socket);
		 // Tell the UV handle that the UdpSocketHandler has been closed.
		 //m_uvHandle->data = nullptr;

		 //// Don't read more.
		 //int err = uv_udp_recv_stop(m_uvHandle);

		 //if (err != 0)
		 //{
			// ERROR_EX_LOG("uv_udp_recv_stop() failed: %s", uv_strerror(err));
		 //}

		 //uv_close(reinterpret_cast<uv_handle_t*>(m_uvHandle), static_cast<uv_close_cb>(onClose));
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
		 {
			 std::shared_ptr<csend_data> data_ptr = std::make_shared<csend_data>();
			 memcpy(&data_ptr->data[0], data, len);
			 data_ptr->len = len;
			 memcpy(&data_ptr->addr, addr, sizeof(*addr));
			 std::lock_guard<std::mutex> lock(m_send_lock);
			 m_send_queue.push_back(data_ptr);
			// ::sendto(m_socket, (char *)data, len, 0, addr, sizeof(*addr));

		 }
		 // sending queue 
		 return;
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

		 err = getsockname(m_socket, reinterpret_cast<struct sockaddr*>(&m_local_addr), &len);

		 if (err != 0)
		 {
			 ERROR_EX_LOG("getsockname() failed: %s", uv_strerror(err));

			 return false;
		 }

		 int family;

		 uv_ip::GetAddressInfo(
			 reinterpret_cast<const struct sockaddr*>(&m_local_addr), family, m_local_ip, m_local_port);

		 return true;
	 }
	 std::mutex g_mutex;
	 void cudp_socket_handler::_work_pthread()
	 {
		 fd_set fd_read_backup_;
		 fd_set fd_write_backup_;
		 fd_set fd_exp_backup_;

		 SOCKET maxfd_ = 0;
		 fd_set fd_read;
		 fd_set fd_write;
		 fd_set fd_exp;
		 FD_ZERO(&fd_read);
		 FD_ZERO(&fd_write);
		 FD_ZERO(&fd_exp);
		 bool fd_read_reset = false;
		 //bool fd_write_reset = false;
		// bool fd_exp_reset = false;
		 static const int32 timeout = 10;
		 std::list<std::shared_ptr<csend_data>>  temp_queue;
		 while (!m_closed)
		 {
			 FD_SET(m_socket, &fd_read);
			 //FD_SET(m_socket, &fd_write);
			 maxfd_ = m_socket;
			 //struct timeval tv = { 0/*timeout / 1000*/, timeout /*% 1000 * 1000*/ };
			 struct timeval tv = {0};
			 tv.tv_usec = timeout;
			/*  std::chrono::steady_clock::time_point cur_time_ms;
				 std::chrono::steady_clock::time_point pre_time = std::chrono::steady_clock::now();
				 std::chrono::steady_clock::duration dur;
				 std::chrono::microseconds ms;*/
			 int ret = ::select((int)maxfd_ + 1, &fd_read, NULL /*&fd_write*/, NULL, &tv);
			 
			 /*cur_time_ms = std::chrono::steady_clock::now();
			 dur = cur_time_ms - pre_time;
			 ms = std::chrono::duration_cast<std::chrono::microseconds>(dur);
			 static FILE *out_file_ptr = fopen("./net_udp.csv", "wb+");
			 if (out_file_ptr)
			 {
				 std::lock_guard<std::mutex> lock(g_mutex);
				 fprintf(out_file_ptr, "mcs = %" PRIi64 "\n", ms.count());
				 fflush(out_file_ptr);
			 }
*/
			 if (ret <= 0) {
#if defined(__linux) || defined(__linux__) 
				 if (errno == EINTR) {
					 return true;
				 }
#endif 
				// std::this_thread::sleep_for(std::chrono::microseconds()
			 }
			 else
			 {
				 if (FD_ISSET(m_socket, &fd_read)) 
				 {
					// events |= EVENT_IN;
					// int recv_byte = ::recvfrom(m_socket, (char *)m_ReadBuffer_ptr, ReadBufferSize, 0, );
					 struct sockaddr_storage from;
					 int from_len;
					 int32 bytes_read = 0;
					 do {
						 memset(&from, 0, sizeof from);
						 from_len = sizeof from;
						   bytes_read = ::recvfrom(m_socket, (char *)(m_ReadBuffer_ptr), ReadBufferSize, 0, (struct sockaddr*)&from, &from_len);
						 if (bytes_read > 0)
						 {
							 // read_callback_(m_data_ptr, bytes_read, &from);
							 UserOnUdpDatagramReceived(m_ReadBuffer_ptr, bytes_read, (const sockaddr*)&from);
						 }
					 } while (bytes_read > 0);
				 }
				 //if (FD_ISSET(m_socket, &fd_write))
				 {
					 {
						 std::lock_guard<std::mutex> lock(m_send_lock);
						 if (!m_send_queue.empty())
						 {
							 temp_queue.swap(m_send_queue);
						 }
					 }
					 while (temp_queue.size())
					 {
						 std::shared_ptr<csend_data> & send_data_ptr = temp_queue.front();

						 ::sendto(m_socket, (char *)&send_data_ptr->data[0], send_data_ptr->len, 0, &send_data_ptr->addr, sizeof(*&send_data_ptr->addr));

						 temp_queue.pop_front();
					 }
				 }
			 }
			
		 }
	 }

}
