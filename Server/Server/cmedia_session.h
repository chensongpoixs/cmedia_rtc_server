/***********************************************************************************************
created: 		2023-01-15

author:			chensong

purpose:		_C_CODEC_PAYLOAD_H_
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

#ifndef _C_MEDIA_SESSION_H_
#define _C_MEDIA_SESSION_H_
#include "cnet_type.h"
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include "crtc_sdp.h"
#include "cmedia_desc.h"
#include "cmedia_source.h"
#include "cring_buffer.h"
#include <unordered_set>
#include <random>
#include "crtp_connection.h"
namespace chen {

	class cmedia_session
	{
	public:
		explicit cmedia_session(){}
		virtual ~cmedia_session() {}

	public:
		static cmedia_session* construct();
		static void destroy(cmedia_session * session);
	public:
		bool init(const std::string & url_suffix = "live");
		void destroy();


	public:

		bool add_source(MediaChannelId channel_id, cmedia_source* source);
		bool remove_source(MediaChannelId channel_id);

		bool start_multicast();

		std::string get_rtsp_url_suffix() const { return m_suffix; }
		void set_rtsp_url_suffix(const std::string & suffix) { m_suffix = suffix; }


		std::string get_sdp_message(const std::string &ip, const std::string & session_name = "");

		cmedia_source * get_media_source(MediaChannelId channel_id);

		bool handler_frame(MediaChannelId channel_id, AVFrame frame);

		bool add_client(crtp_connection* rtspconnection);

		bool remove_client(crtp_connection* rtsp_connection);

		uint64 get_media_session_id() const { return m_media_session; }

		uint32 get_num_client() const { return m_clients.size(); }

		bool is_multicast() const {
			return m_is_multicast;
		}
		std::string get_multicast_ip() const { return m_multicast_ip; }
		uint16 get_multicast_port (MediaChannelId channel_id) const 
		{
			if (channel_id >= MAX_MEDIA_CHANNEL)
			{
				return 0;
			}
			return m_multicast_port[channel_id];
		}
	protected:
	private:
		uint64			m_media_session;
		std::string		m_suffix;
		std::string		m_sdp;

		std::vector<cmedia_source*>    m_media_sources;
		std::vector<cring_buffer<AVFrame>>	m_buffers;


		std::map<crtp_connection*, uint32>     m_clients;


		bool			m_is_multicast = false;
		uint16          m_multicast_port[MAX_MEDIA_CHANNEL];
		std::string		m_multicast_ip;
		std::atomic_bool m_has_new_client;

		

	};


	class MulticastAddr
	{
	public:
		static MulticastAddr& instance()
		{
			static MulticastAddr s_multi_addr;
			return s_multi_addr;
		}

		std::string GetAddr()
		{
			std::lock_guard<std::mutex> lock(mutex_);
			std::string addr_str;
			struct sockaddr_in addr = { 0 };
			std::random_device rd;

			for (int n = 0; n <= 10; n++) {
				uint32_t range = 0xE8FFFFFF - 0xE8000100;
				addr.sin_addr.s_addr = htonl(0xE8000100 + (rd()) % range);
				addr_str = inet_ntoa(addr.sin_addr);

				if (addrs_.find(addr_str) != addrs_.end()) {
					addr_str.clear();
				}
				else {
					addrs_.insert(addr_str);
					break;
				}
			}

			return addr_str;
		}

		void Release(std::string addr) {
			std::lock_guard<std::mutex> lock(mutex_);
			addrs_.erase(addr);
		}

	private:
		std::mutex mutex_;
		std::unordered_set<std::string> addrs_;
	};
}

#endif