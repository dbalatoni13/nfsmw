#ifndef SUPPORT_UTILITY_UMATH_H
#define SUPPORT_UTILITY_UMATH_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <cmath>

#ifdef EA_PLATFORM_XENON
#include <ppcintrinsics.h>
#endif

#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"
// #include "UEALibs.hpp"
#include "UTypes.h"
#include "UVectorMath.h"

#define FLOAT_EPSILON 0.000001f

namespace UMath {

inline float Sina(const float a) {
    return VU0_Sin(a * (float)M_TWOPI);
}

inline float Cosa(const float a) {
    return VU0_Cos(a * (float)M_TWOPI);
}

inline float Sinr(const float a) {
    return VU0_Sin(RAD2ANGLE(a) * (float)M_TWOPI);
}

inline float Cosr(const float a) {
    return VU0_Cos(RAD2ANGLE(a) * (float)M_TWOPI);
}

void BuildRotate(Matrix4 &m, float r, float x, float y, float z);

float Ceil(const float x);

inline float Distance(const Vector3 &a, const Vector3 &b) {
    return VU0_v3distance(a, b);
}

inline float Distancexz(const Vector3 &a, const Vector3 &b) {
    return VU0_v3distancexz(a, b);
}

inline float DistanceSquare(const Vector3 &a, const Vector3 &b) {
#ifdef EA_PLATFORM_XENON
    return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z);
#else
    return VU0_v3distancesquare(a, b);
#endif
}

inline float DistanceSquarexz(const Vector3 &a, const Vector3 &b) {
    return VU0_v3distancesquare(a, b);
}

inline void Clear(Vector3 &r) {
#ifdef EA_PLATFORM_PLAYSTATION2
    *reinterpret_cast<uint64 *>(&r.x) = 0;
    *reinterpret_cast<uint32 *>(&r.z) = 0;
#else
    *reinterpret_cast<uint32 *>(&r.x) = 0;
    *reinterpret_cast<uint32 *>(&r.y) = 0;
    *reinterpret_cast<uint32 *>(&r.z) = 0;
#endif
}

inline void Copy(const Matrix4 &a, Matrix4 &r) {
    VU0_MATRIX4Copy(a, r);
}

inline void Set(Matrix4 &m, unsigned int row, const Vector4 &a) {
    VU0_v4Copy(a, m[row]);
}

inline void Copy(const Vector4 &a, Vector4 &r) {
    VU0_v4Copy(a, r);
}

#ifdef EA_PLATFORM_XENON
void Transpose(const Matrix4 &m, Matrix4 &r);
#else
inline void Transpose(const Matrix4 &m, Matrix4 &r) {
    VU0_MATRIX4_transpose(m, r);
}
#endif

inline void Transpose(const Vector4 &q, Vector4 &r) {
    VU0_qtranspose(q, r);
}

inline const Vector3 &ExtractAxis(const Matrix4 &m, unsigned int row) {
    return *reinterpret_cast<const Vector3 *>(&m[row]);
}

inline void ExtractXAxis(const Vector4 &q, Vector3 &r) {
    VU0_ExtractXAxis3FromQuat(q, r);
}

inline void ExtractYAxis(const Vector4 &q, Vector3 &r) {
    VU0_ExtractYAxis3FromQuat(q, r);
}

