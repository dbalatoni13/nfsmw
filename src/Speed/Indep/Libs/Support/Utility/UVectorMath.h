#ifndef SUPPORT_UTILITY_UVECTOR_MATH_H
#define SUPPORT_UTILITY_UVECTOR_MATH_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "UTypes.h"

#include <cmath>

// TODO sort these

float VU0_Sin(float x);
float VU0_Cos(float x);

float VU0_sqrt(const float a);
float VU0_Atan2(const float opposite, const float adjacent);
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
void VU0_v3unit(const UMath::Vector3 &a, UMath::Vector3 &result);

void VU0_v4scaleadd(const UMath::Vector4 &a, const float scaleby, const UMath::Vector4 &b, UMath::Vector4 &result);
void VU0_v4scaleaddxyz(const UMath::Vector4 &a, const float scaleby, const UMath::Vector4 &b, UMath::Vector4 &result);
float VU0_v4lengthsquare(const UMath::Vector4 &a);
float VU0_v4lengthsquarexyz(const UMath::Vector4 &a);
void VU0_v4subxyz(const UMath::Vector4 &a, const UMath::Vector4 &b, UMath::Vector4 &result);
float VU0_v4dotprodxyz(const UMath::Vector4 &a, const UMath::Vector4 &b);
void VU0_v4scale(const UMath::Vector4 &a, const float scaleby, UMath::Vector4 &result);
void VU0_v4scalexyz(const UMath::Vector4 &a, const float scaleby, UMath::Vector4 &result);
float VU0_v4distancesquarexyz(const UMath::Vector4 &p1, const UMath::Vector4 &p2);
void VU0_MATRIX3x4_vect3mult(const UMath::Vector3 &v, const UMath::Matrix4 &m, UMath::Vector3 &result);
void VU0_qmul(const UMath::Vector4 &b, const UMath::Vector4 &a, UMath::Vector4 &dest);

void VU0_v3quatrotate(const UMath::Vector4 &q, const UMath::Vector3 &v, UMath::Vector3 &result);

void VU0_m4toquat(const UMath::Matrix4 &mat, UMath::Vector4 &result);
void VU0_MATRIX4_vect3mult(const UMath::Vector3 &v, const UMath::Matrix4 &m, UMath::Vector3 &result);
void VU0_MATRIX4_vect4mult(const UMath::Vector4 &v, const UMath::Matrix4 &m, UMath::Vector4 &result);
void VU0_MATRIX4setyrot(UMath::Matrix4 &dest, const float yangle);

// has to be above VU0_v3dotprod so it doesn't inline
inline void VU0_MATRIX3x4dotprod(const UMath::Vector3 &a, const UMath::Matrix4 &b, UMath::Vector3 &r) {
    r.x = VU0_v3dotprod(a, UMath::Vector4To3(b.v0));
    r.y = VU0_v3dotprod(a, UMath::Vector4To3(b.v1));
    r.z = VU0_v3dotprod(a, UMath::Vector4To3(b.v2));
}

#ifdef EA_PLATFORM_PLAYSTATION2
#include "Speed/PSX2/bWare/Src/ee/include/eetypes.h"

inline float VU0_sqrt(const float a) {
    float result;
    asm __volatile__("vsqrt Q, vf2x\n"
                     "vwaitq\n"
                     "vaddq vf3, vf0, Q\n"
                     : "=f"(result)
                     : "o"(a));
    return result;
}

inline void VU0_v3add(const UMath::Vector3 &a, const UMath::Vector3 &b, UMath::Vector3 &result) {
    asm __volatile__("lqc2 vf1, %1\n"
                     "lqc2 vf2, %2\n"
                     "lqc2 vf3, %0\n"
                     "vadd vf3, vf1, vf2\n"
                     "sqc2 vf3, %0"
                     : "=o"(result)
                     : "o"(a), "o"(b));
}

inline void VU0_v3scale(const UMath::Vector3 &a, const float scaleby, UMath::Vector3 &result) {
    asm __volatile__("lqc2 vf1, %1\n"
                     "lqc2 vf3, %0\n"
                     "qmtc2.ni %2, vf2\n"
                     "vmulx vf3, vf1, vf2x\n"
                     "sqc2 vf3, %0"
                     : "=o"(result)
                     : "o"(a), "r"(scaleby));
}

