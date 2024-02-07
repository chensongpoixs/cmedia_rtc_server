/***********************************************************************************************
created: 		2019-04-30

author:			chensong

purpose:		wan_server
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

#include "cwan_server.h"
#include "clog.h"
#include "ccfg.h"
#include "cglobal_rtc.h"
#include "cmsg_base_id.h"

namespace chen {
	 
	cwan_server   g_wan_server;
	cwan_server::cwan_server()
		: m_websocket_server_ptr(NULL)
		, m_stoped(false)
		, m_max_session_num(0)
		, m_session_map( )
		, m_update_timer(0)
	{
	}
	cwan_server::~cwan_server()
	{
	}
	bool cwan_server::init()
	{
		m_websocket_server_ptr = new cwebsocket_server_mgr();
		if (!m_websocket_server_ptr)
		{
			ERROR_EX_LOG("new cwebsocket_server_mgr failed !!!");
			return false;
		}
		/*m_max_session_num = g_cfg.get_uint32(ECI_WanClientMaxNum);

		const uint32 recv_buf_size = g_cfg.get_uint32(ECI_WanInputBufSize);
		const uint32 send_buf_size = g_cfg.get_uint32(ECI_WanOutputBufSize);
		const uint32 pool_size = g_cfg.get_uint32(ECI_WanMemPoolSize);*/
		if (!m_websocket_server_ptr->init())
		{
			return false;
		}

		m_websocket_server_ptr->set_connect_callback(
			std::bind(&cwan_server::on_connect, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
		);

		m_websocket_server_ptr->set_msg_callback(
			std::bind(&cwan_server::on_msg_receive, this, std::placeholders::_1, std::placeholders::_2
				, std::placeholders::_3)
		);

		m_websocket_server_ptr->set_disconnect_callback(
			std::bind(&cwan_server::on_disconnect, this, std::placeholders::_1)
		);

		 
		 

		 
		return true;
	}
	void cwan_server::destroy()
	{
		/*if (m_session_ptr)
		{
			for (uint32 i = 0; i < m_max_session_num; ++i)
			{
				m_session_ptr[i].destroy();
			}
			delete[] m_session_ptr;
			m_session_ptr = NULL;
		}*/
		if (m_websocket_server_ptr)
		{
			m_websocket_server_ptr->destroy();
			if (m_websocket_server_ptr)
			{
				delete m_websocket_server_ptr;
				m_websocket_server_ptr = NULL;
			}

		}
		m_max_session_num = 0;
	}
	bool cwan_server::startup()
	{
		if (!m_websocket_server_ptr)
		{
			return false;
		}
		if (!m_websocket_server_ptr->startup(1, g_cfg.get_string(ECI_WebSocketWanIp).c_str(), g_cfg.get_uint32(ECI_WebSocketWanPort) /*"0.0.0.0", 9909*/ /*g_cfg.get_string(ECI_WanIp).c_str(), g_cfg.get_uint32(ECI_WanPort)*/))
		{
			return false;
		}
		return true;
	}
	void cwan_server::update(uint32 uDeltaTime)
	{
		static const uint32 UPDATE_TIMER = 15 * 1000;

		m_websocket_server_ptr->process_msg();

		m_update_timer += uDeltaTime;

		if (m_update_timer >= UPDATE_TIMER)
		{
			m_update_timer -= UPDATE_TIMER;

			for (std::map<uint64, cwan_session*>::iterator iter = m_session_map.begin(); iter != m_session_map.end(); ++iter)
			{
				//if (iter->second->is_used())
				iter->second->update(UPDATE_TIMER);
			}
			//for (uint32 i = 0; i < m_max_session_num; ++i)
			//{
			//	/*if (m_session_ptr[i].is_used())
			//	{
			//		m_session_ptr[i].update(UPDATE_TIMER);
			//	}*/
			//}
			//if (m_net_ptr)
			//{
			//	//m_net_ptr->show_info();
			//}
		}

	}
	void cwan_server::shutdown()
	{
		if (m_websocket_server_ptr)
		{
			m_websocket_server_ptr->shutdown();
		}
	}
	void cwan_server::on_connect(uint64_t session_id,  const char * ip, uint16_t port)
	{
		NORMAL_EX_LOG("[session_id = %u][ip = %s][port = %u]", session_id, ip, port);
		//if (para)
		/*uint32 index = get_session_index(session_id);
		if (m_session_ptr[index].is_used())
		{
			WARNING_EX_LOG("session_id = %u, used !!!", index);
			return;
		}
		m_session_ptr[index].set_used();
		m_session_ptr[index].set_session_id(session_id);*/
		//WARNING_EX_LOG("session_id = %lu", session_id);
		std::map<uint64, cwan_session*>::iterator iter =  m_session_map.find(session_id);
		if (iter != m_session_map.end())
		{
			WARNING_EX_LOG("find session id = %u", session_id);
			//iter->second->init(); // login --->>
			return;
		}
		cwan_session * session_ptr = new cwan_session();
		if (!session_ptr)
		{
			WARNING_EX_LOG("alloc wan session failed !!!");
			m_websocket_server_ptr->close(session_id);
			return  ;
		}
		session_ptr->set_session_id(session_id);
		session_ptr->init(ip, port);
		if (!m_session_map.insert(std::make_pair(session_id, session_ptr)).second)
		{
			WARNING_EX_LOG("insert session_id = %u, failed !!!", session_id);
			m_websocket_server_ptr->close(session_id);
			delete session_ptr;
			session_ptr = NULL;
		}

		
	}
	void cwan_server::on_msg_receive(uint64_t session_id,   const void * p, uint32 size)
	{

		std::map<uint64, cwan_session*>::iterator iter =  m_session_map.find(session_id);
		if (iter != m_session_map.end())
		{
			iter->second->handler_msg(session_id, p, size);
		}
		else
		{
			WARNING_EX_LOG("not find session map [session id = %u]", session_id);
		
		}

		/*uint32 index = get_session_index(session_id);
		if (!m_session_ptr[index].is_used())
		{
			WARNING_EX_LOG("session_id = %u, not  used !!!", session_id);
			return;
		}*/
		//m_websocket_server_ptr->send_msg(session_id, "chensong", 8);
		//WARNING_EX_LOG("session_id = %lu,  [data = %s]  size = %lu", session_id,   p, size);
		
		//m_session_ptr[index].OnMessage(session_id , msg_id, p, size);
		/*if (msg_id == C2S_Login  )
		{ 
			m_session_ptr[session_id].handler_login(p, size);
		}
		else
		{

		}*/
	}
	void cwan_server::on_disconnect(uint64_t session_id)
	{
		NORMAL_EX_LOG("session_id = %u", session_id);
		std::map<uint64, cwan_session*>::iterator iter = m_session_map.find(session_id);
		if (iter != m_session_map.end())
		{
			iter->second->disconnect();
			delete iter->second;
			m_session_map.erase(iter);
		}
		else
		{
			WARNING_EX_LOG("not find session map [session id = %u]", session_id);

		}
		/*uint32 index = get_session_index(session_id);
		if (!m_session_ptr[index].is_used())
		{
			WARNING_EX_LOG("session_id = %u, used !!!", session_id  );
			return;
		}
		m_session_ptr[index].disconnect();*/
		//WARNING_EX_LOG("session_id = %lu", session_id );
	}

	void cwan_server::send_msg(uint32 session_id, uint16 msg_id, const void * p, uint32 size)
	{
		//DEBUG_EX_LOG("[session_id = %u][msg_id = %u][data = %s]", session_id, msg_id, p);
		if (!m_websocket_server_ptr->send_msg(session_id,  p, size))
		{
			WARNING_EX_LOG("send session_id = %lu", session_id);
		}
	}

	cwan_session * cwan_server::get_session(uint64 session_id)
	{
		/*if (!index_valid(index))
		{
			ERROR_LOG("[%s] invalid session index, index = %u", __FUNCTION__, index);
			return NULL;
		}

		return &(m_session_ptr[index]);*/
		std::map<uint64, cwan_session*>::iterator iter =  m_session_map.find(session_id);
		if (iter != m_session_map.end())
		{
			return iter->second;
		}
		return NULL;
	}

	//cwan_session * cwan_server::get_session(uint32 index, uint32 session_id)
	//{
	//	/*if (!index_valid(index))
	//	{
	//		ERROR_LOG("[%s] invalid session index, index = %u, session id = %u", __FUNCTION__, index, session_id);
	//		return NULL;
	//	}

	//	if (m_session_ptr[index].get_session_id() != session_id)
	//	{
	//		return NULL;
	//	}


	//	return &(m_session_ptr[index]);*/
	//}

	void cwan_server::close(uint32 session_id)
	{
		if (m_websocket_server_ptr)
		{
			m_websocket_server_ptr->close(session_id);
		}
	}

}//namespace chen 