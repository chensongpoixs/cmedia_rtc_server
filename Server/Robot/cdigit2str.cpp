/********************************************************************
created:	2019-03-24

author:		chensong

purpose:	�ַ�����������
//__pragma(warning(suppress:6334))(sizeof ((_Val) + (float)0) == sizeof (float) ? 'f' : sizeof ((_Val) + (double)0) == sizeof (double) ? 'd' : 'l')
*********************************************************************/
#include "cdigit2str.h"
#include <algorithm>
#include <cstdio>

#ifdef _MSC_VER
#pragma warning (disable:4996)
#endif
namespace chen
{
	template<>
	int32 digit2str_dec<int8>(char* buf, int32 buf_size, int8 value)
	{
		int8 i = value;
		char* p = buf;
		int num = 0;
		do {
			i /= 10;
			++num;
		} while (i != 0);
		if (num > buf_size)
		{
			return 0;
		}
		i = value;
		do
		{
			*p++ = '0' + (i % 10);
			i /= 10;
		} while (i != 0);

		if (value < 0)
		{
			*p++ = '-';
		}
		*p = '\0';
		std::reverse(buf, p);

		return static_cast<int>(p - buf);
	}
	template<>
	int32 digit2str_dec<uint8>(char* buf, int32 buf_size, uint8 value)
	{
		uint8 i = value;
		char* p = buf;
		int num = 0;
		do {
			i /= 10;
			++num;
		} while (i != 0);
		if (num > buf_size)
		{
			return 0;
		}
		i = value;
		do
		{
			*p++ = '0' + (i % 10);
			i /= 10;
		} while (i != 0);

		/*if (value < 0)
		{
		*p++ = '-';
		}*/
		*p = '\0';
		std::reverse(buf, p);

		return static_cast<int>(p - buf);
	}
	template<>
	int32 digit2str_dec<int16>(char* buf, int32 buf_size, int16 value)
	{
		int16 i = value;
		char* p = buf;
		int num = 0;
		do {
			i /= 10;
			++num;
		} while (i != 0);
		if (num > buf_size)
		{
			return 0;
		}
		i = value;
		do
		{
			*p++ = '0' + (i % 10);
			i /= 10;
		} while (i != 0);

		if (value < 0)
		{
			*p++ = '-';
		}
		*p = '\0';
		std::reverse(buf, p);

		return static_cast<int>(p - buf);
	}
	template<>
	int32 digit2str_dec<uint16>(char* buf, int32 buf_size, uint16 value)
	{
		uint16 i = value;
		char* p = buf;
		int num = 0;
		do {
			i /= 10;
			++num;
		} while (i != 0);
		if (num > buf_size)
		{
			return 0;
		}
		i = value;
		do
		{
			*p++ = '0' + (i % 10);
			i /= 10;
		} while (i != 0);

		/*if (value < 0)
		{
		*p++ = '-';
		}*/
		*p = '\0';
		std::reverse(buf, p);

		return static_cast<int>(p - buf);
	}
	template<>
	int32 digit2str_dec<int32>(char* buf, int32 buf_size, int32 value)
	{
		int32 i = value;
		char* p = buf;
		int num = 0;
		do {
			i /= 10;
			++num;
		} while (i != 0);
		if (num > buf_size)
		{
			return 0;
		}
		i = value;
		do
		{
			*p++ = '0' + (i % 10);
			i /= 10;
		} while (i != 0);

		if (value < 0)
		{
			*p++ = '-';
		}
		*p = '\0';
		std::reverse(buf, p);

		return static_cast<int>(p - buf);
	}

	template<>
	int32 digit2str_dec<uint32>(char* buf, int32 buf_size, uint32 value)
	{
		uint32 i = value;
		char* p = buf;
		int num = 0;
		do {
			i /= 10;
			++num;
		} while (i != 0);
		if (num > buf_size)
		{
			return 0;
		}
		i = value;
		do
		{
			*p++ = '0' + (i % 10);
			i /= 10;
		} while (i != 0);

		/*if (value < 0)
		{
		*p++ = '-';
		}*/
		*p = '\0';
		std::reverse(buf, p);

		return static_cast<int>(p - buf);
	}

	template<>
	int32 digit2str_dec<signed long >(char* buf, int32 buf_size, signed long value)
	{
		signed long i = value;
		char* p = buf;
		int num = 0;
		do {
			i /= 10;
			++num;
		} while (i != 0);
		if (num > buf_size)
		{
			return 0;
		}
		i = value;
		do
		{
			*p++ = '0' + (i % 10);
			i /= 10;
		} while (i != 0);

		if (value < 0)
		{
			*p++ = '-';
		}
		*p = '\0';
		std::reverse(buf, p);

		return static_cast<int>(p - buf);
	}
	template<>
	int32 digit2str_dec<unsigned long>(char* buf, int32 buf_size, unsigned long value)
	{
		unsigned long i = value;
		char* p = buf;
		int num = 0;
		do {
			i /= 10;
			++num;
		} while (i != 0);
		if (num > buf_size)
		{
			return 0;
		}
		i = value;
		do
		{
			*p++ = '0' + (i % 10);
			i /= 10;
		} while (i != 0);

		/*if (value < 0)
		{
		*p++ = '-';
		}*/
		*p = '\0';
		std::reverse(buf, p);

		return static_cast<int>(p - buf);
	}
	template<>
	int32 digit2str_dec<signed long  long>(char* buf, int32 buf_size, signed long long value)
	{
		signed long long i = value;
		char* p = buf;
		int num = 0;
		do {
			i /= 10;
			++num;
		} while (i != 0);
		if (num > buf_size)
		{
			return 0;
		}
		i = value;
		do
		{
			*p++ = '0' + (i % 10);
			i /= 10;
		} while (i != 0);

		if (value < 0)
		{
			*p++ = '-';
		}
		*p = '\0';
		std::reverse(buf, p);

		return static_cast<int>(p - buf);
	}
	template<>
	int32 digit2str_dec<unsigned long long>(char* buf, int32 buf_size, unsigned long long value)
	{
		unsigned long long i = value;
		char* p = buf;
		int num = 0;
		do {
			i /= 10;
			++num;
		} while (i != 0);
		if (num > buf_size)
		{
			return 0;
		}
		i = value;
		do
		{
			*p++ = '0' + (i % 10);
			i /= 10;
		} while (i != 0);

		/*if (value < 0)
		{
		*p++ = '-';
		}*/
		*p = '\0';
		std::reverse(buf, p);

		return static_cast<int>(p - buf);
	}
	template<>
	int digit2str_dec<float>(char* buf, int buf_size, float value)
	{
		if (6 > buf_size)
		{
			return 0;
		}

		return sprintf(buf, "%.2g", value);
	}
	/**
	* ��ѧ������
	* stata �� ������ 1.333e+09 ����ʲô��˼
	* ��������̫���ˣ��ÿ�ѧ��������ʾ��
	* ����  �����ݴη��������1.333e-09����˵��������0�� ��1.33*10�ĸ�9�η�
	* ��1330000000��Ҳ���� 1.33 * (10^9)
	*/
	template<>
	int digit2str_dec<double>(char* buf, int buf_size, double value)
	{
		if (6 > buf_size)
		{
			return 0;
		}
		return sprintf(buf, "%.2g", value);
	}


} // namespace chen