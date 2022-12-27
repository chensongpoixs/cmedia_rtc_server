/***********************************************************************************************
created: 		2022-12-27

author:			chensong

purpose:		ip_address


************************************************************************************************/

#include "cnet_adapter_test.h"
#include "csocket_util.h"
#include "clog.h"
namespace chen {
	int32 net_adapter_test( )
	{
		std::set<std::string> ips;
		chen::get_network_ips(ips);
		for (const std::string & ip : ips)
		{
			NORMAL_EX_LOG("[ip = %s]", ip.c_str());
		}
		return 0;
	}
}