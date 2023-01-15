/***********************************************************************************************
created: 		2022-11-10

author:			chensong

purpose:		crtc_player
************************************************************************************************/
 
#include "crtc_player.h"


namespace chen 
{
	crtc_player::crtc_player()
		: m_stoped(false)
	{
	}
	crtc_player::~crtc_player()
	{
	}
	bool crtc_player::Init()
	{
		return false;
	}
	bool crtc_player::Loop()
	{


		m_stoped = false;

		while (!m_stoped)
		{

		}

		return true;
	}
	void crtc_player::Destroy()
	{
	}
	void crtc_player::stop()
	{
		m_stoped = true;
	}
}

 