// Defs.h
//

#pragma once

////////////////////////////////////////////////////////////////////////////////
// SGL 名字空间
////////////////////////////////////////////////////////////////////////////////

#define SGL_NS_BEGIN    namespace SGL {
#define SGL_NS_END      }

SGL_NS_BEGIN

////////////////////////////////////////////////////////////////////////////////
// SGL 版本
////////////////////////////////////////////////////////////////////////////////

#define SGL_VER         0x01000001
#define SGL_VER_STR     "1.0.0.1"

////////////////////////////////////////////////////////////////////////////////
// DLL 导入导出
////////////////////////////////////////////////////////////////////////////////

#ifdef _SGL_EXPORTS
#define SGL_API __declspec(dllexport)
#else
#define SGL_API __declspec(dllimport)
#endif

#ifdef _SGL_WARNING_OFF_EXPORTS
#pragma warning(disable: 4275)  // 禁止警告: 导出类从非导出类派生
#pragma warning(disable: 4251)  // 禁止警告: 需要导出类
#endif

// 只产生一个全局存储, 以便在 .h 中定义全局变量
#define SGL_GLOBAL __declspec(selectany)

// SGL 模块名
const char      MODULE_NAMEA[]  = "SGL";
const wchar_t   MODULE_NAMEW[]  = L"SGL";
const _TCHAR    MODULE_NAME[]   = _T("SGL");

////////////////////////////////////////////////////////////////////////////////
// 辅助功能
////////////////////////////////////////////////////////////////////////////////

// 字符串化字面量
#define _SGL_STRINGIZE(x)       #x
#define SGL_STRINGIZE(x)        _SGL_STRINGIZE(x)
#define SGL_STRINGIZET(x)       _T(SGL_STRINGIZE(x))

// 拼接
#define _SGL_CONCAT(x, y)       x##y
#define SGL_CONCAT(x, y)        _SGL_CONCAT(x, y)

// 宽化字符串字面量
#define _SGL_WIDE(x)            L##x
#define SGL_WIDE(x)             _SGL_WIDE(x)

// 如果 TRUE 则做
#define SGL_TRUE_DO(x, exp)     if (x) { exp; }
#define SGL_ON_DO(x, exp)       SGL_TRUE_DO((x), exp)

// 如果 FALSE 则做
#define SGL_FALSE_DO(x, exp)    if (!(x)) { exp; }
#define SGL_OFF_DO(x, exp)      SGL_FALSE_DO((x), exp)

// 做完某事后 goto
#define SGL_GOTO(exp, label)    { exp; goto label; }

// HRESULT 测试失败则做
#define SGL_FAILED_DO(hr, exp)  if (FAILED(hr)) { exp; }

// 掩码值
// 32bit 掩码值的整数 x 有效取值 0 ~ 31, 32bit VC 中 SGL_MASK(0) = SGL_MASK(32) = 1
#define SGL_MASK(x)             (1 << (x))

SGL_NS_END
