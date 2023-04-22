/***********************************************************************************************
created: 		2018-10-02

author:			chensong

purpose:		async_log
输赢不重要，答案对你们有什么意义才重要。

光阴者，百代之过客也，唯有奋力奔跑，方能生风起时，是时代造英雄，英雄存在于时代。或许世人道你轻狂，可你本就年少啊。 看护好，自己的理想和激情。


我可能会遇到很多的人，听他们讲好2多的故事，我来写成故事或编成歌，用我学来的各种乐器演奏它。
然后还可能在一个国家遇到一个心仪我的姑娘，她可能会被我帅气的外表捕获，又会被我深邃的内涵吸引，在某个下雨的夜晚，她会全身淋透然后要在我狭小的住处换身上的湿衣服。
3小时候后她告诉我她其实是这个国家的公主，她愿意向父皇求婚。我不得已告诉她我是穿越而来的男主角，我始终要回到自己的世界。
然后我的身影慢慢消失，我看到她眼里的泪水，心里却没有任何痛苦，我才知道，原来我的心被丢掉了，我游历全世界的原因，就是要找回自己的本心。
于是我开始有意寻找各种各样失去心的人，我变成一块砖头，一颗树，一滴水，一朵白云，去听大家为什么会失去自己的本心。
我发现，刚出生的宝宝，本心还在，慢慢的，他们的本心就会消失，收到了各种黑暗之光的侵蚀。
从一次争论，到嫉妒和悲愤，还有委屈和痛苦，我看到一只只无形的手，把他们的本心扯碎，蒙蔽，偷走，再也回不到主人都身边。
我叫他本心猎手。他可能是和宇宙同在的级别 但是我并不害怕，我仔细回忆自己平淡的一生 寻找本心猎手的痕迹。
沿着自己的回忆，一个个的场景忽闪而过，最后发现，我的本心，在我写代码的时候，会回来。
安静，淡然，代码就是我的一切，写代码就是我本心回归的最好方式，我还没找到本心猎手，但我相信，顺着这个线索，我一定能顺藤摸瓜，把他揪出来。
************************************************************************************************/

#include "casync_log.h"
#include "ctime_api.h"
#include <cassert>
#include <thread>
#include <iostream>
#include <mutex>
#ifdef _MSC_VER
//#define _CRT_SECURE_NO_WARNINGS
#include <io.h>
#include <direct.h>
#elif defined(__GNUC__) 
#include <unistd.h>
#include <dirent.h>
#else
// 其他不支持的编译器需要自己实现这个方法
#error unexpected c complier (msc/gcc), Need to implement this method for demangle
#endif
namespace chen {

#pragma pack(push,1)
	struct clog_item
	{
		int				timestamp;
		signed char		level;
		unsigned int	size;
		//char			buf[1];
		char *			buf;
	};
#pragma pack(pop)
	static const unsigned int LOG_ITEM_SIZE = sizeof(clog_item);
	static const unsigned int LOG_BUF_MAX_SIZE = 1024 * 1024;


	struct clog_name_color
	{
		const char* name;
		ELogColorType color;
	}
	static const g_log_name_colors[] =
	{
		{ "",		ELCT_White },
		{ "system", ELCT_Blue },
		{ "fatal",  ELCT_Red },
		{ "error",  ELCT_Pink },
		{ "warn",	ELCT_Yellow },
		{ "info",	ELCT_White },
		{ "debug",	ELCT_Green },
	};

	
	static void gen_log_file_name(char* p, const std::string& path, const std::string& name
		, const std::string& ext, ELogNameType name_type, int&& day_stamp)
	{
		p += sprintf(p, "%s", path.c_str());
		/*if (name_type == ELogName_Date)
		{*/
		char dateTime[ASCII_DATETIME_LEN] = { 0 };
		ctime_base_api::time64_datetime_format(::time(NULL), dateTime, 0, 0, 0);
		//	p += ctime_base_api::datetime_format(p, 0, -1, -1);
		/*	*p++ = '_';
		}
		else if (name_type == ELogName_DateTime)
		{
		p += ctime_base_api::datetime_format(p, 0, 0, 0);
		*p++ = '_';
		}
		else if (name_type == ELogName_AutoDate)
		{
		if (day_stamp == 0)
		{
		day_stamp = ctime_base_api::get_today_stamp_time32(0);
		}
		p += ctime_base_api::time32_datetime_format(day_stamp, p, 0, -1, -1);
		*p++ = '_';
		day_stamp += ETC_Day;
		}*/
		sprintf(p, "%s_%s%s", dateTime, name.c_str(), ext.c_str());
	}

