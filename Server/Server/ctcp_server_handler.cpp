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


#include "ctcp_server_handler.h"
#include "clog.h"
#include "cuv_util.h"
#include "cuv_ip.h"

namespace chen {

	/* Static methods for UV callbacks. */

	inline static void onConnection(uv_stream_t* handle, int status)
	{
		auto* server = static_cast<ctcp_server_handler*>(handle->data);

		if (server)
			server->OnUvConnection(status);
	}

	inline static void onClose(uv_handle_t* handle)
	{
		delete handle;
	}

	/* Instance methods. */

	// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init)
	ctcp_server_handler::ctcp_server_handler(uv_tcp_t* uvHandle, int backlog) : uvHandle(uvHandle)
	{
		//MS_TRACE();

		int err;

		this->uvHandle->data = static_cast<void*>(this);

		err = uv_listen(
			reinterpret_cast<uv_stream_t*>(this->uvHandle),
			backlog,
			static_cast<uv_connection_cb>(onConnection));

		if (err != 0)
		{
			uv_close(reinterpret_cast<uv_handle_t*>(this->uvHandle), static_cast<uv_close_cb>(onClose));

			ERROR_EX_LOG("uv_listen() failed: %s", uv_strerror(err));
		}

		// Set local address.
		if (!SetLocalAddress())
		{
			uv_close(reinterpret_cast<uv_handle_t*>(this->uvHandle), static_cast<uv_close_cb>(onClose));

			ERROR_EX_LOG("error setting local IP and port");
		}
	}

	ctcp_server_handler::~ctcp_server_handler()
	{
		//MS_TRACE();

		if (!this->closed)
			Close();
	}

	void ctcp_server_handler::Close()
	{
		//MS_TRACE();

		if (this->closed)
			return;

		this->closed = true;

		// Tell the UV handle that the TcpServerHandler has been closed.
		this->uvHandle->data = nullptr;

		NORMAL_EX_LOG("closing %zu active connections", this->connections.size());

		for (auto* connection : this->connections)
		{
			delete connection;
		}

		uv_close(reinterpret_cast<uv_handle_t*>(this->uvHandle), static_cast<uv_close_cb>(onClose));
	}

	void ctcp_server_handler::Dump() const
	{
		NORMAL_EX_LOG("<TcpServerHandler>");
		NORMAL_EX_LOG(
			"  [TCP, local:%s :%" PRIu16 ", status:%s, connections:%zu]",
			this->localIp.c_str(),
			static_cast<uint16_t>(this->localPort),
			(!this->closed) ? "open" : "closed",
			this->connections.size());
		NORMAL_EX_LOG("</TcpServerHandler>");
	}

	void ctcp_server_handler::AcceptTcpConnection(ctcp_connection_handler* connection)
	{
		//MS_TRACE();

		cassert_desc(connection != nullptr, "TcpConnectionHandler pointer was not allocated by the user");

		try
		{
			connection->Setup(this, &(this->localAddr), this->localIp, this->localPort);
		}
		catch (const std::exception& error)
		{
			delete connection;

			return;
		}

		// Accept the connection.
		int err = uv_accept(
			reinterpret_cast<uv_stream_t*>(this->uvHandle),
			reinterpret_cast<uv_stream_t*>(connection->GetUvHandle()));

		if (err != 0)
		{
			ERROR_EX_LOG("uv_accept() failed: %s", uv_strerror(err));
		}

		// Start receiving data.
		try
		{
			// NOTE: This may throw.
			connection->Start();
		}
		catch (const std::exception& error)
		{
			delete connection;

			return;
		}

		// Store it.
		this->connections.insert(connection);
	}

	bool ctcp_server_handler::SetLocalAddress()
	{
		//MS_TRACE();

		int err;
		int len = sizeof(this->localAddr);

		err =
			uv_tcp_getsockname(this->uvHandle, reinterpret_cast<struct sockaddr*>(&this->localAddr), &len);

		if (err != 0)
		{
			ERROR_EX_LOG("uv_tcp_getsockname() failed: %s", uv_strerror(err));

			return false;
		}

		int family;

		uv_ip::GetAddressInfo(
			reinterpret_cast<const struct sockaddr*>(&this->localAddr), family, this->localIp, this->localPort);

		return true;
	}

	inline void ctcp_server_handler::OnUvConnection(int status)
	{
		//MS_TRACE();

		if (this->closed)
			return;

		if (status != 0)
		{
			ERROR_EX_LOG("error while receiving a new TCP connection: %s", uv_strerror(status));

			return;
		}

		// Notify the subclass about a new TCP connection attempt.
		UserOnTcpConnectionAlloc();
	}

	inline void ctcp_server_handler::OnTcpConnectionClosed(ctcp_connection_handler* connection)
	{
		//MS_TRACE();

		NORMAL_EX_LOG("TCP connection closed");

		// Remove the TcpConnectionHandler from the set.
		this->connections.erase(connection);

		// Notify the subclass.
		UserOnTcpConnectionClosed(connection);

		// Delete it.
		delete connection;
	}



}