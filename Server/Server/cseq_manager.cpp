/***********************************************************************************************
created: 		2023-02-20

author:			chensong

purpose:		api_rtc_publish


************************************************************************************************/
#include "cseq_manager.h"
namespace chen {
	template<typename T>
	inline bool cseq_manager<T>::IsSeqLowerThan(const T lhs, const T rhs)
	{
		return isSeqLowerThan(lhs, rhs);
	}
	template<typename T>
	inline bool cseq_manager<T>::IsSeqHigherThan(const T lhs, const T rhs)
	{
		return false;
	}
	template<typename T>
	void cseq_manager<T>::Sync(T input)
	{
	}
	template<typename T>
	void cseq_manager<T>::Drop(T input)
	{
	}
	template<typename T>
	void cseq_manager<T>::Offset(T offset)
	{
	}
	template<typename T>
	bool cseq_manager<T>::Input(const T input, T & output)
	{
		return false;
	}
	template<typename T>
	T cseq_manager<T>::GetMaxInput() const
	{
		return T();
	}
	template<typename T>
	T cseq_manager<T>::GetMaxOutput() const
	{
		return T();
	}
}