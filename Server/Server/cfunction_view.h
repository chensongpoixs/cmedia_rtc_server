/***********************************************************************************************
created: 		2023-04-03

author:			chensong

purpose:		api_rtc_publish


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