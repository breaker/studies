// StdAfx.h
//

#pragma once

#ifndef WINVER
#define WINVER 0x0501           // Windows XP+
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif

#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS 0x0410   // Windows 98+
#endif

#ifndef _WIN32_IE
#define _WIN32_IE 0x0600        // IE 6.0+
#endif

////////////////////////////////////////////////////////////////////////////////
// C 标准库与运行时库 (CRT)
////////////////////////////////////////////////////////////////////////////////

#define _CRT_SECURE_NO_WARNINGS
#define _CRTDBG_MAP_ALLOC       // 使用 Debug 版的 malloc 和 strdup

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include <assert.h>
#include <errno.h>
#include <crtdbg.h>

#include <string.h>
#include <mbstring.h>
#include <wchar.h>
#include <tchar.h>
#include <locale.h>

#include <math.h>
#include <stdarg.h>
#include <sal.h>
#include <typeinfo.h>

////////////////////////////////////////////////////////////////////////////////
// Windows API
////////////////////////////////////////////////////////////////////////////////

// 排除 Windows 头文件中不常用的功能
#define WIN32_LEAN_AND_MEAN

// 不使用 Windows 头文件中定义的 min, max 宏, 它会影响 std 名字空间的 min, max
#define NOMINMAX

// 同时使用 VC 2005 Windows API 和 DX8 SDK 时, 会在 winnt.h 报错, 需定义 POINTER_64
#if (_MSC_VER >= 1300) && !(defined(MIDL_PASS) || defined(RC_INVOKED))
#define POINTER_64 __ptr64
#else
#define POINTER_64
#endif

#include <Windows.h>

////////////////////////////////////////////////////////////////////////////////
// DirectX
////////////////////////////////////////////////////////////////////////////////

#include <D3D8.h>
