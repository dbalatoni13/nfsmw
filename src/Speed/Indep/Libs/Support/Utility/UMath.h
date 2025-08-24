#ifndef SUPPORT_UTILITY_UMATH_H
#define SUPPORT_UTILITY_UMATH_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "UTypes.h"
#include "UVectorMath.h"

namespace UMath {

void BuildRotate(Matrix4 &m, float r, float x, float y, float z);

inline float DistanceSquarexz(const Vector3 &a, const Vector3 &b) {
    return VU0_v3distancesquare(a, b);
}

inline Vector3 &Vector4To3(Vector4 &c4) {
    return *reinterpret_cast<Vector3 *>(&c4);
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

inline void SetYRot(Matrix4 &r, float a) {
    VU0_MATRIX4setyrot(r, a);
}

inline void Rotate(const Vector3 &a, const Matrix4 &m, Vector3 &r) {
    VU0_MATRIX3x4_vect3mult(a, m, r);
}

inline float Dot(const Vector3 &a, const Vector3 &b) {
    return VU0_v3dotprod(a, b);
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

} // namespace UMath

#endif
