#ifndef EAGL4ANIM_ANIMUTIL_H
#define EAGL4ANIM_ANIMUTIL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "types.h"
#include <cmath>


namespace EAGL4Anim {

inline int FloatToInt(float f) {
    return static_cast<int>(f);
}

inline float FastSqrt(float x) {
    return sqrtf(x);
}

inline void EulToQuat(const float *eulData, float *quatData) {
    float ss;
    float sc;
    float cs;
    float cc;
    float sh;
    float sj;
    float si;
    float ch;
    float cj;
    float ci;
    float th;
    float tj;
    float ti;

    ti = eulData[0] * 0.5f;
    tj = eulData[1] * 0.5f;
    th = eulData[2] * 0.5f;
    ci = cosf(ti);
    cj = cosf(tj);
    ch = cosf(th);
    si = sinf(ti);
    sj = sinf(tj);
    sh = sinf(th);
    cc = ci * cj;
    cs = ci * sj;
    sc = si * cj;
    ss = si * sj;

    quatData[0] = sc * ch - cs * sh;
    quatData[1] = cs * ch + sc * sh;
    quatData[2] = cc * sh - ss * ch;
    quatData[3] = cc * ch + ss * sh;
}

inline intptr_t AlignSize16(intptr_t size) {
    return (size + 15) & ~15;
}

inline intptr_t AlignSize8(intptr_t size) {
    return (size + 7) & ~7;
}

inline intptr_t AlignSize4(intptr_t size) {
    return (size + 3) & ~3;
}

inline intptr_t AlignSize2(intptr_t size) {
    return (size + 1) & ~1;
}

// inline float DegToRad(float deg) {}

inline void FastQuatBlendF4(float w, const float *d0, const float *d1, float *out) {
    if (d0[0] * d1[0] + d0[1] * d1[1] + d0[2] * d1[2] + d0[3] * d1[3] > 0.0f) {
        out[0] = w * (d1[0] - d0[0]) + d0[0];
        out[1] = w * (d1[1] - d0[1]) + d0[1];
        out[2] = w * (d1[2] - d0[2]) + d0[2];
        out[3] = w * (d1[3] - d0[3]) + d0[3];
    } else {
        out[0] = d0[0] - w * (d1[0] + d0[0]);
        out[1] = d0[1] - w * (d1[1] + d0[1]);
        out[2] = d0[2] - w * (d1[2] + d0[2]);
        out[3] = d0[3] - w * (d1[3] + d0[3]);
    }

    float s = 1.0f / FastSqrt(out[0] * out[0] + out[1] * out[1] + out[2] * out[2] + out[3] * out[3]);

    out[0] *= s;
    out[1] *= s;
    out[2] *= s;
    out[3] *= s;
}

inline void QuatMult(const UMath::Vector4 &a, const UMath::Vector4 &b, UMath::Vector4 &result) {
    result.x = (a.x * b.w - a.y * b.z) + a.z * b.y + a.w * b.x;
    result.y = ((a.x * b.z + a.y * b.w) - a.z * b.x) + a.w * b.y;
    result.z = ((-a.x) * b.y + a.y * b.x) + a.z * b.w + a.w * b.z;
    result.w = ((-a.x * b.x - a.y * b.y) - a.z * b.z) + a.w * b.w;
}

}; // namespace EAGL4Anim

#endif
