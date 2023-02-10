/***********************************************************************************************
created: 		2023-03-10

author:			chensong

purpose:		ctransport_mgr


************************************************************************************************/
#include "crtc_transport.h"
#include "ctransport_mgr.h"
#include "clog.h"
namespace chen {
	ctransport_mgr   g_transport_mgr;
	ctransport_mgr::~ctransport_mgr()
	{
	}
	bool ctransport_mgr::init()
	{
		return true;;
	}
	void ctransport_mgr::update(uint32 uDeltaTime)
	{

		for (TRANSPORT_MAP::iterator iter = m_all_transport_map.begin(); iter != m_all_transport_map.end(); ++iter)
		{
			// 心跳包的检查
			//if ()
			{
				iter->second->update(uDeltaTime);
			}
		}
	}
	void ctransport_mgr::destroy()
	{
		for (TRANSPORT_MAP::iterator iter = m_all_transport_map.begin(); iter != m_all_transport_map.end(); ++iter)
		{
			WARNING_EX_LOG("destroy transport map key [username = %s]", iter->first.c_str());
			delete iter->second;
		}
		m_all_transport_map.clear();
	}
	crtc_transport * ctransport_mgr::find_username(const std::string & username)
	{
		TRANSPORT_MAP::iterator iter = m_all_transport_map.find(username);

		if (iter != m_all_transport_map.end())
		{
			return iter->second;
		}
		return nullptr;
	}
	const crtc_transport * ctransport_mgr::find_username(const std::string & username) const
	{
		TRANSPORT_MAP::const_iterator iter = m_all_transport_map.find(username);

		if (iter != m_all_transport_map.end())
		{
			return iter->second;
		}
		return nullptr;
	}
	bool ctransport_mgr::insert_username(const std::string & username, crtc_transport * transport)
	{

		if (!m_all_transport_map.insert(std::make_pair(username, transport)).second)
		{
			WARNING_EX_LOG("insert transport map failed !!! [username = %s]", username.c_str());
			return false;
		}
		 
		return false;
	}
	bool ctransport_mgr::remove_username(const std::string & username)
	{
		crtc_transport * transport_ptr = find_username(username);
		if (!transport_ptr)
		{
			WARNING_EX_LOG("all transport map not find key [username = %s]", username.c_str());
			return  false;
		}
		size_t size = m_all_transport_map.erase(username);
		NORMAL_EX_LOG("delete key [username = %s][size = %llu]", username.c_str(), size);
		transport_ptr->destroy();
		delete transport_ptr;
		transport_ptr = NULL;

		return false;
	}
}

