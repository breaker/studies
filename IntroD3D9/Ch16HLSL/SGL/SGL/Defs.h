// Defs.h
//

#pragma once

////////////////////////////////////////////////////////////////////////////////
// SGL ���ֿռ�
////////////////////////////////////////////////////////////////////////////////

#define SGL_NS_BEGIN    namespace SGL {
#define SGL_NS_END      }

SGL_NS_BEGIN

////////////////////////////////////////////////////////////////////////////////
// SGL �汾
////////////////////////////////////////////////////////////////////////////////

#define SGL_VER         0x01000001
#define SGL_VER_STR     "1.0.0.1"

////////////////////////////////////////////////////////////////////////////////
// DLL ���뵼��
////////////////////////////////////////////////////////////////////////////////

#ifdef _SGL_EXPORTS
#define SGL_API __declspec(dllexport)
#else
#define SGL_API __declspec(dllimport)
#endif

#ifdef _SGL_WARNING_OFF_EXPORTS
#pragma warning(disable: 4275)  // ��ֹ����: ������ӷǵ���������
#pragma warning(disable: 4251)  // ��ֹ����: ��Ҫ������
#endif

// ֻ����һ��ȫ�ִ洢, �Ա��� .h �ж���ȫ�ֱ���
#define SGL_GLOBAL __declspec(selectany)

// SGL ģ����
const char      MODULE_NAMEA[]  = "SGL";
const wchar_t   MODULE_NAMEW[]  = L"SGL";
const _TCHAR    MODULE_NAME[]   = _T("SGL");

////////////////////////////////////////////////////////////////////////////////
// ��������
////////////////////////////////////////////////////////////////////////////////

// �ַ�����������
#define _SGL_STRINGIZE(x)       #x
#define SGL_STRINGIZE(x)        _SGL_STRINGIZE(x)
#define SGL_STRINGIZET(x)       _T(SGL_STRINGIZE(x))

// ƴ��
#define _SGL_CONCAT(x, y)       x##y
#define SGL_CONCAT(x, y)        _SGL_CONCAT(x, y)

// ���ַ���������
#define _SGL_WIDE(x)            L##x
#define SGL_WIDE(x)             _SGL_WIDE(x)

// ��� TRUE ����
#define SGL_TRUE_DO(x, exp)     if (x) { exp; }
#define SGL_ON_DO(x, exp)       SGL_TRUE_DO((x), exp)

// ��� FALSE ����
#define SGL_FALSE_DO(x, exp)    if (!(x)) { exp; }
#define SGL_OFF_DO(x, exp)      SGL_FALSE_DO((x), exp)

// ����ĳ�º� goto
#define SGL_GOTO(exp, label)    { exp; goto label; }

// HRESULT ����ʧ������
#define SGL_FAILED_DO(hr, exp)  if (FAILED(hr)) { exp; }

// ����ֵ
// 32bit ����ֵ������ x ��Чȡֵ 0 ~ 31, 32bit VC �� SGL_MASK(0) = SGL_MASK(32) = 1
#define SGL_MASK(x)             (1 << (x))

SGL_NS_END
