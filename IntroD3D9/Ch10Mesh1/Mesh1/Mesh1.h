// Mesh1.h
//

#ifndef __MESH1_H__
#define __MESH1_H__

// 资源 ID 符号常量
#include "res/Resource.h"

////////////////////////////////////////////////////////////////////////////////
// 全局量
////////////////////////////////////////////////////////////////////////////////

#undef MODULE_NAME
#define MODULE_NAME     "Mesh1"     // 程序模块名

#define BUF_SIZE        256         // 常用缓冲区大小

class CMesh1App;
extern CMesh1App        Mesh1App;   // 应用程序实例

////////////////////////////////////////////////////////////////////////////////
// 调试功能
////////////////////////////////////////////////////////////////////////////////

// Debug 版 new
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 以 "模块名!函数名:" 形式输出调试报告
#define FMT(fmt)                    _T(MODULE_NAME) _T("!") _T(__FUNCTION__) _T(": ")  _T(fmt) _T("\n")
#define FMTA(fmt)                   MODULE_NAME "!" __FUNCTION__ ": " fmt "\n"

#define MYTRACE(fmt, ...)           TRACE(FMT(fmt), __VA_ARGS__)
#define MYTRACEA(fmt, ...)          TRACE(FMTA(fmt), __VA_ARGS__)

#define MYTRACE_DX(func, err)       MYTRACE("%s failed: %ld: %s", _T(func), err, DXGetErrorDescription(err))
#define MYTRACE_DXA(func, err)      MYTRACEA("%s failed: %ld: %s", func, err, DXGetErrorDescriptionA(err))

////////////////////////////////////////////////////////////////////////////////
// 应用程序类 CMesh1App
////////////////////////////////////////////////////////////////////////////////

class CMesh1App : public CWinApp {
public:
    CMesh1App() {}

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

#endif  // __MESH1_H__
