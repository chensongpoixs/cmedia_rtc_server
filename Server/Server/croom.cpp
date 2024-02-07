/***********************************************************************************************
created: 		2022-08-10

author:			chensong

purpose:		room

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


#include "croom.h"
#include "clog.h"
#include "cwan_server.h"
#include "cmsg_base_id.h"
#include "ccrypto_random.h"
namespace chen {
	croom::croom()
		: m_room_name("")
	{
	}
	croom::~croom()
	{
	}
	bool croom::init(const std::string & room_name)
	{
		m_room_name = room_name;
		return true;
	}
	bool croom::join_userinfo(uint64 session_id, const std::string & username, uint32 type)
	{
		Json::Value reply;
		reply["username"] = username;
		_broadcast_message(session_id, S2C_JoinRoomUpdate, reply);
		CUSERINFO_MAP::iterator iter =   m_userinfo_map.find(session_id);
		if (iter != m_userinfo_map.end())
		{
			WARNING_EX_LOG("[room_name = %s][session_id = %u][old user_name = %s][new username = %s]", m_room_name.c_str(), session_id, iter->second.username.c_str(), username.c_str()  );
			iter->second.username = username;
			return true;
		}

		cuser_info userinfo;
		userinfo.session_id = session_id;
		userinfo.username = username;
		userinfo.m_type = type;
		if (!m_userinfo_map.insert(std::make_pair(session_id, userinfo)).second)
		{
			ERROR_EX_LOG("[room_name = %s][session_id = %u][ user_name = %s] insert failed !!!", m_room_name.c_str(), session_id, username.c_str());
			return false;
		}
			return true;
	}
	bool croom::join_userinfo(const cuser_info& user_info)
	{
		Json::Value reply;
		reply["username"] = user_info.username;
		_broadcast_message(user_info.session_id, S2C_JoinRoomUpdate, reply);
		CUSERINFO_MAP::iterator iter = m_userinfo_map.find(user_info.session_id);
		if (iter != m_userinfo_map.end())
		{
			WARNING_EX_LOG("[room_name = %s][session_id = %u][old user_name = %s][new username = %s]", m_room_name.c_str(), user_info.session_id, iter->second.username.c_str(), user_info.username.c_str());
			iter->second.username = user_info.username;
			return true;
		}

		/*cuser_info userinfo;
		userinfo.session_id = session_id;
		userinfo.username = username;
		userinfo.m_type = type;*/
		if (!m_userinfo_map.insert(std::make_pair(user_info.session_id, user_info)).second)
		{
			ERROR_EX_LOG("[room_name = %s][session_id = %u][ user_name = %s] insert failed !!!", m_room_name.c_str(), user_info.session_id, user_info.username.c_str());
			return false;
		}
		return true;
	}
	bool croom::leave_userinfo(uint64 session_id)
	{
		Json::Value reply;
		
		CUSERINFO_MAP::iterator iter = m_userinfo_map.find(session_id);
		if (iter != m_userinfo_map.end())
		{
			reply["username"] = iter->second.username;
			_broadcast_message(session_id, S2C_LevalRoomUpdate, reply);
			WARNING_EX_LOG("[room_name = %s][session_id = %u][old user_name = %s][new username = %s]", m_room_name.c_str(), session_id, iter->second.username.c_str(), iter->second.username.c_str());
			m_userinfo_map.erase(iter);
			return true;
		}
		WARNING_EX_LOG("[room_name = %s][session_id = %u] not find  failed !!!", m_room_name.c_str(), session_id );
		reply["username"] = "";
		_broadcast_message(session_id, S2C_LevalRoomUpdate, reply);
		return true;
	}
	bool croom::create_webrtc(uint64 session_id, Json::Value & value)
	{
		cuser_info* user_info_ptr = _get_user_info(session_id);
		if (!user_info_ptr)
		{
			ERROR_EX_LOG("not find session_id = %u", session_id);
			return false;
		}


		std::string transportId = s_crypto_random.GetRandomString(20);
		/*cwebrtc_transport* transport_ptr = new cwebrtc_transport();
		if (!transport_ptr)
		{
			ERROR_EX_LOG("webrtc transport alloc  failed !!!");
			return false;
		}
		if (!transport_ptr->init(transportId))
		{
			delete transport_ptr;
			transport_ptr = NULL;
			ERROR_EX_LOG("webrtc transport init failed !!!");
			return false;
		}

		user_info_ptr->producers[transportId] = transport_ptr;*/

		return true;
	}
	bool croom::webrtc_message(uint64 session_id, Json::Value & value)
	{
		return _broadcast_message(session_id, S2C_WebrtcMessageUpdate , value);
	}
	bool croom::_broadcast_message(uint64 session_id, uint32 msg_id, Json::Value & value)
	{
		for (const std::pair<uint64, const cuser_info> &pi : m_userinfo_map)
		{
			if (pi.first != session_id)
			{
				// 
				Json::Value reply;
				reply["msg_id"] = msg_id;
				reply["data"] = value;
				Json::StyledWriter swriter;
				std::string str = swriter.write(reply);
				NORMAL_EX_LOG("[sesson_id = %u][join room = %s][username = %s][message = %s]", pi.first, m_room_name.c_str(), pi.second.username.c_str(), str.c_str());
				//g_wan_server.send_msg(m_session_id, msg_id, str.c_str(), str.length());
				g_wan_server.send_msg(pi.first, msg_id, str.c_str(), str.length());
			}
		}
		return true;
	}
	cuser_info * croom::_get_user_info(uint64 session_id)
	{
		CUSERINFO_MAP::iterator iter = m_userinfo_map.find(session_id);
		if (iter != m_userinfo_map.end())
		{
			return NULL;
		}
		return &iter->second;
	}

	void croom::build_user_info(std::vector<cuser_info> & infos)
	{
		for (const std::pair<uint64, cuser_info> & pi : m_userinfo_map)
		{
			/*cuser_info info;
			info.username = pi.second.username;
			info.m_type = pi.second.m_type;*/

			infos.push_back(pi.second);
		}
	}
	void croom::build_while_user(croom_info &room_info)
	{
		room_info.m_while_list = m_while_list_username;
		//for (const std::pair<uint64, cuser_info> & pi : m_userinfo_map)
		//{
		//	/*cuser_info info;
		//	info.username = pi.second.username;
		//	info.m_type = pi.second.m_type;*/

		//	infos.push_back(pi.second);
		//}
	}
	bool croom::has_type(uint32 type)
	{
		for (const std::pair<uint64, cuser_info>& pi : m_userinfo_map)
		{

			if (type == pi.second.m_type)
			{
				return true;
			}
		}
		return false;
	}
	bool croom::has_username(const std::string & user_name)
	{
		
		for (const std::pair<uint64, cuser_info> & pi : m_userinfo_map)
		{
			 
			if (user_name == pi.second.username)
			{
				return true;
			}
		}

		return false;
	}
	bool croom::has_while_user(const std::string &user_name)
	{
		auto iter = m_while_list_username.find(user_name);
		if (iter != m_while_list_username.end())
		{
			return true;
		}
		return false;
	}
	void croom::build_client_p2p()
	{
		if (m_userinfo_map.size() > 1)
		{
			Json::Value value;
			_broadcast_message(0, S2C_RtpP2pUpdate, value);
		}
	}
	uint32 croom::kick_username(const std::string & username)
	{
		for (const std::pair<uint64, cuser_info> & pi : m_userinfo_map)
		{
			/*cuser_info info;
			info.username = pi.second.username;
			info.m_type = pi.second.m_type;*/
			if (pi.second.username == username)
			{
				g_wan_server.close(pi.second.session_id);
				return 0;
			}

			//infos.push_back(pi.second);
		}
		return 501;//
	}
	uint32 croom::kick_type(uint32 type)
	{
		for (const std::pair<uint64, cuser_info>& pi : m_userinfo_map)
		{
			/*cuser_info info;
			info.username = pi.second.username;
			info.m_type = pi.second.m_type;*/
			if (pi.second.m_type == type)
			{
				g_wan_server.close(pi.second.session_id);
				return 0;
			}

			//infos.push_back(pi.second);
		}
		return 501;//
	}
	uint32 croom::add_while_username(const std::string & username)
	{
		m_while_list_username.insert(username);
		for (const std::pair<uint64, cuser_info> & pi : m_userinfo_map)
		{
			/*cuser_info info;
			info.username = pi.second.username;
			info.m_type = pi.second.m_type;*/
			if (pi.second.username == username)
			{
				g_wan_server.close(pi.second.session_id);
				return 0;
			}

			//infos.push_back(pi.second);
		}
		return 0;
	}
	uint32 croom::delete_while_username(const std::string & username)
	{
		m_while_list_username.erase(username);
		 
		return 0;
	}
}