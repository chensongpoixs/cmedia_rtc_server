/***********************************************************************************************
					created: 		2019-05-13
					author:			chensong
					purpose:		msg_dipatch
************************************************************************************************/

#include "cclient_msg_dispatch.h"
#include "clog.h"
#include "cwebrtc_transport.h"
#include <string>
#include "cwan_session.h"
namespace chen {
	cclient_msg_dispatch g_client_msg_dispatch;
	cclient_msg_dispatch::cclient_msg_dispatch()
	{
	}
	cclient_msg_dispatch::~cclient_msg_dispatch()
	{
	}
	bool cclient_msg_dispatch::init()
	{
		_register_msg_handler(C2S_Login, "C2S_Login", &cwan_session::handler_login);
		_register_msg_handler(C2S_JoinRoom, "C2S_JoinRoom", &cwan_session::handler_join_room);
		_register_msg_handler(C2S_DestroyRoom, "C2S_DestroyRoom", &cwan_session::handler_destroy_room);
		_register_msg_handler(C2S_WebrtcMessage, "C2S_WebrtcMessage", &cwan_session::handler_webrtc_message);
		_register_msg_handler(C2S_CreateRtc, "C2S_CreateRtc", &cwan_session::handler_create_rtc);
		_register_msg_handler(C2S_RtcConnect, "C2S_ConnectRtc", &cwan_session::handler_connect_rtc);
		_register_msg_handler(C2S_RtcProduce, "C2S_RtcProduce", &cwan_session::handler_rtc_produce);
		_register_msg_handler(C2S_RtcConsume, "C2S_RtcConsume", &cwan_session::handler_rtc_consume);
		//_register_msg_handler(C2S_CreateRoom, "C2S_CreateRoom", &cwan_session::handler_create_room);
		//_register_msg_handler(C2S_DestroyRoom, "C2S_DestroyRoom", &cwan_session::handler_destroy_room);
		return true;
	}
	void cclient_msg_dispatch::destroy()
	{
	}
	cclient_msg_handler * cclient_msg_dispatch::get_msg_handler(uint16 msg_id)
	{
		/*if (static_cast<int> (msg_id) > S2C_WebrtcMessageUpdate)
		{
			return NULL;
		}*/
		if ((msg_id < S2C_WebrtcMessageUpdate) || (msg_id < Msg_Client_Max && msg_id > S2C_LevalRoomUpdate))
		{
			return &(m_msg_handler[msg_id]);
		}
		return NULL;
		 
	}
	void cclient_msg_dispatch::_register_msg_handler(uint16 msg_id, const std::string & msg_name, handler_client_type handler)
	{
		if ((static_cast<int> (msg_id) > S2C_WebrtcMessageUpdate && static_cast<int> (msg_id) < S2C_RtpCapabilitiesUpdate) || static_cast<int> (msg_id) > Msg_Client_Max || m_msg_handler[msg_id].handler)
		{
			ERROR_LOG("[%s] register msg error, msg_id = %u, msg_name = %s", __FUNCTION__, msg_id, msg_name.c_str());
			return;
		}

		m_msg_handler[msg_id].msg_name = msg_name;//   数据统计
		m_msg_handler[msg_id].handle_count = 0;
		m_msg_handler[msg_id].handler = handler;
	}
}