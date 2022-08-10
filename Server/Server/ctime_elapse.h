/********************************************************************
created:	2019-12-24

author:		chensong

purpose:	ctime_elapse

*********************************************************************/
#ifndef _C_TIME_ELAPSE_H
#define _C_TIME_ELAPSE_H
#include <chrono>
#include <ctime>
#include "ctime_api.h"
namespace chen
{
	class ctime_elapse
	{
	public:
		ctime_elapse() : m_cur_time(0)
		{
			m_cur_time = ctime_base_api::get_time_ms();
		}

		unsigned int get_elapse()
		{
			long long last_time = m_cur_time;
			m_cur_time = ctime_base_api::get_time_ms();
			if (m_cur_time >= last_time)
			{
				return static_cast<unsigned int>(m_cur_time - last_time);
			}
			else
			{
				return 1;
			}
		}

	private:
		long long m_cur_time;
	};
}  // namespace chen

#endif //!#define _C_TIME_ELAPSE_H

