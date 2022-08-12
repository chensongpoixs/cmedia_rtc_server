/***********************************************************************************************
created: 		2022-08-10

author:			chensong

purpose:		cwebrtc_mgr

Copyright boost
************************************************************************************************/

#include "cwebrtc_mgr.h"
#include "ccrypto_random.h"
#include "cwebrtc_transport.h"
namespace chen {
	cwebrtc_mgr::cwebrtc_mgr()
		: m_webrtc_transport_map()
	{
	}
	cwebrtc_mgr::~cwebrtc_mgr()
	{
	}
	bool cwebrtc_mgr::handler_info(Json::Value & value)
	{
		return true;
	}
	bool cwebrtc_mgr::handler_create_webrtc(Json::Value & value)
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
	bool cwebrtc_mgr::handler_destroy_webrtc(Json::Value & value)
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
}