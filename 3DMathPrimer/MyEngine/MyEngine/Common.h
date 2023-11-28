/////////////////////////////////////////////////////////////////////////////
//
// 3D Math Primer for Games and Graphics Development
//
// Common.h - Miscelaneous declarations that don't belong elsewhere.
//
// Visit gamemath.com for the latest version of this file.
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <MyEngine/Defs.h>

MYENGINE_NS_BEGIN

// Return TRUE if we're under the debugger

MYENGINE_API BOOL IsDebugged();

// Declare a function to terminate the program with a printf-like
// formatted error message

MYENGINE_API void DoAbort(const char* fmt, ...);

// Normally, we will call this function using the ABORT macro, which also
// reports the source file and line number.  This nasty looking macro
// is a handy little trick to allow our macro appear to to take a variable
// number of argmuents

MYENGINE_API extern const char* gAbortSourceFile;
MYENGINE_API extern int gAbortSourceLine;

#define MYENGINE_ABORT (gAbortSourceFile = __FILE__, gAbortSourceLine = __LINE__, DoAbort)

// Standard min and max functions

template <class Type>
inline const Type& Min(const Type& a, const Type& b)
{
    return (a < b) ? a : b;
}

template <class Type>
inline const Type& Max(const Type& a, const Type& b)
{
    return (a > b) ? a : b;
}

// Standard template swap routine

template <class Type>
inline void Swap(Type& a, Type& b)
{
    Type tmp(a);
    a = b;
    b = tmp;
}

MYENGINE_NS_END
