/***********************************************************************************************
created: 		2022-11-10

author:			chensong

purpose:		crtc_player
************************************************************************************************/
#ifndef _C_RTC_PLAYER_H_
#define _C_RTC_PLAYER_H_



namespace chen 
{
	class crtc_player
	{
	public:
		crtc_player();
		~crtc_player();
		


	public:
		bool Init();
		bool Loop();
		void Destroy();
		void stop();

	private:



		 
	private:


		bool m_stoped;
	};
}

#endif // _C_SDP_UTIL_H_