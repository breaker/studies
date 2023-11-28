// MainFrm.cpp
//

#include "StdAfx.h"
#include "Toon.h"
#include "MainFrm.h"

#include "RenderToon.h"

////////////////////////////////////////////////////////////////////////////////
// 主框架窗口类 CMainFrame
////////////////////////////////////////////////////////////////////////////////

// MFC RTTI 支持
IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

// 消息映射
BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
    ON_COMMAND(ID_APP_ABOUT, &CMainFrame::OnAppAbout)
    ON_WM_ACTIVATE()
    ON_WM_CLOSE()
    ON_COMMAND(ID_FILE_OPEN, &CMainFrame::OnFileOpen)
END_MESSAGE_MAP()

// PreCreateWindow 先于 CWinApp::Run 执行
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs) {
    if (!CFrameWnd::PreCreateWindow(cs))
        return FALSE;

    // NOTE:
    // 不要在这里设置窗口大小 (cs.cx, cs,cy), 因为将根据视频模式重新设置窗口大小
    // MFC 内部会给窗口 style 加上 WS_CLIPSIBLINGS, 所以 CWnd::GetStyle 比用户设置的多 WS_CLIPSIBLINGS
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
    m_GameMain->SetPumpMessageType(SGL::Main::PUMPMSG_GET); // 换成 GetMessage 消息循环, 因为 PeekMessage 可能引起 Game->Debugger 死循环
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
// 关于对话框类 CAboutDlg
////////////////////////////////////////////////////////////////////////////////

// 消息映射
BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()
