// Utils.h
//

#pragma once

#include <SGL/Depends.h>
#include <SGL/Defs.h>

SGL_NS_BEGIN

////////////////////////////////////////////////////////////////////////////////
// ��������
////////////////////////////////////////////////////////////////////////////////

// �� 0 ���
template <class T>
inline void ZeroObj(__out T* ptr) {
    ::ZeroMemory(ptr, sizeof(T));
}

template <class T>
inline void ZeroArr(__out T* ptr, size_t num) {
    ::ZeroMemory(ptr, num * sizeof(T));
}

template <class T>
inline void Release(T& t) {
    if (t != NULL) {
        t->Release();
        t = NULL;
    }
}

template <class T>
inline void Delete(T& t) {
    if (t != NULL) {
        delete t;
        t = NULL;
    }
}

////////////////////////////////////////////////////////////////////////////////
// ���Թ���
////////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG

// ����������������� SGL_RPT
// char ��
#define SGL_RPTA(rptType, fmt, ...) \
    (void) ((1 != _CrtDbgReport(rptType, 0, 0, 0, fmt, __VA_ARGS__)) || \
    (_CrtDbgBreak(), 0))

#define SGL_RPTFA(rptType, fmt, ...) \
    (void) ((1 != _CrtDbgReport(rptType, __FILE__, __LINE__, 0, fmt, __VA_ARGS__)) || \
    (_CrtDbgBreak(), 0))

// wchar_t ��
#define SGL_RPTW(rptType, fmt, ...) \
    (void) ((1 != _CrtDbgReportW(rptType, 0, 0, 0, fmt, __VA_ARGS__)) || \
    (_CrtDbgBreak(), 0))

#define SGL_RPTFW(rptType, fmt, ...) \
    (void) ((1 != _CrtDbgReportW(rptType, SGL_WIDE(__FILE__), __LINE__, 0, fmt, __VA_ARGS__)) || \
    (_CrtDbgBreak(), 0))

// _TCHAR ��
#ifdef _UNICODE
#define SGL_RPT     SGL_RPTW
#define SGL_RPTF    SGL_RPTFW
#else
#define SGL_RPT     SGL_RPTA
#define SGL_RPTF    SGL_RPTFA
#endif

#else
// _DEBUG NOT defined

#define SGL_RPTA(rptType, fmt, ...)     0
#define SGL_RPTFA(rptType, fmt, ...)    0
#define SGL_RPTW(rptType, fmt, ...)     0
#define SGL_RPTFW(rptType, fmt, ...)    0
#define SGL_RPT(rptType, fmt, ...)      0
#define SGL_RPTF(rptType, fmt, ...)     0

#endif

// �� "ģ����!������:" ��ʽ������Ա���
#define SGL_RPTFMT(fmt)     _T("%s!") _T(__FUNCTION__) _T(": ")  _T(fmt) _T("\n")
#define SGL_RPTFMTA(fmt)    "%s!" __FUNCTION__ ": " fmt "\n"

#define SGL_FMTRPT(module, rptType, fmt, ...)   SGL_RPT(rptType, SGL_RPTFMT(fmt), module, __VA_ARGS__)
#define SGL_FMTRPTA(module, rptType, fmt, ...)  SGL_RPTA(rptType, SGL_RPTFMTA(fmt), module, __VA_ARGS__)

#define SGL_MRPT(rptType, fmt, ...)             SGL_FMTRPT(SGL::MODULE_NAME, rptType, fmt, __VA_ARGS__)
#define SGL_MRPTA(rptType, fmt, ...)            SGL_FMTRPTA(SGL::MODULE_NAMEA, rptType, fmt, __VA_ARGS__)

#define SGL_RPTDX(module, rptType, func, err)   SGL_FMTRPT(module, rptType, "%s failed %ld: %s", _T(func), err, DXGetErrorDescription(err))
#define SGL_RPTDXA(module, rptType, func, err)  SGL_FMTRPTA(module, rptType, "%s failed %ld: %s", func, err, DXGetErrorDescriptionA(err))

