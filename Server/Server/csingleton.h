/**
*
*	Copyright (C) 2010 FastTime
*  Description: ����ģʽ���̰߳�ȫ��ģ����
*	Author: chensong
*	Date:	2016.4.20
*/
#ifndef _C_SINGLETON_H
#define _C_SINGLETON_H

namespace chen
{
	template < typename T >
	class csingleton
	{
	public:
		static T& get_instance()
		{
			static T v;
			return v;
		}
	};
}//namespace chen

#endif // !#define _C_SINGLETON_H
