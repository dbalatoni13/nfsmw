#ifndef CAMERA_ICE_ICEMATH_H
#define CAMERA_ICE_ICEMATH_H

#include "Speed/Indep/Src/Ecstasy/eMath.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

void bMulMatrix(bVector4 *dest, const bMatrix4 *a, const bVector4 *b);

inline void bQuaternionToMatrix(bMatrix4 *matrix, const bQuaternion *quaternion) {
    quaternion->GetMatrix(matrix);
}

namespace ICE {

// total size: 0x8
struct Vector2 {
    float x; // offset 0x0, size 0x4
    float y; // offset 0x4, size 0x4
};

// total size: 0x10
struct Vector3 {
    Vector3() { x = 0.0f; y = 0.0f; z = 0.0f; pad = 0.0f; }
    Vector3(float _x, float _y, float _z) { x = _x; y = _y; z = _z; pad = 0.0f; }
    Vector3(const Vector3 &v) { bCopy(reinterpret_cast<bVector3 *>(this), reinterpret_cast<const bVector3 *>(&v)); }

    float x;   // offset 0x0, size 0x4
    float y;   // offset 0x4, size 0x4
    float z;   // offset 0x8, size 0x4
    float pad; // offset 0xC, size 0x4
};

// total size: 0x10
struct Vector4 {
    Vector4() { x = 0.0f; y = 0.0f; z = 0.0f; w = 0.0f; }

    float x; // offset 0x0, size 0x4
    float y; // offset 0x4, size 0x4
    float z; // offset 0x8, size 0x4
    float w; // offset 0xC, size 0x4
};

// total size: 0x40
struct Matrix4 {
    Matrix4() {}

