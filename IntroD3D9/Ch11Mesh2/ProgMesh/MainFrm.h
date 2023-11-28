// MainFrm.h
//

#ifndef __MAINFRAME_H__
#define __MAINFRAME_H__

////////////////////////////////////////////////////////////////////////////////
// ����ܴ����� CMainFrame
////////////////////////////////////////////////////////////////////////////////

class CMainFrame : public CFrameWnd {
    DECLARE_DYNAMIC(CMainFrame) // MFC RTTI ֧��

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
    afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
    afx_msg void OnClose();
    afx_msg void OnFileOpen();

    DECLARE_MESSAGE_MAP()

private:
    SGL::Main* m_GameMain;
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
