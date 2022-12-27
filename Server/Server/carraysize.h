/***********************************************************************************************
created: 		2022-12-27

author:			chensong

purpose:		ip_address


************************************************************************************************/

#ifndef _C_ARRAY_SIZE_H_
#define _C_ARRAY_SIZE_H_


#include <stddef.h>

// This file defines the arraysize() macro and is derived from Chromium's
// base/macros.h.

// The arraysize(arr) macro returns the # of elements in an array arr.
// The expression is a compile-time constant, and therefore can be
// used in defining new arrays, for example.  If you use arraysize on
// a pointer by mistake, you will get a compile-time error.

// This template function declaration is used in defining arraysize.
// Note that the function doesn't need an implementation, as we only
// use its type.
template <typename T, size_t N>
char(&ArraySizeHelper(T(&array)[N]))[N];

#define arraysize(array) (sizeof(ArraySizeHelper(array)))

namespace chen {

}
#endif // _C_ARRAY_SIZE_H_