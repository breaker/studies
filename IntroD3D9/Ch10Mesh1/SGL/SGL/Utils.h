// Utils.h
//

#ifndef __SGL_UTILS_H__
#define __SGL_UTILS_H__

#include <SGL/Depends.h>
#include <SGL/Defs.h>

SGL_NS_BEGIN

////////////////////////////////////////////////////////////////////////////////
// ��������
////////////////////////////////////////////////////////////////////////////////

// �ַ�����������
#define _SGL_STRINGIZE(x)       #x
#define SGL_STRINGIZE(x)        _SGL_STRINGIZE(x)
#define SGL_STRINGIZET(x)       _T(SGL_STRINGIZE(x))

// ƴ��
#define _SGL_CONCAT(x, y)       x##y
#define SGL_CONCAT(x, y)        _SGL_CONCAT(x, y)

// ���ַ���������
#define _SGL_WIDE(s)            L##s
#define SGL_WIDE(s)             _SGL_WIDE(s)

// ��� TRUE ����
#define SGL_TRUE_DO(x, exp)     if (x) { exp; }
#define SGL_ON_DO(x, exp)       SGL_TRUE_DO((x), exp)

// ��� FALSE ����
#define SGL_FALSE_DO(x, exp)    if (!(x)) { exp; }
#define SGL_OFF_DO(x, exp)      SGL_FALSE_DO((x), exp)

// ����ĳ�º� goto
#define SGL_GOTO(exp, label)    { exp; goto label; }

// HRESULT ����ʧ������
#define SGL_FAILED_DO(hr, exp)  if (FAILED(hr)) { exp; }

// ����ֵ
// 32bit ����ֵ������ x ��Чȡֵ 0 ~ 31, 32bit VC �� SGL_MASK_VAL(0) = SGL_MASK_VAL(32) = 1
#define SGL_MASK_VAL(x)         (1 << (x))

// �� 0 ���

template <class T>
inline void ZeroObj(__out T* ptr)
{
    ::ZeroMemory(ptr, sizeof(T));
}

template <class T>
inline void ZeroArr(__out T* ptr, size_t num)
{
    ::ZeroMemory(ptr, num * sizeof(T));
}

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
    DWORD   m_Start;
};

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

#endif  // __SGL_UTILS_H__
