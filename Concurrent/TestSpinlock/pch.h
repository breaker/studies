// pch.h
//

#pragma once

////////////////////////////////////////////////////////////////////////////////
// Target Platforms
////////////////////////////////////////////////////////////////////////////////

#ifndef _WIN32_WINNT
#define _WIN32_WINNT    0x0600  // Windows Vista
#endif

////////////////////////////////////////////////////////////////////////////////
// Dependent Libraries
////////////////////////////////////////////////////////////////////////////////

// C Runtime Library (CRT)

// Use security-enhanced rand_s()
#define _CRT_RAND_S

#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <locale.h>
#include <crtdbg.h>
#include <errno.h>

// C++ Standard Library (STL)
#include <vector>

// Windows API

// Exclude rarely-used stuff from Windows headers
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
