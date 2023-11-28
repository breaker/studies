////////////////////////////////////////////////////////////////////////////////
// 文    件 : D3DXCreate.cpp
//
// 版    本 : 1.0
// 作    者 : Breaker Zhao <breaker.zy_AT_gmail>
// 日    期 : 2010-10
////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "D3DXCreate.h"
#include "RenderD3DXCreate.h"
#include "MainFrame.h"

////////////////////////////////////////////////////////////////////////////////
// 全局量
////////////////////////////////////////////////////////////////////////////////

CD3DXCreateApp    g_D3DXCreateApp;  // 应用程序实例

////////////////////////////////////////////////////////////////////////////////
// 应用程序类 CD3DXCreateApp
////////////////////////////////////////////////////////////////////////////////

BOOL CD3DXCreateApp::InitInstance()
{
    CWinApp::InitInstance();

    // 如果装配文件 (manifest) 中指定使用 Windows 通用控件 ComCtl32.dll 6.0+ 版本, 则在 Windows XP 下需要调用 InitCommonControlsEx(), 否则窗口创建将失败
    // 设置程序中能够使用的通用控件类, ICC_WIN95_CLASSES 表示所有 Win95 通用控件
    INITCOMMONCONTROLSEX initCtrls;
    initCtrls.dwSize = sizeof(initCtrls);
    initCtrls.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&initCtrls);

    InitLocale();

    // 计算主模块全路径, 设置工作目录
    InitModulePath(MAX_PATH);
    InitWorkDir(GetModuleDir());

    // NOTE:
    // 保存应用设置选项的注册表键, 通常命名为开发公司, 组织, 团队的名字
    // 该函数设定 m_pszRegistryKey, 并影响 GetProfileInt 等函数
    // 保存应用设置的注册表键:
    // HKCU\Software\<company name>\<application name>\<section name>\<value name>
    // 当不使用注册表而使用 ini 文件时, 请去掉 SetRegistryKey
    SetRegistryKey(_T(MODULE_NAME));

    try {
        CMainFrame* pFrmWnd = new CMainFrame();

        // 由共享资源 IDR_MAINFRAME 创建主框架窗口, IDR_MAINFRAME 可同时标识: 菜单(必需), 快捷键, 主图标, 标题栏字符串
        pFrmWnd->LoadFrame(IDR_MAINFRAME);

        // NOTE: 在 Run 中显示窗口
        pFrmWnd->ShowWindow(SW_HIDE);

        m_pMainWnd = pFrmWnd;
    }
    catch (std::exception& e) {
        ERR_TRACEA("std::exception: what: %s, type: %s", e.what(), typeid(e).name());
        return FALSE;
    }
    catch (...) {
        ERR_TRACE("unknown exception");
        return FALSE;
    }

    return TRUE;
}

// NOTE: 进入 ExitInstance 前, m_pMainWnd 窗口对象已被释放掉了
int CD3DXCreateApp::ExitInstance()
{
    _tchdir(m_OldWorkDir);  // 恢复旧的工作目录
    CWinApp::ExitInstance();
    return m_ExitCode;
}

void CD3DXCreateApp::InitLocale()
{
#define _USE_CRT_LOCALE

#if _MSC_VER > 1400
#define _USE_IOS_LOCALE
#endif

// CRT 全局 locale 和 iostream imbue locale 冲突
// VC 2005 问题 (_MSC_VER = 1400)
// VC 2010 正常 (_MSC_VER = 1600)
#if _MSC_VER <= 1400 && defined(_USE_CRT_LOCALE) && defined(_USE_IOS_LOCALE)
#error cannot use CRT global locale and iostream imbue locale at the same time, when _MSC_VER <= 1400
#endif

#ifdef _USE_CRT_LOCALE
    _tsetlocale(LC_ALL, _T(""));
#endif

#ifdef _USE_IOS_LOCALE
    // 设置 C++ iostream 标准 IO 流 locale
    std::locale loc(std::locale(""), std::locale::classic(), std::locale::numeric);
    std::cout.imbue(loc);
    std::cerr.imbue(loc);
    std::cin.imbue(loc);
#endif
}

