/***********************************************************************************************
created: 		2023-04-03

author:			chensong

purpose:		api_rtc_publish

输赢不重要，答案对你们有什么意义才重要。

光阴者，百代之过客也，唯有奋力奔跑，方能生风起时，是时代造英雄，英雄存在于时代。或许世人道你轻狂，可你本就年少啊。 看护好，自己的理想和激情。


我可能会遇到很多的人，听他们讲好2多的故事，我来写成故事或编成歌，用我学来的各种乐器演奏它。
然后还可能在一个国家遇到一个心仪我的姑娘，她可能会被我帅气的外表捕获，又会被我深邃的内涵吸引，在某个下雨的夜晚，她会全身淋透然后要在我狭小的住处换身上的湿衣服。
3小时候后她告诉我她其实是这个国家的公主，她愿意向父皇求婚。我不得已告诉她我是穿越而来的男主角，我始终要回到自己的世界。
然后我的身影慢慢消失，我看到她眼里的泪水，心里却没有任何痛苦，我才知道，原来我的心被丢掉了，我游历全世界的原因，就是要找回自己的本心。
于是我开始有意寻找各种各样失去心的人，我变成一块砖头，一颗树，一滴水，一朵白云，去听大家为什么会失去自己的本心。
我发现，刚出生的宝宝，本心还在，慢慢的，他们的本心就会消失，收到了各种黑暗之光的侵蚀。
从一次争论，到嫉妒和悲愤，还有委屈和痛苦，我看到一只只无形的手，把他们的本心扯碎，蒙蔽，偷走，再也回不到主人都身边。
我叫他本心猎手。他可能是和宇宙同在的级别 但是我并不害怕，我仔细回忆自己平淡的一生 寻找本心猎手的痕迹。
沿着自己的回忆，一个个的场景忽闪而过，最后发现，我的本心，在我写代码的时候，会回来。
安静，淡然，代码就是我的一切，写代码就是我本心回归的最好方式，我还没找到本心猎手，但我相信，顺着这个线索，我一定能顺藤摸瓜，把他揪出来。
************************************************************************************************/

#ifndef _C_FUNCTION_VIEW_H_
#define _C_FUNCTION_VIEW_H_
#include "cnet_type.h"
#include "clog.h"
namespace chen {

	template <typename T>
	class cfunction_view;  // Undefined.

	template <typename RetT, typename... ArgT>
	class cfunction_view<RetT(ArgT...)> final {
	public:
		// Constructor for lambdas and other callables; it accepts every type of
		// argument except those noted in its enable_if call.
		template <
			typename F,
			typename std::enable_if<
			// Not for function pointers; we have another constructor for that
			// below.
			!std::is_function<typename std::remove_pointer<
			typename std::remove_reference<F>::type>::type>::value &&

			// Not for nullptr; we have another constructor for that below.
			!std::is_same<std::nullptr_t,
			typename std::remove_cv<F>::type>::value &&

			// Not for FunctionView objects; we have another constructor for that
			// (the implicitly declared copy constructor).
			!std::is_same<cfunction_view,
			typename std::remove_cv<typename std::remove_reference<
			F>::type>::type>::value>::type* = nullptr>
			cfunction_view(F&& f)
			: call_(CallVoidPtr<typename std::remove_reference<F>::type>) {
			f_.void_ptr = &f;
		}

		// Constructor that accepts function pointers. If the argument is null, the
		// result is an empty FunctionView.
		template <
			typename F,
			typename std::enable_if<std::is_function<typename std::remove_pointer<
			typename std::remove_reference<F>::type>::type>::value>::type* =
			nullptr>
			cfunction_view(F&& f)
			: call_(f ? CallFunPtr<typename std::remove_pointer<F>::type> : nullptr) {
			f_.fun_ptr = reinterpret_cast<void(*)()>(f);
		}

		// Constructor that accepts nullptr. It creates an empty cfunction_view.
		template <typename F,
			typename std::enable_if<std::is_same<
			std::nullptr_t,
			typename std::remove_cv<F>::type>::value>::type* = nullptr>
			cfunction_view(F&& f) : call_(nullptr) {}

		// Default constructor. Creates an empty FunctionView.
		cfunction_view() : call_(nullptr) {}

		RetT operator()(ArgT... args) const {
			//RTC_DCHECK(call_);
			cassert(call_);
			return call_(f_, std::forward<ArgT>(args)...);
		}

		// Returns true if we have a function, false if we don't (i.e., we're null).
		explicit operator bool() const { return !!call_; }

	private:
		union VoidUnion {
			void* void_ptr;
			void(*fun_ptr)();
		};

		template <typename F>
		static RetT CallVoidPtr(VoidUnion vu, ArgT... args) {
			return (*static_cast<F*>(vu.void_ptr))(std::forward<ArgT>(args)...);
		}
		template <typename F>
		static RetT CallFunPtr(VoidUnion vu, ArgT... args) {
			return (reinterpret_cast<typename std::add_pointer<F>::type>(vu.fun_ptr))(
				std::forward<ArgT>(args)...);
		}

		// A pointer to the callable thing, with type information erased. It's a
		// union because we have to use separate types depending on if the callable
		// thing is a function pointer or something else.
		VoidUnion f_;

		// Pointer to a dispatch function that knows the type of the callable thing
		// that's stored in f_, and how to call it. A FunctionView object is empty
		// (null) iff call_ is null.
		RetT(*call_)(VoidUnion, ArgT...);
	};

}

#endif // 