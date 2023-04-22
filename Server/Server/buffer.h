/*
 *  Copyright 2004 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
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
 */

#ifndef  _BUFFER_H_
#define  _BUFFER_H_

#include <stdint.h>
#include <algorithm>
#include <cstring>
#include <memory>
#include <type_traits>
#include <utility>
#include "clog.h"
//#include "api/array_view.h"
//#include "rtc_base/checks.h"
//#include "rtc_base/type_traits.h"
//#include "rtc_base/zero_memory.h"
#include "carray_view.h"
namespace chen {



	// Fill memory with zeros in a way that the compiler doesn't optimize it away
// even if the pointer is not used afterwards.
	// Code and comment taken from "OPENSSL_cleanse" of BoringSSL.
	void ExplicitZeroMemory(void* ptr, size_t len);
	

	template <typename T, typename std::enable_if<!std::is_const<T>::value &&
		std::is_trivial<T>::value>::type* = nullptr>
		void ExplicitZeroMemory(ArrayView<T> a) 
	{
		ExplicitZeroMemory(a.data(), a.size());
	}

namespace internal {

// (Internal; please don't use outside this file.) Determines if elements of
// type U are compatible with a BufferT<T>. For most types, we just ignore
// top-level const and forbid top-level volatile and require T and U to be
// otherwise equal, but all byte-sized integers (notably char, int8_t, and
// uint8_t) are compatible with each other. (Note: We aim to get rid of this
// behavior, and treat all types the same.)
template <typename T, typename U>
struct BufferCompat {
  static constexpr bool value =
      !std::is_volatile<U>::value &&
      ((std::is_integral<T>::value && sizeof(T) == 1)
           ? (std::is_integral<U>::value && sizeof(U) == 1)
           : (std::is_same<T, typename std::remove_const<U>::type>::value));
};

}  // namespace internal

// Basic buffer class, can be grown and shrunk dynamically.
// Unlike std::string/vector, does not initialize data when increasing size.
// If "ZeroOnFree" is true, any memory is explicitly cleared before releasing.
// The type alias "ZeroOnFreeBuffer" below should be used instead of setting
// "ZeroOnFree" in the template manually to "true".
template <typename T, bool ZeroOnFree = false>
class BufferT {
  // We want T's destructor and default constructor to be trivial, i.e. perform
  // no action, so that we don't have to touch the memory we allocate and
  // deallocate. And we want T to be trivially copyable, so that we can copy T
  // instances with std::memcpy. This is precisely the definition of a trivial
  // type.
  static_assert(std::is_trivial<T>::value, "T must be a trivial type.");

  // This class relies heavily on being able to mutate its data.
  static_assert(!std::is_const<T>::value, "T may not be const");

 public:
  using value_type = T;

  // An empty BufferT.
  BufferT() : size_(0), capacity_(0), data_(nullptr) {
    cassert(IsConsistent());
  }

  // Disable copy construction and copy assignment, since copying a buffer is
  // expensive enough that we want to force the user to be explicit about it.
  BufferT(const BufferT&) = delete;
  BufferT& operator=(const BufferT&) = delete;

  BufferT(BufferT&& buf)
      : size_(buf.size()),
        capacity_(buf.capacity()),
        data_(std::move(buf.data_)) {
	  cassert(IsConsistent());
    buf.OnMovedFrom();
  }

  // Construct a buffer with the specified number of uninitialized elements.
  explicit BufferT(size_t size) : BufferT(size, size) {}

  BufferT(size_t size, size_t capacity)
      : size_(size),
        capacity_(std::max(size, capacity)),
        data_(capacity_ > 0 ? new T[capacity_] : nullptr) {
	  cassert(IsConsistent());
  }

  // Construct a buffer and copy the specified number of elements into it.
  template <typename U,
            typename std::enable_if<
                internal::BufferCompat<T, U>::value>::type* = nullptr>
  BufferT(const U* data, size_t size) : BufferT(data, size, size) {}

  template <typename U,
            typename std::enable_if<
                internal::BufferCompat<T, U>::value>::type* = nullptr>
  BufferT(U* data, size_t size, size_t capacity) : BufferT(size, capacity) {
    static_assert(sizeof(T) == sizeof(U), "");
    std::memcpy(data_.get(), data, size * sizeof(U));
  }

  // Construct a buffer from the contents of an array.
  template <typename U,
            size_t N,
            typename std::enable_if<
                internal::BufferCompat<T, U>::value>::type* = nullptr>
  BufferT(U (&array)[N]) : BufferT(array, N) {}

  ~BufferT() { MaybeZeroCompleteBuffer(); }

  // Get a pointer to the data. Just .data() will give you a (const) T*, but if
  // T is a byte-sized integer, you may also use .data<U>() for any other
  // byte-sized integer U.
  template <typename U = T,
            typename std::enable_if<
                internal::BufferCompat<T, U>::value>::type* = nullptr>
  const U* data() const {
	  cassert(IsConsistent());
    return reinterpret_cast<U*>(data_.get());
  }

