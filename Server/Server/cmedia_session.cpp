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
#include "cmedia_source.h"
#include "cmedia_session.h"
#include <random>
namespace chen {
	//std::atomic_uint cmedia_session::m_last_session_id_(1);

	static uint64 g_last_session_id = 0;

	cmedia_session* cmedia_session::construct()
	{
		return new cmedia_session();
	}

	void cmedia_session::destroy(cmedia_session * session)
	{
		delete session;
	}

	bool cmedia_session::init(const std::string & url_suffix)
	{
		//suffix_(url_suffxx)
			//, media_sources_(MAX_MEDIA_CHANNEL)
			//, buffer_(MAX_MEDIA_CHANNEL)
		m_suffix = url_suffix;
		m_media_sources.resize(MAX_MEDIA_CHANNEL);
		//m_buffers.resize(MAX_MEDIA_CHANNEL);
		m_has_new_client = false;
		m_media_session = ++g_last_session_id ;

		for (int n = 0; n < MAX_MEDIA_CHANNEL; n++) 
		{
			m_multicast_port[n] = 0;
		}
		return true;
	}

	void cmedia_session::destroy()
	{
	}

	bool cmedia_session::add_source(MediaChannelId channel_id, cmedia_source * source)
	{
		source->set_send_frame_callback([this](MediaChannelId channel_id, RtpPacket pkt) 
		{
			//std::forward_list<std::shared_ptr<RtpConnection>> clients;
			std::map<int, RtpPacket> packets;
			{
				//std::lock_guard<std::mutex> lock(map_mutex_);
				/*for (auto iter = clients_.begin(); iter != clients_.end();) {
					auto conn = iter->second.lock();
					if (conn == nullptr) {
						clients_.erase(iter++);
					}
					else {
						int id = conn->GetId();
						if (id >= 0) {
							if (packets.find(id) == packets.end()) {
								RtpPacket tmp_pkt;
								memcpy(tmp_pkt.data.get(), pkt.data.get(), pkt.size);
								tmp_pkt.size = pkt.size;
								tmp_pkt.last = pkt.last;
								tmp_pkt.timestamp = pkt.timestamp;
								tmp_pkt.type = pkt.type;
								packets.emplace(id, tmp_pkt);
							}
							clients.emplace_front(conn);
						}
						iter++;
					}
				}*/
			}

			int count = 0;
			for (const std::pair<crtp_connection*, int32> pi : m_clients) 
			{
				int ret = 0;
				//int id = iter->GetId();
				//if (id >= 0) 
				{
					//auto iter2 = packets.find(id);
					//if (iter2 != packets.end()) 
					{
						//count++;
						ret = pi.first->SendRtpPacket(channel_id, pkt);
						if (m_is_multicast && ret == 0) 
						{
							break;
						}
					}
				}
			}
			return true;
		});

		m_media_sources[channel_id] = source;
		return true;
	}

	bool cmedia_session::remove_source(MediaChannelId channel_id)
	{
		delete m_media_sources[channel_id];
		m_media_sources[channel_id] = NULL;
		return true;
	}

	bool cmedia_session::start_multicast()
	{
		if (m_is_multicast)
		{
			return true;
		}

		m_multicast_ip = MulticastAddr::instance().GetAddr();
		if (m_multicast_ip == "") 
		{
			return false;
		}

		std::random_device rd;
		m_multicast_port[channel_0] = htons(rd() & 0xfffe);
		m_multicast_port[channel_1] = htons(rd() & 0xfffe);

		m_is_multicast = true;
		return true;
		return true;
	}

	std::string cmedia_session::get_sdp_message(const std::string & ip, const std::string & session_name)
	{
		if (m_sdp != "") {
			return m_sdp;
		}

		if (m_media_sources.empty()) {
			return "";
		}

		char buf[2048] = { 0 };

		snprintf(buf, sizeof(buf),
			"v=0\r\n"
			"o=- 9%ld 1 IN IP4 %s\r\n"
			"t=0 0\r\n"
			"a=control:*\r\n",
			(long)std::time(NULL), ip.c_str());

		if (session_name != "") {
			snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf),
				"s=%s\r\n",
				session_name.c_str());
		}

		if (m_is_multicast) {
			snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf),
				"a=type:broadcast\r\n"
				"a=rtcp-unicast: reflection\r\n");
		}

		for (uint32_t chn = 0; chn < m_media_sources.size(); chn++) {
			if (m_media_sources[chn]) {
				if (m_is_multicast) {
					snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf),
						"%s\r\n",
						m_media_sources[chn]->get_media_description(m_multicast_port[chn]).c_str());

					snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf),
						"c=IN IP4 %s/255\r\n",
						m_multicast_ip.c_str());
				}
				else {
					snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf),
						"%s\r\n",
						m_media_sources[chn]->get_media_description(0).c_str());
				}

				snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf),
					"%s\r\n",
					m_media_sources[chn]->get_attribute().c_str());

				snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf),
					"a=control:track%d\r\n", chn);
			}
		}

		m_sdp = buf;
		return m_sdp;
	}

	cmedia_source * cmedia_session::get_media_source(MediaChannelId channel_id)
	{
		if (m_media_sources[channel_id]) 
		{
			return m_media_sources[channel_id];
		}
		return nullptr;
	}

	bool cmedia_session::handler_frame(MediaChannelId channel_id, AVFrame frame)
	{
		if (m_media_sources[channel_id]) {
			m_media_sources[channel_id]->handler_frame(channel_id, frame);
		}
		else {
			return false;
		}

		return true;
	}

	bool cmedia_session::add_client(crtp_connection * rtspconnection)
	{
		auto iter = m_clients.find(rtspconnection);
		if (iter == m_clients.end()) 
		{
			m_clients.insert(std::make_pair(rtspconnection, 1));
			//std::weak_ptr<RtpConnection> rtp_conn_weak_ptr = rtp_conn;
			//clients_.emplace(rtspfd, rtp_conn_weak_ptr);
			//for (auto& callback : notify_connected_callbacks_) {
			//	callback(session_id_, rtp_conn->GetIp(), rtp_conn->GetPort());
			//}

			m_has_new_client = true;
			return true;
		}

		return false;
	}

	bool cmedia_session::remove_client(crtp_connection* rtsp_connection)
	{
		auto iter = m_clients.find(rtsp_connection);
		if (iter != m_clients.end()) {
			//auto conn = iter->second.lock();
			//if (conn) {
			//	for (auto& callback : notify_disconnected_callbacks_) {
			//		callback(session_id_, conn->GetIp(), conn->GetPort());
			//	}
			//}
			m_clients.erase(iter);
			return true;
		}
		return false;
	}

}