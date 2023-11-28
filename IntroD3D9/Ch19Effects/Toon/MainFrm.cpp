// MainFrm.cpp
//

#include "StdAfx.h"
#include "Toon.h"
#include "MainFrm.h"

#include "RenderToon.h"

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
    ON_COMMAND(ID_FILE_OPEN, &CMainFrame::OnFileOpen)
END_MESSAGE_MAP()

// PreCreateWindow ���� CWinApp::Run ִ��
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs) {
    if (!CFrameWnd::PreCreateWindow(cs))
        return FALSE;

    // NOTE:
    // ��Ҫ���������ô��ڴ�С (cs.cx, cs,cy), ��Ϊ��������Ƶģʽ�������ô��ڴ�С
    // MFC �ڲ�������� style ���� WS_CLIPSIBLINGS, ���� CWnd::GetStyle ���û����õĶ� WS_CLIPSIBLINGS
    cs.style &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX);
    cs.lpszClass = AfxRegisterWndClass(0, ::LoadCursor(NULL, IDC_ARROW), NULL, AfxGetApp()->LoadIcon(IDR_MAINFRAME));
    return TRUE;
}

void CMainFrame::OnAppAbout() {
    CAboutDlg dlg;
    dlg.DoModal();
}

void CMainFrame::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) {
    CFrameWnd::OnActivate(nState, pWndOther, bMinimized);
    SGL_TRUE_DO(m_GameMain != NULL, m_GameMain->OnWindowActive(nState, bMinimized));
}

void CMainFrame::OnClose() {
    m_GameMain->SetPumpMessageType(SGL::Main::PUMPMSG_GET); // ���� GetMessage ��Ϣѭ��, ��Ϊ PeekMessage �������� Game->Debugger ��ѭ��
    CFrameWnd::OnClose();
}

void CMainFrame::OnFileOpen() {
    CString filter;
    filter.LoadString(IDS_OPEN_FILE_FILTER);

    CFileDialog fileDlg(TRUE, 0, 0, OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, filter, this, 0);
    if (fileDlg.DoModal() == IDOK && m_GameMain != NULL) {
        MYTRACE("X file path: %s", (LPCTSTR) fileDlg.GetPathName());
        RenderToon* render = (RenderToon*) m_GameMain->GetRender();
    }
}

////////////////////////////////////////////////////////////////////////////////
// ���ڶԻ����� CAboutDlg
////////////////////////////////////////////////////////////////////////////////

// ��Ϣӳ��
BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()
