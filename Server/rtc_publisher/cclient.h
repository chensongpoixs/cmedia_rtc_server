/***********************************************************************************************
created: 		2019-03-02

author:			chensong

purpose:		log
************************************************************************************************/
#ifndef _C_CLIENT_H
#define _C_CLIENT_H
#include <iostream>
#include "cwebsocket_mgr.h"
namespace chen {
	class cclient
	{
	public:
		explicit cclient()
		: m_websocket_mgr(){}
		
		virtual ~cclient();
		 

	public:
		bool init();
		void Loop();
		void destroy();


	protected:
	private:
		 			
		cwebsocket_mgr					m_websocket_mgr;
	};
}

#endif // 