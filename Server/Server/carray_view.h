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

#ifndef _C_ARRAY_VIEW_H_
#define _C_ARRAY_VIEW_H_
#include "cnet_type.h"
#include "clog.h"
#include <algorithm>
#include <array>
#include <type_traits>
#include <cstddef>
#include <type_traits>
#include <cassert>
namespace chen {

	// Determines if the given class has zero-argument .data() and .size() methods
	// whose return values are convertible to T* and size_t, respectively.
	template <typename DS, typename T>
	class HasDataAndSize {
	private:
		template <
			typename C,
			typename std::enable_if<
			std::is_convertible<decltype(std::declval<C>().data()), T*>::value &&
			std::is_convertible<decltype(std::declval<C>().size()),
			std::size_t>::value>::type* = nullptr>
			static int Test(int);

		template <typename>
		static char Test(...);

	public:
		static constexpr bool value = std::is_same<decltype(Test<DS>(0)), int>::value;
	};

	namespace test_has_data_and_size {

		template <typename DR, typename SR>
		struct Test1 {
			DR data();
			SR size();
		};
		static_assert(HasDataAndSize<Test1<int*, int>, int>::value, "");
		static_assert(HasDataAndSize<Test1<int*, int>, const int>::value, "");
		static_assert(HasDataAndSize<Test1<const int*, int>, const int>::value, "");
		static_assert(!HasDataAndSize<Test1<const int*, int>, int>::value,
			"implicit cast of const int* to int*");
		static_assert(!HasDataAndSize<Test1<char*, size_t>, int>::value,
			"implicit cast of char* to int*");

		struct Test2 {
			int* data;
			size_t size;
		};
		static_assert(!HasDataAndSize<Test2, int>::value,
			".data and .size aren't functions");

		struct Test3 {
			int* data();
		};
		static_assert(!HasDataAndSize<Test3, int>::value, ".size() is missing");

		class Test4 {
			int* data();
			size_t size();
		};
		//static_assert(!HasDataAndSize<Test4, int>::value, ".data() and .size() are private");

	}  // namespace test_has_data_and_size

	namespace type_traits_impl {

		// Determines if the given type is an enum that converts implicitly to
		// an integral type.
		template <typename T>
		struct IsIntEnum {
		private:
			// This overload is used if the type is an enum, and unary plus
			// compiles and turns it into an integral type.
			template <typename X,
				typename std::enable_if<
				std::is_enum<X>::value &&
				std::is_integral<decltype(+std::declval<X>())>::value>::type* =
				nullptr>
				static int Test(int);

			// Otherwise, this overload is used.
			template <typename>
			static char Test(...);

		public:
			static constexpr bool value =
				std::is_same<decltype(Test<typename std::remove_reference<T>::type>(0)),
				int>::value;
		};

	}  // namespace type_traits_impl

	// Determines if the given type is integral, or an enum that
	// converts implicitly to an integral type.
	template <typename T>
	struct IsIntlike {
	private:
		using X = typename std::remove_reference<T>::type;

	public:
		static constexpr bool value =
			std::is_integral<X>::value || type_traits_impl::IsIntEnum<X>::value;
	};

	namespace test_enum_intlike {

		enum E1 { e1 };
		enum { e2 };
		enum class E3 { e3 };
		struct S {};

		static_assert(type_traits_impl::IsIntEnum<E1>::value, "");
		static_assert(type_traits_impl::IsIntEnum<decltype(e2)>::value, "");
		static_assert(!type_traits_impl::IsIntEnum<E3>::value, "");
		static_assert(!type_traits_impl::IsIntEnum<int>::value, "");
		static_assert(!type_traits_impl::IsIntEnum<float>::value, "");
		static_assert(!type_traits_impl::IsIntEnum<S>::value, "");

