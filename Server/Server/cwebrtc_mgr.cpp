/***********************************************************************************************
created: 		2022-08-10

author:			chensong

purpose:		cwebrtc_mgr

Copyright boost
************************************************************************************************/

#include "cwebrtc_mgr.h"
#include "ccrypto_random.h"
#include "cwebrtc_transport.h"
#include "cmsg_dispatch.h"
namespace chen {
	cwebrtc_mgr g_global_webrtc_mgr;
	cwebrtc_mgr::cwebrtc_mgr()
		: m_webrtc_transport_map()
	{
	}
	cwebrtc_mgr::~cwebrtc_mgr()
	{
	}
	bool cwebrtc_mgr::handler_info(uint64 session_id, Json::Value & value)
	{
		return true;
	}
	bool cwebrtc_mgr::handler_create_webrtc(uint64 session_id, Json::Value & value)
	{
		std::string transportId = s_crypto_random.GetRandomString(20);
		cwebrtc_transport* transport_ptr = new cwebrtc_transport();
		if (!transport_ptr )
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
		//m_webrtc_transport_map.insert(std::make_pair(transportId, transport));
		if (!m_webrtc_transport_map.insert(std::make_pair(transportId, transport_ptr)).second)
		{
			delete transport_ptr;
			transport_ptr = NULL;
			WARNING_EX_LOG("webrtc transport map install failed !!!");
			return false;
		} 
		DEBUG_EX_LOG("webrtc transport create [transportId = %s]", transportId.c_str());


		//TODO@chensong 20220812 返回客户端创建webrtc的信息

		return true;
	}
	bool cwebrtc_mgr::handler_destroy_webrtc(uint64 session_id, Json::Value & value)
	{
		// client tranid 
		std::string transportId;
		std::unordered_map<std::string, cwebrtc_transport*>::iterator iter = m_webrtc_transport_map.find(transportId);

		if (iter != m_webrtc_transport_map.end())
		{
			NORMAL_EX_LOG("find webrtc transport map [transport id = %s] destroy !!!", transportId.c_str());

			

			if (iter->second)
			{
				// 1、关闭所有已经连接上的通道
				iter->second->CloseProducersAndConsumers();

				delete iter->second;
			}
			m_webrtc_transport_map.erase(iter);

		}
		else
		{
			WARNING_EX_LOG("not webrtc transport map [transport id = %s]", transportId.c_str());
		}
		//TODO@chensong 20220812 返回客户端创建webrtc的信息
		return true;
	}
	bool cwebrtc_mgr::handler_webrtc(uint64 session_id, Json::Value & value)
	{
		if (!value.isMember("transport_id"))
		{
			ERROR_EX_LOG("not find type transport_id [value = %s]", value.asString().c_str());
			return false;
		}
		const uint16 msg_id = value["msg_id"].asUInt();
		const std::string transport_id = value["transport_id"].asString();

		std::unordered_map<std::string, cwebrtc_transport*>::iterator iter =  m_webrtc_transport_map.find(transport_id);
		if (iter != m_webrtc_transport_map.end())
		{
			cmsg_handler* handler_ptr = g_msg_dispatch.get_msg_handler(msg_id);
			if (!handler_ptr)
			{
				WARNING_EX_LOG("not find [msg_id = %u]type ", msg_id);
				// reply 客户端错误信息
				return false;
			}
			++handler_ptr->handle_count;
			if (iter->second)
			{
				return ((*iter->second).*(handler_ptr->handler))(session_id, value);
				//iter->second
			}
		}
		else
		{
			WARNING_EX_LOG("not find transport id = %s", transport_id.c_str());
		}
		
		

		
		return true;
	}
}