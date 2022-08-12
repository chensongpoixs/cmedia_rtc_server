/***********************************************************************************************
created: 		2022-08-12

author:			chensong

purpose:		cuv_ip
************************************************************************************************/


#ifndef _C_UV_IP_H_
#define _C_UV_IP_H_

#include "clog.h"
#include "cnet_type.h"
namespace chen {
	namespace uv_ip
	{
		  int GetFamily(const std::string& ip);

		 void GetAddressInfo(const struct sockaddr* addr, int& family, std::string& ip, uint16_t& port);

		 bool CompareAddresses(const struct sockaddr* addr1, const struct sockaddr* addr2);

		 struct sockaddr_storage CopyAddress(const struct sockaddr* addr);

		  void NormalizeIp(std::string& ip);
	}
}

#endif //_C_UV_IP_H_