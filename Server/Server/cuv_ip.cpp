/***********************************************************************************************
created: 		2022-08-12

author:			chensong

purpose:		cuv_ip
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

#include "cuv_ip.h"


namespace chen {
	namespace uv_ip
	{
		int GetFamily(const std::string & ip)
		{
			if (ip.size() >= INET6_ADDRSTRLEN)
			{
				return AF_UNSPEC;
			}

			auto ipPtr = ip.c_str();
			char ipBuffer[INET6_ADDRSTRLEN] = { 0 };

			if (uv_inet_pton(AF_INET, ipPtr, ipBuffer) == 0)
			{
				return AF_INET;
			}
			else if (uv_inet_pton(AF_INET6, ipPtr, ipBuffer) == 0)
			{
				return AF_INET6;
			}
			else
			{
				return AF_UNSPEC;
			}
		}
		void GetAddressInfo(const sockaddr * addr, int & family, std::string & ip, uint16_t & port)
		{
			char ipBuffer[INET6_ADDRSTRLEN] = { 0 };
			int err;

			switch (addr->sa_family)
			{
			case AF_INET:
			{
				err = uv_inet_ntop(
					AF_INET,
					std::addressof(reinterpret_cast<const struct sockaddr_in*>(addr)->sin_addr),
					ipBuffer,
					sizeof(ipBuffer));

				//if (err)
				//	MS_ABORT("uv_inet_ntop() failed: %s", uv_strerror(err));

				port =
					static_cast<uint16_t>(ntohs(reinterpret_cast<const struct sockaddr_in*>(addr)->sin_port));

				break;
			}

			case AF_INET6:
			{
				err = uv_inet_ntop(
					AF_INET6,
					std::addressof(reinterpret_cast<const struct sockaddr_in6*>(addr)->sin6_addr),
					ipBuffer,
					sizeof(ipBuffer));

				//if (err)
					//MS_ABORT("uv_inet_ntop() failed: %s", uv_strerror(err));

				port =
					static_cast<uint16_t>(ntohs(reinterpret_cast<const struct sockaddr_in6*>(addr)->sin6_port));

				break;
			}

			default:
			{
			//	MS_ABORT("unknown network family: %d", static_cast<int>(addr->sa_family));
			}
			}

			family = addr->sa_family;
			ip.assign(ipBuffer);
		}
		bool CompareAddresses(const sockaddr * addr1, const sockaddr * addr2)

		{
			// Compare family.
			if (addr1->sa_family != addr2->sa_family || (addr1->sa_family != AF_INET && addr1->sa_family != AF_INET6))
			{
				return false;
			}

			// Compare port.
			if (
				reinterpret_cast<const struct sockaddr_in*>(addr1)->sin_port !=
				reinterpret_cast<const struct sockaddr_in*>(addr2)->sin_port)
			{
				return false;
			}

			// Compare IP.
			switch (addr1->sa_family)
			{
			case AF_INET:
			{
				return (
					reinterpret_cast<const struct sockaddr_in*>(addr1)->sin_addr.s_addr ==
					reinterpret_cast<const struct sockaddr_in*>(addr2)->sin_addr.s_addr);
			}

			case AF_INET6:
			{
				return (
					std::memcmp(
						std::addressof(reinterpret_cast<const struct sockaddr_in6*>(addr1)->sin6_addr),
						std::addressof(reinterpret_cast<const struct sockaddr_in6*>(addr2)->sin6_addr),
						16) == 0
					? true
					: false);
			}

			default:
			{
				return false;
			}
			}
		}
		sockaddr_storage CopyAddress(const sockaddr * addr)

		{
			struct sockaddr_storage copiedAddr;

			switch (addr->sa_family)
			{
			case AF_INET:
				std::memcpy(std::addressof(copiedAddr), addr, sizeof(struct sockaddr_in));
				break;

			case AF_INET6:
				std::memcpy(std::addressof(copiedAddr), addr, sizeof(struct sockaddr_in6));
				break;
			}

			return copiedAddr;
		}
		void NormalizeIp(std::string & ip)
		{
			sockaddr_storage addrStorage;
			char ipBuffer[INET6_ADDRSTRLEN] = { 0 };
			int err;

			switch ( GetFamily(ip))
			{
			case AF_INET:
			{
				err = uv_ip4_addr(ip.c_str(), 0, reinterpret_cast<struct sockaddr_in*>(&addrStorage));

				if (err != 0)
				{
					ERROR_EX_LOG("uv_ip4_addr() failed [ip:'%s']: %s", ip.c_str(), uv_strerror(err));
				}

				err = uv_ip4_name(
					reinterpret_cast<const struct sockaddr_in*>(std::addressof(addrStorage)),
					ipBuffer,
					sizeof(ipBuffer));

				if (err != 0)
				{
					ERROR_EX_LOG("uv_ipv4_name() failed [ip:'%s']: %s", ip.c_str(), uv_strerror(err));
				}

				ip.assign(ipBuffer);

				break;
			}

			case AF_INET6:
			{
				err = uv_ip6_addr(ip.c_str(), 0, reinterpret_cast<struct sockaddr_in6*>(&addrStorage));

				if (err != 0)
				{
					ERROR_EX_LOG("uv_ip6_addr() failed [ip:'%s']: %s", ip.c_str(), uv_strerror(err));
				}

				err = uv_ip6_name(
					reinterpret_cast<const struct sockaddr_in6*>(std::addressof(addrStorage)),
					ipBuffer,
					sizeof(ipBuffer));

				if (err != 0)
				{
					ERROR_EX_LOG("uv_ipv6_name() failed [ip:'%s']: %s", ip.c_str(), uv_strerror(err));
				}

				ip.assign(ipBuffer);

				break;
			}

			default:
			{
				ERROR_EX_LOG("invalid IP '%s'", ip.c_str());
			}
			}
		}
	}
}