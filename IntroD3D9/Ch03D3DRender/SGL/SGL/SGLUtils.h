// SGLUtils.h
//

#ifndef __SGLUTILS_H__
#define __SGLUTILS_H__

#include <SGL/SGLDepends.h>
#include <SGL/SGLDefs.h>

_SGL_NS_BEGIN

////////////////////////////////////////////////////////////////////////////////
// 游戏错误码
////////////////////////////////////////////////////////////////////////////////

namespace SGL_ERR {
enum ERR_VALUE {
    ERR_SUCCESS     = 0,    // 成功
    ERR_OK          = 0,
    ERR_UNKNOWN     = -1,   // 不关心原因的错误, 未知错误
    ERR_INVAL       = 1,    // 参数错误
};
}   // namespace SGL_ERR

////////////////////////////////////////////////////////////////////////////////
// 游戏异常基类 SGLExcept
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
// 调试功能
////////////////////////////////////////////////////////////////////////////////

// 以 "模块名!函数名:" 形式输出调试报告
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
// 游戏时钟类 Clock
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
    DWORD   delay_; // 以 tick 为单位
};

////////////////////////////////////////////////////////////////////////////////
// 输入输出功能
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
