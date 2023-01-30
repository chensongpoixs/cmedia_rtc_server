/********************************************************************
created:	2019-03-24

author:		chensong

purpose:	字符串处理工具类

*********************************************************************/
#ifndef _C_STR2DIGIT_H_
#define _C_STR2DIGIT_H_
#include <algorithm>
#include <cstdio>
#include "cnet_type.h"

namespace chen {

	std::string str2hex(const std::string &str);
	std::string str2hex(const char * str, int32 length);
	std::string str2hex(const char * str, int32 length, int32 limit);
	std::string str2hex(const char * str, int32 length, int32 limit, char seperator, int line_limit, char newline);
}

#endif // 