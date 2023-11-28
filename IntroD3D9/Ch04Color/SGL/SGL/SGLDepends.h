// SGLDepends.h
//

#ifndef __SGLDEPENDS_H__
#define __SGLDEPENDS_H__

////////////////////////////////////////////////////////////////////////////////
// C ��׼��������ʱ�� (CRT)
////////////////////////////////////////////////////////////////////////////////

#define _CRTDBG_MAP_ALLOC   // ʹ�� Debug ��� malloc �� strdup

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

#include <exception>
#include <stdexcept>

#include <locale>
#include <memory>

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
// Simp
////////////////////////////////////////////////////////////////////////////////

#include <Simp/Defs.h>
#include <Simp/Memory.h>
#include <Simp/Pattern/Uncopyable.h>

#endif  // __SGLDEPENDS_H__
