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
		//NORMAL_EX_LOG("[all_transport = %u]", m_all_transport_map.size());
		for (TRANSPORT_MAP::iterator iter = m_all_transport_map.begin(); iter != m_all_transport_map.end(); )
		{
			// 心跳包的检查
			//if ()
			{
				iter->second->update(uDeltaTime);
			}
			
			if (iter->second  &&
				!iter->second->is_active())
			{
				iter->second->destroy();
				crtc_transport* ptr = iter->second;
				delete ptr;
				iter = m_all_transport_map.erase(iter);
			}
			else
			{
				++iter;
			}
		}

		// stream url 说明ice 密钥协商成功 之后在放到 stream_url_map中


	}
	void ctransport_mgr::destroy()
	{
		for (TRANSPORT_MAP::iterator iter = m_all_transport_map.begin(); iter != m_all_transport_map.end(); ++iter)
		{
			WARNING_EX_LOG("destroy transport map key [username = %s]", iter->first.c_str());
			delete iter->second;
		}
		m_all_transport_map.clear();
		m_all_stream_url_map.clear();
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
		 
		return true;
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

		return true;
	}
	crtc_transport * ctransport_mgr::find_stream_name(const std::string & stream_url)
	{
		STREAM_URL_MAP::iterator iter = m_all_stream_url_map.find(stream_url);

		if (iter != m_all_stream_url_map.end())
		{
			return iter->second;
		}
		return nullptr;
	}
	const crtc_transport * ctransport_mgr::find_stream_name(const std::string & stream_url) const
	{
		STREAM_URL_MAP::const_iterator iter = m_all_stream_url_map.find(stream_url);

		if (iter != m_all_stream_url_map.end())
		{
			return iter->second;
		}
		return nullptr;
	}
	bool ctransport_mgr::insert_stream_name(const std::string & stream_url, crtc_transport * transport)
	{
		
		if (!m_all_stream_url_map.insert(std::make_pair(stream_url, transport)).second)
		{
			m_all_stream_url_map[stream_url] = transport;
			WARNING_EX_LOG("insert  stream url  map ---> !!! [stream_url = %s]", stream_url.c_str());
			return false;
		}

		return true;
	}
	bool ctransport_mgr::remove_stream_name(const std::string & stream_url)
	{
		crtc_transport * transport_ptr = find_username(stream_url);
		if (!transport_ptr)
		{
			WARNING_EX_LOG("all stream url  map not find key [stream_url = %s]", stream_url.c_str());
			return  false;
		}
		size_t size = m_all_transport_map.erase(stream_url);
		NORMAL_EX_LOG("delete key [stream_url = %s][size = %llu]", stream_url.c_str(), size);

		// TODO@chensong 2023-02-10 删除一个网络对象有两个map管理的需要在username中管理非常好


		//transport_ptr->destroy();
		//delete transport_ptr;
		//transport_ptr = NULL;

		return true;
	}
}

