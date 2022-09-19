/********************************************************************
created:	2019-05-07

author:		chensong

level:		网络层

purpose:	网络数据的收发
*********************************************************************/


#ifndef _C_WAN_SESSION_H_
#define _C_WAN_SESSION_H_
#include "cnoncopyable.h"
#include "cnet_type.h"
#include <list>
#include <json/json.h>
#include "crtc_sdp.h"
namespace chen {

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
		bool init();
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

		bool	handler_create_room(  Json::Value & value);
		bool	handler_destroy_room(Json::Value & value);
		/*void	handler_webrtc_command(const void* ptr, uint32 msg_size);

		void    handler_mediasoupstatusupdate(const void * ptr, uint32 msg_size);*/
	private:
		//void   _collecion_update_mediasoup_type(EMediasoupType type);
	public:
		bool is_used();
		void set_used();
		void disconnect();

	public:
		bool send_msg(uint16 msg_id , const std::string & data);
	private:
		//cnoncopyable(cnoncopyable&&);
		cwan_session(const cwan_session&);
		//cnoncopyable &operator =(cnoncopyable &&);
		cwan_session& operator=(const cwan_session&);
	
	private:
		uint64_t					m_session_id; //会话id
		std::string				m_user_name;
		EClientConnectType      m_client_connect_type;
		uint64					m_client_session;
		std::list<std::string>  m_room_list; // 加入房间列表
	//	M_MSG_MAP				m_msg_map;
		Json::Reader							m_json_reader;					// json解析
		Json::Value								m_json_response;

		crtc_sdp								m_rtc_sdp;

	};
}//namespace chen

#endif // !#define _C_WAN_SESSION_H_