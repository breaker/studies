////////////////////////////////////////////////////////////////////////////////
// 文    件 : MainFrame.cpp
// 功能说明 :
//   主框架窗口
//
// 版    本 : 1.0
// 作    者 : Breaker Zhao <breaker.zy_AT_gmail>
// 日    期 : 2010-10
////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "D3DXCreate.h"
#include "MainFrame.h"

////////////////////////////////////////////////////////////////////////////////
// 主框架窗口类 CMainFrame
////////////////////////////////////////////////////////////////////////////////

// MFC RTTI 支持
IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

// 消息映射
BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
    ON_COMMAND(ID_APP_ABOUT, &CMainFrame::OnAppAbout)
    ON_WM_CLOSE()
    ON_WM_KEYDOWN()
    ON_WM_ACTIVATE()
END_MESSAGE_MAP()

// PreCreateWindow 先于 CWinApp::Run 执行
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
    if (!CFrameWnd::PreCreateWindow(cs))
        return FALSE;

    // NOTE: 不要在这里设置窗口大小 (cs.cx, cs,cy), 因为将根据视频模式重新设置窗口大小

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
    // 应同时检测 Inactive 和 Minimized 状态, 因为有 Minimized Active 状态
    // Minimized Active: 当点击任务栏程序按钮时, 就是这种状态 (最小化 + 激活)
    // Minimized Active 先后产生两条 WM_ACTIVATE 消息: 1. WA_INACTIVE 2. WA_ACTIVE + Minimized
    // 而点击最小化按钮只产生一条 WA_INACTIVE 消息
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

    // NOTE: MFC 内部会给窗口 style 加上 WS_CLIPSIBLINGS, 所以 CWnd::GetStyle 比 m_Style 多 WS_CLIPSIBLINGS
    AdjustWindowRectEx(&rect, GetStyle(), GetMenu() != NULL, GetExStyle());
    // NOTE: 如果 rePaint = FALSE, 则从全屏变为窗口时, 一部分画面残留到屏幕上
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

    // CAUTION: 这里会销毁 this 对象, 调用 ReCreate 后, 不要调用旧对象的方法, 而应使用 ReCreate 返回的新对象调用方法
    // CWnd::DestroyWindow 和 delete pOldWnd 有相同语义 (MFC 内部删除对象), 不要重复调用
    pOldWnd->DestroyWindow();

    return pFrmWnd;
}

////////////////////////////////////////////////////////////////////////////////
// 关于对话框类 CAboutDlg
////////////////////////////////////////////////////////////////////////////////

// 消息映射
BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()
