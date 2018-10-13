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

#ifdef NEEDS_INT_DEFINED
#define __int8 char
#define __int16 short
#define __int32 int
#define __int64 long long
#endif

namespace msl {
  namespace safeint3 {
    enum SafeIntError {
      SafeintNoError = 0,
      SafeIntArithmeticOverflow,
      SafeIntDivideByZero
    };
  }
}

#if defined SAFEINT_REMOVE_NOTHROW
#define SAFEINT_NOTHROW
#else
#define SAFEINT_NOTHROW throw()
#endif

namespace msl
{

namespace safeint3
{
  
#if !defined SAFEINT_ASSERT
#include <assert.h>
#define SAFEINT_ASSERT(x) assert(x)
#endif

#if defined SAFEINT_ASSERT_ON_EXCEPTION
inline void SafeIntExceptionAssert() SAFEINT_NOTHROW {SAFEINT_ASSERT(false);}
#else
inline void SafeIntExceptionAssert() SAFEINT_NOTHROW {}
#endif

#if SAFEINT_COMPILER == GCC_COMPILER || SAFEINT_COMPILER == CLANG_COMPILER
#define SAFEINT_NORETURN __attribute__((noreturn))
#define SAFEINT_STDCALL
#define SAFEINT_VISIBLE __attribute__((__visibility__("default")))
#define SAFEINT_WEAK __attribute__((weak))
#else
#define SAFEINT_NORETURN __declspec(noreturn)
#define SAFEINT_STDCALL __stdcall
#define SAFEINT_VISIBLE
#define SAFEINT_WEAK 
#endif

class SAFEINT_VISIBLE SafeIntException
{
public:
  SafeIntException() SAFEINT_NOTHROW { m_code = SafeintNoError;}
  SafeIntException( SafeIntError code ) SAFEINT_NOTHROW
  {
    m_code = code;
  }
  SafeIntError m_code;
};

namespace SafeIntInternal
{
  template<typename E>class SafeIntExceptionHandler;
  template<>class SafeIntExceptionHandler<SafeIntException>
  {
  public:
    static SAFEINT_NORETURN void SAFEINT_STDCALL SafeIntOnOverflow()
    {
      SafeIntExceptionAssert();
      throw SafeIntException(SafeIntArithmeticOverflow);
    }

    static SAFEINT_NORETURN void SAFEINT_STDCALL SafeIntOnDivZero()
    {
      SafeIntExceptionAssert();
      throw SafeIntException(SafeIntDivideByZero);
    }
  };

#if !defined _CRT_SECURE_INVALID_PARAMETER
#include <stdlib.h>
#define _CRT_SECURE_INVALID_PARAMETER(msg) abort()
#endif

class SafeInt_InvalidParameter
{
public:
  static SAFEINT_NORETURN void SafeIntOnOverflow() SAFEINT_NOTHROW
  {
    SafeIntExceptionAssert();
    _CRT_SECURE_INVALID_PARAMETER("SafeInt Arithmetic Overflow");
  }

  static SAFEINT_NORETURN void SafeIntOnDivZero() SAFEINT_NOTHROW
  {
    SafeIntExceptionAssert();
    _CRT_SECURE_INVALID_PARAMETER("SafeInt Divide By Zero");
  }
};

#if defined _WINDOWS_
class SafeIntWin32ExceptionHandler
{
public:
  static SAFEINT_NORETURN void SAFEINT_STDCALL SafeIntOnOverflow() SAFEINT_NOTHROW
  {
    SafeIntExceptionAssert();
    RaiseException(static_cast<DWORD>(EXCEPTION_INT_OVERFLOW), EXCEPTION_NONCONTINUABLE,0,0);
  }

  static SAFEINT_NORETURN void SAFEINT_STDCALL SafeIntOnDivZero() SAFEINT_NOTHROW
  {
    SafeIntExceptionAssert();
    RaiseException(static_cast<DWORD>(EXCEPTION_INT_DIVIDE_BY_ZERO), EXCEPTION_NONCONTINUABLE,0,0);
  }
};
#endif
}

#if defined VISUAL_STUDIO_SAFEINT_COMPAT
typedef CPlusPlusExceptionHandler SafeIntErrorPolicy_SafeIntException;
typedef InvalidParameterExceptionHandler SafeIntErrorPolicy_InvalidParameter;
#endif

#if !defined SafeIntDefaultExceptionHandler
    #if defined SAFEINT_RAISE_EXCEPTION
        #if !defined _WINDOWS_
        #error Include windows.h in order to use Win32 exceptions
        #endif

