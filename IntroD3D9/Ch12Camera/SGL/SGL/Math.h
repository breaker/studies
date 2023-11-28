// Math.h
//

#ifndef __MATH_H__
#define __MATH_H__

#include <SGL/Depends.h>
#include <SGL/Defs.h>

SGL_NS_BEGIN

////////////////////////////////////////////////////////////////////////////////
// 数学常量
////////////////////////////////////////////////////////////////////////////////

#define SGL_PI      3.141592654f    // pi
#define SGL_2PI     6.283185308f    // 2 * pi
#define SGL_PIBY2   1.570796327f    // pi / 2
#define SGL_1BYPI   0.318309886f    // 1 / pi

#define SGL_FLT_MAX         FLT_MAX
#define SGL_FLT_EPSILON     FLT_EPSILON

////////////////////////////////////////////////////////////////////////////////
// 辅助功能
////////////////////////////////////////////////////////////////////////////////

inline BOOL Equals(float l, float r)
{
    float d = r - l;
    return (d > -SGL_FLT_EPSILON && d < SGL_FLT_EPSILON);
}

SGL_NS_END

#endif  // __MATH_H__
