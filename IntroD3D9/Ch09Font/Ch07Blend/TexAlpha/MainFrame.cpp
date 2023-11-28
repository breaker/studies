// MainFrame.cpp
//

#include "StdAfx.h"
#include "TexAlpha.h"
#include "MainFrame.h"

////////////////////////////////////////////////////////////////////////////////
// ����ܴ����� CMainFrame
////////////////////////////////////////////////////////////////////////////////

// MFC RTTI ֧��
IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

// ��Ϣӳ��
BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
    ON_COMMAND(ID_APP_ABOUT, &CMainFrame::OnAppAbout)
    ON_WM_ACTIVATE()
    ON_WM_CLOSE()
END_MESSAGE_MAP()

// PreCreateWindow ���� CWinApp::Run ִ��
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
    if (!CFrameWnd::PreCreateWindow(cs))
        return FALSE;

    // NOTE:
    // ��Ҫ���������ô��ڴ�С (cs.cx, cs,cy), ��Ϊ��������Ƶģʽ�������ô��ڴ�С
    // MFC �ڲ�������� style ���� WS_CLIPSIBLINGS, ���� CWnd::GetStyle ���û����õĶ� WS_CLIPSIBLINGS
    cs.style &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX);
    cs.lpszClass = AfxRegisterWndClass(0, ::LoadCursor(NULL, IDC_ARROW), NULL, AfxGetApp()->LoadIcon(IDR_MAINFRAME));
    return TRUE;
}

void CMainFrame::OnAppAbout()
{
    CAboutDlg dlg;
    dlg.DoModal();
}

void CMainFrame::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
    CFrameWnd::OnActivate(nState, pWndOther, bMinimized);
    m_GameMain->OnWindowActive(nState, bMinimized);
}

void CMainFrame::OnClose()
{
    m_GameMain->SetPumpMessageType(SGL::Main::PUMPMSG_GET); // ���� GetMessage ��Ϣѭ��, ��Ϊ PeekMessage �������� Game->Debugger ��ѭ��
    CFrameWnd::OnClose();
}

////////////////////////////////////////////////////////////////////////////////
// ���ڶԻ����� CAboutDlg
////////////////////////////////////////////////////////////////////////////////

// ��Ϣӳ��
BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()
