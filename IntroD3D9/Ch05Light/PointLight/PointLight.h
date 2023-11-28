////////////////////////////////////////////////////////////////////////////////
// ��    �� : PointLight.h
// ����˵�� :
//   �������ںͳ�ʼ��
//
// ��    �� : 1.0
// ��    �� : Breaker Zhao <breaker.zy_AT_gmail>
// ��    �� : 2010-10
////////////////////////////////////////////////////////////////////////////////

#ifndef __POINTLIGHT_H__
#define __POINTLIGHT_H__

// ��Դ ID ���ų���
#include "res/Resource.h"

////////////////////////////////////////////////////////////////////////////////
// ȫ����
////////////////////////////////////////////////////////////////////////////////

#undef MODULE_NAME
#define MODULE_NAME     "PointLight"      // ����ģ����

#define BUF_SIZE        256             // ���û�������С

class CPointLightApp;
extern CPointLightApp     g_PointLightApp;  // Ӧ�ó���ʵ��

////////////////////////////////////////////////////////////////////////////////
// ���ֿռ�
////////////////////////////////////////////////////////////////////////////////

using Simp::Uncopyable;
using Simp::Delete;

using Simp::STDDEL;
using Simp::STDDEL_Z;
using Simp::STDDEL_C;
using Simp::STDDEL_CZ;

using Simp::STDDELARR;
using Simp::STDDELARR_Z;
using Simp::STDDELARR_C;
using Simp::STDDELARR_CZ;

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
// Ӧ�ó����� CPointLightApp
////////////////////////////////////////////////////////////////////////////////

class CPointLightApp : public CWinApp {
public:
    CPointLightApp() : m_GameRun(NULL), m_PumpMsgFunc(NULL), m_ExitCode(0) {}

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

#endif  // __POINTLIGHT_H__
