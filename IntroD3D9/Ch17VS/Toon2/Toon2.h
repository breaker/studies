// Toon2.h
//

#pragma once

// 资源 ID 符号常量
#include "res/Resource.h"

////////////////////////////////////////////////////////////////////////////////
// 全局量
////////////////////////////////////////////////////////////////////////////////

// 程序模块名
const char      MODULE_NAMEA[]  = "Toon2";
const wchar_t   MODULE_NAMEW[]  = L"Toon2";
const _TCHAR    MODULE_NAME[]   = _T("Toon2");

// 常用缓冲区大小
const int BUF_SIZE = 256;

// 应用程序实例
class CToon2App;
extern CToon2App Toon2App;

////////////////////////////////////////////////////////////////////////////////
// 调试功能
////////////////////////////////////////////////////////////////////////////////

// Debug 版 new
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 以 "模块名!函数名:" 形式输出调试报告
#define FMT(fmt)    SGL_RPTFMT(fmt), MODULE_NAME
#define FMTA(fmt)   SGL_RPTFMTA(fmt), MODULE_NAMEA

#define MYTRACE(fmt, ...)       TRACE(FMT(fmt), __VA_ARGS__)
#define MYTRACEA(fmt, ...)      TRACE(FMTA(fmt), __VA_ARGS__)

#define MYTRACE_DX(func, err)   MYTRACE("%s failed %ld: %s", _T(func), err, DXGetErrorDescription(err))
#define MYTRACE_DXA(func, err)  MYTRACEA("%s failed %ld: %s", func, err, DXGetErrorDescriptionA(err))

////////////////////////////////////////////////////////////////////////////////
// 应用程序类 CToon2App
////////////////////////////////////////////////////////////////////////////////

class CToon2App : public CWinApp {
public:
    CToon2App() {}

public:
    virtual BOOL InitInstance();
    virtual int Run();
    int Exit(int exitCode);

public:
    // 得到主程序模块全路径
    const CString& GetModulePath() const {
        return m_ModulePath;
    }

    // 取得主程序所在目录全路径
    CString GetModuleDir() const;

    enum WIN_SIZE {
        WIN_WIDTH   = 640,
        WIN_HEIGHT  = 480
    };

private:
    void InitLocale();
    BOOL InitModulePath(DWORD size);        // 计算主程序模块的全路径 m_ModulePath
    BOOL InitWorkDir(const _TCHAR* dir);    // 设置工作目录

private:
    CString     m_ModulePath;   // 主程序模块的全路径
    CString     m_OldWorkDir;   // 旧的工作目录
    std::auto_ptr<SGL::Main>    m_GameMain;
};
