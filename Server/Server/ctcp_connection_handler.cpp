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

#include "ctcp_connection_handler.h"
#include "clog.h"
#include "cuv_util.h"
#include "cuv_ip.h"
#include <iostream>
namespace chen {


	/* Static methods for UV callbacks. */

	inline static void onAlloc(uv_handle_t* handle, size_t suggestedSize, uv_buf_t* buf)
	{
		auto* connection = static_cast<ctcp_connection_handler*>(handle->data);

		if (connection)
			connection->OnUvReadAlloc(suggestedSize, buf);
	}

	inline static void onRead(uv_stream_t* handle, ssize_t nread, const uv_buf_t* buf)
	{
		auto* connection = static_cast<ctcp_connection_handler*>(handle->data);

		if (connection)
			connection->OnUvRead(nread, buf);
	}

	inline static void onWrite(uv_write_t* req, int status)
	{
		auto* writeData = static_cast<ctcp_connection_handler::UvWriteData*>(req->data);
		auto* handle = req->handle;
		auto* connection = static_cast<ctcp_connection_handler*>(handle->data);
		auto* cb = writeData->cb;

		if (connection)
			connection->OnUvWrite(status, cb);

		// Delete the UvWriteData struct and the cb.
		delete writeData;
	}

	inline static void onClose(uv_handle_t* handle)
	{
		delete handle;
	}

	inline static void onShutdown(uv_shutdown_t* req, int /*status*/)
	{
		auto* handle = req->handle;

		delete req;

		// Now do close the handle.
		uv_close(reinterpret_cast<uv_handle_t*>(handle), static_cast<uv_close_cb>(onClose));
	}

	/* Instance methods. */

	// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init)
	ctcp_connection_handler::ctcp_connection_handler(size_t bufferSize) : bufferSize(bufferSize)
	{
		//MS_TRACE();

		this->uvHandle = new uv_tcp_t;
		this->uvHandle->data = static_cast<void*>(this);

		// NOTE: Don't allocate the buffer here. Instead wait for the first uv_alloc_cb().
	}

	ctcp_connection_handler::~ctcp_connection_handler()
	{
		//MS_TRACE();

		if (!this->closed)
			Close();

		delete[] this->buffer;
	}

	void ctcp_connection_handler::Close()
	{
		//MS_TRACE();

		if (this->closed)
			return;

		int err;

		this->closed = true;

		// Tell the UV handle that the TcpConnectionHandler has been closed.
		this->uvHandle->data = nullptr;

		// Don't read more.
		err = uv_read_stop(reinterpret_cast<uv_stream_t*>(this->uvHandle));

		if (err != 0)
		{
			ERROR_EX_LOG("uv_read_stop() failed: %s", uv_strerror(err));
		}

		// If there is no error and the peer didn't close its connection side then close gracefully.
		if (!this->hasError && !this->isClosedByPeer)
		{
			// Use uv_shutdown() so pending data to be written will be sent to the peer
			// before closing.
			auto req = new uv_shutdown_t;
			req->data = static_cast<void*>(this);
			err = uv_shutdown(
				req, reinterpret_cast<uv_stream_t*>(this->uvHandle), static_cast<uv_shutdown_cb>(onShutdown));

			if (err != 0)
			{
				ERROR_EX_LOG("uv_shutdown() failed: %s", uv_strerror(err));
			}
		}
		// Otherwise directly close the socket.
		else
		{
			uv_close(reinterpret_cast<uv_handle_t*>(this->uvHandle), static_cast<uv_close_cb>(onClose));
		}
	}

	void ctcp_connection_handler::Dump() const
	{
		NORMAL_EX_LOG("<TcpConnectionHandler>");
		NORMAL_EX_LOG("  localIp    : %s", this->localIp.c_str());
		NORMAL_EX_LOG("  localPort  : %" PRIu16, static_cast<uint16_t>(this->localPort));
		NORMAL_EX_LOG("  remoteIp   : %s", this->peerIp.c_str());
		NORMAL_EX_LOG("  remotePort : %" PRIu16, static_cast<uint16_t>(this->peerPort));
		NORMAL_EX_LOG("  closed     : %s", !this->closed ? "open" : "closed");
		NORMAL_EX_LOG("</TcpConnectionHandler>");
	}

	void ctcp_connection_handler::Setup(
		Listener* listener, struct sockaddr_storage* localAddr, const std::string& localIp, uint16_t localPort)
	{
		//MS_TRACE();

		// Set the UV handle.
		int err = uv_tcp_init(uv_util::get_loop(), this->uvHandle);

		if (err != 0)
		{
			delete this->uvHandle;
			this->uvHandle = nullptr;

			ERROR_EX_LOG("uv_tcp_init() failed: %s", uv_strerror(err));
		}

		// Set the listener.
		this->listener = listener;

		// Set the local address.
		this->localAddr = localAddr;
		this->localIp = localIp;
		this->localPort = localPort;
	}

