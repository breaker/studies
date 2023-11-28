////////////////////////////////////////////////////////////////////////////////
// ��    �� : MainFrame.cpp
// ����˵�� :
//   ����ܴ���
//
// ��    �� : 1.0
// ��    �� : Breaker Zhao <breaker.zy_AT_gmail>
// ��    �� : 2010-10
////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "D3DXCreate.h"
#include "MainFrame.h"

////////////////////////////////////////////////////////////////////////////////
// ����ܴ����� CMainFrame
////////////////////////////////////////////////////////////////////////////////

// MFC RTTI ֧��
IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

// ��Ϣӳ��
BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
    ON_COMMAND(ID_APP_ABOUT, &CMainFrame::OnAppAbout)
    ON_WM_CLOSE()
    ON_WM_KEYDOWN()
    ON_WM_ACTIVATE()
END_MESSAGE_MAP()

// PreCreateWindow ���� CWinApp::Run ִ��
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
    if (!CFrameWnd::PreCreateWindow(cs))
        return FALSE;

    // NOTE: ��Ҫ���������ô��ڴ�С (cs.cx, cs,cy), ��Ϊ��������Ƶģʽ�������ô��ڴ�С

    cs.style &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX);
    cs.lpszClass = AfxRegisterWndClass(0, ::LoadCursor(NULL, IDC_ARROW), NULL, AfxGetApp()->LoadIcon(IDR_MAINFRAME));

    m_Style = cs.style;
    m_Menu = cs.hMenu;
    return TRUE;
}

void CMainFrame::OnAppAbout()
{
    CAboutDlg dlg;
    dlg.DoModal();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
    return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CMainFrame::OnClose()
{
    ((CD3DXCreateApp*) AfxGetApp())->GetGameRun()->stop();
    CFrameWnd::OnClose();
}

void CMainFrame::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    switch (nChar) {
    case VK_F4:
        return;

    case VK_ESCAPE:
        OnClose();
        return;
    }

    CFrameWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CMainFrame::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
    CFrameWnd::OnActivate(nState, pWndOther, bMinimized);

    // NOTE:
    // Ӧͬʱ��� Inactive �� Minimized ״̬, ��Ϊ�� Minimized Active ״̬
    // Minimized Active: ���������������ťʱ, ��������״̬ (��С�� + ����)
    // Minimized Active �Ⱥ�������� WM_ACTIVATE ��Ϣ: 1. WA_INACTIVE 2. WA_ACTIVE + Minimized
    // �������С����ťֻ����һ�� WA_INACTIVE ��Ϣ
    if (nState == WA_INACTIVE || bMinimized) {
        m_GameRun->stop();
        g_D3DXCreateApp.SetPumpMsgType(CD3DXCreateApp::PUMP_GET_MSG);
    }
    else {
        m_GameRun->resume();
        g_D3DXCreateApp.SetPumpMsgType(CD3DXCreateApp::PUMP_PEEK_MSG);
    }
}

void CMainFrame::AdjustWindowed()
{
    ShowWindow(SW_HIDE);

    ::SetWindowLongPtr(GetSafeHwnd(), GWL_STYLE, m_Style);
    ::SetMenu(GetSafeHwnd(), m_Menu);

    SGLRender* render = m_GameRun->getRender();
    CRect rect(0, 0, render->getWidth(), render->getHeight());

    // NOTE: MFC �ڲ�������� style ���� WS_CLIPSIBLINGS, ���� CWnd::GetStyle �� m_Style �� WS_CLIPSIBLINGS
    AdjustWindowRectEx(&rect, GetStyle(), GetMenu() != NULL, GetExStyle());
    // NOTE: ��� rePaint = FALSE, ���ȫ����Ϊ����ʱ, һ���ֻ����������Ļ��
    MoveWindow(&rect, TRUE);
    CenterWindow();

    ShowWindow(SW_SHOW);
}

void CMainFrame::SetGameRun(SGLGameRun* gamerun)
{
    ASSERT(gamerun != NULL);
    m_GameRun = gamerun;
}

void CMainFrame::AdjustExclusive()
{
    ShowWindow(SW_HIDE);
    ::SetWindowLongPtr(GetSafeHwnd(), GWL_STYLE, WS_POPUP);
    SetMenu(NULL);
    ShowWindow(SW_SHOW);
}

CMainFrame* CMainFrame::ReCreate()
{
    CWnd* pOldWnd = AfxGetApp()->m_pMainWnd;
    pOldWnd->ShowWindow(SW_HIDE);

    CMainFrame* pFrmWnd = new CMainFrame();
    pFrmWnd->LoadFrame(IDR_MAINFRAME);
    pFrmWnd->ShowWindow(SW_HIDE);
    AfxGetApp()->m_pMainWnd = pFrmWnd;

    // CAUTION: ��������� this ����, ���� ReCreate ��, ��Ҫ���þɶ���ķ���, ��Ӧʹ�� ReCreate ���ص��¶�����÷���
    // CWnd::DestroyWindow �� delete pOldWnd ����ͬ���� (MFC �ڲ�ɾ������), ��Ҫ�ظ�����
    pOldWnd->DestroyWindow();

    return pFrmWnd;
}

////////////////////////////////////////////////////////////////////////////////
// ���ڶԻ����� CAboutDlg
////////////////////////////////////////////////////////////////////////////////

// ��Ϣӳ��
BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()
