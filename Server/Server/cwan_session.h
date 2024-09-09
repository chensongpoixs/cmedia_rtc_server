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


#ifndef _C_WAN_SESSION_H_
#define _C_WAN_SESSION_H_
#include "cnoncopyable.h"
#include "cnet_type.h"
#include <list>
#include <json/json.h>
#include "crtc_sdp.h"
namespace chen {


#define P2P (1)


	enum EClientConnectType
	{
		EClientConnectNone = 0,
		EClientConnected,
		EClientConnectSession
	};


	class cwan_session //:private cnoncopyable
	{
	private:
		//typedef void (cwan_session::* msg_handler_type)(const void* ptr, uint32 msg_size);
		//typedef std::map<uint16, msg_handler_type>			M_MSG_MAP;
	public:
		explicit cwan_session();
		~cwan_session();
	public:
		bool init(const char * ip, uint16_t port);
		void destroy();
	public:
		void update(uint32 uDeltaTime);

		void handler_msg(uint64_t session_id,   const void* p, uint32 size);
	public:
		uint32 get_session_id() const { return m_session_id; }
		void	set_session_id(uint64_t session_id) { m_session_id = session_id; }
		void	close();
	public:

		bool    handler_login(  Json::Value & value);
		bool	handler_join_room(Json::Value &value);
		bool	handler_destroy_room(Json::Value& value);

		bool	handler_webrtc_message(Json::Value& value);


		bool    handler_rtc_datachannel(Json::Value& value);
		//bool	handler_create_answer(Json::Value& value);
		//bool	handler_candidate(Json::Value& value);
		//bool	handler_create_room(  Json::Value & value);
		//bool	handler_destroy_room(Json::Value & value);
		/*void	handler_webrtc_command(const void* ptr, uint32 msg_size);

		void    handler_mediasoupstatusupdate(const void * ptr, uint32 msg_size);*/



		bool   handler_create_rtc(Json::Value& value);
		bool   handler_connect_rtc(Json::Value& value);
		bool   handler_rtc_produce(Json::Value& value);
		bool   handler_rtc_consume(Json::Value& value);


		//////////////////////
		bool   handler_rtc_publisher(Json::Value & value);
		bool   handler_rtc_player(Json::Value & value);
		bool   handler_rtc_request_frame(Json::Value & value);

	private:
		//void   _collecion_update_mediasoup_type(EMediasoupType type);
	public:
		bool is_used();
		void set_used();
		void disconnect();

	public:
		bool send_msg(uint16 msg_id , int32 error,    Json::Value   data);
	private:
		//cnoncopyable(cnoncopyable&&);
		cwan_session(const cwan_session&);
		//cnoncopyable &operator =(cnoncopyable &&);
		cwan_session& operator=(const cwan_session&);
	
	private:
		uint64_t					m_session_id; //会话id
		bool					m_master;
		std::string				m_room_name;
		std::string				m_user_name;
		EClientConnectType      m_client_connect_type;
		uint64					m_client_session;
		std::list<std::string>  m_room_list; // 加入房间列表
	//	M_MSG_MAP				m_msg_map;
		Json::Reader							m_json_reader;					// json解析
		Json::Value								m_json_response;
		std::string								m_remote_ip;
		uint16								m_remote_port;
		//crtc_sdp								m_rtc_sdp;

	};
}//namespace chen

#endif // !#define _C_WAN_SESSION_H_