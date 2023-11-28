// Utils.h
//

#ifndef __SGL_UTILS_H__
#define __SGL_UTILS_H__

#include <SGL/Depends.h>
#include <SGL/Defs.h>

SGL_NS_BEGIN

////////////////////////////////////////////////////////////////////////////////
// 辅助功能
////////////////////////////////////////////////////////////////////////////////

// 字符串化字面量
#define _SGL_STRINGIZE(x)       #x
#define SGL_STRINGIZE(x)        _SGL_STRINGIZE(x)
#define SGL_STRINGIZET(x)       _T(SGL_STRINGIZE(x))

// 拼接
#define _SGL_CONCAT(x, y)       x##y
#define SGL_CONCAT(x, y)        _SGL_CONCAT(x, y)

// 宽化字符串字面量
#define _SGL_WIDE(s)            L##s
#define SGL_WIDE(s)             _SGL_WIDE(s)

// 如果 TRUE 则做
#define SGL_TRUE_DO(x, exp)     if (x) { exp; }
#define SGL_ON_DO(x, exp)       SGL_TRUE_DO((x), exp)

// 如果 FALSE 则做
#define SGL_FALSE_DO(x, exp)    if (!(x)) { exp; }
#define SGL_OFF_DO(x, exp)      SGL_FALSE_DO((x), exp)

// 做完某事后 goto
#define SGL_GOTO(exp, label)    { exp; goto label; }

// HRESULT 测试失败则做
#define SGL_FAILED_DO(hr, exp)  if (FAILED(hr)) { exp; }

// 掩码值
// 32bit 掩码值的整数 x 有效取值 0 ~ 31, 32bit VC 中 SGL_MASK_VAL(0) = SGL_MASK_VAL(32) = 1
#define SGL_MASK_VAL(x)         (1 << (x))

// 用 0 填充

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
// 禁止拷贝基类 Uncopyable
////////////////////////////////////////////////////////////////////////////////

class Uncopyable {
protected:
    Uncopyable() {}
    ~Uncopyable() {}
private:
    // 禁止拷贝
    Uncopyable(const Uncopyable&);
    Uncopyable& operator=(const Uncopyable&);
};

////////////////////////////////////////////////////////////////////////////////
// 计时器类 Timer
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

SGL_NS_END

#endif  // __SGL_UTILS_H__
