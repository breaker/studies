// TargetVer.h
//

#ifndef __TARGETVER_H__
#define __TARGETVER_H__

// ָ��Ŀ�� Windows �汾�����ַ���:
// 1. ���� SDKDDKVer.h, �ٶ�Ŀ��ƽ̨�Ϳ���ƽ̨�İ汾��ͬ
//    Ҫ��: VC 2010 (_MSC_VER = 1600) ֮ǰ��Ҫ���� Windows SDK Ŀ¼
// 2. ָ�� _WIN32_WINNT �Ȱ汾���ƺ�, Ȼ���ѡ����� SDKDDKVer.h
//    Ҫ��: ���ϵ�Ŀ�� Windows ��Ҫ�Ȱ��� WinSDKVer.h

#if _MSC_VER < 1600
#ifndef WINVER
#define WINVER          0x0501  // Windows XP+ ����
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT    0x0501  // Windows XP+ ����
#endif

#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS  0x0410  // Windows 98+ ����
#endif

#ifndef _WIN32_IE
#define _WIN32_IE       0x0600  // IE 6.0+ ����
#endif
#endif  // _MSC_VER < 1600

#if _MSC_VER >= 1600
#include <SDKDDKVer.h>
#endif

#endif  // __TARGETVER_H__
