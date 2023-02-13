/***********************************************************************************************
created: 		2023-02-13

author:			chensong

purpose:		 rtc——stun define
************************************************************************************************/


#ifndef _C_ICE_SERVER_H_
#define _C_ICE_SERVER_H_
#include "cnet_type.h"

namespace chen {
	class cice_server
	{
	public:
		explicit cice_server() {}
		virtual ~cice_server();

	public:
		bool init();
		void update(uint32 uDeltaTime);
		void destroy();



	protected:
	private:



	};
}


#endif // _C_ICE_SERVER_H_