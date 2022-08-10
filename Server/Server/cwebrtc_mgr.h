/***********************************************************************************************
created: 		2022-08-10

author:			chensong

purpose:		cwebrtc_mgr

Copyright boost
************************************************************************************************/

#ifndef _C_WEBRTC_MGR_H_
#define _C_WEBRTC_MGR_H_
#include <string>
#include "cnet_type.h"
#include <map>
namespace chen
{ 
	class cwebrtc_mgr
	{
	public:
		cwebrtc_mgr();
		~cwebrtc_mgr();

	public:


	private:
		//cnoncopyable(cnoncopyable&&);
		cwebrtc_mgr(const cwebrtc_mgr&);
		//cnoncopyable &operator =(cnoncopyable &&);
		cwebrtc_mgr& operator=(const cwebrtc_mgr&);
	private:

	};
}

#endif // _C_WEBRTC_MGR_H_