#ifndef EAGL4ANIM_ANIMUTIL_H
#define EAGL4ANIM_ANIMUTIL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "types.h"

#include <cmath>


namespace EAGL4Anim {

inline int FloatToInt(float f) {
    return static_cast<int>(f);
}

inline float FastSqrt(float x) {
    return sqrtf(x);
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

inline float DegToRad(float deg) {
    return deg * 0.017453294f;
}

inline void LinearBlendF3(float w, const float *d0, const float *d1, float *out) {
    out[0] = w * (d1[0] - d0[0]) + d0[0];
    out[1] = w * (d1[1] - d0[1]) + d0[1];
    out[2] = w * (d1[2] - d0[2]) + d0[2];
}

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

inline void QuatMult(const UMath::Vector4 &q0, const UMath::Vector4 &q1, UMath::Vector4 &result) {
    result.x = q1.x * q0.w - q1.y * q0.z + q1.z * q0.y + q1.w * q0.x;
    result.y = q1.x * q0.z + q1.y * q0.w - q1.z * q0.x + q1.w * q0.y;
    result.z = -q1.x * q0.y + q1.y * q0.x + q1.z * q0.w + q1.w * q0.z;
    result.w = -q1.x * q0.x - q1.y * q0.y - q1.z * q0.z + q1.w * q0.w;
}

inline void EulToQuat(const float *eulData, float *quatData) {
    float ti = eulData[0] * 0.5f;
    float tj = eulData[1] * 0.5f;
    float th = eulData[2] * 0.5f;
    float ci = cosf(ti);
    float cj = cosf(tj);
    float ch = cosf(th);
    float si = sinf(ti);
    float sj = sinf(tj);
    float sh = sinf(th);
    float cc = ci * ch;
    float cs = ci * sh;
    float sc = si * ch;
    float ss = si * sh;

    quatData[0] = cj * sc - sj * cs;
    quatData[1] = cj * ss + sj * cc;
    quatData[2] = cj * cs - sj * sc;
    quatData[3] = cj * cc + sj * ss;
}

inline void QuatRotate(const UMath::Vector4 &q, const UMath::Vector4 &v, UMath::Vector4 &result) {
    float x2 = q.x + q.x, y2 = q.y + q.y, z2 = q.z + q.z;
    float wx = q.w * x2, wy = q.w * y2, wz = q.w * z2;
    float xx = q.x * x2, xy = q.x * y2, xz = q.x * z2;
    float yy = q.y * y2, yz = q.y * z2, zz = q.z * z2;

    result.x = v.x * (1.0f - (yy + zz)) + v.y * (xy - wz) + v.z * (xz + wy);
    result.y = v.x * (xy + wz) + v.y * (1.0f - (xx + zz)) + v.z * (yz - wx);
    result.z = v.x * (xz - wy) + v.y * (yz + wx) + v.z * (1.0f - (xx + yy));
    result.w = 1.0f;
}

inline void QuatTransformPoint(const UMath::Vector4 &q, const UMath::Vector4 &p, UMath::Vector4 &result) {
    float s = 2.0f;
    float xs = q.x * s, ys = q.y * s, zs = q.z * s;
    float wx = q.w * xs, wy = q.w * ys, wz = q.w * zs;
    float xx = q.x * xs, xy = q.x * ys, xz = q.x * zs;
    float yy = q.y * ys, yz = q.y * zs, zz = q.z * zs;

    result.x = p.x * (1.0f - (yy + zz)) + p.y * (xy - wz) + p.z * (xz + wy);
    result.y = p.x * (xy + wz) + p.y * (1.0f - (xx + zz)) + p.z * (yz - wx);
    result.z = p.x * (xz - wy) + p.y * (yz + wx) + p.z * (1.0f - (xx + yy));
    result.w = 1.0f;
}

}; // namespace EAGL4Anim

#endif
