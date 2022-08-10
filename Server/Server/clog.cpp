/***********************************************************************************************
created: 		2019-04-30

author:			chensong

purpose:		log
************************************************************************************************/
#include "clog.h"
#include <cstdarg>
#include "cdigit2str.h"
namespace chen {
	static casync_log* g_log_ptr = nullptr;
	clog::clog()
		:m_len(0)
		, m_level(ELogLevel_None)
	{
	}
	clog::clog(ELogLevelType level)
		: m_len(0)
		, m_level(level)
	{
	}
	clog::clog(ELogLevelType level, const char * func, int line)
		: m_len(0)
		, m_level(level)
	{
		*this << '[' << func << ':' << line << "] ";
	}
	bool clog::init(const std::string & path, const std::string & name, const std::string & ext, ELogNameType name_type, bool mod_append, bool show_screen)
	{
		g_log_ptr = new casync_log();
		if (!g_log_ptr)
		{
			std::cout << " alloc casync_log new fail " << std::endl;
			return false;
		}
		if (!g_log_ptr->init(path, name, ext, true))
		{
			std::cout << "log init error " << std::endl;
			return false;
		}
		return true;
	}
	void clog::fix_log(ELogLevelType level, const void * p, int len)
	{
		if (g_log_ptr && len > 0 /*&& m_level <= g_log_ptr->get_level()*/)
		{
			g_log_ptr->append_fix(level, p, len);
		}
	}
	void clog::var_log(ELogLevelType level, const char * format, ...)
	{
		if (!g_log_ptr)
		{
			return;
		}
		va_list argptr;
		va_start(argptr, format);
		g_log_ptr->append_var(level, format, argptr);
		va_end(argptr);
	}
	void clog::set_level(ELogLevelType level)
	{
		if (!g_log_ptr)
		{
			return;
		}
		g_log_ptr->set_level(level);
	}
	void clog::destroy()
	{
		if (g_log_ptr)
		{
			g_log_ptr->destroy();
		}
	}
	clog & clog::operator<<(bool value)
	{
		// TODO: 在此处插入 return 语句
		if (value)
		{
			return *this << '1';
		}
		else
		{
			return *this << '0';
		}
	}
	clog & clog::operator<<(char value)
	{
		// TODO: 在此处插入 return 语句
		if (m_len < EBuf_Size)
		{
			m_data[m_len++] = value;
		}
		return *this;
	}
	clog & clog::operator<<(signed char value)
	{
		// TODO: 在此处插入 return 语句
		if (m_len < EBuf_Size)
		{
			m_data[m_len++] = value;
		}
		//return *this;
		return *this;
	}
	clog & clog::operator<<(unsigned char value)
	{
		// TODO: 在此处插入 return 语句
		if (m_len < EBuf_Size)
		{
			m_data[m_len++] = value;
		}
		return *this;
	}
	clog & clog::operator<<(signed short value)
	{
		// TODO: 在此处插入 return 语句
		if (m_len < EBuf_Size)
		{
			m_len += digit2str_dec(m_data + m_len, EBuf_Size - m_len, value);
		}
		return *this;
	}
	clog & clog::operator<<(unsigned short value)
	{
		// TODO: 在此处插入 return 语句
		if (m_len < EBuf_Size)
		{
			m_len += digit2str_dec(m_data + m_len, EBuf_Size - m_len, value);
		}
		return *this;
	}
	clog & clog::operator<<(signed int value)
	{
		// TODO: 在此处插入 return 语句
		if (m_len < EBuf_Size)
		{
			m_len += digit2str_dec(m_data + m_len, EBuf_Size - m_len, value);
		}
		return *this;
	}
	clog & clog::operator<<(unsigned int value)
	{
		// TODO: 在此处插入 return 语句
		if (m_len < EBuf_Size)
		{
			m_len += digit2str_dec(m_data + m_len, EBuf_Size - m_len, value);
		}
		return *this;
	}
	clog & clog::operator<<(signed long value)
	{
		// TODO: 在此处插入 return 语句
		if (m_len < EBuf_Size)
		{
			m_len += digit2str_dec(m_data + m_len, EBuf_Size - m_len, value);
		}
		return *this;
	}
	clog & clog::operator<<(unsigned long value)
	{
		// TODO: 在此处插入 return 语句
		if (m_len < EBuf_Size)
		{
			m_len += digit2str_dec(m_data + m_len, EBuf_Size - m_len, value);
		}
		return *this;
	}
	clog & clog::operator<<(signed long long value)
	{
		// TODO: 在此处插入 return 语句
		if (m_len < EBuf_Size)
		{
			m_len += digit2str_dec(m_data + m_len, EBuf_Size - m_len, value);
		}
		return *this;
	}
	clog & clog::operator<<(unsigned long long value)
	{
		// TODO: 在此处插入 return 语句
		if (m_len < EBuf_Size)
		{
			m_len += digit2str_dec(m_data + m_len, EBuf_Size - m_len, value);
		}
		return *this;
	}
	clog & clog::operator<<(const char * value)
	{
		// TODO: 在此处插入 return 语句
		if (m_len < EBuf_Size)
		{
			//m_data[m_len++] = value;
			if (!memcpy(m_data + m_len, value, strlen(value)))
			{
				return *this;
			}
			m_len += static_cast<int32>( strlen(value));
		}
		return *this;
	}
	clog & clog::operator<<(const std::string & value)
	{
		// TODO: 在此处插入 return 语句
		if (m_len < EBuf_Size)
		{
			//m_data[m_len++] = value;
			//m_len += digit2str_dec(m_data + m_len, EBuf_Size - m_len, value);
			/*m_len += */
			if (!memcpy(m_data + m_len, value.c_str(), value.length()))
			{
				return *this;
			}
			m_len += static_cast<int32>(value.length());
		}
		return *this;
	}
	clog & clog::operator<<(float value)
	{
		// TODO: 在此处插入 return 语句
		if (m_len < EBuf_Size)
		{
			//m_data[m_len++] = value;
			m_len += digit2str_dec(m_data + m_len, EBuf_Size - m_len, value);
		}
		return *this;
	}
	clog & clog::operator<<(double value)
	{
		// TODO: 在此处插入 return 语句
		if (m_len < EBuf_Size)
		{
			/*m_data[m_len++] =*/ m_len += digit2str_dec(m_data + m_len, EBuf_Size - m_len,value);
		}
		return *this;
	}
	clog::~clog()
	{
		if (g_log_ptr && m_len > 0 /*&& m_level <= g_log_ptr->get_level()*/)
		{
			g_log_ptr->append_fix(m_level, m_data, m_len);
		}
	}


} //namespace chen