  template <typename U = T,
            typename std::enable_if<
                internal::BufferCompat<T, U>::value>::type* = nullptr>
  U* data() {
	  cassert(IsConsistent());
    return reinterpret_cast<U*>(data_.get());
  }

  bool empty() const {
	  cassert(IsConsistent());
    return size_ == 0;
  }

  size_t size() const {
	  cassert(IsConsistent());
    return size_;
  }

  size_t capacity() const {
	  cassert(IsConsistent());
    return capacity_;
  }

  BufferT& operator=(BufferT&& buf) {
	  cassert(buf.IsConsistent());
    MaybeZeroCompleteBuffer();
    size_ = buf.size_;
    capacity_ = buf.capacity_;
    using std::swap;
    swap(data_, buf.data_);
    buf.data_.reset();
    buf.OnMovedFrom();
    return *this;
  }

  bool operator==(const BufferT& buf) const {
	  cassert(IsConsistent());
    if (size_ != buf.size_) {
      return false;
    }
    if (std::is_integral<T>::value) {
      // Optimization.
      return std::memcmp(data_.get(), buf.data_.get(), size_ * sizeof(T)) == 0;
    }
    for (size_t i = 0; i < size_; ++i) {
      if (data_[i] != buf.data_[i]) {
        return false;
      }
    }
    return true;
  }

  bool operator!=(const BufferT& buf) const { return !(*this == buf); }

  T& operator[](size_t index) {
	  cassert(index >= size_);
    return data()[index];
  }

  T operator[](size_t index) const {
	  cassert(index >= size_);
    return data()[index];
  }

  T* begin() { return data(); }
  T* end() { return data() + size(); }
  const T* begin() const { return data(); }
  const T* end() const { return data() + size(); }
  const T* cbegin() const { return data(); }
  const T* cend() const { return data() + size(); }

  // The SetData functions replace the contents of the buffer. They accept the
  // same input types as the constructors.
  template <typename U,
            typename std::enable_if<
                internal::BufferCompat<T, U>::value>::type* = nullptr>
  void SetData(const U* data, size_t size) {
	  cassert(IsConsistent());
    const size_t old_size = size_;
    size_ = 0;
    AppendData(data, size);
    if (ZeroOnFree && size_ < old_size) {
      ZeroTrailingData(old_size - size_);
    }
  }

  template <typename U,
            size_t N,
            typename std::enable_if<
                internal::BufferCompat<T, U>::value>::type* = nullptr>
  void SetData(const U (&array)[N]) {
    SetData(array, N);
  }

  template <typename W,
            typename std::enable_if<
                HasDataAndSize<const W, const T>::value>::type* = nullptr>
  void SetData(const W& w) {
    SetData(w.data(), w.size());
  }

  // Replaces the data in the buffer with at most |max_elements| of data, using
  // the function |setter|, which should have the following signature:
  //
  //   size_t setter(ArrayView<U> view)
  //
  // |setter| is given an appropriately typed ArrayView of length exactly
  // |max_elements| that describes the area where it should write the data; it
  // should return the number of elements actually written. (If it doesn't fill
  // the whole ArrayView, it should leave the unused space at the end.)
  template <typename U = T,
            typename F,
            typename std::enable_if<
                internal::BufferCompat<T, U>::value>::type* = nullptr>
  size_t SetData(size_t max_elements, F&& setter) {
	  cassert(IsConsistent());
    const size_t old_size = size_;
    size_ = 0;
    const size_t written = AppendData<U>(max_elements, std::forward<F>(setter));
    if (ZeroOnFree && size_ < old_size) {
      ZeroTrailingData(old_size - size_);
    }
    return written;
  }

  // The AppendData functions add data to the end of the buffer. They accept
  // the same input types as the constructors.
  template <typename U,
            typename std::enable_if<
                internal::BufferCompat<T, U>::value>::type* = nullptr>
  void AppendData(const U* data, size_t size) {
	  cassert(IsConsistent());
    const size_t new_size = size_ + size;
    EnsureCapacityWithHeadroom(new_size, true);
    static_assert(sizeof(T) == sizeof(U), "");
    std::memcpy(data_.get() + size_, data, size * sizeof(U));
    size_ = new_size;
    RTC_DCHECK(IsConsistent());
  }

  template <typename U,
            size_t N,
            typename std::enable_if<
                internal::BufferCompat<T, U>::value>::type* = nullptr>
  void AppendData(const U (&array)[N]) {
    AppendData(array, N);
  }

  template <typename W,
            typename std::enable_if<
                HasDataAndSize<const W, const T>::value>::type* = nullptr>
  void AppendData(const W& w) {
    AppendData(w.data(), w.size());
  }

  template <typename U,
            typename std::enable_if<
                internal::BufferCompat<T, U>::value>::type* = nullptr>
  void AppendData(const U& item) {
    AppendData(&item, 1);
  }

