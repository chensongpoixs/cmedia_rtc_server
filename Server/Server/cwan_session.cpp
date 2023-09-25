/********************************************************************
created:	2019-05-07

author:		chensong

level:		网络层

purpose:	网络数据的收发
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
*********************************************************************/
#include "cwan_session.h"

#include "clog.h"
#include "cmsg_dispatch.h"
#include <json/json.h>
#include "cwan_server.h"
#include "croom_mgr.h"
#include "cclient_msg_dispatch.h"
#include "cmsg_dispatch.h"
#include "cshare_proto_error.h"
namespace chen {
	cwan_session::cwan_session()
		: m_session_id(0)
		, m_master(false)
		, m_room_name("")
		, m_user_name("")
		, m_client_connect_type(EClientConnectNone)
		, m_remote_ip("")
		, m_remote_port(0)
	{
		
	}
	cwan_session::~cwan_session()
	{
		//WARNING_EX_LOG("");
		 
	}
	bool cwan_session::init(const char * ip, uint16_t port)
	{
		//WARNING_EX_LOG("");
			m_client_connect_type = EClientConnectNone;
			m_remote_ip = ip;
			m_remote_port = port;
			return true;
	}
	void cwan_session::destroy()
	{
		//WARNING_EX_LOG("");
		m_client_connect_type = EClientConnectNone;
		m_room_name.clear();
		m_user_name.clear();
		m_session_id = 0;
		//m_rtc_sdp.destroy();
	}
	void cwan_session::update(uint32 uDeltaTime)
	{
		//m_client_connect_type = EClientConnectNone;
	}
	
	void cwan_session::close()
	{
		//WARNING_EX_LOG("");
		m_client_connect_type = EClientConnectNone;
		m_room_list.clear();
	}
	bool cwan_session::is_used()
	{
		//DEBUG_LOG("m_session_id = %u, m_client_session = %lu, m_client_connect_type = %u", m_session_id, m_client_session, m_client_connect_type);
		return m_client_connect_type > EClientConnectNone;
	}

	void cwan_session::set_used()
	{
		m_client_connect_type = EClientConnected;
	}
	
	void cwan_session::disconnect()
	{
		m_client_connect_type = EClientConnectNone;

		if (m_master)
		{
			g_room_mgr.m_master[m_room_name] = 0;
		}
		m_master = false;
		NORMAL_EX_LOG("session_id = %u, [ip = %s][port = %u]", m_session_id, m_remote_ip.c_str(), m_remote_port);
		m_remote_ip = "";
		m_remote_port = 0;
		// 退出房间
		for (const std::string & room_name : m_room_list)
		{
			g_room_mgr.leave_room(m_session_id, room_name);
		}

		g_room_mgr.leave_room(m_session_id, m_room_name);
		/*if (!g_client_collection_mgr.client_remove_session_collection(m_client_session))
		{
			ERROR_EX_LOG("client collection remove session_id = %lu failed !!!", m_client_session);
		}
		else
		{
			NORMAL_LOG("client collection remove session_id = %lu ok ", m_client_session);
		}*/


		/*collection* collection_ptr = g_client_collection_mgr.get_client_session_collection(m_client_session);
		if (!collection_ptr)
		{
			 return;
		}
		collection_ptr->set_status(ECollection_Init);*/
	}

	bool cwan_session::send_msg(uint16 msg_id, int32 result,  Json::Value  data)
	{
		Json::Value value;
		value["msg_id"] = msg_id;
		value["result"] = result;
		value["data"] = data;
		Json::StyledWriter swriter;
		std::string str = swriter.write(value);
		 g_wan_server.send_msg(m_session_id, msg_id, str.c_str(), str.length() );
		 return true;
	}

	



	 
	 
}//namespace chen