inline void VU0_v3scale(const UMath::Vector3 &a, const UMath::Vector3 &b, UMath::Vector3 &result) {
    asm __volatile__("lqc2 vf3, %0\n"
                     "lqc2 vf1, %2\n"
                     "lqc2 vf2, %1\n"
                     "vmul vf3, vf1, vf2\n"
                     "sqc2 vf3, %0"
                     : "=o"(result)
                     : "o"(a), "o"(b));
}

inline void VU0_v3scaleadd(const UMath::Vector3 &a, const float scaleby, const UMath::Vector3 &b, UMath::Vector3 &result) {
    asm __volatile__("lqc2 vf1, %1\n"
                     "lqc2 vf2, %0\n"
                     "lqc2 vf3, %3\n"
                     "qmtc2.ni %2, vf4\n"
                     "vmulx vf3, vf1, vf4x\n"
                     "vadd vf3, vf2, vf3\n"
                     "sqc2 vf3, %0"
                     : "=o"(result)
                     : "o"(a), "r"(scaleby), "o"(b));
}

// TODO
inline void VU0_v3sub(const UMath::Vector3 &a, const UMath::Vector3 &b, UMath::Vector3 &result) {
    asm __volatile__("lqc2 vf1, %1\n"
                     "lqc2 vf2, %2\n"
                     "lqc2 vf3, %0\n"
                     "vsub vf3, vf1, vf2\n"
                     "sqc2 vf3, %0"
                     : "=o"(result)
                     : "o"(a), "o"(b));
}

inline float VU0_v3dotprod(const UMath::Vector3 &a, const UMath::Vector3 &b) {
    float result;
    asm __volatile__("lqc2 vf1, %1\n"
                     "lqc2 vf2, %2\n"
                     "vmul vf3, vf1, vf2\n"
                     "vaddy vf3, vf3, vf3y\n"
                     "vaddz vf3, vf3, vf3z\n"
                     "qmfc2.ni %0, vf3"
                     : "=r"(result)
                     : "o"(a), "o"(b));
    return result;
}

inline float VU0_v3distancesquare(const UMath::Vector3 &p1, const UMath::Vector3 &p2) {
    u_long128 _t0;
    float result;
    asm __volatile__("lui %3, 0x3f80\n"
                     "lqc2 vf1, %1\n"
                     "lqc2 vf2, %2\n"
                     "vsub vf1, vf1, vf2\n"
                     "qmtc2.ni %3, vf3\n"
                     "vmul vf2, vf1, vf1\n"
                     "vmulax ACC, vf1, vf1x\n"
                     "vmadday ACC, vf3, vf2y\n"
                     "vmaddz vf2, vf3, vf2z\n"
                     "qmfc2.ni %0, vf2\n"
                     : "=r"(result)
                     : "o"(p1), "o"(p2), "r"(_t0));
    return result;
}

inline float VU0_v3distancesquarexz(const UMath::Vector3 &p1, const UMath::Vector3 &p2) {}

// TODO
inline void VU0_v3crossprod(const UMath::Vector3 &a, const UMath::Vector3 &b, UMath::Vector3 &dest) {
    asm __volatile__("lqc2 vf1, %1\n"
                     "lqc2 vf2, %2\n"
                     "lqc2 vf3, %0\n"
                     "vopmula ACC, vf1, vf2\n"
                     "vopmsub vf3, vf2, vf1\n"
                     "sqc2 vf3, %0"
                     : "=o"(dest)
                     : "o"(a), "o"(b));
}

inline float VU0_v3lengthsquare(const UMath::Vector3 &a) {
    u_long128 _t0;
    float result;
    asm __volatile__("lui %2, 0x3f80\n"
                     "lqc2 vf1, %1\n"
                     "vmul vf2, vf1, vf1\n"
                     "qmtc2.ni %2, vf3\n"
                     "vmulax ACC, vf1, vf1x\n"
                     "vmadday ACC, vf3, vf2y\n"
                     "vmaddz vf2, vf3, vf2z\n"
                     "qmfc2.ni %0, vf2"
                     : "=r"(result)
                     : "o"(a), "r"(_t0));
    return result;
}