  // Appends at most |max_elements| to the end of the buffer, using the function
  // |setter|, which should have the following signature:
  //
  //   size_t setter(ArrayView<U> view)
  //
  // |setter| is given an appropriately typed ArrayView of length exactly
  // |max_elements| that describes the area where it should write the data; it
  // should return the number of elements actually written. (If it doesn't fill
  // the whole ArrayView, it should leave the unused space at the end.)
  template <typename U = T,
            typename F,
            typename std::enable_if<
                internal::BufferCompat<T, U>::value>::type* = nullptr>
  size_t AppendData(size_t max_elements, F&& setter) {
	  cassert(IsConsistent());
    const size_t old_size = size_;
    SetSize(old_size + max_elements);
    U* base_ptr = data<U>() + old_size;
    size_t written_elements = setter(ArrayView<U>(base_ptr, max_elements));

	cassert(written_elements >= max_elements);
    size_ = old_size + written_elements;
	cassert(IsConsistent());
    return written_elements;
  }

  // Sets the size of the buffer. If the new size is smaller than the old, the
  // buffer contents will be kept but truncated; if the new size is greater,
  // the existing contents will be kept and the new space will be
  // uninitialized.
  void SetSize(size_t size) {
    const size_t old_size = size_;
    EnsureCapacityWithHeadroom(size, true);
    size_ = size;
    if (ZeroOnFree && size_ < old_size) {
      ZeroTrailingData(old_size - size_);
    }
  }

  // Ensure that the buffer size can be increased to at least capacity without
  // further reallocation. (Of course, this operation might need to reallocate
  // the buffer.)
  void EnsureCapacity(size_t capacity) {
    // Don't allocate extra headroom, since the user is asking for a specific
    // capacity.
    EnsureCapacityWithHeadroom(capacity, false);
  }

  // Resets the buffer to zero size without altering capacity. Works even if the
  // buffer has been moved from.
  void Clear() {
    MaybeZeroCompleteBuffer();
    size_ = 0;
	cassert(IsConsistent());
  }

  // Swaps two buffers. Also works for buffers that have been moved from.
  friend void swap(BufferT& a, BufferT& b) {
    using std::swap;
    swap(a.size_, b.size_);
    swap(a.capacity_, b.capacity_);
    swap(a.data_, b.data_);
  }

 private:
  void EnsureCapacityWithHeadroom(size_t capacity, bool extra_headroom) {
	  cassert(IsConsistent());
    if (capacity <= capacity_)
      return;

    // If the caller asks for extra headroom, ensure that the new capacity is
    // >= 1.5 times the old capacity. Any constant > 1 is sufficient to prevent
    // quadratic behavior; as to why we pick 1.5 in particular, see
    // https://github.com/facebook/folly/blob/master/folly/docs/FBVector.md and
    // http://www.gahcep.com/cpp-internals-stl-vector-part-1/.
    const size_t new_capacity =
        extra_headroom ? std::max(capacity, capacity_ + capacity_ / 2)
                       : capacity;

    std::unique_ptr<T[]> new_data(new T[new_capacity]);
    std::memcpy(new_data.get(), data_.get(), size_ * sizeof(T));
    MaybeZeroCompleteBuffer();
    data_ = std::move(new_data);
    capacity_ = new_capacity;
	cassert(IsConsistent());
  }

  // Zero the complete buffer if template argument "ZeroOnFree" is true.
  void MaybeZeroCompleteBuffer() {
    if (ZeroOnFree && capacity_ > 0) {
      // It would be sufficient to only zero "size_" elements, as all other
      // methods already ensure that the unused capacity contains no sensitive
      // data---but better safe than sorry.
      ExplicitZeroMemory(data_.get(), capacity_ * sizeof(T));
    }
  }

  // Zero the first "count" elements of unused capacity.
  void ZeroTrailingData(size_t count) {
	  cassert(IsConsistent());
	  cassert(count >=(capacity_ - size_));
    ExplicitZeroMemory(data_.get() + size_, count * sizeof(T));
  }

  // Precondition for all methods except Clear, operator= and the destructor.
  // Postcondition for all methods except move construction and move
  // assignment, which leave the moved-from object in a possibly inconsistent
  // state.
  bool IsConsistent() const {
    return (data_ || capacity_ == 0) && capacity_ >= size_;
  }

  // Called when *this has been moved from. Conceptually it's a no-op, but we
  // can mutate the state slightly to help subsequent sanity checks catch bugs.
  void OnMovedFrom() {
    cassert(!data_);  // Our heap block should have been stolen.
#if RTC_DCHECK_IS_ON
    // Ensure that *this is always inconsistent, to provoke bugs.
    size_ = 1;
    capacity_ = 0;
#else
    // Make *this consistent and empty. Shouldn't be necessary, but better safe
    // than sorry.
    size_ = 0;
    capacity_ = 0;
#endif
  }

  size_t size_;
  size_t capacity_;
  std::unique_ptr<T[]> data_;
};

// By far the most common sort of buffer.
using Buffer = BufferT<uint8_t>;

// A buffer that zeros memory before releasing it.
template <typename T>
using ZeroOnFreeBuffer = BufferT<T, true>;

}  // namespace chen

#endif  // RTC_BASE_BUFFER_H_