		static_assert(IsIntlike<E1>::value, "");
		static_assert(IsIntlike<decltype(e2)>::value, "");
		static_assert(!IsIntlike<E3>::value, "");
		static_assert(IsIntlike<int>::value, "");
		static_assert(!IsIntlike<float>::value, "");
		static_assert(!IsIntlike<S>::value, "");

	}  // namespace test_enum_intlike
	// tl;dr: rtc::ArrayView is the same thing as gsl::span from the Guideline
	//        Support Library.
	//
	// Many functions read from or write to arrays. The obvious way to do this is
	// to use two arguments, a pointer to the first element and an element count:
	//
	//   bool Contains17(const int* arr, size_t size) {
	//     for (size_t i = 0; i < size; ++i) {
	//       if (arr[i] == 17)
	//         return true;
	//     }
	//     return false;
	//   }
	//
	// This is flexible, since it doesn't matter how the array is stored (C array,
	// std::vector, rtc::Buffer, ...), but it's error-prone because the caller has
	// to correctly specify the array length:
	//
	//   Contains17(arr, arraysize(arr));     // C array
	//   Contains17(arr.data(), arr.size());  // std::vector
	//   Contains17(arr, size);               // pointer + size
	//   ...
	//
	// It's also kind of messy to have two separate arguments for what is
	// conceptually a single thing.
	//
	// Enter rtc::ArrayView<T>. It contains a T pointer (to an array it doesn't
	// own) and a count, and supports the basic things you'd expect, such as
	// indexing and iteration. It allows us to write our function like this:
	//
	//   bool Contains17(rtc::ArrayView<const int> arr) {
	//     for (auto e : arr) {
	//       if (e == 17)
	//         return true;
	//     }
	//     return false;
	//   }
	//
	// And even better, because a bunch of things will implicitly convert to
	// ArrayView, we can call it like this:
	//
	//   Contains17(arr);                             // C array
	//   Contains17(arr);                             // std::vector
	//   Contains17(rtc::ArrayView<int>(arr, size));  // pointer + size
	//   Contains17(nullptr);                         // nullptr -> empty ArrayView
	//   ...
	//
	// ArrayView<T> stores both a pointer and a size, but you may also use
	// ArrayView<T, N>, which has a size that's fixed at compile time (which means
	// it only has to store the pointer).
	//
	// One important point is that ArrayView<T> and ArrayView<const T> are
	// different types, which allow and don't allow mutation of the array elements,
	// respectively. The implicit conversions work just like you'd hope, so that
	// e.g. vector<int> will convert to either ArrayView<int> or ArrayView<const
	// int>, but const vector<int> will convert only to ArrayView<const int>.
	// (ArrayView itself can be the source type in such conversions, so
	// ArrayView<int> will convert to ArrayView<const int>.)
	//
	// Note: ArrayView is tiny (just a pointer and a count if variable-sized, just
	// a pointer if fix-sized) and trivially copyable, so it's probably cheaper to
	// pass it by value than by const reference.

	namespace impl {

		// Magic constant for indicating that the size of an ArrayView is variable
		// instead of fixed.
		enum : std::ptrdiff_t { kArrayViewVarSize = -4711 };

		// Base class for ArrayViews of fixed nonzero size.
		template <typename T, std::ptrdiff_t Size>
		class ArrayViewBase {
			static_assert(Size > 0, "ArrayView size must be variable or non-negative");

		public:
			ArrayViewBase(T* data, size_t size) : data_(data) {}

			static constexpr size_t size() { return Size; }
			static constexpr bool empty() { return false; }
			T* data() const { return data_; }

		protected:
			static constexpr bool fixed_size() { return true; }

		private:
			T* data_;
		};

		// Specialized base class for ArrayViews of fixed zero size.
		template <typename T>
		class ArrayViewBase<T, 0> {
		public:
			explicit ArrayViewBase(T* data, size_t size) {}

			static constexpr size_t size() { return 0; }
			static constexpr bool empty() { return true; }
			T* data() const { return nullptr; }

		protected:
			static constexpr bool fixed_size() { return true; }
		};