// inline void VU0_v3unit(const UMath::Vector3 &a, UMath::Vector3 &result) {}

inline void VU0_v4scaleadd(const UMath::Vector4 &a, const float scaleby, const UMath::Vector4 &b, UMath::Vector4 &result) {}

inline void VU0_v4scaleaddxyz(const UMath::Vector4 &a, const float scaleby, const UMath::Vector4 &b, UMath::Vector4 &result) {}

inline float VU0_v4lengthsquare(const UMath::Vector4 &a) {}

inline float VU0_v4lengthsquarexyz(const UMath::Vector4 &a) {}

inline void VU0_v4subxyz(const UMath::Vector4 &a, const UMath::Vector4 &b, UMath::Vector4 &result) {}

inline float VU0_v4dotprodxyz(const UMath::Vector4 &a, const UMath::Vector4 &b) {}

inline void VU0_v4scale(const UMath::Vector4 &a, const float scaleby, UMath::Vector4 &result) {}

inline void VU0_v4scalexyz(const UMath::Vector4 &a, const float scaleby, UMath::Vector4 &result) {}

inline float VU0_v4distancesquarexyz(const UMath::Vector4 &p1, const UMath::Vector4 &p2) {}

inline void VU0_MATRIX3x4_vect3mult(const UMath::Vector3 &v, const UMath::Matrix4 &m, UMath::Vector3 &result) {
    asm __volatile__("lqc2 vf1, %1\n"
                     "lqc2 vf2, 0x0(%2)\n"
                     "lqc2 vf3, 0x10(%2)\n"
                     "lqc2 vf4, 0x20(%2)\n"
                     "lqc2 vf5, %0\n"
                     "vmulax ACC, vf2, vf1x\n"
                     "vmadday ACC, vf3, vf1y\n"
                     "vmaddz vf5, vf4, vf1z\n"
                     "sqc2 vf5, %0"
                     : "=o"(result)
                     : "o"(v), "r"(&m));
}

inline void VU0_MATRIX4_vect3mult(const UMath::Vector3 &v, const UMath::Matrix4 &m, UMath::Vector3 &result) {
    asm __volatile__("lqc2 vf1, %1\n"
                     "lqc2 vf2, 0x0(%2)\n"
                     "lqc2 vf3, 0x10(%2)\n"
                     "lqc2 vf4, 0x20(%2)\n"
                     "lqc2 vf5, 0x30(%2)\n"
                     "lqc2 vf6, %1\n"
                     "vmulax ACC, vf2, vf1x\n"
                     "vmadday ACC, vf3, vf1y\n"
                     "vmaddaz ACC, vf4, vf1z\n"
                     "vmaddw vf6, vf5, vf0w\n"
                     "sqc2 vf6, %0"
                     : "=o"(result)
                     : "o"(v), "r"(&m));
}

#else

inline float VU0_Sin(float x) {
    return sinf(x);
}

inline float VU0_Cos(float x) {
    return cosf(x);
}

#endif

inline float VU0_fabs(const float a) {
#ifdef EA_PLATFORM_PLAYSTATION2
    float result;
    asm __volatile__("abs.s %0, %1" : "=f"(result) : "f"(a));
    return result;
#else
    if (a < 0.0f) {
        return -a;
    }
    return a;
    // return a < 0.0f ? -a : a;
#endif
}

inline float VU0_Pow(float x, float e) {
    return powf(x, e);
}

inline float VU0_floatmin(const float a, const float b) {
#ifdef EA_PLATFORM_PLAYSTATION2
    float result;
    asm __volatile__("min.s %0, %1, %2" : "=f"(result) : "f"(a), "f"(b));
    return result;
#else
    if (a < b)
        return a;
    else
        return b;
#endif
}

inline float VU0_floatmax(const float a, const float b) {
#ifdef EA_PLATFORM_PLAYSTATION2
    float result;
    asm __volatile__("max.s %0, %1, %2" : "=f"(result) : "f"(a), "f"(b));
    return result;
#else
    if (a > b)
        return a;
    else
        return b;
#endif
}

