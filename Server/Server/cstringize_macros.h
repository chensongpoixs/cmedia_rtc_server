/********************************************************************
created:	2021-11-24

author:		chensong

purpose:	Location

*********************************************************************/
 
#ifndef  _STRINGIZE_MACROS_H_
#define  _STRINGIZE_MACROS_H_

// This is not very useful as it does not expand defined symbols if
// called directly. Use its counterpart without the _NO_EXPANSION
// suffix, below.
#define STRINGIZE_NO_EXPANSION(x) #x

// Use this to quote the provided parameter, first expanding it if it
// is a preprocessor symbol.
//
// For example, if:
//   #define A FOO
//   #define B(x) myobj->FunctionCall(x)
//
// Then:
//   STRINGIZE(A) produces "FOO"
//   STRINGIZE(B(y)) produces "myobj->FunctionCall(y)"
#define STRINGIZE(x) STRINGIZE_NO_EXPANSION(x)

#endif  //  _STRINGIZE_MACROS_H_
