#ifndef SUPPORT_UTILITY_UMATH_H
#define SUPPORT_UTILITY_UMATH_H

#include <cmath>
#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"
#include "UTypes.h"
#include "UVectorMath.h"

#define FLOAT_EPSILON 0.000001f

namespace UMath {

void BuildRotate(Matrix4 &m, float r, float x, float y, float z);

inline float DistanceSquarexz(const Vector3 &a, const Vector3 &b) {
    return VU0_v3distancesquare(a, b);
}

inline void Clear(Vector3 &r) {
    r.x = 0.0f;
    r.y = 0.0f;
    r.z = 0.0f;
}

inline Vector3 &Vector4To3(Vector4 &c4) {
    return *reinterpret_cast<Vector3 *>(&c4);
}

inline const Vector3 &Vector4To3(const Vector4 &c4) {
    return *reinterpret_cast<const Vector3 *>(&c4);
}

inline Vector2 Vector2Make(float x, float y) {
    Vector2 c;
    c.x = x;
    c.y = y;
    return c;
}

inline Vector4 Vector4Make(const Vector3 &c, float w) {
    // Vector4 res = {c.x, c.y, c.z, w};
    Vector4 res;
    res.x = c.x;
    res.y = c.y;
    res.z = c.z;
    res.w = w;
    return res;
}

inline const Vector3 &ExtractAxis(const Matrix4 &m, unsigned int row) {
    return *reinterpret_cast<const Vector3 *>(&m[row]);
}

inline void RotateTranslate(const Vector3 &a, const Matrix4 &m, Vector3 &r) {
    VU0_MATRIX4_vect3mult(a, m, r);
}

inline void RotateTranslate(const Vector4 &a, const Matrix4 &m, Vector4 &r) {
    VU0_MATRIX4_vect4mult(a, m, r);
}

inline void Add(const Vector3 &a, const Vector3 &b, Vector3 &r) {
    VU0_v3add(a, b, r);
}

inline void Scale(const Vector3 &a, const float s, Vector3 &r) {
    VU0_v3scale(a, s, r);
}

inline void Scale(const Vector4 &a, const float s, Vector4 &r) {
    VU0_v4scale(a, s, r);
}

inline void ScaleAdd(const Vector3 &a, const float s, const Vector3 &b, Vector3 &r) {
    VU0_v3scaleadd(a, s, b, r);
}

inline void Sub(const UMath::Vector3 &a, const UMath::Vector3 &b, UMath::Vector3 &r) {
    VU0_v3sub(a, b, r);
}

inline void SetYRot(Matrix4 &r, float a) {
    VU0_MATRIX4setyrot(r, a);
}

inline void Rotate(const Vector3 &a, const Matrix4 &m, Vector3 &r) {
    VU0_MATRIX3x4_vect3mult(a, m, r);
}

inline float Dot(const Vector3 &a, const Vector3 &b) {
    return VU0_v3dotprod(a, b);
}

inline void Cross(const UMath::Vector3 &a, const UMath::Vector3 &b, UMath::Vector3 &r) {
    VU0_v3crossprod(a, b, r);
}

inline void UnitCross(const UMath::Vector3 &a, const UMath::Vector3 &b, UMath::Vector3 &r) {
    VU0_v3unitcrossprod(a, b, r);
}

inline float Length(const UMath::Vector3 &a) {
    return VU0_v3length(a);
}

inline float LengthSquare(const UMath::Vector3 &a) {
    return VU0_v3lengthsquare(a);
}

inline float Clamp(const float a, const float amin, const float amax) {
    return VU0_floatmax(amin, VU0_floatmin(a, amax));
}

inline float Abs(const float a) {
    return VU0_fabs(a);
}

inline float Pow(const float f, const float e) {
    return VU0_Pow(f, e);
}

inline float Ramp(const float a, const float amin, const float amax) {
    float arange = amax - amin;

    return VU0_floatmax(0.0f, VU0_floatmin((a - amin) / arange, 1.0f));
}

inline float Lerp(const float a, const float b, const float t) {
    return a + (b - a) * t;
}

inline void Negate(Vector3 &r) {
    VU0_v3negate(r);
}

inline float Min(const float a, const float b) {
    return VU0_floatmin(a, b);
}

inline float Max(const float a, const float b) {
    return VU0_floatmax(a, b);
}

inline unsigned int Min(const unsigned int a, const unsigned int b) {
    return a > b ? b : a;
}

inline unsigned int Max(const unsigned int a, const unsigned int b) {
    return a < b ? b : a;
}

inline float Atan2d(float o, float a) {
    return ANGLE2DEG(VU0_Atan2(o, a));
}

inline float Sqrt(const float f) {
    return sqrtf(f);
}

// Credits: Brawltendo
// Limits the input value to the range [a,l]
inline float Limit(const float a, const float l) {
    float retval;
    if (!(a * l > 0.f)) {
        retval = a;
    } else {
        if (a > 0.f) {
            retval = UMath::Min(a, l);

        } else {
            retval = UMath::Max(a, l);
        }
    }
    return retval;
}

inline float Sina(const float a) {
    return VU0_Sin(a);
}

} // namespace UMath

#endif