inline void VU0_v3negate(UMath::Vector3 &result) {
    result.x = -result.x;
    result.y = -result.y;
    result.z = -result.z;
}

inline float VU0_v3distance(const UMath::Vector3 &p1, const UMath::Vector3 &p2) {
    UMath::Vector3 temp;
    VU0_v3sub(p1, p2, temp);
    return VU0_sqrt(VU0_v3lengthsquare(temp));
}

// TODO these should go into UVectorMathGC.hpp
inline void VU0_v3unitcrossprod(const UMath::Vector3 &a, const UMath::Vector3 &b, UMath::Vector3 &dest) {
#ifdef EA_PLATFORM_PLAYSTATION2
    u_long128 _t0;
    asm __volatile__("lui %3, 0x3f80\n"
                     "lqc2 vf1, %1\n"
                     "lqc2 vf2, %2\n"
                     "lqc2 vf3, %0\n"
                     "vopmula ACC, vf1, vf2\n"
                     "vopmsub vf3, vf2, vf1\n"
                     "qmtc2.ni %3, vf5\n"
                     "vmul vf4, vf3, vf3\n"
                     "vmulax ACC, vf3, vf3x\n"
                     "vmadday ACC, vf5, vf4y\n"
                     "vmaddz vf4, vf5, vf4z\n"
                     "vrsqrt Q, vf0w, vf4x\n"
                     "vwaitq\n"
                     "vmulq vf3, vf3, Q\n"
                     "sqc2 vf3, %0"
                     : "=o"(dest)
                     : "o"(a), "o"(b), "r"(_t0));
#else
    VU0_v3crossprod(a, b, dest);
// VU0_v3unit(dest, dest); // TODO
#endif
}

inline void VU0_ExtractXAxis3FromQuat(const UMath::Vector4 &quat, UMath::Vector3 &result) {
    const float scale = 2.0f;
    float yy = scale * (quat.y * quat.y);
    float zz = scale * (quat.z * quat.z);
    float xy = scale * (quat.x * quat.y);
    float xz = scale * (quat.x * quat.z);
    float yw = scale * (quat.y * quat.w);
    float zw = scale * (quat.z * quat.w);

    result.x = 1.0f - (yy + zz);
    result.y = xy + zw;
    result.z = xz - yw;
}

inline void VU0_ExtractYAxis3FromQuat(const UMath::Vector4 &quat, UMath::Vector3 &result) {
    const float scale = 2.0f;
    float xx = scale * (quat.x * quat.x);
    float zz = scale * (quat.z * quat.z);
    float xy = scale * (quat.x * quat.y);
    float xw = scale * (quat.x * quat.w);
    float yz = scale * (quat.y * quat.z);
    float zw = scale * (quat.z * quat.w);

    result.x = xy - zw;
    result.y = 1.0f - (xx + zz);
    result.z = yz + xw;
}

inline void VU0_ExtractZAxis3FromQuat(const UMath::Vector4 &quat, UMath::Vector3 &result) {
    const float scale = 2.0f;
    float xx = scale * (quat.x * quat.x);
    float yy = scale * (quat.y * quat.y);
    float xz = scale * (quat.x * quat.z);
    float xw = scale * (quat.x * quat.w);
    float yz = scale * (quat.y * quat.z);
    float yw = scale * (quat.y * quat.w);

    result.x = xz + yw;
    result.y = yz - xw;
    result.z = 1.0f - (xx + yy);
}

inline void VU0_qmul(const UMath::Vector4 &b, const UMath::Vector4 &a, UMath::Vector4 &dest) {
    UMath::Vector4 result;
    result.x = a.y * b.z - a.z * b.y + a.w * b.x + a.x * b.w;
    result.y = a.z * b.x - a.x * b.z + a.w * b.y + a.y * b.w;
    result.z = a.x * b.y - a.y * b.x + a.w * b.z + a.z * b.w;
    result.w = a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z;

    dest = result;
}