	casync_log::casync_log()
		: m_show_screen(false)
		, m_show_log(false)
		, m_level_log(ELogLevel_System)
		, m_color_ptr(NULL)
		, m_stoped(false)
		, m_date_time(0)
		, m_path("")
		, m_name("")
		, m_ext("")
	{
		m_file_name.clear();
		m_fd.close();
	}

	casync_log::~casync_log()
	{

	}
	bool casync_log::init(const std::string & path, const std::string & name, const std::string & ext, bool show_screen)
	{
#ifdef _MSC_VER
		if (_access(path.c_str(), 0) == -1)
		{

			_mkdir(path.c_str());
		}
#elif defined(__GNUC__) 
		if (access(path.c_str(), 0) == -1)
		{
			mkdir(path.c_str(), 0777);
		}
#else
		// 其他不支持的编译器需要自己实现这个方法
#error unexpected c complier (msc/gcc), Need to implement this method for demangle
		return false;
#endif
		/*m_file_name = path + "/" + name + ext;*/
		m_log_buf = new char[LOG_BUF_MAX_SIZE];
		if (!m_log_buf)
		{
			std::cout << "memoy new err  " << std::endl;
			return false;
		}
		char log_name[1024] = { 0 };
		m_path = path;
		m_name = name;
		m_ext = ext;
		m_date_time = ctime_base_api::get_today_stamp_time64(0);;
		gen_log_file_name(log_name, m_path + "/", m_name, m_ext, ELogName_DateTime, 1);

		m_fd.open(log_name, std::ios::out | std::ios::trunc);
		if (!m_fd.is_open())
		{
			std::cout << "not open log file dest url = " << log_name << std::endl;
			return false;
		}

		m_show_log = true;
		m_show_screen = show_screen;
		if (m_show_screen)
		{
			m_color_ptr = new clog_color();
			if (!m_color_ptr)
			{
				std::cerr << "new wlog_color failed" << std::endl;
				return false;
			}
		}

		std::thread td(&casync_log::_work_pthread, this);
		m_thread.swap(td);
		//std::move(m_thread, td);
		return true;
	}
	void casync_log::append_fix(ELogLevelType level, const void * str, unsigned int len)
	{
		if (level < ELogLevel_None || level > ELogLevel_Num)
		{
			return;
		}
		if (m_stoped)
		{
			return;
		}
		//static const unsigned int max_size = 1024;
		m_lock.lock();
		//char * buf = new char[LOG_BUF_MAX_SIZE];
		//memset(m_log_buf, 0x00, LOG_BUF_MAX_SIZE);
		//uint32 cnt = vsnprintf(m_log_buf, LOG_BUF_MAX_SIZE, format, ap);

		if (len < 0)
		{
			//delete[] buf;
			return;
		}

		//得到一个新的log
		clog_item* log_item_ptr = _get_new_buf();
		if (!log_item_ptr)
		{
			//delete[] buf;
			return;
		}
		log_item_ptr->buf = new char[len];
		if (!log_item_ptr->buf)
		{
			delete log_item_ptr;
			return;
		}

		log_item_ptr->size = len;
		log_item_ptr->level = level;
		log_item_ptr->timestamp = static_cast<int>(ctime_base_api::get_gmt());

		memcpy(log_item_ptr->buf, str, len);

		m_log_item.push_back(log_item_ptr);
		m_lock.unlock();
		{
			m_condition.notify_one();
		}
	}
	void casync_log::append_var(ELogLevelType level, const char * format, va_list ap)
	{
		if (level < ELogLevel_None || level > ELogLevel_Num)
		{
			return;
		}
		if (m_stoped)
		{
			return;
		}
		//static const unsigned int max_size = 1024;
		m_lock.lock();
		//char * buf = new char[LOG_BUF_MAX_SIZE];
		//memset(m_log_buf, 0x00, LOG_BUF_MAX_SIZE);
		uint32 cnt = vsnprintf(m_log_buf, LOG_BUF_MAX_SIZE, format, ap);

		if (cnt < 0)
		{
			//delete[] buf;
			m_lock.unlock();

			return;
		}

		//得到一个新的log
		clog_item* log_item_ptr = _get_new_buf();
		if (!log_item_ptr)
		{
			//delete[] buf;
			m_lock.unlock();

			return;
		}
		log_item_ptr->buf = new char[cnt];
		if (!log_item_ptr->buf)
		{
			delete log_item_ptr;
			m_lock.unlock();

			return;
		}

		log_item_ptr->size = cnt;
		log_item_ptr->level = level;
		log_item_ptr->timestamp = static_cast<int>(ctime_base_api::get_gmt());

		memcpy(log_item_ptr->buf, m_log_buf, cnt);

		m_log_item.push_back(log_item_ptr);
		m_lock.unlock();
		{
			m_condition.notify_one();
		}
	}
	void casync_log::set_level(ELogLevelType level)
	{
		m_level_log = level;
	}
	bool casync_log::_init_log()
	{
		return true;
	}
	void casync_log::_work_pthread()
	{
		clog_item * log_item_ptr;
		while (!m_stoped || !m_log_item.empty())
		{
			{
				std::unique_lock<std::mutex> lock(m_lock);
				m_condition.wait(lock, [this]() {return m_log_item.size() > 0 || m_stoped; });
			}
			_handler_check_log_file();
			while (!m_log_item.empty())
			{
				{
					std::lock_guard<std::mutex> lock{ m_lock };
					log_item_ptr = m_log_item.front();
					m_log_item.pop_front();
				}

				if (!log_item_ptr)
				{
					continue;
				}

				_handler_log_item(log_item_ptr);

				if (log_item_ptr->buf)
				{
					delete[] log_item_ptr->buf;
				}

				delete log_item_ptr;

			}
		}
	}
	clog_item * casync_log::_get_new_buf()
	{
		clog_item* log_item_ptr = new clog_item();
		if (!log_item_ptr)
		{
			return NULL;
		}
		return log_item_ptr;
	}

