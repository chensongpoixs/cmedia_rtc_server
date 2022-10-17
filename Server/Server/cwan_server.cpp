/***********************************************************************************************
created: 		2019-04-30

author:			chensong

purpose:		wan_server

************************************************************************************************/

#include "cwan_server.h"
#include "clog.h"
#include "ccfg.h"



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
			std::bind(&cwan_server::on_connect, this, std::placeholders::_1, std::placeholders::_2)
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
	void cwan_server::on_connect(uint64_t session_id,  const char * buf)
	{
		NORMAL_EX_LOG("[session_id = %u]", session_id);
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
			iter->second->init(); // login --->>
			return;
		}
		cwan_session * session_ptr = new cwan_session();
		if (!session_ptr)
		{
			WARNING_EX_LOG("alloc wan session failed !!!");
			m_websocket_server_ptr->close(session_id);
			return  ;
		}
		session_ptr->init();
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
		DEBUG_EX_LOG("[session_id = %u][msg_id = %u][data = %s]", session_id, msg_id, p);
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