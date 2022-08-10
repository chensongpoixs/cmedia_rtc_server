/********************************************************************
created:	2019-03-24

author:		chensong

purpose:	字符串处理工具类

*********************************************************************/
#ifndef _C_DIGIT2STR_H_
#define _C_DIGIT2STR_H_
#include <algorithm>
#include <cstdio>
#include "cnet_type.h"
#ifdef _MSC_VER
#pragma warning (disable:4996)
#endif
namespace chen
{
	//convert int to decimal string, add '\0' at end
	template<typename T>
	int32 digit2str_dec(char* buf, int32 buf_size, T value);


}// namespace chen

#endif //_C_DIGIT2STR_H_