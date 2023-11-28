////////////////////////////////////////////////////////////////////////////////
// 文    件 : Cube.h
// 功能说明 :
//   程序的入口和初始化
//
// 版    本 : 1.0
// 作    者 : Breaker Zhao <breaker.zy_AT_gmail>
// 日    期 : 2010-10
////////////////////////////////////////////////////////////////////////////////

#ifndef __CUBE_H__
#define __CUBE_H__

// 资源 ID 符号常量
#include "res/Resource.h"

////////////////////////////////////////////////////////////////////////////////
// 全局量
////////////////////////////////////////////////////////////////////////////////

#undef MODULE_NAME
#define MODULE_NAME     "Cube"      // 程序模块名

#define BUF_SIZE        256             // 常用缓冲区大小

class CCubeApp;
extern CCubeApp     g_CubeApp;  // 应用程序实例

////////////////////////////////////////////////////////////////////////////////
// 名字空间
////////////////////////////////////////////////////////////////////////////////

namespace SGL {
    class SGLExcept;
    class SGLRender;
    class SGLGameRun;
}
using SGL::SGLExcept;
using SGL::SGLRender;
using SGL::SGLGameRun;

using std::auto_ptr;

////////////////////////////////////////////////////////////////////////////////
// 调试功能
////////////////////////////////////////////////////////////////////////////////

// Debug 版 new
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 以 "模块名!函数名:" 形式输出调试报告
#define FMT(fmt)    _T(MODULE_NAME) _T("!") _T(__FUNCTION__) _T(": ")  _T(fmt) _T("\n")
#define FMTA(fmt)   MODULE_NAME "!" __FUNCTION__ ": " fmt "\n"

#define ERR_TRACE(fmt, ...)     TRACE(FMT(fmt), __VA_ARGS__)
#define ERR_TRACEA(fmt, ...)    TRACE(FMTA(fmt), __VA_ARGS__)

////////////////////////////////////////////////////////////////////////////////
// 应用程序类 CCubeApp
////////////////////////////////////////////////////////////////////////////////

class CCubeApp : public CWinApp {
public:
    CCubeApp() : m_GameRun(NULL), m_PumpMsgFunc(NULL), m_ExitCode(0) {}

public:
    virtual BOOL InitInstance();
    virtual int ExitInstance();
    virtual int Run();

public:
    // 得到主程序模块全路径
    const CString& GetModulePath() const {
        return m_ModulePath;
    }

    // 取得主程序所在目录全路径
    CString GetModuleDir() const;

    SGLGameRun* GetGameRun() const {
        return m_GameRun.get();
    }

    enum PUMP_MSG_TYPE {
        PUMP_GET_MSG,
        PUMP_PEEK_MSG
    };
    void SetPumpMsgType(PUMP_MSG_TYPE type);

private:
    void InitLocale();
    BOOL InitModulePath(DWORD size);    // 计算主程序模块的全路径 m_ModulePath
    BOOL InitWorkDir(const _TCHAR* dir);// 设置工作目录

    // PumpMsgFunc 函数约定: 返回 FALSE 表示消息循环结束
    typedef BOOL (*PumpMsgFunc)();
    static BOOL GetMsg();
    static BOOL PeekMsg();

private:
    CString     m_ModulePath;   // 主程序模块的全路径
    CString     m_OldWorkDir;   // 旧的工作目录
    int         m_ExitCode;     // 程序结束返回码

    auto_ptr<SGLGameRun>    m_GameRun;
    PumpMsgFunc m_PumpMsgFunc;

    static const UINT   DEF_WIDTH   = 640;
    static const UINT   DEF_HEIGHT  = 480;
};

#endif  // __CUBE_H__
