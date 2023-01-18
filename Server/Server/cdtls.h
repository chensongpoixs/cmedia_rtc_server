/***********************************************************************************************
created: 		2023-01-17

author:			chensong

purpose:		_C_DTLS_ _H_


************************************************************************************************/

#ifndef _C_DTLS_H_
#define _C_DTLS_H_
#include "cnet_type.h"
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include "crtc_sdp.h"
#include "cmedia_desc.h"
namespace chen {

	class cdtls 
	{
	public:

		cdtls() {}
		~cdtls();
	
	public:
		static bool init();


		static void destory();

	protected:
	private:
	};
}


#endif // _C_DTLS_IMPL_H_