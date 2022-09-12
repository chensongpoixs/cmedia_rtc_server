/***********************************************************************************************
created: 		2022-09-11

author:			chensong

purpose:		robot
************************************************************************************************/

#ifndef _C_ROBOT_H_
#define _C_ROBOT_H_


namespace chen {



	class Robot
	{
	public:
		Robot()
			: m_stoped(false)
		{}
		~Robot();


	public:
		bool init(const char* log_path, const char* config_file);
		bool Loop();
		void destroy();

		void stop();
	private:
		bool	m_stoped;
	};
	extern Robot g_robot;
}

#endif // _C_ROBOT_H_
