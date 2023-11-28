////////////////////////////////////////////////////////////////////////////////
// ��    �� : MainFrame.h
// ����˵�� :
//   ����ܴ���
//
// ��    �� : 1.0
// ��    �� : Breaker Zhao <breaker.zy_AT_gmail>
// ��    �� : 2010-10
////////////////////////////////////////////////////////////////////////////////

#ifndef __MAINFRAME_H__
#define __MAINFRAME_H__

////////////////////////////////////////////////////////////////////////////////
// ����ܴ����� CMainFrame
////////////////////////////////////////////////////////////////////////////////

class CMainFrame : public CFrameWnd {
    DECLARE_DYNAMIC(CMainFrame) // MFC RTTI ֧��

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
    // ���� Debug �����Ϻ���
    virtual void AssertValid() const {
        CFrameWnd::AssertValid();
    }
    virtual void Dump(CDumpContext& dc) const {
        CFrameWnd::Dump(dc);
    }
#endif  // _DEBUG

private:
    // ��Ϣ����
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
// ���ڶԻ����� CAboutDlg
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

    // ��Ϣ����
    DECLARE_MESSAGE_MAP()
};

#endif  // __MAINFRAME_H__