	void ctcp_connection_handler::Start()
	{
	//	MS_TRACE();

		if (this->closed)
			return;

		int err = uv_read_start(
			reinterpret_cast<uv_stream_t*>(this->uvHandle),
			static_cast<uv_alloc_cb>(onAlloc),
			static_cast<uv_read_cb>(onRead));

		if (err != 0)
			ERROR_EX_LOG("uv_read_start() failed: %s", uv_strerror(err));

		// Get the peer address.
		if (!SetPeerAddress())
			ERROR_EX_LOG("error setting peer IP and port");
	}

	void ctcp_connection_handler::Write(
		const uint8_t* data, size_t len, ctcp_connection_handler::onSendCallback* cb)
	{
		//MS_TRACE();

		if (this->closed)
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

		// First try uv_try_write(). In case it can not directly write all the given
		// data then build a uv_req_t and use uv_write().

		uv_buf_t buffer = uv_buf_init(reinterpret_cast<char*>(const_cast<uint8_t*>(data)), len);
		int written = uv_try_write(reinterpret_cast<uv_stream_t*>(this->uvHandle), &buffer, 1);

		// All the data was written. Done.
		if (written == static_cast<int>(len))
		{
			// Update sent bytes.
			this->sentBytes += written;

			if (cb)
			{
				(*cb)(true);
				delete cb;
			}

			return;
		}
		// Cannot write any data at first time. Use uv_write().
		else if (written == UV_EAGAIN || written == UV_ENOSYS)
		{
			// Set written to 0 so pendingLen can be properly calculated.
			written = 0;
		}
		// Any other error.
		else if (written < 0)
		{
			WARNING_EX_LOG("uv_try_write() failed, trying uv_write(): %s", uv_strerror(written));

			// Set written to 0 so pendingLen can be properly calculated.
			written = 0;
		}

		size_t pendingLen = len - written;
		auto* writeData = new UvWriteData(pendingLen);

		writeData->req.data = static_cast<void*>(writeData);
		std::memcpy(writeData->store, data + written, pendingLen);
		writeData->cb = cb;

		buffer = uv_buf_init(reinterpret_cast<char*>(writeData->store), pendingLen);

		int err = uv_write(
			&writeData->req,
			reinterpret_cast<uv_stream_t*>(this->uvHandle),
			&buffer,
			1,
			static_cast<uv_write_cb>(onWrite));

		if (err != 0)
		{
			WARNING_EX_LOG("uv_write() failed: %s", uv_strerror(err));

			if (cb)
				(*cb)(false);

			// Delete the UvWriteData struct (it will delete the store and cb too).
			delete writeData;
		}
		else
		{
			// Update sent bytes.
			this->sentBytes += pendingLen;
		}
	}

	void ctcp_connection_handler::Write(
		const uint8_t* data1,
		size_t len1,
		const uint8_t* data2,
		size_t len2,
		ctcp_connection_handler::onSendCallback* cb)
	{
		//MS_TRACE();

		if (this->closed)
		{
			if (cb)
			{
				(*cb)(false);
				delete cb;
			}

			return;
		}

		if (len1 == 0 && len2 == 0)
		{
			if (cb)
			{
				(*cb)(false);
				delete cb;
			}

			return;
		}

		size_t totalLen = len1 + len2;
		uv_buf_t buffers[2];
		int written{ 0 };
		int err;

		// First try uv_try_write(). In case it can not directly write all the given
		// data then build a uv_req_t and use uv_write().

		buffers[0] = uv_buf_init(reinterpret_cast<char*>(const_cast<uint8_t*>(data1)), len1);
		buffers[1] = uv_buf_init(reinterpret_cast<char*>(const_cast<uint8_t*>(data2)), len2);
		written = uv_try_write(reinterpret_cast<uv_stream_t*>(this->uvHandle), buffers, 2);

		// All the data was written. Done.
		if (written == static_cast<int>(totalLen))
		{
			// Update sent bytes.
			this->sentBytes += written;

			if (cb)
			{
				(*cb)(true);
				delete cb;
			}

			return;
		}
		// Cannot write any data at first time. Use uv_write().
		else if (written == UV_EAGAIN || written == UV_ENOSYS)
		{
			// Set written to 0 so pendingLen can be properly calculated.
			written = 0;
		}
		// Any other error.
		else if (written < 0)
		{
			WARNING_EX_LOG("uv_try_write() failed, trying uv_write(): %s", uv_strerror(written));

			// Set written to 0 so pendingLen can be properly calculated.
			written = 0;
		}

		size_t pendingLen = totalLen - written;
		auto* writeData = new UvWriteData(pendingLen);

		writeData->req.data = static_cast<void*>(writeData);

		// If the first buffer was not entirely written then splice it.
		if (static_cast<size_t>(written) < len1)
		{
			std::memcpy(
				writeData->store, data1 + static_cast<size_t>(written), len1 - static_cast<size_t>(written));
			std::memcpy(writeData->store + (len1 - static_cast<size_t>(written)), data2, len2);
		}
		// Otherwise just take the pending data in the second buffer.
		else
		{
			std::memcpy(
				writeData->store,
				data2 + (static_cast<size_t>(written) - len1),
				len2 - (static_cast<size_t>(written) - len1));
		}

		writeData->cb = cb;

		uv_buf_t buffer = uv_buf_init(reinterpret_cast<char*>(writeData->store), pendingLen);

		err = uv_write(
			&writeData->req,
			reinterpret_cast<uv_stream_t*>(this->uvHandle),
			&buffer,
			1,
			static_cast<uv_write_cb>(onWrite));

		if (err != 0)
		{
			WARNING_EX_LOG("uv_write() failed: %s", uv_strerror(err));

			if (cb)
				(*cb)(false);

			// Delete the UvWriteData struct (it will delete the store and cb too).
			delete writeData;
		}
		else
		{
			// Update sent bytes.
			this->sentBytes += pendingLen;
		}
	}

