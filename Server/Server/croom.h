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

#ifndef _C_ROOM_H_
#define _C_ROOM_H_
#include <string>
#include "cnet_type.h"
#include <map>
#include <json/json.h>
#include "cwebrtc_transport.h"
namespace chen
{
	struct cuser_info
	{
		uint64      session_id;
		std::string username;
		uint32		m_type;
		std::string m_remote_ip;
		uint16		m_remote_port;
		
		std::unordered_map<std::string, cwebrtc_transport*>     producers;
		std::unordered_map<std::string, cwebrtc_transport*>     consumers;
		cuser_info()
			: session_id(0)
			, username("")
			, producers()
			, consumers()
			, m_type(0)
			, m_remote_ip("")
			, m_remote_port(0)
			{}
	};

	struct croom_info
	{
		std::string  room_name;
		std::vector< cuser_info> infos;
		std::set<std::string > m_while_list;
		croom_info() : room_name(), infos(), m_while_list() {}
	};
	class croom
	{
	public:
		


		typedef std::map<uint64, cuser_info>						CUSERINFO_MAP;
	public:
		 croom();
		~croom();

	public:

		bool init(const std::string & room_name);

		bool join_userinfo(uint64 sesssion_id, const std::string & username, uint32 type);
		bool join_userinfo(const cuser_info& user_info);

		uint32 get_userssize() const { return m_userinfo_map.size(); }

		bool leave_userinfo(uint64 session_id );

		bool create_webrtc(uint64 session_id, Json::Value& value);


		bool webrtc_message(uint64 session_id, Json::Value& value);

		void build_user_info(std::vector<cuser_info> & infos);
		void build_while_user(croom_info &room_info);
		bool has_type(uint32 type);

		bool has_username(const std::string & user_name);
		bool has_while_user(const std::string &user_name);

		void build_client_p2p();


		uint32 kick_username(const std::string & username);
		uint32 add_while_username(const std::string & username);
		uint32 delete_while_username(const std::string & username);
	private:

		// 房间中广播消息
		bool _broadcast_message(uint64 session_id,  uint32 msg_id, Json::Value & value);
	private:
		//cnoncopyable(cnoncopyable&&);
		//croom(const croom&);
		//cnoncopyable &operator =(cnoncopyable &&);
		//croom& operator=(const croom&);
		
		cuser_info* _get_user_info(uint64 session_id);

	private:
		std::string					m_room_name;
		//uint64						m_room_id;
		CUSERINFO_MAP               m_userinfo_map;
		std::set<std::string>		m_while_list_username;
	};
}

#endif // _C_ROOM_H_