	void casync_log::_handler_log_item(const clog_item * log_item_ptr)
	{

		if (m_show_screen && m_color_ptr)
		{
			m_color_ptr->set_color(g_log_name_colors[log_item_ptr->level].color);
		}

		if (!(log_item_ptr->level == ELogLevel_Info || log_item_ptr->level == ELogLevel_System || log_item_ptr->level == m_level_log))
		{

			char dateTime[ASCII_DATETIME_LEN] = { 0 };
			ctime_base_api::time64_datetime_format(log_item_ptr->timestamp, dateTime, '-', ' ', ':');

			if (m_fd.is_open())
			{
				m_fd << '[' << dateTime << "][" << g_log_name_colors[log_item_ptr->level].name << "]";
			}
			if (m_show_screen)
			{
				std::cout << '[' << dateTime << "][" << g_log_name_colors[log_item_ptr->level].name << "]";
			}
		}


		if (m_fd.is_open())
		{
			m_fd.write(log_item_ptr->buf, log_item_ptr->size);
			m_fd.write("\n", 1);
			m_fd.flush();
		}
		if (m_show_screen)
		{
			std::cout.write(log_item_ptr->buf, log_item_ptr->size);
			std::cout.write("\n", 1);
			std::cout.flush();
		}
	}

	void casync_log::_handler_check_log_file()
	{
		int32 day_time = ctime_base_api::get_today_stamp_time64(0);
		if (day_time > m_date_time)
		{
			m_date_time = day_time;
			if (m_fd.is_open())
			{
				m_fd.flush();
				m_fd.close();
			}

			char log_name[1024] = { 0 };
			gen_log_file_name(log_name, m_path + "/", m_name, m_ext, ELogName_DateTime, 1);
			//gen_log_file_name(log_name, m_path + "/", "decoder", ".log", ELogName_AutoDate, m_date_time);
			m_fd.open(log_name, std::ios::out | std::ios::trunc);
			if (!m_fd.is_open())
			{
				std::cout << "not open log file dest url = " << log_name << std::endl;
				//return false;
			}


		}
	}

	void casync_log::destroy()
	{

		m_stoped.store(true);

		m_condition.notify_all();

		if (m_thread.joinable())
		{
			m_thread.join();
		}

		if (m_fd.is_open())
		{
			m_fd.close();
		}
		if (m_color_ptr)
		{
			delete m_color_ptr;
		}
		if (m_log_buf)
		{
			delete[] m_log_buf;
		}
		m_show_screen = false;
		m_show_log = false;
	}
}//namespace chen