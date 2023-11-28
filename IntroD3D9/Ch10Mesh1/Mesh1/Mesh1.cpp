// Mesh1.cpp
//

#include "StdAfx.h"
#include "Mesh1.h"
#include "RenderMesh1.h"
#include "MainFrm.h"

////////////////////////////////////////////////////////////////////////////////
// 名字空间
////////////////////////////////////////////////////////////////////////////////

using std::auto_ptr;

////////////////////////////////////////////////////////////////////////////////
// 全局量
////////////////////////////////////////////////////////////////////////////////

CMesh1App Mesh1App; // 应用程序实例

////////////////////////////////////////////////////////////////////////////////
// 应用程序类 CMesh1App
////////////////////////////////////////////////////////////////////////////////

BOOL CMesh1App::InitInstance()
{
    CWinApp::InitInstance();

    // 如果装配文件 (manifest) 中指定使用 Windows 通用控件 ComCtl32.dll 6.0+ 版本, 则在 Windows XP 下需要调用 InitCommonControlsEx(), 否则窗口创建将失败
    // 设置程序中能够使用的通用控件类, ICC_WIN95_CLASSES 表示所有 Win95 通用控件
    INITCOMMONCONTROLSEX initCtrls;
    initCtrls.dwSize = sizeof(initCtrls);
    initCtrls.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&initCtrls);

    // 设置 CRT 调试输出
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_DEBUG);

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
        CMainFrame* frmWnd = new CMainFrame();
        frmWnd->LoadFrame(IDR_MAINFRAME);   // 由共享资源 IDR_MAINFRAME 创建主框架窗口, IDR_MAINFRAME 可同时标识: 菜单(必需), 快捷键, 主图标, 标题栏字符串
        frmWnd->ShowWindow(SW_HIDE);        // 在 Run 中显示窗口
        m_pMainWnd = frmWnd;
    }
    catch (std::exception& e) {
        MYTRACEA("std::exception: what: %s, type: %s", e.what(), typeid(e).name());
        return FALSE;
    }
    catch (...) {
        MYTRACE("unknown exception");
        return FALSE;
    }

    return TRUE;
}

int CMesh1App::Exit(int exitCode)
{
    // 如果正常退出 (非失败), 则 ExitInstance 之前 m_pMainWnd 已被释放掉了
    if (m_pMainWnd != NULL) {
        delete m_pMainWnd;
        m_pMainWnd = NULL;
    }

    _tchdir(m_OldWorkDir);  // 恢复旧的工作目录
    CWinApp::ExitInstance();
    return exitCode;
}

void CMesh1App::InitLocale()
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

BOOL CMesh1App::InitModulePath(DWORD size)
{
    TCHAR* buf = new TCHAR[size];
    DWORD ret = GetModuleFileName(m_hInstance, buf, size);
    if (ret == 0) {
        MYTRACE("GetModuleFileName failed: %lu", GetLastError());
        delete[] buf;
        return FALSE;
    }
    m_ModulePath = buf;
    delete[] buf;
    return TRUE;
}

BOOL CMesh1App::InitWorkDir(const _TCHAR* dir)
{
    _TCHAR* oldDir = _tgetcwd(0, 0);
    if (oldDir == 0)
        return FALSE;
    m_OldWorkDir = oldDir;
    free(oldDir);
    return (_tchdir(dir) == 0);
}

CString CMesh1App::GetModuleDir() const
{
    int i = m_ModulePath.ReverseFind(_T('\\'));
    return (i == -1 ? _T("") : m_ModulePath.Left(i + 1));   // 没找到 '\' 时, 返回 ""
}

int CMesh1App::Run()
{
    HWND hwnd = m_pMainWnd->GetSafeHwnd();
    BOOL windowed = TRUE;
    auto_ptr<RenderMesh1> render(new RenderMesh1());
    if (!render->Init(WIN_WIDTH, WIN_HEIGHT, hwnd, windowed, D3DDEVTYPE_HAL)) {
        m_pMainWnd->MessageBox(_T("Initialize rendering failed."), _T("Error"), MB_OK | MB_ICONERROR);
        return Exit(-1);
    }

    auto_ptr<Mesh1Input> input(new Mesh1Input());
    m_GameMain.reset(new SGL::Main());
    m_GameMain->Init(render.get(), input.get());

    // 调整窗口大小
    CMainFrame* pFrmWnd = (CMainFrame*) m_pMainWnd;
    pFrmWnd->SetGameMain(m_GameMain.get());
    if (windowed)
        m_GameMain->AdjustWindowed(pFrmWnd->GetStyle(), pFrmWnd->GetExStyle(), pFrmWnd->GetMenu()->GetSafeHmenu());
    else
        m_GameMain->AdjustFullscreen();

    // 更新窗口显示
    m_pMainWnd->ShowWindow(m_nCmdShow);
    m_pMainWnd->UpdateWindow();

    // 消息循环
    int exitCode = m_GameMain->StartLoop(SGL::MFCIdle, SGL::MFCPreTransMessage);

    // 按照约定重载 Run 应手工调用 ExitInstance
    return Exit(exitCode);
}