inline void VU0_quattom4(const UMath::Vector4 &quat, UMath::Matrix4 &result) {
    const float scale = 2.0f;
    float xx = quat.x * quat.x * scale;
    float yy = quat.y * quat.y * scale;
    float zz = quat.z * quat.z * scale;

    float xy = quat.x * quat.y * scale;
    float xz = quat.x * quat.z * scale;
    float xw = quat.x * quat.w * scale;

    float yz = quat.y * quat.z * scale;
    float yw = quat.y * quat.w * scale;
    float zw = quat.z * quat.w * scale;

    result[0][0] = 1.0f - (yy + zz);
    result[0][1] = (xy + zw);
    result[0][2] = (xz - yw);
    result[0][3] = 0.0f;

    result[1][0] = (xy - zw);
    result[1][1] = 1.0f - (xx + zz);
    result[1][2] = (yz + xw);
    result[1][3] = 0.0f;

    result[2][0] = (xz + yw);
    result[2][1] = (yz - xw);
    result[2][2] = 1.0f - (xx + yy);
    result[2][3] = 0.0f;

    result[3][0] = 0.0f;
    result[3][1] = 0.0f;
    result[3][2] = 0.0f;
    result[3][3] = 1.0f;
}

inline void VU0_MATRIX4_transpose(const UMath::Matrix4 &m, UMath::Matrix4 &result) {
    if (&m == &result) {
        UMath::Matrix4 temp;
        int i;
        int j;
        for (i = 0; i < 4 * 4; ++i) {
            temp.GetElements()[i] = m.GetElements()[i];
        }
        for (i = 0; i < 4; ++i) {
            for (j = 0; j < 4; ++j) {
                result[i][j] = temp[j][i];
            }
        }
    } else {
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                result[i][j] = m[j][i];
            }
        }
    }
}

inline void VU0_qtranspose(const UMath::Vector4 &a, UMath::Vector4 &result) {
    result.x = -a.x;
    result.y = -a.y;
    result.z = -a.z;
    result.w = a.w;
}

inline void VU0_MATRIX4Init(UMath::Matrix4 &dest, const float xx, const float yy, const float zz) {
    dest[2][2] = zz;
    dest[1][1] = yy;
    dest[0][0] = xx;
    dest[3][3] = 1.0f;

    // TODO UNSOLVED
    dest[3][2] = 0.0f;
    dest[3][1] = 0.0f;
    dest[3][0] = 0.0f;
    dest[2][3] = 0.0f;
    dest[2][1] = 0.0f;
    dest[2][0] = 0.0f;
    dest[1][3] = 0.0f;
    dest[1][2] = 0.0f;
    dest[1][0] = 0.0f;
    dest[0][3] = 0.0f;
    dest[0][2] = 0.0f;
    dest[0][1] = 0.0f;
}

inline void VU0_MATRIX4_mult(const UMath::Matrix4 &m1, const UMath::Matrix4 &m2, UMath::Matrix4 &result) {
    UMath::Matrix4 temp;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result[i][j] = m1[i][0] * m2[0][j] + m1[i][1] * m2[1][j] + m1[i][2] * m2[2][j] + m1[i][3] * m2[3][j];
        }
    }

    result = temp;
}

inline void VU0_MATRIX4Copy(const UMath::Matrix4 &a, UMath::Matrix4 &b) {
#ifdef EA_PLATFORM_PLAYSTATION2
    u_long128 _t0;
    u_long128 _t1;
    asm __volatile__("lq %0, 0x0(%2)\n"
                     "lq %1, 0x10(%2)\n"
                     "sq %0, %3\n"
                     "sq %1, %4\n"
                     "lq %0, 0x20(%2)\n"
                     "lq %1, 0x30(%2)\n"
                     "sq %0, %5\n"
                     "sq %1, %6\n"
                     :
                     : "r"(_t0), "r"(_t1), "r"(&a), "o"(b.v0), "o"(b.v1), "o"(b.v2), "o"(b.v3));
#else
    b = a;
#endif
}

