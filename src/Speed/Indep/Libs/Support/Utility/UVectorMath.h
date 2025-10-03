#ifndef SUPPORT_UTILITY_UVECTOR_MATH_H
#define SUPPORT_UTILITY_UVECTOR_MATH_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "UTypes.h"

float VU0_Atan2(const float opposite, const float adjacent);
float VU0_sqrt(const float a);
float VU0_rsqrt(const float a);

void VU0_v3add(const UMath::Vector3 &a, const UMath::Vector3 &b, UMath::Vector3 &result);
void VU0_v3scale(const UMath::Vector3 &a, const float scaleby, UMath::Vector3 &result);
void VU0_v3scale(const UMath::Vector3 &a, const UMath::Vector3 &b, UMath::Vector3 &result);
void VU0_v3scaleadd(const UMath::Vector3 &a, const float scaleby, const UMath::Vector3 &b, UMath::Vector3 &result);
void VU0_v3sub(const UMath::Vector3 &a, const UMath::Vector3 &b, UMath::Vector3 &result);
float VU0_v3dotprod(const UMath::Vector3 &a, const UMath::Vector3 &b);
float VU0_v3distancesquare(const UMath::Vector3 &p1, const UMath::Vector3 &p2);
float VU0_v3distancesquarexz(const UMath::Vector3 &p1, const UMath::Vector3 &p2);
void VU0_v3crossprod(const UMath::Vector3 &a, const UMath::Vector3 &b, UMath::Vector3 &dest);
float VU0_v3lengthsquare(const UMath::Vector3 &a);

void VU0_v4subxyz(const UMath::Vector4 &a, const UMath::Vector4 &b, UMath::Vector4 &result);
void VU0_v4scale(const UMath::Vector4 &a, const float scaleby, UMath::Vector4 &result);
float VU0_v4distancesquarexyz(const UMath::Vector4 &p1, const UMath::Vector4 &p2);

void VU0_MATRIX3x4_vect3mult(const UMath::Vector3 &v, const UMath::Matrix4 &m, UMath::Vector3 &result);

void VU0_MATRIX4_vect3mult(const UMath::Vector3 &v, const UMath::Matrix4 &m, UMath::Vector3 &result);
void VU0_MATRIX4_vect4mult(const UMath::Vector4 &v, const UMath::Matrix4 &m, UMath::Vector4 &result);
void VU0_MATRIX4setyrot(UMath::Matrix4 &dest, const float yangle);

inline float VU0_fabs(const float a) {
    if (a < 0.0f) {
        return -a;
    }
    return a;
    // return a < 0.0f ? -a : a;
}

inline float VU0_Pow(float x, float e) {
    return powf(x, e);
}

inline float VU0_floatmin(const float a, const float b) {
    if (a < b)
        return a;
    else
        return b;
}

inline float VU0_floatmax(const float a, const float b) {
    if (a > b)
        return a;
    else
        return b;
}

inline void VU0_v3negate(UMath::Vector3 &result) {
    result.x = -result.x;
    result.y = -result.y;
    result.z = -result.z;
}

inline float VU0_Sin(float x) {
    return sinf(x);
}

inline void VU0_v3unitcrossprod(const UMath::Vector3 &a, const UMath::Vector3 &b, UMath::Vector3 &dest) {
    // TODO
}

inline float VU0_v3length(const struct UMath::Vector3 &a) {
    return VU0_sqrt(VU0_v3lengthsquare(a));
}

#endif
