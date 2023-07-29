/***********************************************************************************************
created: 		2023-03-10

author:			chensong

purpose:		ctransport_mgr

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

#ifndef _C_TRANSPORT_TUPLE_H_
#define _C_TRANSPORT_TUPLE_H_
#include "cnet_type.h"
#include "crtc_sdp.h"
#include <unordered_map>
#include "crtc_source_description.h"
#include <crtc_transport.h>
#include "crtp_stream.h"
namespace chen {

	class ctransport_tuple
	{
	public:
		enum ERtcProtocol
		{
			ERtcUdp = 0,
			ERtcTcp,
		};
	public:
		ctransport_tuple(cudp_socket* udpSocket, const struct sockaddr* udpRemoteAddr)
			: m_udp_socket_ptr(udpSocket)
			, m_udp_remote_addr_ptr((struct sockaddr*)udpRemoteAddr)
			, m_tcp_connection_ptr(NULL)
			, m_local_ip("")
			, m_udp_remote_addr_storage()
			, m_rtc_portocol(ERtcUdp)
		{
		}

		explicit ctransport_tuple(ctcp_connection* tcpConnection)
			: m_udp_socket_ptr(NULL)
			, m_udp_remote_addr_ptr(NULL)
			, m_tcp_connection_ptr(tcpConnection)
			, m_local_ip("")
			, m_udp_remote_addr_storage()
			, m_rtc_portocol(ERtcTcp)
		{
		}

		explicit ctransport_tuple(const ctransport_tuple* tuple)
			: m_udp_socket_ptr(tuple->m_udp_socket_ptr), m_udp_remote_addr_ptr(tuple->m_udp_remote_addr_ptr),
			m_tcp_connection_ptr(tuple->m_tcp_connection_ptr), m_local_ip(tuple->m_local_ip),
			m_rtc_portocol(tuple->m_rtc_portocol)
		{
			if (m_rtc_portocol == ERtcUdp)
			{
				StoreUdpRemoteAddress();
			}
		}


	public:
		void StoreUdpRemoteAddress()
		{
			// Clone the given address into our address storage and make the sockaddr
			// pointer point to it.
			this->m_udp_remote_addr_storage = uv_ip::CopyAddress(this->m_udp_remote_addr_ptr);
			this->m_udp_remote_addr_ptr = (struct sockaddr*)&this->m_udp_remote_addr_storage;
		}


		bool Compare(const ctransport_tuple* tuple) const
		{
			if (this->m_rtc_portocol == ERtcUdp && tuple->GetProtocol() == ERtcUdp)
			{
				return (
					this->m_udp_socket_ptr == tuple->m_udp_socket_ptr &&
					uv_ip::CompareAddresses(this->m_udp_remote_addr_ptr, tuple->GetRemoteAddress()));
			}
			else if (this->m_rtc_portocol == ERtcTcp && tuple->GetProtocol() == ERtcTcp)
			{
				return (this->m_tcp_connection_ptr == tuple->m_tcp_connection_ptr);
			}
			else
			{
				return false;
			}
		}

		void SetLocalAnnouncedIp(const std::string& localAnnouncedIp)
		{
			this->m_local_ip = localAnnouncedIp;
		}

		void Send(const uint8_t* data, size_t len)
		{
			if (this->m_rtc_portocol == ERtcUdp)
			{
				this->m_udp_socket_ptr->Send(data, len, m_udp_remote_addr_ptr, NULL);
			}
			else
			{
				this->m_tcp_connection_ptr->Send(data, len, NULL);
			}
		}

		ERtcProtocol GetProtocol() const
		{
			return this->m_rtc_portocol;
		}

		const struct sockaddr* GetLocalAddress() const
		{
			if (this->m_rtc_portocol == ERtcUdp)
				return this->m_udp_socket_ptr->GetLocalAddress();
			else
				return this->m_tcp_connection_ptr->GetLocalAddress();
		}

		const struct sockaddr* GetRemoteAddress() const
		{
			if (this->m_rtc_portocol == ERtcUdp)
			{
				return (const struct sockaddr*)this->m_udp_remote_addr_ptr;
			}
			else
			{
				return this->m_tcp_connection_ptr->GetPeerAddress();
			}
			return 0;
		}

		size_t GetRecvBytes() const
		{
			if (this->m_rtc_portocol == ERtcUdp)
			{
				return this->m_udp_socket_ptr->GetRecvBytes();
			}
			else
			{
				return this->m_tcp_connection_ptr->GetRecvBytes();
			}
			return 0;
		}

		size_t GetSentBytes() const
		{
			if (this->m_rtc_portocol == ERtcUdp)
			{
				return this->m_udp_socket_ptr->GetSentBytes();
			}
			else
			{
				return this->m_tcp_connection_ptr->GetSentBytes();
			}
			return 0;
		}
	protected:
	private:

		cudp_socket *			m_udp_socket_ptr;
		struct sockaddr *		m_udp_remote_addr_ptr;
		ctcp_connection*		m_tcp_connection_ptr;
		std::string				m_local_ip;

		// others
		struct sockaddr_storage m_udp_remote_addr_storage;
		ERtcProtocol		    m_rtc_portocol;
	};
}

#endif // 