	void ctcp_connection_handler::ErrorReceiving()
	{
		//MS_TRACE();

		Close();

		this->listener->OnTcpConnectionClosed(this);
	}

	bool ctcp_connection_handler::SetPeerAddress()
	{
		//MS_TRACE();

		int err;
		int len = sizeof(this->peerAddr);

		err = uv_tcp_getpeername(this->uvHandle, reinterpret_cast<struct sockaddr*>(&this->peerAddr), &len);

		if (err != 0)
		{
			ERROR_EX_LOG("uv_tcp_getpeername() failed: %s", uv_strerror(err));

			return false;
		}

		int family;

		uv_ip::GetAddressInfo(
			reinterpret_cast<const struct sockaddr*>(&this->peerAddr), family, this->peerIp, this->peerPort);

		return true;
	}

	inline void ctcp_connection_handler::OnUvReadAlloc(size_t /*suggestedSize*/, uv_buf_t* buf)
	{
		//MS_TRACE();

		// If this is the first call to onUvReadAlloc() then allocate the receiving buffer now.
		if (!this->buffer)
			this->buffer = new uint8_t[this->bufferSize];

		// Tell UV to write after the last data byte in the buffer.
		buf->base = reinterpret_cast<char*>(this->buffer + this->bufferDataLen);

		// Give UV all the remaining space in the buffer.
		if (this->bufferSize > this->bufferDataLen)
		{
			buf->len = this->bufferSize - this->bufferDataLen;
		}
		else
		{
			buf->len = 0;

			WARNING_EX_LOG("no available space in the buffer");
		}
	}

	inline void ctcp_connection_handler::OnUvRead(ssize_t nread, const uv_buf_t* /*buf*/)
	{
		//MS_TRACE();

		if (nread == 0)
			return;

		// Data received.
		if (nread > 0)
		{
			// Update received bytes.
			this->recvBytes += nread;

			// Update the buffer data length.
			this->bufferDataLen += static_cast<size_t>(nread);

			// Notify the subclass.
			UserOnTcpConnectionRead();
		}
		// Client disconnected.
		else if (nread == UV_EOF || nread == UV_ECONNRESET)
		{
			DEBUG_EX_LOG("connection closed by peer, closing server side");

			this->isClosedByPeer = true;

			// Close server side of the connection.
			Close();

			// Notify the listener.
			this->listener->OnTcpConnectionClosed(this);
		}
		// Some error.
		else
		{
			WARNING_EX_LOG("read error, closing the connection: %s", uv_strerror(nread));

			this->hasError = true;

			// Close server side of the connection.
			Close();

			// Notify the listener.
			this->listener->OnTcpConnectionClosed(this);
		}
	}

	inline void ctcp_connection_handler::OnUvWrite(int status, ctcp_connection_handler::onSendCallback* cb)
	{
		//MS_TRACE();

		// NOTE: Do not delete cb here since it will be delete in onWrite() above.

		if (status == 0)
		{
			if (cb)
				(*cb)(true);
		}
		else
		{
			if (status != UV_EPIPE && status != UV_ENOTCONN)
				this->hasError = true;

			WARNING_EX_LOG("write error, closing the connection: %s", uv_strerror(status));

			if (cb)
				(*cb)(false);

			Close();

			this->listener->OnTcpConnectionClosed(this);
		}
	}

}