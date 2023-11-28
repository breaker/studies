// LightTex.h
//

#pragma once

// ��Դ ID ���ų���
#include "res/Resource.h"

////////////////////////////////////////////////////////////////////////////////
// ȫ����
////////////////////////////////////////////////////////////////////////////////

// ����ģ����
const char      MODULE_NAMEA[]  = "LightTex";
const wchar_t   MODULE_NAMEW[]  = L"LightTex";
const _TCHAR    MODULE_NAME[]   = _T("LightTex");

// ���û�������С
const int BUF_SIZE = 256;

// Ӧ�ó���ʵ��
class CLightTexApp;
extern CLightTexApp LightTexApp;

////////////////////////////////////////////////////////////////////////////////
// ���Թ���
////////////////////////////////////////////////////////////////////////////////

// Debug �� new
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// �� "ģ����!������:" ��ʽ������Ա���
#define FMT(fmt)    SGL_RPTFMT(fmt), MODULE_NAME
#define FMTA(fmt)   SGL_RPTFMTA(fmt), MODULE_NAMEA

#define MYTRACE(fmt, ...)       TRACE(FMT(fmt), __VA_ARGS__)
#define MYTRACEA(fmt, ...)      TRACE(FMTA(fmt), __VA_ARGS__)

#define MYTRACE_DX(func, err)   MYTRACE("%s failed %ld: %s", _T(func), err, DXGetErrorDescription(err))
#define MYTRACE_DXA(func, err)  MYTRACEA("%s failed %ld: %s", func, err, DXGetErrorDescriptionA(err))

////////////////////////////////////////////////////////////////////////////////
// Ӧ�ó����� CLightTexApp
////////////////////////////////////////////////////////////////////////////////

class CLightTexApp : public CWinApp {
public:
    CLightTexApp() {}

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