#define SGL_MRPTDX(rptType, func, err)          SGL_RPTDX(SGL::MODULE_NAME, rptType, func, err)
#define SGL_MRPTDXA(rptType, func, err)         SGL_RPTDXA(SGL::MODULE_NAMEA, rptType, func, err)

////////////////////////////////////////////////////////////////////////////////
// ��ֹ�������� Uncopyable
////////////////////////////////////////////////////////////////////////////////

class Uncopyable {
protected:
    Uncopyable() {}
    ~Uncopyable() {}
private:
    // ��ֹ����
    Uncopyable(const Uncopyable&);
    Uncopyable& operator=(const Uncopyable&);
};

////////////////////////////////////////////////////////////////////////////////
// ��ʱ���� Timer
////////////////////////////////////////////////////////////////////////////////

class Timer : private Uncopyable {
public:
    Timer() {
        Reset();
    }

    void Reset() {
        m_Start = ::GetTickCount();
    }

    DWORD Elapse() {
        return (::GetTickCount() - m_Start);
    }

    float ElapseSecond() {
        return (::GetTickCount() - m_Start) * 0.001f;
    }

    float ElapseSecondReset() {
        DWORD cur = ::GetTickCount();
        float sec = (cur - m_Start) * 0.001f;
        m_Start = cur;
        return sec;
    }

protected:
    DWORD m_Start;
};

////////////////////////////////////////////////////////////////////////////////
// ֡�ʼ����� FPS
////////////////////////////////////////////////////////////////////////////////

class FPS : private Uncopyable {
public:
    FPS() : m_FrameCnt(0), m_TimeElapsed(0.0f), m_FPS(0.0f), m_StrPrefix(_T("")), m_StrSuffix(_T("")), m_StrSize(0), m_StrBuf(NULL), m_StrPrefixLen(0) {}

    ~FPS() {
        Clean();
    }

    void Clean() {
        if (m_StrBuf != NULL) {
            free(m_StrBuf);
            m_StrBuf = NULL;
            m_StrSize = 0;
        }
    }

    // strSize Ӧ������ prefix �� suffix
    BOOL Init(size_t strSize, LPCTSTR prefix = _T(""), LPCTSTR suffix = _T("")) {
        m_StrPrefixLen = _tcslen(prefix);
        _ASSERTE(strSize > m_StrPrefixLen + _tcslen(suffix));
        _ASSERTE(prefix != NULL && suffix != NULL);

        Clean();
        m_StrBuf = (LPTSTR) malloc(strSize * sizeof(TCHAR));
        if (m_StrBuf == NULL)
            return FALSE;
        m_StrSize = strSize;

        m_StrPrefix = prefix;
        m_StrSuffix = suffix;
        _tcscpy_s(m_StrBuf, strSize, prefix);   // Ԥ�ȿ���ǰ׺

        return TRUE;
    }

    template <BOOL FormatStr>
    float CalcFPST(float timeDelta) {
        ++m_FrameCnt;
        m_TimeElapsed += timeDelta;
        if (m_TimeElapsed >= 1.0f) {
            m_FPS = (float) m_FrameCnt / m_TimeElapsed;

            if (FormatStr)
                _stprintf_s(m_StrBuf + m_StrPrefixLen, m_StrSize - m_StrPrefixLen, _T("%f%s"), m_FPS, m_StrSuffix);

            m_TimeElapsed = 0.0f;
            m_FrameCnt = 0;
        }
        return m_FPS;
    }

    float CalcFPS(float timeDelta) {
        return CalcFPST<FALSE>(timeDelta);
    }

    LPCTSTR CalcFPSStr(float timeDelta) {
        CalcFPST<TRUE>(timeDelta);
        return m_StrBuf;
    }

private:
    DWORD   m_FrameCnt;
    float   m_TimeElapsed;
    float   m_FPS;
    LPTSTR  m_StrBuf;
    size_t  m_StrSize;
    LPCTSTR m_StrPrefix;
    size_t  m_StrPrefixLen;
    LPCTSTR m_StrSuffix;
};

SGL_NS_END
