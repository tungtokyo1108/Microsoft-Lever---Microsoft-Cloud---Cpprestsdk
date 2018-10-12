/*
 * SafeInt3.hpp
 *
 *  Created on: Oct 12, 2018
 *      Student (MIG Virtual Developer): Tung Dang
 */

#ifndef INCLUDE_CPPREST_DETAILS_SAFEINT3_HPP_
#define INCLUDE_CPPREST_DETAILS_SAFEINT3_HPP_

// Option Compiler
#define VISUAL_STUDIO_COMPILER 0
#define CLANG_COMPILER 1
#define GCC_COMPILER 2
#define UNKNOWN_COMPILER -1

#if defined __clang__
#define SAFEINT_COMPILER CLANG_COMPILER
#elif defined __GNUC__
#define SAFEINT_COMPILER GCC_COMPILER
#elif defined _MSC_VER
#define SAFEINT_COMPILER VISUAL_STUDIO_COMPILER
#else
#define SAFEINT_COMPILER UNKNOWN_COMPILER
#endif

#if SAFEINT_COMPILER == VISUAL_STUDIO_COMPILER
#pragma warning( push )
#pragma warning( disable:4987 4820 4987 4820 )

#endif

#include <cstddef>
#include <cstdlib>

#if SAFEINT_COMPILER == VISUAL_STUDIO_COMPILER && defined _M_AMD64
#include <intrin.h>
#define SAFEINT_USE_INTRINSICS 1
#else
#define SAFEINT_USE_INTRINSICS 0
#endif

#if SAFEINT_COMPILER == VISUAL_STUDIO_COMPILER
#pragma warning(pop)
#endif

#if SAFEINT_COMPILER == GCC_COMPILER || SAFEINT_COMPILER == CLANG_COMPILER
#define NEEDS_INT_DEFINED
#if !defined NULL
#define NULL 0
#endif

#if SAFEINT_COMPILER == GCC_COMPILER
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#endif

#include <stdint.h>

#if SAFEINT_COMPILER == CLANG_COMPILER
#if __has_feature(cxx_nullptr)
#define NEEDS_NULLPTR_DEFINED 0
#endif

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++11-long-long"
#pragma clang diagnostic ignored "-Wold-style-cast"
#pragma clang diagnostic ignored "-Wunused-local-typedef"
#endif
#endif

#if !defined NEEDS_NULLPTR_DEFINED
#if SAFEINT_COMPILER == VISUAL_STUDIO_COMPILER

#if (_MSC_VER < 1600)
#define NEEDS_NULLPTR_DEFINED 1
#else
#define NEEDS_NULLPTR_DEFINED 0
#endif

#else

#if __cplusplus >= 201103L
#define NEEDS_NULLPTR_DEFINED 0
#else
#define NEEDS_NULLPTR_DEFINED 1
#endif

#endif
#endif

#if NEEDS_NULLPTR_DEFINED
#define nullptr NULL
#endif

#ifndef C_ASSERT
#define C_ASSERT(e) typedef char __C_ASSERT__[(e)?1:-1]
#endif
C_ASSERT(-1 == static_cast<int>(0xffffffff));

#endif /* INCLUDE_CPPREST_DETAILS_SAFEINT3_HPP_ */
