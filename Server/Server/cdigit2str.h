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
#include <malloc.h>
#include <wchar.h>
#include <windows.h>
#define alloca _alloca

#define STACK_ARRAY(TYPE, LEN) \
  static_cast<TYPE*>(::alloca((LEN) * sizeof(TYPE)))
#endif
namespace chen
{
	//convert int to decimal string, add '\0' at end
	template<typename T>
	int32 digit2str_dec(char* buf, int32 buf_size, T value);
	// TODO(jonasolsson): replace with absl::Hex when that becomes available.
	std::string ToHex(const int i);

#ifdef _MSC_VER

	inline std::wstring ToUtf16(const char* utf8, size_t len) {
		int len16 = ::MultiByteToWideChar(CP_UTF8, 0, utf8, static_cast<int>(len), nullptr, 0);
		wchar_t* ws = STACK_ARRAY(wchar_t, len16);
		::MultiByteToWideChar(CP_UTF8, 0, utf8, static_cast<int>(len), ws, len16);
		return std::wstring(ws, len16);
	}

	inline std::wstring ToUtf16(const std::string& str) {
		return ToUtf16(str.data(), str.length());
	}

	inline std::string ToUtf8(const wchar_t* wide, size_t len) {
		int len8 = ::WideCharToMultiByte(CP_UTF8, 0, wide, static_cast<int>(len),
			nullptr, 0, nullptr, nullptr);
		char* ns = STACK_ARRAY(char, len8);
		::WideCharToMultiByte(CP_UTF8, 0, wide, static_cast<int>(len), ns, len8,
			nullptr, nullptr);
		return std::string(ns, len8);
	}

	inline std::string ToUtf8(const wchar_t* wide) {
		return ToUtf8(wide, wcslen(wide));
	}

	inline std::string ToUtf8(const std::wstring& wstr) {
		return ToUtf8(wstr.data(), wstr.length());
	}

#endif

}// namespace chen

#endif //_C_DIGIT2STR_H_