		// Specialized base class for ArrayViews of variable size.
		template <typename T>
		class ArrayViewBase<T, impl::kArrayViewVarSize> {
		public:
			ArrayViewBase(T* data, size_t size)
				: data_(size == 0 ? nullptr : data), size_(size) {}

			size_t size() const { return size_; }
			bool empty() const { return size_ == 0; }
			T* data() const { return data_; }

		protected:
			static constexpr bool fixed_size() { return false; }

		private:
			T* data_;
			size_t size_;
		};

	}  // namespace impl

	template <typename T, std::ptrdiff_t Size = impl::kArrayViewVarSize>
	class ArrayView final : public impl::ArrayViewBase<T, Size> {
	public:
		using value_type = T;
		using const_iterator = const T*;

		// Construct an ArrayView from a pointer and a length.
		template <typename U>
		ArrayView(U* data, size_t size)
			: impl::ArrayViewBase<T, Size>::ArrayViewBase(data, size) {
			RTC_DCHECK_EQ(size == 0 ? nullptr : data, this->data());
			RTC_DCHECK_EQ(size, this->size());
			RTC_DCHECK_EQ(!this->data(),
				this->size() == 0);  // data is null iff size == 0.
		}

		// Construct an empty ArrayView. Note that fixed-size ArrayViews of size > 0
		// cannot be empty.
		ArrayView() : ArrayView(nullptr, 0) {}
		ArrayView(std::nullptr_t)  // NOLINT
			: ArrayView() {}
		ArrayView(std::nullptr_t, size_t size)
			: ArrayView(static_cast<T*>(nullptr), size) {
			static_assert(Size == 0 || Size == impl::kArrayViewVarSize, "");
			RTC_DCHECK_EQ(0, size);
		}

		// Construct an ArrayView from a C-style array.
		template <typename U, size_t N>
		ArrayView(U(&array)[N])  // NOLINT
			: ArrayView(array, N) {
			static_assert(Size == N || Size == impl::kArrayViewVarSize,
				"Array size must match ArrayView size");
		}

		// (Only if size is fixed.) Construct a fixed size ArrayView<T, N> from a
		// non-const std::array instance. For an ArrayView with variable size, the
		// used ctor is ArrayView(U& u) instead.
		template <typename U,
			size_t N,
			typename std::enable_if<
			Size == static_cast<std::ptrdiff_t>(N)>::type* = nullptr>
			ArrayView(std::array<U, N>& u)  // NOLINT
			: ArrayView(u.data(), u.size()) {}

		// (Only if size is fixed.) Construct a fixed size ArrayView<T, N> where T is
		// const from a const(expr) std::array instance. For an ArrayView with
		// variable size, the used ctor is ArrayView(U& u) instead.
		template <typename U,
			size_t N,
			typename std::enable_if<
			Size == static_cast<std::ptrdiff_t>(N)>::type* = nullptr>
			ArrayView(const std::array<U, N>& u)  // NOLINT
			: ArrayView(u.data(), u.size()) {}

		// (Only if size is fixed.) Construct an ArrayView from any type U that has a
		// static constexpr size() method whose return value is equal to Size, and a
		// data() method whose return value converts implicitly to T*. In particular,
		// this means we allow conversion from ArrayView<T, N> to ArrayView<const T,
		// N>, but not the other way around. We also don't allow conversion from
		// ArrayView<T> to ArrayView<T, N>, or from ArrayView<T, M> to ArrayView<T,
		// N> when M != N.
		template <
			typename U,
			typename std::enable_if<Size != impl::kArrayViewVarSize &&
			HasDataAndSize<U, T>::value>::type* = nullptr>
			ArrayView(U& u)  // NOLINT
			: ArrayView(u.data(), u.size()) {
			static_assert(U::size() == Size, "Sizes must match exactly");
		}

