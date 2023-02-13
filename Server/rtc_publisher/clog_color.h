/********************************************************************
created:	2019-05-01

author:		chensong

purpose:	system log color
*********************************************************************/

#ifndef _C_LOG_COLOR_H_
#define _C_LOG_COLOR_H_
#include "cnoncopyable.h"
namespace chen
{
	enum ELogColorType
	{
		ELCT_White = 0,
		ELCT_Red,
		ELCT_Pink,
		ELCT_Yellow,
		ELCT_Blue,
		ELCT_Green,
		ELCT_Max
	};

	class clog_color :private cnoncopyable
	{
	public:
		explicit clog_color();

		void set_color(ELogColorType color);
		~clog_color() {}
	private:
		int	m_color_fg[ELCT_Max];
	};
}//namespace chen
#endif //!#define _C_LOG_COLOR_H_