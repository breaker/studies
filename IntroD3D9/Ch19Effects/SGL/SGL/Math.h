// Math.h
//

#pragma once

SGL_NS_BEGIN

////////////////////////////////////////////////////////////////////////////////
// 数学常量
////////////////////////////////////////////////////////////////////////////////

const float PI      = 3.141592654f; // pi
const float PI_X2   = 6.283185308f; // 2 * pi
const float PI_HALF = 1.570796327f; // pi / 2
const float PI_DENO = 0.318309886f; // 1 / pi

const float FLOAT_MAX       = FLT_MAX;
const float FLOAT_EPSILON   = FLT_EPSILON;

////////////////////////////////////////////////////////////////////////////////
// 辅助功能
////////////////////////////////////////////////////////////////////////////////

inline BOOL Equals(float l, float r) {
    float d = r - l;
    return (d > -FLOAT_EPSILON && d < FLOAT_EPSILON);
}

inline float Lerp(float a, float b, float t)
{
    return a - (a * t) + (b * t);
}

SGL_NS_END
