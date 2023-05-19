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


#ifndef _C_TCP_CONNECTION_HANDLER_H_
#define _C_TCP_CONNECTION_HANDLER_H_

#include "cnet_type.h"

#include <uv.h>
#include <functional>
#include <cstring>
#include <ws2def.h>
#include <string>

namespace chen {

	class ctcp_connection_handler
	{
	protected:
	protected:
		using onSendCallback = const std::function<void(bool sent)>;

	public:
		class Listener
		{
		public:
			virtual ~Listener() = default;

		public:
			//virtual void OnTcpConnectionNew(ctcp_connection_handler* connection) = 0;
			virtual void OnTcpConnectionClosed(ctcp_connection_handler* connection) = 0;
		};

	public:
		/* Struct for the data field of uv_req_t when writing into the connection. */
		struct UvWriteData
		{
			explicit UvWriteData(size_t storeSize)
			{
				this->store = new uint8_t[storeSize];
			}

			// Disable copy constructor because of the dynamically allocated data (store).
			UvWriteData(const UvWriteData&) = delete;

			~UvWriteData()
			{
				delete[] this->store;
				delete this->cb;
			}

			uv_write_t req;
			uint8_t* store{ nullptr };
			ctcp_connection_handler::onSendCallback* cb{ nullptr };
		};

	public:
		explicit ctcp_connection_handler(size_t bufferSize);
		ctcp_connection_handler& operator=(const ctcp_connection_handler&) = delete;
		ctcp_connection_handler(const ctcp_connection_handler&) = delete;
		virtual ~ctcp_connection_handler();

	public:
		void Close();
		virtual void Dump() const;
		void Setup(
			Listener* listener,
			struct sockaddr_storage* localAddr,
			const std::string& localIp,
			uint16_t localPort);
		bool IsClosed() const
		{
			return this->closed;
		}
		uv_tcp_t* GetUvHandle() const
		{
			return this->uvHandle;
		}
		void Start();
		void Write(const uint8_t* data, size_t len, ctcp_connection_handler::onSendCallback* cb);
		void Write(
			const uint8_t* data1,
			size_t len1,
			const uint8_t* data2,
			size_t len2,
			ctcp_connection_handler::onSendCallback* cb);
		void ErrorReceiving();
		const struct sockaddr* GetLocalAddress() const
		{
			return reinterpret_cast<const struct sockaddr*>(this->localAddr);
		}
		int GetLocalFamily() const
		{
			return reinterpret_cast<const struct sockaddr*>(this->localAddr)->sa_family;
		}
		const std::string& GetLocalIp() const
		{
			return this->localIp;
		}
		uint16_t GetLocalPort() const
		{
			return this->localPort;
		}
		const struct sockaddr* GetPeerAddress() const
		{
			return reinterpret_cast<const struct sockaddr*>(&this->peerAddr);
		}
		const std::string& GetPeerIp() const
		{
			return this->peerIp;
		}
		uint16_t GetPeerPort() const
		{
			return this->peerPort;
		}
		size_t GetRecvBytes() const
		{
			return this->recvBytes;
		}
		size_t GetSentBytes() const
		{
			return this->sentBytes;
		}

	private:
		bool SetPeerAddress();

		/* Callbacks fired by UV events. */
	public:
		void OnUvReadAlloc(size_t suggestedSize, uv_buf_t* buf);
		void OnUvRead(ssize_t nread, const uv_buf_t* buf);
		void OnUvWrite(int status, onSendCallback* cb);

		/* Pure virtual methods that must be implemented by the subclass. */
	protected:
		virtual void UserOnTcpConnectionRead() = 0;

	protected:
		// Passed by argument.
		size_t bufferSize{ 0u };
		// Allocated by this.
		uint8_t* buffer{ nullptr };
		// Others.
		size_t bufferDataLen{ 0u };
		std::string localIp;
		uint16_t localPort{ 0u };
		struct sockaddr_storage peerAddr;
		std::string peerIp;
		uint16_t peerPort{ 0u };

	private:
		// Passed by argument.
		Listener* listener{ nullptr };
		// Allocated by this.
		uv_tcp_t* uvHandle{ nullptr };
		// Others.
		struct sockaddr_storage* localAddr{ nullptr };
		bool closed{ false };
		size_t recvBytes{ 0u };
		size_t sentBytes{ 0u };
		bool isClosedByPeer{ false };
		bool hasError{ false };
	public:
	protected:
	private:
	};
}
#endif // 