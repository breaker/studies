// Shadow.cpp
//

#include "StdAfx.h"
#include "Shadow.h"
#include "RenderShadow.h"
#include "MainFrame.h"

////////////////////////////////////////////////////////////////////////////////
// ���ֿռ�
////////////////////////////////////////////////////////////////////////////////

using std::auto_ptr;

////////////////////////////////////////////////////////////////////////////////
// ȫ����
////////////////////////////////////////////////////////////////////////////////

CShadowApp ShadowApp;   // Ӧ�ó���ʵ��

////////////////////////////////////////////////////////////////////////////////
// Ӧ�ó����� CShadowApp
////////////////////////////////////////////////////////////////////////////////

BOOL CShadowApp::InitInstance()
{
    CWinApp::InitInstance();

    // ���װ���ļ� (manifest) ��ָ��ʹ�� Windows ͨ�ÿؼ� ComCtl32.dll 6.0+ �汾, ���� Windows XP ����Ҫ���� InitCommonControlsEx(), ���򴰿ڴ�����ʧ��
    // ���ó������ܹ�ʹ�õ�ͨ�ÿؼ���, ICC_WIN95_CLASSES ��ʾ���� Win95 ͨ�ÿؼ�
    INITCOMMONCONTROLSEX initCtrls;
    initCtrls.dwSize = sizeof(initCtrls);
    initCtrls.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&initCtrls);

    // ���� CRT �������
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_DEBUG);

    InitLocale();

    // ������ģ��ȫ·��, ���ù���Ŀ¼
    InitModulePath(MAX_PATH);
    InitWorkDir(GetModuleDir());

    // NOTE:
    // ����Ӧ������ѡ���ע����, ͨ������Ϊ������˾, ��֯, �Ŷӵ�����
    // �ú����趨 m_pszRegistryKey, ��Ӱ�� GetProfileInt �Ⱥ���
    // ����Ӧ�����õ�ע����:
    // HKCU\Software\<company name>\<application name>\<section name>\<value name>
    // ����ʹ��ע����ʹ�� ini �ļ�ʱ, ��ȥ�� SetRegistryKey
    SetRegistryKey(_T(MODULE_NAME));

    try {
        CMainFrame* pFrmWnd = new CMainFrame();

        // �ɹ�����Դ IDR_MAINFRAME ��������ܴ���, IDR_MAINFRAME ��ͬʱ��ʶ: �˵�(����), ��ݼ�, ��ͼ��, �������ַ���
        pFrmWnd->LoadFrame(IDR_MAINFRAME);

        // NOTE: �� Run ����ʾ����
        pFrmWnd->ShowWindow(SW_HIDE);

        m_pMainWnd = pFrmWnd;
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

// NOTE: ���� ExitInstance ǰ, m_pMainWnd ���ڶ����ѱ��ͷŵ���
int CShadowApp::Exit(int exitCode)
{
    _tchdir(m_OldWorkDir);  // �ָ��ɵĹ���Ŀ¼
    CWinApp::ExitInstance();
    return exitCode;
}

void CShadowApp::InitLocale()
{
#define _USE_CRT_LOCALE

#if _MSC_VER > 1400
#define _USE_IOS_LOCALE
#endif

// CRT ȫ�� locale �� iostream imbue locale ��ͻ
// VC 2005 ���� (_MSC_VER = 1400)
// VC 2010 ���� (_MSC_VER = 1600)
#if _MSC_VER <= 1400 && defined(_USE_CRT_LOCALE) && defined(_USE_IOS_LOCALE)
#error cannot use CRT global locale and iostream imbue locale at the same time, when _MSC_VER <= 1400
#endif

#ifdef _USE_CRT_LOCALE
    _tsetlocale(LC_ALL, _T(""));
#endif

#ifdef _USE_IOS_LOCALE
    // ���� C++ iostream ��׼ IO �� locale
    std::locale loc(std::locale(""), std::locale::classic(), std::locale::numeric);
    std::cout.imbue(loc);
    std::cerr.imbue(loc);
    std::cin.imbue(loc);
#endif
}

BOOL CShadowApp::InitModulePath(DWORD size)
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

BOOL CShadowApp::InitWorkDir(const _TCHAR* dir)
{
    _TCHAR* oldDir = _tgetcwd(0, 0);
    if (oldDir == 0)
        return FALSE;
    m_OldWorkDir = oldDir;
    free(oldDir);
    return (_tchdir(dir) == 0);
}

CString CShadowApp::GetModuleDir() const
{
    int i = m_ModulePath.ReverseFind(_T('\\'));
    return (i == -1 ? _T("") : m_ModulePath.Left(i + 1));   // û�ҵ� '\' ʱ, ���� ""
}

int CShadowApp::Run()
{
    HWND hwnd = m_pMainWnd->GetSafeHwnd();
    BOOL windowed = TRUE;
    auto_ptr<RenderShadow> render(new RenderShadow());
    if (!render->Init(WIN_WIDTH, WIN_HEIGHT, hwnd, windowed, D3DDEVTYPE_HAL))
        return Exit(-1);

    auto_ptr<ShadowInput> input(new ShadowInput());
    m_GameMain.reset(new SGL::Main());
    m_GameMain->Init(render.get(), input.get());

    // �������ڴ�С
    CMainFrame* pFrmWnd = (CMainFrame*) m_pMainWnd;
    pFrmWnd->SetGameMain(m_GameMain.get());
    if (windowed)
        m_GameMain->AdjustWindowed(pFrmWnd->GetStyle(), pFrmWnd->GetExStyle(), pFrmWnd->GetMenu()->GetSafeHmenu());
    else
        m_GameMain->AdjustFullscreen();

    // ���´�����ʾ
    m_pMainWnd->ShowWindow(m_nCmdShow);
    m_pMainWnd->UpdateWindow();

    // ��Ϣѭ��
    int exitCode = 0;
    exitCode = m_GameMain->StartLoop(SGL::MFCIdle, SGL::MFCPreTransMessage);

    // NOTE: ����Լ��Ӧ���� ExitInstance, ���� Run ʱ, ���Զ����� ExitInstance
    return Exit(exitCode);
}
