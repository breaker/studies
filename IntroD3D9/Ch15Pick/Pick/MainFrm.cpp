// MainFrame.cpp
//

#include "StdAfx.h"
#include "Pick.h"
#include "MainFrm.h"

#include "RenderPick.h"

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
    ON_WM_LBUTTONDOWN()
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
        RenderPick* render = (RenderPick*) m_GameMain->GetRender();
    }
}

void CMainFrame::OnLButtonDown(UINT nFlags, CPoint point)
{
    IDirect3DDevice9* dev;
    RenderPick* render;
    if (m_GameMain == NULL || (render = (RenderPick*) m_GameMain->GetRender()) == NULL || (dev = m_GameMain->GetRender()->GetD3DDev()) == NULL) {
        CFrameWnd::OnLButtonDown(nFlags, point);
        return;
    }

    // compute the ray in view space given the clicked screen point
    SGL::Ray ray;
    ray.CalcPickingRay(dev, point.x, point.y);

    // transform the ray to world space
    D3DXMATRIX view;
    dev->GetTransform(D3DTS_VIEW, &view);

    D3DXMATRIX viewInverse;
    D3DXMatrixInverse(&viewInverse, 0, &view);

    ray.TransformRay(viewInverse);

    // test for a hit
    if (ray.RaySphereIntTest(render->m_BSphere))
        MYTRACE("Hit");

    CFrameWnd::OnLButtonDown(nFlags, point);
}

////////////////////////////////////////////////////////////////////////////////
// ���ڶԻ����� CAboutDlg
////////////////////////////////////////////////////////////////////////////////

// ��Ϣӳ��
BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()
