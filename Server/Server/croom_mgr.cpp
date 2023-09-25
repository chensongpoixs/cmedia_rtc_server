#include "croom_mgr.h"
/***********************************************************************************************
created: 		2022-08-10

author:			chensong

purpose:		room_mgr

Copyright boost
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
#include "croom_mgr.h"
#include "clog.h"
#include "croom.h"
namespace chen {
	croom_mgr g_room_mgr;
	croom_mgr::croom_mgr()
	{
	}
	croom_mgr::~croom_mgr()
	{
	}
	bool croom_mgr::init()
	{
		return true;
	}
	void croom_mgr::update(uint32 uDeltaTime)
	{
	}
	void croom_mgr::destory()
	{
	}

	bool croom_mgr::join_room(uint64 session_id, const std::string & room_name, const std::string & user_name, uint32 type)
	{
		CROOM_MAP::iterator iter =  m_room_map.find(room_name);
		if (iter == m_room_map.end()) 
		{
			croom room;
			room.init(room_name);
			if (!m_room_map.insert(std::make_pair(room_name, room)).second)
			{
				WARNING_EX_LOG("insert [room name = %s] failed !!!", room_name.c_str());
			}
			iter = m_room_map.find(room_name);
		}
		if (iter == m_room_map.end())
		{
			WARNING_EX_LOG("not find [room name = %s] [user_name  = %s]failed !!!", room_name.c_str(), user_name.c_str());
			return false;
		}
		
		return iter->second.join_userinfo(session_id, user_name, type);
	}

	bool croom_mgr::join_room(const std::string & room_name, const cuser_info& user_info)
	{
		CROOM_MAP::iterator iter = m_room_map.find(room_name);
		if (iter == m_room_map.end())
		{
			croom room;
			room.init(room_name);
			if (!m_room_map.insert(std::make_pair(room_name, room)).second)
			{
				WARNING_EX_LOG("insert [room name = %s] failed !!!", room_name.c_str());
			}
			iter = m_room_map.find(room_name);
		}
		if (iter == m_room_map.end())
		{
			WARNING_EX_LOG("not find [room name = %s] [user_name  = %s]failed !!!", room_name.c_str(), user_info.username.c_str());
			return false;
		}

		return iter->second.join_userinfo(user_info);
	}

	bool croom_mgr::leave_room(uint64 session_id, const std::string & room_name)
	{
		CROOM_MAP::iterator iter = m_room_map.find(room_name);
		if (iter == m_room_map.end())
		{
			croom room;
			room.init(room_name);
			if (!m_room_map.insert(std::make_pair(room_name, room)).second)
			{
				WARNING_EX_LOG("insert [room name = %s] failed !!!", room_name.c_str());
			}
			iter = m_room_map.find(room_name);
		}
		if (iter == m_room_map.end())
		{
			WARNING_EX_LOG("not find [room name = %s]  failed !!!", room_name.c_str() );
			return false;
		}
		return iter->second.leave_userinfo(session_id );
	}

	bool croom_mgr::webrtc_message(const std::string & room_name, uint64 session_id, Json::Value & value)
	{
		CROOM_MAP::iterator iter =  m_room_map.find(room_name);
		if (iter != m_room_map.end())
		{
			return iter->second.webrtc_message(session_id, value);
		}
		WARNING_EX_LOG("ont find room name = %s", room_name.c_str());
		return false;
	}

	bool croom_mgr::room_user_type(const std::string & room_name, uint32 type)
	{
		CROOM_MAP::iterator iter = m_room_map.find(room_name);
		if (iter != m_room_map.end())
		{
			return iter->second.has_type(type);
		}
		return false;
	}
	bool croom_mgr::room_has_username(const std::string & room_name, const std::string & user_name)
	{
		CROOM_MAP::iterator iter = m_room_map.find(room_name);
		if (iter != m_room_map.end())
		{
			return iter->second.has_username(user_name);
		}
		return false;
	}
	bool croom_mgr::room_has_while_username(const std::string & room_name, const std::string & user_name)
	{
		CROOM_MAP::iterator iter = m_room_map.find(room_name);
		if (iter != m_room_map.end())
		{
			return iter->second.has_while_user(user_name);
		}
		return false;
		return true;
	}
	bool croom_mgr::get_room_info(const std::string & room_name, std::vector<cuser_info> & infos)
	{
		CROOM_MAP::iterator iter = m_room_map.find(room_name);
		if (iter != m_room_map.end())
		{
			iter->second.build_user_info(infos);
		}
		return true;
	}
	
	void croom_mgr::build_client_p2p(const std::string & room_name)
	{
		CROOM_MAP::iterator iter = m_room_map.find(room_name);
		if (iter != m_room_map.end())
		{
			iter->second.build_client_p2p() ;
		}
	}


	void croom_mgr::build_all_room_info(std::vector< croom_info>& room_infos)
	{

		for (  std::pair<const   std::string,    croom> &pi: m_room_map)
		{
			croom_info room_info;
			room_info.room_name = pi.first;
			pi.second.build_user_info(room_info.infos);
			pi.second.build_while_user(room_info);
			room_infos.push_back(room_info);
		}
	}
	uint32_t croom_mgr::kick_room_username(const std::string & roomname, const std::string & username)
	{
		CROOM_MAP::iterator iter = m_room_map.find(roomname);
		if (iter == m_room_map.end())
		{
			return 500;// 
			//iter->second.build_client_p2p();
		}
		
		return iter->second.kick_username(username);
	}
	//uint32_t croom_mgr::white_room_username(const std::string & roomname, const std::string & username)
	//{
	//	CROOM_MAP::iterator iter = m_room_map.find(roomname);
	//	if (iter == m_room_map.end())
	//	{
	//		return 500;// 
	//		//iter->second.build_client_p2p();
	//	}

	//	return iter->second.kick_username(username);
	//}

	uint32_t croom_mgr::add_white_room_username(const std::string & roomname, const std::string & username)
	{
		CROOM_MAP::iterator iter = m_room_map.find(roomname);
		if (iter == m_room_map.end())
		{
			return 500;// 
			//iter->second.build_client_p2p();
		}

		return iter->second.add_while_username(username);
	}
	uint32_t croom_mgr::delete_white_room_username(const std::string & roomname, const std::string & username)
	{
		CROOM_MAP::iterator iter = m_room_map.find(roomname);
		if (iter == m_room_map.end())
		{
			return 500;// 
			//iter->second.build_client_p2p();
		}

		return iter->second.delete_while_username(username);
	}
}