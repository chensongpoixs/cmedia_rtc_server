/***********************************************************************************************
created: 		2023-02-18

author:			chensong

purpose:		_C_DTLS_ _H_


************************************************************************************************/

#ifndef _C_GLOBAL_CONFIG_H_
#define _C_GLOBAL_CONFIG_H_
#include "cnet_type.h"
#include <string>
#include <vector>
#include <set>


namespace chen {
	class cglobal_config
	{
	public:
		explicit cglobal_config()
		: m_all_ips(){}
		virtual ~cglobal_config();

	public:
		bool init();
		void update(uint32 uDeltaTime);
		void destroy();
	public:
		
		 std::set<std::string> & get_all_ips();
		 const std::set<std::string>&    get_all_ips() const ;
	protected:
	private:
		std::set<std::string>		m_all_ips;
	};

	extern cglobal_config  g_global_config;
}


#endif // _C_GLOBAL_CONFIG_H_