/***********************************************************************************************
created: 		2022-08-09

author:			chensong

purpose:		cwebsocket_server


************************************************************************************************/

#include "cwebsocket_server.h"
namespace chen {
	cwebsocket_server   g_websocket_server;
	cwebsocket_server::cwebsocket_server()
	{
	}
	/*cwebsocket_server::cwebsocket_server()
	{
	}*/
	cwebsocket_server::~cwebsocket_server()
	{
	}
	bool cwebsocket_server::init()
	{
		return false;
	}
	void cwebsocket_server::destroy()
	{
	}
	bool cwebsocket_server::startup()
	{
		return false;
	}
	void cwebsocket_server::update(uint32 uDeltaTime)
	{
	}
	void cwebsocket_server::shutdown()
	{
	}
	void cwebsocket_server::on_connect(uint32 session_id, uint32 para, const char* buf)
	{
	}
	void cwebsocket_server::on_msg_receive(uint32 session_id, uint16 msg_id, const void* p, uint32 size)
	{
	}
	void cwebsocket_server::on_disconnect(uint32 session_id)
	{
	}
	void cwebsocket_server::send_msg(uint32 session_id, uint16 msg_id, const void* p, uint32 size)
	{
	}
	cwebsocket_session* cwebsocket_server::get_session(uint32 index)
	{
		return nullptr;
	}
	cwebsocket_session* cwebsocket_server::get_session(uint32 index, uint32 session_id)
	{
		return nullptr;
	}
	void cwebsocket_server::close(uint32 session_id)
	{
	}
}