        #define SafeIntDefaultExceptionHandler Win32ExceptionHandler
    #elif defined SAFEINT_FAILFAST
        #define SafeIntDefaultExceptionHandler InvalidParameterExceptionHandler
    #else
        #define SafeIntDefaultExceptionHandler CPlusPlusExceptionHandler
		#if !defined SAFEINT_EXCEPTION_HANDLER_CPP
		#define SAFEINT_EXCEPTION_HANDLER_CPP 1
		#endif
    #endif
#endif

#if !defined SAFEINT_EXCEPTION_HANDLER_CPP
#define SAFEINT_EXCEPTION_HANDLER_CPP 0
#endif

#if SAFEINT_EXCEPTION_HANDLER_CPP
#define SAFEINT_CPP_THROW
#else
#define SAFEINT_CPP_THROW SAFEINT_NOTHROW
#endif

/*
Turn out
We can fool the Compile into not seeing Compile-time constants
*/
template<int method>class CompileConst;
template<> class CompileConst<true> {public: static bool Value() SAFEINT_NOTHROW {return true;}};
template<> class CompileConst<false> {public: static bool Value() SAFEINT_NOTHROW {return false;}};

/*
With following template, we are not allowed to cast a float to an enum.
In this case, if we happen to assign an enum to a SafeInt of some type, it will not compile
*/
template<typename T> class NumericType;
#if defined _LIBCPP_TYPE_TRAITS || defined _TYPE_TRAITS_
template<>class NumericType<bool> {public: enum { isBool = true, isFloat = false, isInt = false };};
template<typename T>class NumericType
{
public:
  enum
  {
    isBool = false;
    isFloat = std::is_floating_point<T>::value,
    isInt = std::is_integral<T>::value || std::is_enum<T>::value
  };
};

#else

template<>class NumericType<bool>          {public: enum {isBool = true, isFloat = false, isInt = false};};
template<>class NumericType<char>          {public: enum {isBool = false, isFloat = false, isInt = true};};
template<>class NumericType<unsigned char> {public: enum {isBool = false, isFloat = false, isInt = true};};
template<>class NumericType<signed char>   {public: enum {isBool = false, isFloat = false, isInt = true};};
template<>class NumericType<short>         {public: enum {isBool = false, isFloat = false, isInt = true};};
template<>class NumericType<unsigned short>{public: enum {isBool = false, isFloat = false, isInt = true};};
#if defined SAFEINT_USE_WCHAR_T || defined _NATIVE_WCHAR_T_DEFINED
template <> class NumericType<wchar_t>     {public: enum {isBool = false, isFloat = false, isInt = true};};
#endif
template<>class NumericType<int>           {public: enum {isBool = false, isFloat = false, isInt = true};};
template<>class NumericType<unsigned int>  {public: enum {isBool = false, isFloat = false, isInt = true};};
template<>class NumericType<long>          {public: enum {isBool = false, isFloat = false, isInt = true};};
template<>class NumericType<unsigned long> {public: enum {isBool = false, isFloat = false, isInt = true};};
template<>class NumericType<__int64>       {public: enum {isBool = false, isFloat = false, isInt = true};};
template<>class NumericType<unsigned __int64>{public: enum {isBool = false, isFloat = false, isInt = true};};
template<>class NumericType<float>         {public: enum {isBool = false, isFloat = true, isInt = false};};
template<>class NumericType<double>        {public: enum {isBool = false, isFloat = true, isInt = false};};
template<>class NumericType<long double>   {public: enum {isBool = false, isFloat = true, isInt = false};};

template<typename T>class NumericType
{
public:
  enum
  {
    isBool = false,
    isFloat = false,
    inInt = static_cast<int>(static_cast<T>(0)) == 0
  };
};
#endif

// Use the following template to avoid compile-time const truncation warning

template<int fSigned, int bits> class SafeIntMinMax;
template<> class SafeIntMinMax<true,8>
{
public:
  const static signed __int8 min = (-0x7f - 1);
  const static signed __int8 max = 0x7f;
};
template<> class SafeIntMinMax<true,16>
{
public:
  const static __int16 min = (-0x7fff - 1);
  const static __int16 max = 0x7fff;
};
template<> class SafeIntMinMax<true,32>
{
public:
  const static __int32 min = (-0x7fffffff -1);
  const static __int32 max = 0x7fffffff;
};
template<> class SafeIntMinMax<true,64>
{
public:
  const static __int64 min = static_cast<__int64>(0x8000000000000000LL);
  const static __int64 max = 0x7fffffffffffffffLL;
};

template<> class SafeIntMinMax<false, 8>
{
public:
  const static unsigned __int8 min = 0;
  const static unsigned __int8 max = 0xff;
};
template<> class SafeIntMinMax<false, 16>
{
public:
  const static unsigned __int16 min = 0;
  const static unsigned __int16 max = 0xffff;
};
template<> class SafeIntMinMax<false, 32>
{
public:
  const static unsigned __int32 min = 0;
  const static unsigned __int32 max = 0xffffffff;
};
template<> class SafeIntMinMax<false, 64>
{
public:
  const static unsigned __int64 min = 0;
  const static unsigned __int64 max = 0xffffffffffffffffULL;
};

template<typename T>class IntTraits
{
public:
  C_ASSERT(NumericType<T>::isInt);
  enum
  {
    isSigned = ((T)(-1) < 0),
    is64Bit = (sizeof(T) == 8),
    is32Bit = (sizeof(T) == 4),
    is16Bit = (sizeof(T) == 2),
    is8Bit  = (sizeof(T) == 1),
    isLT32Bit = (sizeof(T) < 4),
    isLT64Bit = (sizeof(T) < 8),
    isInt8  = (sizeof(T) == 1 && isSigned),
    isUint8 = (sizeof(T) == 1 && !isSigned),
    isInt16  = (sizeof(T) == 2 && isSigned),
    isUint16 = (sizeof(T) == 2 && !isSigned),
    isInt32  = (sizeof(T) == 4 && isSigned),
    isUint32 = (sizeof(T) == 4 && !isSigned),
    isInt64  = (sizeof(T) == 8 && isSigned),
    isUint64 = (sizeof(T) == 8 && !isSigned),
    bitCount = (sizeof(T)*8),
    isBool   = ((T)2 == (T)1)
  };

  const static T maxInt = static_cast<T>(SafeIntMinMax<isSigned,bitCount>::max);
  const static T minInt = static_cast<T>(SafeIntMinMax<isSigned,bitCount>::min);
};
  
}
}

#endif /* INCLUDE_CPPREST_DETAILS_SAFEINT3_HPP_ */
