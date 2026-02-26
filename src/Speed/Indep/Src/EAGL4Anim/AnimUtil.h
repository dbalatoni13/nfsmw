#ifndef EAGL4ANIM_ANIMUTIL_H
#define EAGL4ANIM_ANIMUTIL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <cmath>

namespace EAGL4Anim {

inline int FloatToInt(float f) {
    return static_cast<int>(f);
}

inline float FastSqrt(float x) {
    return sqrtf(x);
}

inline int AlignSize16(int size) {
    return (size + 15) & ~15;
}

inline int AlignSize8(int size) {
    return (size + 7) & ~7;
}

inline int AlignSize4(int size) {
    return (size + 3) & ~3;
}

inline int AlignSize2(int size) {
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

}; // namespace EAGL4Anim

#endif
