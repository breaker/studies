////////////////////////////////////////////////////////////////////////////////
// ��    �� : D3DXCreate.cpp
//
// ��    �� : 1.0
// ��    �� : Breaker Zhao <breaker.zy_AT_gmail>
// ��    �� : 2010-10
////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "D3DXCreate.h"
#include "RenderD3DXCreate.h"
#include "MainFrame.h"

////////////////////////////////////////////////////////////////////////////////
// ȫ����
////////////////////////////////////////////////////////////////////////////////

CD3DXCreateApp    g_D3DXCreateApp;  // Ӧ�ó���ʵ��

////////////////////////////////////////////////////////////////////////////////
// Ӧ�ó����� CD3DXCreateApp
////////////////////////////////////////////////////////////////////////////////

BOOL CD3DXCreateApp::InitInstance()
{
    CWinApp::InitInstance();

    // ���װ���ļ� (manifest) ��ָ��ʹ�� Windows ͨ�ÿؼ� ComCtl32.dll 6.0+ �汾, ���� Windows XP ����Ҫ���� InitCommonControlsEx(), ���򴰿ڴ�����ʧ��
    // ���ó������ܹ�ʹ�õ�ͨ�ÿؼ���, ICC_WIN95_CLASSES ��ʾ���� Win95 ͨ�ÿؼ�
    INITCOMMONCONTROLSEX initCtrls;
    initCtrls.dwSize = sizeof(initCtrls);
    initCtrls.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&initCtrls);

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
        ERR_TRACEA("std::exception: what: %s, type: %s", e.what(), typeid(e).name());
        return FALSE;
    }
    catch (...) {
        ERR_TRACE("unknown exception");
        return FALSE;
    }

    return TRUE;
}

// NOTE: ���� ExitInstance ǰ, m_pMainWnd ���ڶ����ѱ��ͷŵ���
int CD3DXCreateApp::ExitInstance()
{
    _tchdir(m_OldWorkDir);  // �ָ��ɵĹ���Ŀ¼
    CWinApp::ExitInstance();
    return m_ExitCode;
}

void CD3DXCreateApp::InitLocale()
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
    return (i == -1 ? _T("") : m_ModulePath.Left(i + 1));   // û�ҵ� '\' ʱ, ���� ""
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

    // ��������Ϣת��Ϊ WM_COMMAND ��Ϣ (��ݼ���Ϣת��), ��ͬ��������Ŀ�괰��
    if (!g_D3DXCreateApp.PreTranslateMessage(&msg)) {
        ::TranslateMessage(&msg);   // virtual-key ��Ϣת��Ϊ WM_CHAR ��Ϣ
        ::DispatchMessage(&msg);
    }

    // NOTE: ���� OnIdle ��Լ��: ����Ϣ����Ϊ��ʱ���� OnIdle
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
    SetPumpMsgType(PUMP_PEEK_MSG);  // ʹ�� PeekMessage ��Ϣѭ��

    // ���´�����ʾ
    m_pMainWnd->ShowWindow(m_nCmdShow);
    m_pMainWnd->UpdateWindow();

    SGL::Clock clock;

    // ��Ϣѭ��
    while (m_PumpMsgFunc()) {
        float timeDelta = clock.elapseSec();
        clock.reset();
        m_GameRun->run(timeDelta);  // Game Loop
    }

    // NOTE: ����Լ��Ӧ���� ExitInstance, ���� Run ʱ, ���Զ����� ExitInstance
    return ExitInstance();
}