BOOL CD3DXCreateApp::InitModulePath(DWORD size)
{
    TCHAR* buf = new TCHAR[size];
    DWORD ret = GetModuleFileName(m_hInstance, buf, size);
    if (ret == 0) {
        ERR_TRACE("GetModuleFileName failed: %lu", GetLastError());
        delete[] buf;
        return FALSE;
    }
    m_ModulePath = buf;
    delete[] buf;
    return TRUE;
}

BOOL CD3DXCreateApp::InitWorkDir(const _TCHAR* dir)
{
    _TCHAR* oldDir = _tgetcwd(0, 0);
    if (oldDir == 0)
        return FALSE;
    m_OldWorkDir = oldDir;
    free(oldDir);
    return (_tchdir(dir) == 0);
}

CString CD3DXCreateApp::GetModuleDir() const
{
    int i = m_ModulePath.ReverseFind(_T('\\'));
    return (i == -1 ? _T("") : m_ModulePath.Left(i + 1));   // 没找到 '\' 时, 返回 ""
}

void CD3DXCreateApp::SetPumpMsgType(PUMP_MSG_TYPE type)
{
    if (type == PUMP_GET_MSG)
        m_PumpMsgFunc = GetMsg;
    else if (type == PUMP_PEEK_MSG)
        m_PumpMsgFunc = PeekMsg;
}

BOOL CD3DXCreateApp::GetMsg()
{
    MSG msg;
    DWORD ret = ::GetMessage(&msg, NULL, 0, 0);
    if (ret == 0 || ret == -1) {
        g_D3DXCreateApp.m_ExitCode = (int) msg.wParam;
        return FALSE;
    }

    // 将按键消息转换为 WM_COMMAND 消息 (快捷键消息转换), 并同步发送至目标窗口
    if (!g_D3DXCreateApp.PreTranslateMessage(&msg)) {
        ::TranslateMessage(&msg);   // virtual-key 消息转换为 WM_CHAR 消息
        ::DispatchMessage(&msg);
    }

    // NOTE: 按照 OnIdle 的约定: 当消息队列为空时处理 OnIdle
    if (!::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
        LONG cnt = 0;
        while (g_D3DXCreateApp.OnIdle(cnt++));
    }
    return TRUE;
}

BOOL CD3DXCreateApp::PeekMsg()
{
    MSG msg;
    if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            g_D3DXCreateApp.m_ExitCode = (int) msg.wParam;
            return FALSE;
        }

        if (!g_D3DXCreateApp.PreTranslateMessage(&msg)) {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
    }
    else {
        LONG cnt = 0;
        while (g_D3DXCreateApp.OnIdle(cnt++));
    }
    return TRUE;
}

int CD3DXCreateApp::Run()
{
    try {
        m_GameRun.reset(new SGLGameRun(new RenderD3DXCreate(DEF_WIDTH, DEF_HEIGHT, m_pMainWnd->GetSafeHwnd(), TRUE, D3DDEVTYPE_HAL)));
    }
    catch (SGLExcept& e) {
        ERR_TRACEA("SGLExcept: what: %s, type: %s", e.what(), typeid(e).name());
        return -1;
    }

    CMainFrame* pFrmWnd = (CMainFrame*) m_pMainWnd;
    pFrmWnd->SetGameRun(m_GameRun.get());
    pFrmWnd->AdjustWindowed();
    SetPumpMsgType(PUMP_PEEK_MSG);  // 使用 PeekMessage 消息循环

    // 更新窗口显示
    m_pMainWnd->ShowWindow(m_nCmdShow);
    m_pMainWnd->UpdateWindow();

    SGL::Clock clock;

    // 消息循环
    while (m_PumpMsgFunc()) {
        float timeDelta = clock.elapseSec();
        clock.reset();
        m_GameRun->run(timeDelta);  // Game Loop
    }

    // NOTE: 按照约定应调用 ExitInstance, 重载 Run 时, 不自动调用 ExitInstance
    return ExitInstance();
}
