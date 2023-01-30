/********************************************************************
created:	2019-03-24

author:		chensong

purpose:	字符串处理工具类

*********************************************************************/


#include "cstr2digit.h"

namespace chen {
	std::string str2hex(const std::string & str)
	{
		return str2hex(str.c_str(), str.length());
	}
	std::string str2hex(const char * str, int32 length)
	{
		return str2hex(str, length, INT_MAX);
	}
	std::string str2hex(const char * str, int32 length, int32 limit)
	{
		return str2hex(str, length, limit, ' ', 128, '\n');
	}
	std::string str2hex(const char * str, int32 length, int32 limit, char seperator, int line_limit, char newline)
	{
		// 1 byte trailing '\0'.
		const int LIMIT = 1024 * 16 + 1;
		static char buf[LIMIT];

		int len = 0;
		for (int i = 0; i < length && i < limit && len < LIMIT; ++i) 
		{
			int nb = snprintf(buf + len, LIMIT - len, "%02x", (uint8_t)str[i]);
			if (nb <= 0 || nb >= LIMIT - len) 
			{
				break;
			}
			len += nb;

			// Only append seperator and newline when not last byte.
			if (i < length - 1 && i < limit - 1 && len < LIMIT) 
			{
				if (seperator) 
				{
					buf[len++] = seperator;
				}

				if (newline && line_limit && i > 0 && ((i + 1) % line_limit) == 0) 
				{
					buf[len++] = newline;
				}
			}
		}

		// Empty string.
		if (len <= 0) 
		{
			return "";
		}

		// If overflow, cut the trailing newline.
		if (newline && len >= LIMIT - 2 && buf[len - 1] == newline) 
		{
			len--;
		}

		// If overflow, cut the trailing seperator.
		if (seperator && len >= LIMIT - 3 && buf[len - 1] == seperator) 
		{
			len--;
		}

		return std::string(buf, len);
	}
}