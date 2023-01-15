/***********************************************************************************************
created: 		2022-11-10

author:			chensong

purpose:		cplayer
************************************************************************************************/
#ifndef _C_PLAYER_H_
#define _C_PLAYER_H_

#include "cnet_types.h"

namespace chen 
{
	class cplayer
	{
	public:
		cplayer();
		~cplayer();
		


	public:
		bool init();
		//bool Loop();

		void update(uint32 dela);
		void destroy();
		//void stop();

	private:



		 
	private:


		bool m_stoped;
	};
}

#endif // _C_SDP_UTIL_H_