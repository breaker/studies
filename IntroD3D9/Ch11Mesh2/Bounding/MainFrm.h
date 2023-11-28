// MainFrm.h
//

#ifndef __MAINFRAME_H__
#define __MAINFRAME_H__

////////////////////////////////////////////////////////////////////////////////
// 主框架窗口类 CMainFrame
////////////////////////////////////////////////////////////////////////////////

class CMainFrame : public CFrameWnd {
    DECLARE_DYNAMIC(CMainFrame) // MFC RTTI 支持

public:
    CMainFrame() : m_GameMain(NULL) {}
    virtual ~CMainFrame() {}

    void SetGameMain(SGL::Main* gameMain) {
        ASSERT(gameMain != NULL);
        m_GameMain = gameMain;
    }

public:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

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
    afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
    afx_msg void OnClose();
    afx_msg void OnFileOpen();

    DECLARE_MESSAGE_MAP()

private:
    SGL::Main* m_GameMain;
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
