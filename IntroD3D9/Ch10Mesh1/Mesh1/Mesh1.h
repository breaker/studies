// Mesh1.h
//

#ifndef __MESH1_H__
#define __MESH1_H__

// ��Դ ID ���ų���
#include "res/Resource.h"

////////////////////////////////////////////////////////////////////////////////
// ȫ����
////////////////////////////////////////////////////////////////////////////////

#undef MODULE_NAME
#define MODULE_NAME     "Mesh1"     // ����ģ����

#define BUF_SIZE        256         // ���û�������С

class CMesh1App;
extern CMesh1App        Mesh1App;   // Ӧ�ó���ʵ��

////////////////////////////////////////////////////////////////////////////////
// ���Թ���
////////////////////////////////////////////////////////////////////////////////

// Debug �� new
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// �� "ģ����!������:" ��ʽ������Ա���
#define FMT(fmt)                    _T(MODULE_NAME) _T("!") _T(__FUNCTION__) _T(": ")  _T(fmt) _T("\n")
#define FMTA(fmt)                   MODULE_NAME "!" __FUNCTION__ ": " fmt "\n"

#define MYTRACE(fmt, ...)           TRACE(FMT(fmt), __VA_ARGS__)
#define MYTRACEA(fmt, ...)          TRACE(FMTA(fmt), __VA_ARGS__)

#define MYTRACE_DX(func, err)       MYTRACE("%s failed: %ld: %s", _T(func), err, DXGetErrorDescription(err))
#define MYTRACE_DXA(func, err)      MYTRACEA("%s failed: %ld: %s", func, err, DXGetErrorDescriptionA(err))

////////////////////////////////////////////////////////////////////////////////
// Ӧ�ó����� CMesh1App
////////////////////////////////////////////////////////////////////////////////

class CMesh1App : public CWinApp {
public:
    CMesh1App() {}

public:
    virtual BOOL InitInstance();
    virtual int Run();
    int Exit(int exitCode);

public:
    // �õ�������ģ��ȫ·��
    const CString& GetModulePath() const {
        return m_ModulePath;
    }

    // ȡ������������Ŀ¼ȫ·��
    CString GetModuleDir() const;

    enum WIN_SIZE {
        WIN_WIDTH   = 640,
        WIN_HEIGHT  = 480
    };

private:
    void InitLocale();
    BOOL InitModulePath(DWORD size);        // ����������ģ���ȫ·�� m_ModulePath
    BOOL InitWorkDir(const _TCHAR* dir);    // ���ù���Ŀ¼

private:
    CString     m_ModulePath;   // ������ģ���ȫ·��
    CString     m_OldWorkDir;   // �ɵĹ���Ŀ¼
    std::auto_ptr<SGL::Main>    m_GameMain;
};

#endif  // __MESH1_H__
