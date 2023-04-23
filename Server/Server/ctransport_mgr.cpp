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
#include "crtc_transport.h"
#include "ctransport_mgr.h"
#include "clog.h"
namespace chen {
	ctransport_mgr   g_transport_mgr;
	ctransport_mgr::~ctransport_mgr()
	{
	}
	bool ctransport_mgr::init()
	{
		return true;;
	}
	void ctransport_mgr::update(uint32 uDeltaTime)
	{
		//NORMAL_EX_LOG("[all_transport = %u]", m_all_transport_map.size());
		for (TRANSPORT_MAP::iterator iter = m_all_transport_map.begin(); iter != m_all_transport_map.end(); )
		{
			// 心跳包的检查
			//if ()
			{
				iter->second->update(uDeltaTime);
			}
			
			if (iter->second  &&
				!iter->second->is_active())
			{
				
				if (iter->second->get_rtc_type() == ERtcClientPublisher)
				{
					m_all_stream_url_map.erase(iter->second->get_stream_name());
				}
				else
				{
					m_all_consumer_map[iter->second->get_stream_name()].erase(iter->second);

				}
				iter->second->destroy();
				crtc_transport* ptr = iter->second;
				delete ptr;
				iter = m_all_transport_map.erase(iter);
			}
			else
			{
				++iter;
			}
		}

		// stream url 说明ice 密钥协商成功 之后在放到 stream_url_map中


	}
	void ctransport_mgr::destroy()
	{
		for (TRANSPORT_MAP::iterator iter = m_all_transport_map.begin(); iter != m_all_transport_map.end(); ++iter)
		{
			WARNING_EX_LOG("destroy transport map key [username = %s]", iter->first.c_str());
			delete iter->second;
		}
		m_all_transport_map.clear();
		m_all_consumer_map.clear();
		
		//m_all_stream_url_map.clear();
	}
	crtc_transport * ctransport_mgr::find_username(const std::string & username)
	{
		TRANSPORT_MAP::iterator iter = m_all_transport_map.find(username);

		if (iter != m_all_transport_map.end())
		{
			return iter->second;
		}
		return nullptr;
	}
	const crtc_transport * ctransport_mgr::find_username(const std::string & username) const
	{
		TRANSPORT_MAP::const_iterator iter = m_all_transport_map.find(username);

		if (iter != m_all_transport_map.end())
		{
			return iter->second;
		}
		return nullptr;
	}
	bool ctransport_mgr::insert_username(const std::string & username, crtc_transport * transport)
	{

		if (!m_all_transport_map.insert(std::make_pair(username, transport)).second)
		{
			WARNING_EX_LOG("insert transport map failed !!! [username = %s]", username.c_str());
			return false;
		}
		 
		return true;
	}
	bool ctransport_mgr::remove_username(const std::string & username)
	{
		crtc_transport * transport_ptr = find_username(username);
		if (!transport_ptr)
		{
			WARNING_EX_LOG("all transport map not find key [username = %s]", username.c_str());
			return  false;
		}
		size_t size = m_all_transport_map.erase(username);
		NORMAL_EX_LOG("delete key [username = %s][size = %llu]", username.c_str(), size);
		transport_ptr->destroy();
		delete transport_ptr;
		transport_ptr = NULL;

		return true;
	}
	 
	bool ctransport_mgr::remove_stream_name(const std::string & stream_url)
	{
		crtc_transport * transport_ptr = find_username(stream_url);
		if (!transport_ptr)
		{
			WARNING_EX_LOG("all stream url  map not find key [stream_url = %s]", stream_url.c_str());
			return  false;
		}
		size_t size = m_all_transport_map.erase(stream_url);
		NORMAL_EX_LOG("delete key [stream_url = %s][size = %llu]", stream_url.c_str(), size);

		// TODO@chensong 2023-02-10 删除一个网络对象有两个map管理的需要在username中管理非常好


		//transport_ptr->destroy();
		//delete transport_ptr;
		//transport_ptr = NULL;

		return true;
	}
}

