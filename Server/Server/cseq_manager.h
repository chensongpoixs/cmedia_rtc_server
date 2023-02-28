/***********************************************************************************************
created: 		2023-02-20

author:			chensong

purpose:		api_rtc_publish


************************************************************************************************/

#ifndef _C_SEQ_MANAGER_H_
#define _C_SEQ_MANAGER_H_
#include "cnet_type.h"
#include <set>

namespace chen {

	template<typename T>
	class cseq_manager
	{
	public:
		static constexpr T MaxValue = std::numeric_limits<T>::max();

	public:
		struct SeqLowerThan
		{
			bool operator()(const T lhs, const T rhs) const;
		};

		struct SeqHigherThan
		{
			bool operator()(const T lhs, const T rhs) const;
		};

	private:
		static const SeqLowerThan isSeqLowerThan;
		static const SeqHigherThan isSeqHigherThan;

	public:
		static bool IsSeqLowerThan(const T lhs, const T rhs);
		static bool IsSeqHigherThan(const T lhs, const T rhs);

	public:
		cseq_manager() = default;

	public:
		void Sync(T input);
		void Drop(T input);
		void Offset(T offset);
		bool Input(const T input, T& output);
		T GetMaxInput() const;
		T GetMaxOutput() const;

	private:
		T base{ 0 };
		T maxOutput{ 0 };
		T maxInput{ 0 };
		std::set<T, SeqLowerThan> dropped;
	};
	
}

#endif // _C_SEQ_MANAGER_H_