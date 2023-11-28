// Mesh1.cpp
//

#include "StdAfx.h"
#include "Mesh1.h"
#include "RenderMesh1.h"
#include "MainFrm.h"

////////////////////////////////////////////////////////////////////////////////
// ���ֿռ�
////////////////////////////////////////////////////////////////////////////////

using std::auto_ptr;

////////////////////////////////////////////////////////////////////////////////
// ȫ����
////////////////////////////////////////////////////////////////////////////////

CMesh1App Mesh1App; // Ӧ�ó���ʵ��

////////////////////////////////////////////////////////////////////////////////
// Ӧ�ó����� CMesh1App
////////////////////////////////////////////////////////////////////////////////

BOOL CMesh1App::InitInstance()
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
        CMainFrame* frmWnd = new CMainFrame();
        frmWnd->LoadFrame(IDR_MAINFRAME);   // �ɹ�����Դ IDR_MAINFRAME ��������ܴ���, IDR_MAINFRAME ��ͬʱ��ʶ: �˵�(����), ��ݼ�, ��ͼ��, �������ַ���
        frmWnd->ShowWindow(SW_HIDE);        // �� Run ����ʾ����
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
    // ��������˳� (��ʧ��), �� ExitInstance ֮ǰ m_pMainWnd �ѱ��ͷŵ���
    if (m_pMainWnd != NULL) {
        delete m_pMainWnd;
        m_pMainWnd = NULL;
    }

    _tchdir(m_OldWorkDir);  // �ָ��ɵĹ���Ŀ¼
    CWinApp::ExitInstance();
    return exitCode;
}

void CMesh1App::InitLocale()
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
    return (i == -1 ? _T("") : m_ModulePath.Left(i + 1));   // û�ҵ� '\' ʱ, ���� ""
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
    int exitCode = m_GameMain->StartLoop(SGL::MFCIdle, SGL::MFCPreTransMessage);

    // ����Լ������ Run Ӧ�ֹ����� ExitInstance
    return Exit(exitCode);
}