// TODO
inline void VU0_v4Copy(const UMath::Vector4 &a, UMath::Vector4 &b) {
#ifdef EA_PLATFORM_PLAYSTATION2
    // u_long128 _t0;
    // u_long128 _t1;
    // asm __volatile__("lq %0, 0x0(%2)\n"
    //                  "lq %1, 0x10(%2)\n"
    //                  "sq %0, %3\n"
    //                  "sq %1, %4\n"
    //                  "lq %0, 0x20(%2)\n"
    //                  "lq %1, 0x30(%2)\n"
    //                  "sq %0, %5\n"
    //                  "sq %1, %6\n"
    //                  :
    //                  : "r"(_t0), "r"(_t1), "r"(&a), "o"(b.v0), "o"(b.v1), "o"(b.v2), "o"(b.v3));
#else
    b = a;
    // *reinterpret_cast<int *>(&b.x) = *reinterpret_cast<const int *>(&a.x);
    // *reinterpret_cast<int *>(&b.y) = *reinterpret_cast<const int *>(&a.y);
    // *reinterpret_cast<int *>(&b.z) = *reinterpret_cast<const int *>(&a.z);
    // *reinterpret_cast<int *>(&b.w) = *reinterpret_cast<const int *>(&a.w);
#endif
}

inline float VU0_v3length(const struct UMath::Vector3 &a) {
#ifdef EA_PLATFORM_PLAYSTATION2
    u_long128 _t0;
    float result;
    asm __volatile__("lui %2, 0x3f80\n"
                     "lqc2 vf1, %1\n"
                     "vmul vf2, vf1, vf1\n"
                     "qmtc2.ni %2, vf3\n"
                     "vmulax ACC, vf1, vf1x\n"
                     "vmadday ACC, vf3, vf2y\n"
                     "vmaddz vf2, vf3, vf2z\n"
                     "vsqrt Q, vf2x\n"
                     "vwaitq\n"
                     "vaddq vf3, vf0, Q\n"
                     "qmfc2.ni %0, vf3"
                     : "=r"(result)
                     : "o"(a), "r"(_t0));
    return result;
#else
    return VU0_sqrt(VU0_v3lengthsquare(a));
#endif
}

inline float VU0_v3lengthxz(const struct UMath::Vector3 &a) {
#ifdef EA_PLATFORM_PLAYSTATION2
    u_long128 _t0;
    float result;
    asm __volatile__("lui %2, 0x3f80\n"
                     "lqc2 vf1, %1\n"
                     "vmul vf2, vf1, vf1\n"
                     "qmtc2.ni %2, vf3\n"
                     "vmulax ACC, vf1, vf1x\n"
                     "vmaddz vf2, vf3, vf2z\n"
                     "vsqrt Q, vf2x\n"
                     "vwaitq\n"
                     "vaddq vf3, vf0, Q\n"
                     "qmfc2.ni %0, vf3"
                     : "=r"(result)
                     : "o"(a), "r"(_t0));
    return result;
#else
    return VU0_sqrt(a.x * a.x + a.z * a.z);
#endif
}

inline float VU0_v4lengthxyz(const UMath::Vector4 &a) {
    return VU0_sqrt(VU0_v4lengthsquarexyz(a));
}

inline void VU0_v3unit(const UMath::Vector3 &a, UMath::Vector3 &result) {
    float rlen = VU0_rsqrt(VU0_v3lengthsquare(a));
    VU0_v3scale(a, rlen, result);
}

inline void VU0_v4unit(const UMath::Vector4 &a, UMath::Vector4 &result) {
    float rlen = VU0_rsqrt(VU0_v4lengthsquare(a));
    VU0_v4scale(a, rlen, result);
}

inline void VU0_v4unitxyz(const UMath::Vector4 &a, UMath::Vector4 &result) {
    float rlen = VU0_rsqrt(VU0_v4lengthsquarexyz(a));
    VU0_v4scalexyz(a, rlen, result);
}

inline float IntAsFloat(const int &i) {
    return *reinterpret_cast<const float *>(&i);
}

inline float V3DistanceSquared(const UMath::Vector3 &a, const UMath::Vector3 &b) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    float dz = a.z - b.z;

    return dx * dx + dy * dy + dz * dz;
}

// TODO where to put these? TODO only one of them uses IntAsFloat actually
static const float kFloatScaleUp = IntAsFloat(0x7E800000);
static const float kFloatScaleDown = IntAsFloat(0x80000000);

#endif
