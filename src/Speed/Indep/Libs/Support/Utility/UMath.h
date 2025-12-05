#ifndef SUPPORT_UTILITY_UMATH_H
#define SUPPORT_UTILITY_UMATH_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <cmath>
#include <cstring>

#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"
#include "UTypes.h"
#include "UVectorMath.h"

#define FLOAT_EPSILON 0.000001f

namespace UMath {

void BuildRotate(Matrix4 &m, float r, float x, float y, float z);

float Ceil(const float x);

inline float Distance(const Vector3 &a, const Vector3 &b) {
    return VU0_v3distance(a, b);
}

inline float DistanceSquare(const Vector3 &a, const Vector3 &b) {
    return VU0_v3distancesquare(a, b);
}

inline float DistanceSquarexz(const Vector3 &a, const Vector3 &b) {
    return VU0_v3distancesquare(a, b);
}

inline void Clear(Vector3 &r) {
    // TODO fake match?
    *reinterpret_cast<int *>(&r.x) = 0;
    *reinterpret_cast<int *>(&r.y) = 0;
    *reinterpret_cast<int *>(&r.z) = 0;
}

inline void Copy(const Matrix4 &a, Matrix4 &r) {
    VU0_MATRIX4Copy(a, r);
}

inline void Copy(const Vector4 &a, Vector4 &r) {
    VU0_v4Copy(a, r);
}

inline void Transpose(const Matrix4 &m, Matrix4 &r) {
    VU0_MATRIX4_transpose(m, r);
}

inline void Transpose(const UMath::Vector4 &q, UMath::Vector4 &r) {
    VU0_qtranspose(q, r);
}

inline const Vector3 &ExtractAxis(const Matrix4 &m, unsigned int row) {
    return *reinterpret_cast<const Vector3 *>(&m[row]);
}

inline void ExtractXAxis(const UMath::Vector4 &q, Vector3 &r) {
    VU0_ExtractXAxis3FromQuat(q, r);
}

inline void ExtractYAxis(const UMath::Vector4 &q, Vector3 &r) {
    VU0_ExtractYAxis3FromQuat(q, r);
}

inline void ExtractZAxis(const UMath::Vector4 &q, Vector3 &r) {
    VU0_ExtractZAxis3FromQuat(q, r);
}

inline void RotateTranslate(const Vector3 &a, const Matrix4 &m, Vector3 &r) {
    VU0_MATRIX4_vect3mult(a, m, r);
}

inline void RotateTranslate(const Vector4 &a, const Matrix4 &m, Vector4 &r) {
    VU0_MATRIX4_vect4mult(a, m, r);
}

inline void Init(Matrix4 &m, const float xx, const float yy, const float zz) {
    VU0_MATRIX4Init(m, xx, yy, zz);
}

inline void Mult(const Vector4 &a, const Vector4 &b, Vector4 &r) {
    VU0_qmul(a, b, r);
}

inline void Mult(const Matrix4 &a, const Matrix4 &b, Matrix4 &r) {
    VU0_MATRIX4_mult(a, b, r);
}

inline void Unit(const UMath::Vector4 &a, Vector4 &r) {
    VU0_v4unit(a, r);
}

inline void Unitxyz(const UMath::Vector4 &a, Vector4 &r) {
    VU0_v4unitxyz(a, r);
}

inline void MultYRot(const UMath::Matrix4 &m, float a, UMath::Matrix4 &r) {
    // TODO
}

inline void QuaternionToMatrix4(const Vector4 &q, Matrix4 &m) {
    VU0_quattom4(q, m);
}

inline void Add(const Vector3 &a, const Vector3 &b, Vector3 &r) {
    VU0_v3add(a, b, r);
}

inline void Scale(const Vector3 &a, const Vector3 &b, Vector3 &r) {
    VU0_v3scale(a, b, r);
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

inline void ScaleAdd(const Vector4 &a, const float s, const Vector4 &b, Vector4 &r) {
    VU0_v4scaleadd(a, s, b, r);
}

inline void ScaleAddxyz(const UMath::Vector4 &a, const float s, const UMath::Vector4 &b, Vector4 &r) {
    VU0_v4scaleaddxyz(a, s, b, r);
}

inline void Sub(const Vector3 &a, const Vector3 &b, Vector3 &r) {
    VU0_v3sub(a, b, r);
}

inline void Subxyz(const UMath::Vector4 &a, const UMath::Vector4 &b, Vector4 &r) {
    VU0_v4subxyz(a, b, r);
}

inline void Addxyz(const UMath::Vector4 &a, const UMath::Vector4 &b, Vector4 &r) {
    VU0_v4addxyz(a, b, r);
}

inline void SetYRot(Matrix4 &r, float a) {
    VU0_MATRIX4setyrot(r, a);
}

inline void Rotate(const Vector3 &a, const Vector4 &q, Vector3 &r) {
    VU0_v3quatrotate(q, a, r);
}

inline void Rotate(const Vector3 &a, const Matrix4 &m, Vector3 &r) {
    VU0_MATRIX3x4_vect3mult(a, m, r);
}

inline float Dot(const Vector3 &a, const Vector3 &b) {
    return VU0_v3dotprod(a, b);
}

inline void Dot(const Vector3 &a, const Matrix4 &b, Vector3 &r) {
    VU0_MATRIX3x4dotprod(a, b, r);
}

inline float Dotxyz(const UMath::Vector4 &a, const UMath::Vector4 &b) {
    return VU0_v4dotprodxyz(a, b);
}

inline void Cross(const Vector3 &a, const Vector3 &b, Vector3 &r) {
    VU0_v3crossprod(a, b, r);
}

inline void UnitCross(const Vector3 &a, const Vector3 &b, Vector3 &r) {
    VU0_v3unitcrossprod(a, b, r);
}

inline float Length(const Vector3 &a) {
    return VU0_v3length(a);
}

inline float Lengthxz(const Vector3 &a) {
    return VU0_v3lengthxz(a);
}

inline float Lengthxyz(const UMath::Vector4 &a) {
    return VU0_v4lengthxyz(a);
}

inline float LengthSquare(const Vector3 &a) {
    return VU0_v3lengthsquare(a);
}

inline void Matrix4ToQuaternion(const UMath::Matrix4 &m, Vector4 &q) {
    VU0_m4toquat(m, q);
}

inline int Clamp(const int a, const int amin, const int amax) {
    return a < amin ? amin : (a > amax ? amax : a);
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
    return arange > FLOAT_EPSILON ? VU0_floatmax(0.0f, VU0_floatmin((a - amin) / arange, 1.0f)) : 0.0f;
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

inline float Atan2a(const float o, const float a) {
    return VU0_Atan2(o, a);
}

inline float Sqrt(const float f) {
    return VU0_sqrt(f);
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
    return VU0_Sin(a * (float)M_TWOPI);
}

inline float Cosa(const float a) {
    return VU0_Cos(a * (float)M_TWOPI);
}

} // namespace UMath

#endif
