// TargetVer.h
//

#ifndef __TARGETVER_H__
#define __TARGETVER_H__

// 指定目标 Windows 版本的两种方法:
// 1. 包含 SDKDDKVer.h, 假定目标平台和开发平台的版本相同
//    要求: VC 2010 (_MSC_VER = 1600) 之前需要配置 Windows SDK 目录
// 2. 指定 _WIN32_WINNT 等版本控制宏, 然后可选择包含 SDKDDKVer.h
//    要求: 较老的目标 Windows 需要先包含 WinSDKVer.h

#if _MSC_VER < 1600
#ifndef WINVER
#define WINVER          0x0501  // Windows XP+ 特性
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT    0x0501  // Windows XP+ 特性
#endif

#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS  0x0410  // Windows 98+ 特性
#endif

#ifndef _WIN32_IE
#define _WIN32_IE       0x0600  // IE 6.0+ 特性
#endif
#endif  // _MSC_VER < 1600

#if _MSC_VER >= 1600
#include <SDKDDKVer.h>
#endif

#endif  // __TARGETVER_H__