		// (Only if size is variable.) Construct an ArrayView from any type U that
		// has a size() method whose return value converts implicitly to size_t, and
		// a data() method whose return value converts implicitly to T*. In
		// particular, this means we allow conversion from ArrayView<T> to
		// ArrayView<const T>, but not the other way around. Other allowed
		// conversions include
		// ArrayView<T, N> to ArrayView<T> or ArrayView<const T>,
		// std::vector<T> to ArrayView<T> or ArrayView<const T>,
		// const std::vector<T> to ArrayView<const T>,
		// rtc::Buffer to ArrayView<uint8_t> or ArrayView<const uint8_t>, and
		// const rtc::Buffer to ArrayView<const uint8_t>.
		template <
			typename U,
			typename std::enable_if<Size == impl::kArrayViewVarSize &&
			HasDataAndSize<U, T>::value>::type* = nullptr>
			ArrayView(U& u)  // NOLINT
			: ArrayView(u.data(), u.size()) {}
		template <
			typename U,
			typename std::enable_if<Size == impl::kArrayViewVarSize &&
			HasDataAndSize<U, T>::value>::type* = nullptr>
			ArrayView(const U& u)  // NOLINT(runtime/explicit)
			: ArrayView(u.data(), u.size()) {}

		// Indexing and iteration. These allow mutation even if the ArrayView is
		// const, because the ArrayView doesn't own the array. (To prevent mutation,
		// use a const element type.)
		T& operator[](size_t idx) const {
			RTC_DCHECK_LT(idx, this->size());
			RTC_DCHECK(this->data());
			return this->data()[idx];
		}
		T* begin() const { return this->data(); }
		T* end() const { return this->data() + this->size(); }
		const T* cbegin() const { return this->data(); }
		const T* cend() const { return this->data() + this->size(); }

		ArrayView<T> subview(size_t offset, size_t size) const {
			return offset < this->size()
				? ArrayView<T>(this->data() + offset,
				(std::min)(size, this->size() - offset))
				: ArrayView<T>();
		}
		ArrayView<T> subview(size_t offset) const {
			return subview(offset, this->size());
		}
	};

	// Comparing two ArrayViews compares their (pointer,size) pairs; it does *not*
	// dereference the pointers.
	template <typename T, std::ptrdiff_t Size1, std::ptrdiff_t Size2>
	bool operator==(const ArrayView<T, Size1>& a, const ArrayView<T, Size2>& b) {
		return a.data() == b.data() && a.size() == b.size();
	}
	template <typename T, std::ptrdiff_t Size1, std::ptrdiff_t Size2>
	bool operator!=(const ArrayView<T, Size1>& a, const ArrayView<T, Size2>& b) {
		return !(a == b);
	}

	// Variable-size ArrayViews are the size of two pointers; fixed-size ArrayViews
	// are the size of one pointer. (And as a special case, fixed-size ArrayViews
	// of size 0 require no storage.)
	static_assert(sizeof(ArrayView<int>) == 2 * sizeof(int*), "");
	static_assert(sizeof(ArrayView<int, 17>) == sizeof(int*), "");
	static_assert(std::is_empty<ArrayView<int, 0>>::value, "");

	template <typename T>
	inline ArrayView<T> MakeArrayView(T* data, size_t size) {
		return ArrayView<T>(data, size);
	}

	// Only for primitive types that have the same size and aligment.
	// Allow reinterpret cast of the array view to another primitive type of the
	// same size.
	// Template arguments order is (U, T, Size) to allow deduction of the template
	// arguments in client calls: reinterpret_array_view<target_type>(array_view).
	template <typename U, typename T, std::ptrdiff_t Size>
	inline ArrayView<U, Size> reinterpret_array_view(ArrayView<T, Size> view) {
		static_assert(sizeof(U) == sizeof(T) && alignof(U) == alignof(T),
			"ArrayView reinterpret_cast is only supported for casting "
			"between views that represent the same chunk of memory.");
		static_assert(
			std::is_fundamental<T>::value && std::is_fundamental<U>::value,
			"ArrayView reinterpret_cast is only supported for casting between "
			"fundamental types.");
		return ArrayView<U, Size>(reinterpret_cast<U*>(view.data()), view.size());
	}
}

#endif //_C_ARRAY_VIEW_H_