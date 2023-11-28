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
#include <stdlib.h>
#include <locale.h>
#include <wchar.h>
#include <crtdbg.h>
#include <string.h>
#include <direct.h>

// C++ Standard Library (STL)
#include <string>
#include <algorithm>
#include <sstream>

// Windows API

// Exclude rarely-used stuff from Windows headers
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>

// DirectX

#define DIRECTINPUT_VERSION 0x0800

#include <dxerr.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <dinput.h>
#include <xinput.h>