    struct Vector4 v0; // offset 0x0, size 0x10
    struct Vector4 v1; // offset 0x10, size 0x10
    struct Vector4 v2; // offset 0x20, size 0x10
    struct Vector4 v3; // offset 0x30, size 0x10
};

inline void Copy(Vector3 *dst, const Vector3 *src) {
    bCopy(reinterpret_cast<bVector3 *>(dst), reinterpret_cast<const bVector3 *>(src));
}

inline void Add(Vector3 *dst, const Vector3 *v1, const Vector3 *v2) {
    bAdd(reinterpret_cast<bVector3 *>(dst), reinterpret_cast<const bVector3 *>(v1), reinterpret_cast<const bVector3 *>(v2));
}

inline void Sub(Vector3 *dst, const Vector3 *v1, const Vector3 *v2) {
    bSub(reinterpret_cast<bVector3 *>(dst), reinterpret_cast<const bVector3 *>(v1), reinterpret_cast<const bVector3 *>(v2));
}

inline void Scale(Vector3 *dst, const Vector3 *src, float scale) {
    bScale(reinterpret_cast<bVector3 *>(dst), reinterpret_cast<const bVector3 *>(src), scale);
}

inline void Scale(Vector3 *dst, const Vector3 *src, const Vector3 *scale) {
    bScale(reinterpret_cast<bVector3 *>(dst), reinterpret_cast<const bVector3 *>(src), reinterpret_cast<const bVector3 *>(scale));
}

inline void Lerp(Vector3 *dst, const Vector3 *v1, const Vector3 *v2, float lerp) {
    Vector3 vdiff;
    Sub(&vdiff, v1, v2);
    Scale(&vdiff, &vdiff, lerp);
    Add(dst, &vdiff, v2);
}

inline void ScaleAdd(Vector3 *dst, const Vector3 *v1, const Vector3 *vscale, float scale) {
    bScaleAdd(reinterpret_cast<bVector3 *>(dst), reinterpret_cast<const bVector3 *>(v1), reinterpret_cast<const bVector3 *>(vscale),
              scale);
}

inline float DistBetween(const Vector3 *v1, const Vector3 *v2) {
    return bDistBetween(*reinterpret_cast<const bVector3 *>(v1), *reinterpret_cast<const bVector3 *>(v2));
}

inline float Length(const Vector3 *src) {
    return bLength(*reinterpret_cast<const bVector3 *>(src));
}

inline float Abs(float x) {
    return fabsf(x);
}

inline float Sqrt(float x) {
    return bSqrt(x);
}

inline void Copy(Matrix4 *dst, const Matrix4 *src) {
    bCopy(reinterpret_cast<bMatrix4 *>(dst), reinterpret_cast<const bMatrix4 *>(src));
}

inline float SignExtendAng(bAngle angle) {
    return static_cast<float>((static_cast<int>(angle) << 16) >> 16);
}

inline float Tan(bAngle angle) {
    return bTan(angle);
}

inline bAngle ATan(float x, float y = 15.960001f) {
    return bATan(x, y);
}

inline float Min(float a, float b) {
    return bMin(a, b);
}

inline float Max(float a, float b) {
    return bMax(a, b);
}

inline int Clamp(int a, int min, int max) {
    return bClamp(a, min, max);
}

inline float Clamp(float a, float min, float max) {
    return bClamp(a, min, max);
}

inline void Clamp(Vector3 *dst, const Vector3 *min, const Vector3 *max) {
    float x = Clamp(dst->x, min->x, max->x);
    float y = Clamp(dst->y, min->y, max->y);
    float z = Clamp(dst->z, min->z, max->z);

    dst->x = x;
    dst->y = y;
    dst->z = z;
}

inline float ToDegrees(float a) {
    return a * 360.0f;
}

// VEDADA la forma clasica `a -= c; ... return a;`: deja el resultado en el pseudo
// del bucle y parte en dos la vida de `frame` (r48/r51, 1.606 compilaciones).
// Con returns, `a` es de solo lectura tras el binding y el retorno del inline
// cae directo en el pseudo del llamador: lwz r8 / mr r11, r8 / subf r8.
inline int SignedMod(int a, int b) {
    if (b > 0) {
        while (a < 0) {
            a += b;
        }
        int c = a / b;
        c = c * b;
        return a - c;
    } else {
        return 0;
    }
}

inline int FloatToInt(float x) {
    return static_cast<int>(x);
}

inline float IntToFloat(int x) {
    return static_cast<float>(x);
}

inline float Random(float x) {
    return bRandom(x);
}

inline void Fill(Vector4 *dst, const Vector3 *src, float w) {
    dst->x = src->x;
    dst->y = src->y;
    dst->z = src->z;
    dst->w = w;
}

inline void Copy(Vector4 *dst, const Vector3 *src, float w) {
    bCopy(reinterpret_cast<bVector4 *>(dst), reinterpret_cast<const bVector3 *>(src), w);
}

inline void Normalize(Vector3 *v) {
    bNormalize(reinterpret_cast<bVector3 *>(v), reinterpret_cast<const bVector3 *>(v));
}

inline void Normalize(Vector3 *dst, const Vector3 *src) {
    bNormalize(reinterpret_cast<bVector3 *>(dst), reinterpret_cast<const bVector3 *>(src));
}

inline float Dot(const Vector3 *v1, const Vector3 *v2) {
    return bDot(reinterpret_cast<const bVector3 *>(v1), reinterpret_cast<const bVector3 *>(v2));
}

inline void Cross(Vector3 *dst, const Vector3 *v1, const Vector3 *v2) {
    bCross(reinterpret_cast<bVector3 *>(dst), reinterpret_cast<const bVector3 *>(v1), reinterpret_cast<const bVector3 *>(v2));
}

inline void Identity(Matrix4 *m) {
    bIdentity(reinterpret_cast<bMatrix4 *>(m));
}

inline void QuatToMatrix(Matrix4 *dst, const bQuaternion *q) {
    bQuaternion bq(q->x, q->y, q->z, q->w);
    bQuaternionToMatrix(reinterpret_cast<bMatrix4 *>(dst), &bq);
}

inline void MulVector(Vector3 *dst, const Matrix4 *m, const Vector3 *v) {
    eMulVector(reinterpret_cast<bVector3 *>(dst), reinterpret_cast<const bMatrix4 *>(m), reinterpret_cast<const bVector3 *>(v));
}

inline void MulMatrix(Vector4 *dst, const Matrix4 *a, const Vector4 *b) {
    bMulMatrix(reinterpret_cast<bVector4 *>(dst), reinterpret_cast<const bMatrix4 *>(a), reinterpret_cast<const bVector4 *>(b));
}

inline void MulMatrix(Vector3 *dst, const Matrix4 *a, const Vector3 *b) {
    bMulMatrix(reinterpret_cast<bVector3 *>(dst), reinterpret_cast<const bMatrix4 *>(a), reinterpret_cast<const bVector3 *>(b));
}

inline void MulMatrix(Matrix4 *dst, const Matrix4 *a, const Matrix4 *b) {
    bMulMatrix(reinterpret_cast<bMatrix4 *>(dst), reinterpret_cast<const bMatrix4 *>(a), reinterpret_cast<const bMatrix4 *>(b));
}

inline void Copy(Matrix4 *dst, const Matrix4 *src, const Vector3 *position) {
    bCopy(reinterpret_cast<bMatrix4 *>(dst), reinterpret_cast<const bMatrix4 *>(src),
          reinterpret_cast<const bVector3 *>(position));
}

inline int StrNICmp(const char *s1, const char *s2, int n) {
    return bStrNICmp(s1, s2, n);
}

inline void Invert(Matrix4 *dst, const Matrix4 *src) {
    eInvertTransformationMatrix(reinterpret_cast<bMatrix4 *>(dst), reinterpret_cast<const bMatrix4 *>(src));
}

}; // namespace ICE

#endif
