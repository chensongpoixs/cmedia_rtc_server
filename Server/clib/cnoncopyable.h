/***********************************************************************************************
created: 		2019-02-26

author:			chensong

purpose:		noncopyable

Copyright boost
************************************************************************************************/

#ifndef _C_NONCOPYABLE_H_
#define _C_NONCOPYABLE_H_

namespace chen
{

	class cnoncopyable
	{
	public:
		cnoncopyable() {}
		~cnoncopyable() {}
	private:
		//cnoncopyable(cnoncopyable&&);
		cnoncopyable(const cnoncopyable&);
		//cnoncopyable &operator =(cnoncopyable &&);
		cnoncopyable& operator=(const cnoncopyable&);
	};

} //namespace chen
#endif // _C_NONCOPYABLE_H_

