////////////////////////////////////////////////////////////////////////////////
// ��    �� : Cube.h
// ����˵�� :
//   �������ںͳ�ʼ��
//
// ��    �� : 1.0
// ��    �� : Breaker Zhao <breaker.zy_AT_gmail>
// ��    �� : 2010-10
////////////////////////////////////////////////////////////////////////////////

#ifndef __CUBE_H__
#define __CUBE_H__

// ��Դ ID ���ų���
#include "res/Resource.h"

////////////////////////////////////////////////////////////////////////////////
// ȫ����
////////////////////////////////////////////////////////////////////////////////

#undef MODULE_NAME
#define MODULE_NAME     "Cube"      // ����ģ����

#define BUF_SIZE        256             // ���û�������С

class CCubeApp;
extern CCubeApp     g_CubeApp;  // Ӧ�ó���ʵ��

////////////////////////////////////////////////////////////////////////////////
// ���ֿռ�
////////////////////////////////////////////////////////////////////////////////

namespace SGL {
    class SGLExcept;
    class SGLRender;
    class SGLGameRun;
}
using SGL::SGLExcept;
using SGL::SGLRender;
using SGL::SGLGameRun;

using std::auto_ptr;

////////////////////////////////////////////////////////////////////////////////
// ���Թ���
////////////////////////////////////////////////////////////////////////////////

// Debug �� new
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// �� "ģ����!������:" ��ʽ������Ա���
#define FMT(fmt)    _T(MODULE_NAME) _T("!") _T(__FUNCTION__) _T(": ")  _T(fmt) _T("\n")
#define FMTA(fmt)   MODULE_NAME "!" __FUNCTION__ ": " fmt "\n"

#define ERR_TRACE(fmt, ...)     TRACE(FMT(fmt), __VA_ARGS__)
#define ERR_TRACEA(fmt, ...)    TRACE(FMTA(fmt), __VA_ARGS__)

////////////////////////////////////////////////////////////////////////////////
// Ӧ�ó����� CCubeApp
////////////////////////////////////////////////////////////////////////////////

class CCubeApp : public CWinApp {
public:
    CCubeApp() : m_GameRun(NULL), m_PumpMsgFunc(NULL), m_ExitCode(0) {}

public:
    virtual BOOL InitInstance();
    virtual int ExitInstance();
    virtual int Run();

public:
    // �õ�������ģ��ȫ·��
    const CString& GetModulePath() const {
        return m_ModulePath;
    }

    // ȡ������������Ŀ¼ȫ·��
    CString GetModuleDir() const;

    SGLGameRun* GetGameRun() const {
        return m_GameRun.get();
    }

    enum PUMP_MSG_TYPE {
        PUMP_GET_MSG,
        PUMP_PEEK_MSG
    };
    void SetPumpMsgType(PUMP_MSG_TYPE type);

private:
    void InitLocale();
    BOOL InitModulePath(DWORD size);    // ����������ģ���ȫ·�� m_ModulePath
    BOOL InitWorkDir(const _TCHAR* dir);// ���ù���Ŀ¼

    // PumpMsgFunc ����Լ��: ���� FALSE ��ʾ��Ϣѭ������
    typedef BOOL (*PumpMsgFunc)();
    static BOOL GetMsg();
    static BOOL PeekMsg();

private:
    CString     m_ModulePath;   // ������ģ���ȫ·��
    CString     m_OldWorkDir;   // �ɵĹ���Ŀ¼
    int         m_ExitCode;     // �������������

    auto_ptr<SGLGameRun>    m_GameRun;
    PumpMsgFunc m_PumpMsgFunc;

    static const UINT   DEF_WIDTH   = 640;
    static const UINT   DEF_HEIGHT  = 480;
};

#endif  // __CUBE_H__
