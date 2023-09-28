/********************************************************************
created:	2019-05-01

author:		chensong

purpose:	system log color
*********************************************************************/
#include "clog_color.h"
#include <cstdio>
#if defined(_MSC_VER)
#include <windows.h>
#endif

namespace chen {
	clog_color::clog_color()
	{
#if defined(_MSC_VER)
		m_color_fg[ELCT_White] = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
		m_color_fg[ELCT_Red] = FOREGROUND_RED | FOREGROUND_INTENSITY;
		m_color_fg[ELCT_Pink] = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
		m_color_fg[ELCT_Green] = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
		m_color_fg[ELCT_Blue] = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
		m_color_fg[ELCT_Yellow] = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;

#elif defined(__GNUC__)

		enum ANSIFgTextAttr
		{
			FG_BLACK = 30, FG_RED, FG_GREEN, FG_YELLOW, FG_BLUE,
			FG_MAGENTA, FG_CYAN, FG_WHITE,
		};

		m_color_fg[ELCT_White] = FG_WHITE;
		m_color_fg[ELCT_Red] = FG_RED;
		m_color_fg[ELCT_Pink] = FG_CYAN;
		m_color_fg[ELCT_Green] = FG_GREEN;
		m_color_fg[ELCT_Blue] = FG_BLUE;
		m_color_fg[ELCT_Yellow] = FG_YELLOW;

#else
#pragma error "unknow platform!!!"

#endif
	}

	void clog_color::set_color(ELogColorType color)
	{
#if defined(_MSC_VER)
		::SetConsoleTextAttribute(::GetStdHandle(STD_OUTPUT_HANDLE), m_color_fg[color]);
#elif defined(__GNUC__)
		//fprintf(stdout, "\033[0;40;%dm", m_color_fg[color]);
		//static const X_CHAR* colorString[TBLUE+1] = {"","\033[22;31m","\033[22;32m","\033[01;33m","\033[0m","\033[01;37m","\033[01;34m",};
		//fputs(colorString[color],stdout); 
#endif
	}

} //namespace chen