inline void ExtractZAxis(const Vector4 &q, Vector3 &r) {
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

#if EA_PLATFORM_XENON
void Mult(const Vector4 &a, const Vector4 &b, Vector4 &r);
#else
inline void Mult(const Vector4 &a, const Vector4 &b, Vector4 &r) {
    VU0_qmul(a, b, r);
}
#endif

inline void Mult(const Matrix4 &a, const Matrix4 &b, Matrix4 &r) {
    VU0_MATRIX4_mult(a, b, r);
}

inline void Unit(const Vector3 &a, Vector3 &r) {
    VU0_v3unit(a, r);
}

inline void Unit(const Vector4 &a, Vector4 &r) {
    VU0_v4unit(a, r);
}

inline void Unitxyz(const Vector4 &a, Vector4 &r) {
    VU0_v4unitxyz(a, r);
}

// UEALibs not working???
void MATRIX4_multyrot(const Matrix4 *m4, float ybangle, Matrix4 *resultm);
inline void MultYRot(const Matrix4 &m, float a, Matrix4 &r) {
    r = m;
    MATRIX4_multyrot(&r, a, &r);
}

#ifdef EA_PLATFORM_XENON
void QuaternionToMatrix4(const Vector4 &q, Matrix4 &m);
#else
inline void QuaternionToMatrix4(const Vector4 &q, Matrix4 &m) {
    VU0_quattom4(q, m);
}
#endif

inline void Add(const Vector3 &a, const Vector3 &b, Vector3 &r) {
#ifdef EA_PLATFORM_XENON
    r.x = a.x + b.x;
    r.y = a.y + b.y;
    r.z = a.z + b.z;
#else
    VU0_v3add(a, b, r);
#endif
}

inline void Scale(const Vector3 &a, const Vector3 &b, Vector3 &r) {
#ifdef EA_PLATFORM_XENON
    r.x = a.x * b.x;
    r.y = a.y * b.y;
    r.z = a.z * b.z;
#else
    VU0_v3scale(a, b, r);
#endif
}

inline void Scale(const Vector3 &a, const float s, Vector3 &r) {
#ifdef EA_PLATFORM_XENON
    r.x = a.x * s;
    r.y = a.y * s;
    r.z = a.z * s;
#else
    VU0_v3scale(a, s, r);
#endif
}

inline void Scale(Vector3 &r, const float s) {
#ifdef EA_PLATFORM_XENON
    r.x *= s;
    r.y *= s;
    r.z *= s;
#else
    VU0_v3scale(r, s, r);
#endif
}

inline void Scale(const Vector4 &a, const float s, Vector4 &r) {
    VU0_v4scale(a, s, r);
}

inline void Scale(Vector4 &r, const float s) {
    VU0_v4scale(r, s, r);
}

inline void ScaleAdd(const Vector3 &a, const float s, const Vector3 &b, Vector3 &r) {
#ifdef EA_PLATFORM_XENON
    r.x = a.x + b.x * s;
    r.y = a.y + b.y * s;
    r.z = a.z + b.z * s;
#else
    VU0_v3scaleadd(a, s, b, r);
#endif
}

inline void ScaleAdd(const Vector4 &a, const float s, const Vector4 &b, Vector4 &r) {
    VU0_v4scaleadd(a, s, b, r);
}

inline void ScaleAddxyz(const Vector4 &a, const float s, const Vector4 &b, Vector4 &r) {
    VU0_v4scaleaddxyz(a, s, b, r);
}

inline void AddScale(const Vector3 &a, const Vector3 &b, const float s, Vector3 &r) {
#ifdef EA_PLATFORM_XENON
#else
    VU0_v3addscale(a, b, s, r);
#endif
}

inline void Sub(const Vector3 &a, const Vector3 &b, Vector3 &r) {
#ifdef EA_PLATFORM_XENON
    r.x = a.x - b.x;
    r.y = a.y - b.y;
    r.z = a.z - b.z;
#else
    VU0_v3sub(a, b, r);
#endif
}

inline void Subxyz(const Vector4 &a, const Vector4 &b, Vector4 &r) {
    VU0_v4subxyz(a, b, r);
}

inline void SetYRot(Matrix4 &r, float a) {
    VU0_MATRIX4setyrot(r, a);
}

#ifdef EA_PLATFORM_XENON
void Rotate(const Vector3 &a, const Vector4 &q, Vector3 &r);
#else
inline void Rotate(const Vector3 &a, const Vector4 &q, Vector3 &r) {
    VU0_v3quatrotate(q, a, r);
}
#endif

inline void RotateInXZ(const float a, const Vector3 &src, Vector3 &out) {
    float xp = src.x * Cosa(a) - src.z * Sina(a);
    float zp = src.x * Sina(a) + src.z * Cosa(a);

    out.x = xp;
    out.y = src.y;
    out.z = zp;
}

inline void Rotate(const Vector3 &a, const Matrix4 &m, Vector3 &r) {
#ifdef EA_PLATFORM_XENON
    Vector3 temp = a;

    r.x = m.v0.x * temp.x + m.v1.x * temp.y + m.v2.x * temp.z;
    r.y = m.v0.y * temp.x + m.v1.y * temp.y + m.v2.y * temp.z;
    r.z = m.v0.z * temp.x + m.v1.z * temp.y + m.v2.z * temp.z;
#else
    VU0_MATRIX3x4_vect3mult(a, m, r);
#endif
}

inline float Dot(const Vector3 &a, const Vector3 &b) {
#ifdef EA_PLATFORM_XENON
    return a.x * b.x + a.y * b.y + a.z * b.z;
#else
    return VU0_v3dotprod(a, b);
#endif
}

inline float Dot(const Vector2 &a, const Vector2 &b) {
    return a.x * b.x + a.y * b.y;
}

inline void Dot(const Vector3 &a, const Matrix4 &b, Vector3 &r) {
#ifdef EA_PLATFORM_XENON
    r.x = Dot(a, UMath::Vector4To3(b.v0));
    r.y = Dot(a, UMath::Vector4To3(b.v1));
    r.z = Dot(a, UMath::Vector4To3(b.v2));
#else
    VU0_MATRIX3x4dotprod(a, b, r);
#endif
}

inline float Dotxyz(const Vector4 &a, const Vector4 &b) {
    return VU0_v4dotprodxyz(a, b);
}

inline void Cross(const Vector3 &a, const Vector3 &b, Vector3 &r) {
#ifdef EA_PLATFORM_XENON
    r.x = a.y * b.z - a.z * b.y;
    r.y = a.z * b.x - a.x * b.z;
    r.z = a.x * b.y - a.y * b.x;
#else
    VU0_v3crossprod(a, b, r);
#endif
}

#ifdef EA_PLATFORM_XENON
void UnitCross(const Vector3 &a, const Vector3 &b, Vector3 &r);
#else
inline void UnitCross(const Vector3 &a, const Vector3 &b, Vector3 &r) {
    VU0_v3unitcrossprod(a, b, r);
}
#endif

inline float Normalize(Vector3 &r) {
    float m = VU0_v3length(r);
    if (m != 0.0f) {
        VU0_v3scale(r, 1.0f / m, r);
    }
    return m;
}

inline void Direction(const UMath::Vector3 &a, const UMath::Vector3 &b, UMath::Vector3 &r) {
    VU0_v3sub(a, b, r);
    VU0_v3unit(r, r);
}

inline float Lengthxz(const Vector3 &a) {
    return VU0_v3lengthxz(a);
}

inline float Lengthxyz(const Vector4 &a) {
    return VU0_v4lengthxyz(a);
}

inline float LengthSquare(const Vector3 &a) {
#ifdef EA_PLATFORM_XENON
    return a.x * a.x + a.y * a.y + a.z * a.z;
#else
    return VU0_v3lengthsquare(a);
#endif
}

inline float Atan2d(float o, float a) {
    return ANGLE2DEG(VU0_Atan2(o, a));
}

inline float Atan2a(const float o, const float a) {
    return VU0_Atan2(o, a);
}

inline float Atan2r(const float o, const float a) {
    return ANGLE2RAD(VU0_Atan2(o, a));
}

inline float Sqrt(const float f) {
#ifdef EA_PLATFORM_XENON
    return __fsqrts(f);
#else
    return VU0_sqrt(f);
#endif
}

inline float Length(const Vector3 &a) {
#ifdef EA_PLATFORM_XENON
    return Sqrt(LengthSquare(a));
#else
    return VU0_v3length(a);
#endif
}

inline void Matrix4ToQuaternion(const Matrix4 &m, Vector4 &q) {
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

inline unsigned int Min(const int a, const int b) {
    return a > b ? b : a;
}

inline unsigned int Max(const int a, const int b) {
    return a < b ? b : a;
}
inline unsigned int Min(const unsigned int a, const unsigned int b) {
    return a > b ? b : a;
}

inline unsigned int Max(const unsigned int a, const unsigned int b) {
    return a < b ? b : a;
}

#ifdef CLANGD_DAMNIT
inline size_t Max(const size_t a, const size_t b) {
    return a < b ? b : a;
}
#endif

// Credits: Brawltendo
// Limits the input value to the range [a,l]
inline float Limit(const float a, const float l) {
    float retval;
    if (!(a * l > 0.f)) {
        retval = a;
    } else {
        if (a > 0.f) {
            retval = Min(a, l);

        } else {
            retval = Max(a, l);
        }
    }
    return retval;
}

} // namespace UMath

#endif
