// Terrain1.h
//

#pragma once

// ��Դ ID ���ų���
#include "res/Resource.h"

////////////////////////////////////////////////////////////////////////////////
// ȫ����
////////////////////////////////////////////////////////////////////////////////

// ����ģ����
const char      MODULE_NAMEA[]  = "Terrain1";
const wchar_t   MODULE_NAMEW[]  = L"Terrain1";
const _TCHAR    MODULE_NAME[]   = _T("Terrain1");

// ���û�������С
const int BUF_SIZE = 256;

// Ӧ�ó���ʵ��
class CTerrain1App;
extern CTerrain1App Terrain1App;

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
// Ӧ�ó����� CTerrain1App
////////////////////////////////////////////////////////////////////////////////

class CTerrain1App : public CWinApp {
public:
    CTerrain1App() {}

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
        WIN_WIDTH   = 800,
        WIN_HEIGHT  = 600
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
