// SGLUtils.h
//

#ifndef __SGLUTILS_H__
#define __SGLUTILS_H__

#include <SGL/SGLDepends.h>
#include <SGL/SGLDefs.h>

_SGL_NS_BEGIN

////////////////////////////////////////////////////////////////////////////////
// ��Ϸ������
////////////////////////////////////////////////////////////////////////////////

namespace SGL_ERR {
enum ERR_VALUE {
    ERR_SUCCESS     = 0,    // �ɹ�
    ERR_OK          = 0,
    ERR_UNKNOWN     = -1,   // ������ԭ��Ĵ���, δ֪����
    ERR_INVAL       = 1,    // ��������
};
}   // namespace SGL_ERR

////////////////////////////////////////////////////////////////////////////////
// ��Ϸ�쳣���� SGLExcept
////////////////////////////////////////////////////////////////////////////////

class SGLExcept : public std::exception {
public:
    explicit SGLExcept(DWORD err = SGL_ERR::ERR_UNKNOWN, const char* msg = "") : std::exception(msg), errCode_(err) {}
    explicit SGLExcept(const char* msg) : std::exception(msg), errCode_(SGL_ERR::ERR_UNKNOWN) {}

public:
    virtual DWORD errCode() const {
        return errCode_;
    }

private:
    DWORD   errCode_;
};

////////////////////////////////////////////////////////////////////////////////
// ���Թ���
////////////////////////////////////////////////////////////////////////////////

// �� "ģ����!������:" ��ʽ������Ա���
#define SGLFMT(fmt)                 _T(MODULE_NAME) _T("!") _T(__FUNCTION__) _T(": ")  _T(fmt) _T("\n")
#define SGLFMTA(fmt)                MODULE_NAME "!" __FUNCTION__ ": " fmt "\n"

#define SGL_TRACE(fmt, ...)         TRACE(SGLFMT(fmt), __VA_ARGS__)
#define SGL_TRACEA(fmt, ...)        TRACE(SGLFMTA(fmt), __VA_ARGS__)

#define SGL_DXTRACE0(func)          SGL_TRACE("%s failed", _T(func))
#define SGL_DXTRACE0A(func)         SGL_TRACEA("%s failed", func)

#define SGL_DXTRACE1(func, err)     SGL_TRACE("%s failed: %ld: %s", _T(func), err, DXGetErrorDescription(err))
#define SGL_DXTRACE1A(func, err)    SGL_TRACEA("%s failed: %ld: %s", func, err, DXGetErrorDescriptionA(err))

#define SGL_FAILED_DO(hr, exp)      if (FAILED(hr)) { exp; }

#define SGL_TRUE_DO(x, exp)         SIMP_TRUE_DO((x), exp)
#define SGL_FALSE_DO(x, exp)        SIMP_FALSE_DO((x), exp)

////////////////////////////////////////////////////////////////////////////////
// ��Ϸʱ���� Clock
////////////////////////////////////////////////////////////////////////////////

class Clock : private Uncopyable {
public:
    Clock(DWORD delay = 0) : delay_(delay) {
        reset();
    }
    void setDelay(DWORD delay) {
        delay_ = delay;
    }
    void reset() {
        start_ = GetTickCount();
    }
    void wait() {
        while (GetTickCount() - start_ < delay_)
            YieldProcessor();
    }
    DWORD elapse() {
        return (GetTickCount() - start_);
    }
    float elapseSec() {
        return (GetTickCount() - start_) * 0.001f;
    }

private:
    DWORD   start_;
    DWORD   delay_; // �� tick Ϊ��λ
};

////////////////////////////////////////////////////////////////////////////////
// �����������
////////////////////////////////////////////////////////////////////////////////

inline BOOL KEY_DOWN(int vkey)
{
    return (GetAsyncKeyState(vkey) & 0x8000);
}

inline BOOL KEY_UP(int vkey)
{
    return !(GetAsyncKeyState(vkey) & 0x8000);
}

_SGL_NS_END

#endif  // __SGLUTILS_H__
