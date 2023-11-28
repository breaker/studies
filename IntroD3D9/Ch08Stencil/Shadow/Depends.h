// Depends.h
//

#ifndef __DEPENDS_H__
#define __DEPENDS_H__

////////////////////////////////////////////////////////////////////////////////
// C ��׼��������ʱ�� (CRT)
////////////////////////////////////////////////////////////////////////////////

#define _CRT_SECURE_NO_WARNINGS     // �رղ���ȫ�� CRT ��������
#define _CRTDBG_MAP_ALLOC           // ʹ�� Debug ��� malloc �� strdup

#include <stddef.h>
#include <stdlib.h>

#include <errno.h>
#include <crtdbg.h>

#include <stdio.h>

#include <string.h>
#include <mbstring.h>
#include <wchar.h>
#include <tchar.h>
#include <locale.h>

#include <sal.h>
#include <typeinfo.h>

////////////////////////////////////////////////////////////////////////////////
// C++ ��׼��
////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include <sstream>

#include <exception>
#include <stdexcept>

#include <new>
#include <memory>
#include <locale>

#include <string>
#include <vector>
#include <list>
#include <deque>

////////////////////////////////////////////////////////////////////////////////
// Windows API
////////////////////////////////////////////////////////////////////////////////

// �ų� Windows ͷ�ļ��в����õĹ���
#define WIN32_LEAN_AND_MEAN

// ��ʹ�� Windows ͷ�ļ��ж���� min, max ��, ����Ӱ�� std ���ֿռ�� min, max
// #define NOMINMAX

// ͬʱʹ�� VC 2005 Windows API �� DX8 SDK ʱ, ���� winnt.h ����, �趨�� POINTER_64
#if (_MSC_VER >= 1300) && !(defined(MIDL_PASS) || defined(RC_INVOKED))
#define POINTER_64 __ptr64
#else
#define POINTER_64
#endif

// ʹ�� MFC ��ʱ��Ҫ���� Windows.h, MFC ͷ�ļ����Ѱ���
// #include <Windows.h>

////////////////////////////////////////////////////////////////////////////////
// ATL/MFC
////////////////////////////////////////////////////////////////////////////////

// �ų� Windows ͷ�ļ��в����õĹ���, ������ MFC
#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

// ATL/MFC �İ�ȫ����
#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS  // ʹ ATL/MFC �� CString ������ʽ�ع��캯�� (explicit)
#define _AFX_ALL_WARNINGS                   // �� MFC �����о���, ����һ����԰�ȫ���Եľ���

#include <afxwin.h>     // MFC ���ĺͱ�׼֧��
#include <afxext.h>     // MFC ��չ֧��

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>   // MFC �� IE 4 ͨ�ÿؼ�֧��
#endif

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>     // MFC �� Windows ͨ�ÿؼ�֧��
#endif

////////////////////////////////////////////////////////////////////////////////
// DirectX
////////////////////////////////////////////////////////////////////////////////

#include <DxErr.h>
#include <d3dx9.h>

////////////////////////////////////////////////////////////////////////////////
// SGL
////////////////////////////////////////////////////////////////////////////////

#include <SGL/Utils.h>
#include <SGL/Input.h>
#include <SGL/Render.h>
#include <SGL/Main.h>

////////////////////////////////////////////////////////////////////////////////
// Windows ͨ�ÿؼ� ComCtl32.dll �汾 6.0 ����Ƕ manifest
////////////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif  // _UNICODE

#endif  // __DEPENDS_H__
