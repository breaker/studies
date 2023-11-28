////////////////////////////////////////////////////////////////////////////////
// 文    件 : MainFrame.h
// 功能说明 :
//   主框架窗口
//
// 版    本 : 1.0
// 作    者 : Breaker Zhao <breaker.zy_AT_gmail>
// 日    期 : 2010-10
////////////////////////////////////////////////////////////////////////////////

#ifndef __MAINFRAME_H__
#define __MAINFRAME_H__

////////////////////////////////////////////////////////////////////////////////
// 主框架窗口类 CMainFrame
////////////////////////////////////////////////////////////////////////////////

class CMainFrame : public CFrameWnd {
    DECLARE_DYNAMIC(CMainFrame) // MFC RTTI 支持

public:
    CMainFrame() : m_GameRun(NULL) {}
    virtual ~CMainFrame() {}

    void SetGameRun(SGLGameRun* gamerun);
    void AdjustWindowed();
    void AdjustExclusive();

public:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

private:
    static CMainFrame* ReCreate();

#ifdef _DEBUG
    // 用于 Debug 版的诊断函数
    virtual void AssertValid() const {
        CFrameWnd::AssertValid();
    }
    virtual void Dump(CDumpContext& dc) const {
        CFrameWnd::Dump(dc);
    }
#endif  // _DEBUG

private:
    // 消息处理
    afx_msg void OnAppAbout();
    afx_msg void OnClose();
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);

    DECLARE_MESSAGE_MAP()

private:
    DWORD       m_Style;
    HMENU       m_Menu;
    SGLGameRun* m_GameRun;
};

////////////////////////////////////////////////////////////////////////////////
// 关于对话框类 CAboutDlg
////////////////////////////////////////////////////////////////////////////////

class CAboutDlg : public CDialog {
public:
    enum { IDD = IDD_ABOUTBOX };

public:
    CAboutDlg() : CDialog(CAboutDlg::IDD) {}

private:
    // DDX/DDV
    virtual void DoDataExchange(CDataExchange* pDX) {
        CDialog::DoDataExchange(pDX);
    }

    // 消息处理
    DECLARE_MESSAGE_MAP()
};

#endif  // __MAINFRAME_H__
