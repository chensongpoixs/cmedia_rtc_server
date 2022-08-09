#include "cwebsocket_session_mgr.h"
/***********************************************************************************************
created: 		2022-08-09

author:			chensong

purpose:		cwebsocket_session


************************************************************************************************/
#include "cwebsocket_session_mgr.h"
#include "clog.h"
#include <memory>
namespace chen {
	cwebsocket_session_mgr g_websocket_sesson_mgr;
	cwebsocket_session_mgr::cwebsocket_session_mgr()/*:m_mem_ptr(NULL)*/
	{
	}
	cwebsocket_session_mgr::~cwebsocket_session_mgr()
	{
	}
	bool cwebsocket_session_mgr::init(uint32 session_size)
	{
		if (session_size < 1)
		{
			ERROR_EX_LOG(" session_size tail small !!! session_size  = %lu", session_size);
			return false;
		}

		/*m_mem_ptr  = ::malloc(sizeof(cwebsocket_session) * session_size);
		if (!m_mem_ptr)
		{
			ERROR_EX_LOG("alloc m_mem_ptr failed !!!!");
			return false;
		}*/

		//for (uint32 i = 0; i < session_size; ++i)
		//{
		//	cwebsocket_session* session_ptr = new cwebsocket_session(); //new((m_mem_ptr + (i * sizeof(cwebsocket_session)))) cwebsocket_session();
		//
		//	if (!m_wait_connect_map.insert(std::make_pair(i, session_ptr)).second)
		//	{
		//		WARNING_EX_LOG("insert wait connect map failed i = %u", i);
		//	}
		//}


		return true;
	}
	void cwebsocket_session_mgr::destroy()
	{
	}
	/*cwebsocket_session* cwebsocket_session_mgr::get_wait_connect_session()
	{
		return nullptr;
	}
	cwebsocket_session* cwebsocket_session_mgr::get_connecting_session(uint64 session_id)
	{
		return nullptr;
	}*/
	std::shared_ptr<session> cwebsocket_session_mgr::get_connected_session(uint64 session_id)
	{
		WEBSOCKET_SESSION_MAP::iterator iter = 	m_connected_map.find(session_id);
		if (iter != m_connected_map.end())
		{
			return iter->second;
		}
		return NULL;
	}
	 bool cwebsocket_session_mgr::add_connected_session(uint64 session_id, std::shared_ptr<session> session_ptr)
	 {
		 if (get_connected_session(session_id))
		 {
			 WARNING_EX_LOG("find session_id = %u", session_id);
			 return false;
		 }
		 if (!m_connected_map.insert(std::make_pair(session_id, session_ptr)).second)
		 {
			 WARNING_EX_LOG("insert failed session_id = %u", session_id);
			 return false;
		 }
		 return true;
	 }
	 bool cwebsocket_session_mgr::delete_connected_session(uint64 session_id)
	 {
		 std::shared_ptr<session> session_ptr = get_connected_session(session_id);
		 if (!session_ptr)
		 {
			 WARNING_EX_LOG("not find session_id = %u", session_id);
			 return false;
		 }
		 m_connected_map.erase(session_id);
		 return false;
	 }
}

 