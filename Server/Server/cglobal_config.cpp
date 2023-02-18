/***********************************************************************************************
created: 		2023-02-18

author:			chensong

purpose:		_C_DTLS_ _H_


************************************************************************************************/
#include "cglobal_config.h"


#include "csocket_util.h"
#include "clog.h"
namespace chen {
	cglobal_config  g_global_config;
	cglobal_config::~cglobal_config()
	{
	}
	bool cglobal_config::init()
	{
		SYSTEM_LOG("get network ips info ... ");
		if (!get_network_ips(m_all_ips))
		{
			WARNING_EX_LOG("get network all ips failed !!!");
			return false;
		}
		SYSTEM_LOG("network all ip : ");
		for (const std::string & ip : m_all_ips)
		{
			SYSTEM_LOG("[ip = %s]", ip.c_str());
		}
		SYSTEM_LOG("network all end OK !!!");





		return true;
	}
	void cglobal_config::update(uint32 uDeltaTime)
	{
	}
	void cglobal_config::destroy()
	{
		SYSTEM_LOG("global config destroy ...");

		{
			SYSTEM_LOG("network ips ...");
			m_all_ips.clear();
			SYSTEM_LOG("network ips OK !!!");
		}



		SYSTEM_LOG("global config destroy OK !!!");

	}
	const std::set<std::string>& cglobal_config::get_all_ips() const
	{
		// TODO: insert return statement here
		return m_all_ips;
	}
	std::set<std::string> &cglobal_config::get_all_ips()
	{
		return m_all_ips;
	}
}