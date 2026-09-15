#ifndef CAMERA_ICE_ICEMATH_H
#define CAMERA_ICE_ICEMATH_H

#include "Speed/Indep/Src/Ecstasy/eMath.hpp"
#include "Speed\Indep\Libs\Support\Utility\UTypes.h"
#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

namespace ICE {

// total size: 0x8
struct Vector2 {
    float x; // offset 0x0, size 0x4
    float y; // offset 0x4, size 0x4
};

// total size: 0x10
struct Vector3 {
    Vector3() {
        x = 0.0f;
        y = 0.0f;
        z = 0.0f;
        pad = 0.0f;
    }

    float x;   // offset 0x0, size 0x4
    float y;   // offset 0x4, size 0x4
    float z;   // offset 0x8, size 0x4
    float pad; // offset 0xC, size 0x4
};

// total size: 0x10
struct Vector4 {
    float x; // offset 0x0, size 0x4
    float y; // offset 0x4, size 0x4
    float z; // offset 0x8, size 0x4
    float w; // offset 0xC, size 0x4
};

// total size: 0x40
struct Matrix4 {
    struct Vector4 v0; // offset 0x0, size 0x10
    struct Vector4 v1; // offset 0x10, size 0x10
    struct Vector4 v2; // offset 0x20, size 0x10
    struct Vector4 v3; // offset 0x30, size 0x10
};

inline void MulMatrix(UMath::Vector3 *dst, const UMath::Matrix4 *a, const UMath::Vector3 *b) {
    bMulMatrix(reinterpret_cast<bVector3 *>(dst), reinterpret_cast<const bMatrix4 *>(a), reinterpret_cast<const bVector3 *>(b));
}

inline void MulVector(Vector3 *dst, const Matrix4 *m, const Vector3 *v) {
    eMulVector(reinterpret_cast<bVector3 *>(dst), reinterpret_cast<const bMatrix4 *>(m), reinterpret_cast<const bVector3 *>(v));
}

inline void Identity(struct UMath::Matrix4 *m) {
    bIdentity(reinterpret_cast<bMatrix4 *>(m));
}

inline void Identity(struct ICE::Matrix4 *m) {
    bIdentity(reinterpret_cast<bMatrix4 *>(m));
}

inline int Clamp(int a, int min, int max) {
    return bClamp(a, min, max);
}

inline void Add(struct UMath::Vector3 *dst, const struct UMath::Vector3 *v1, const struct UMath::Vector3 *v2) {
    bAdd(reinterpret_cast<bVector3 *>(dst), reinterpret_cast<const bVector3 *>(v1), reinterpret_cast<const bVector3 *>(v2));
}

inline void Normalize(struct UMath::Vector3 *dst, const struct UMath::Vector3 *src) {
    bNormalize(reinterpret_cast<bVector3 *>(dst), reinterpret_cast<const bVector3 *>(src));
}

inline float Dot(const struct UMath::Vector3 *v1, const struct UMath::Vector3 *v2) {
    return bDot(reinterpret_cast<const bVector3 *>(v1), reinterpret_cast<const bVector3 *>(v2));
}

inline void Sub(struct UMath::Vector3 *dst, const struct UMath::Vector3 *v1, const struct UMath::Vector3 *v2) {
    bSub(reinterpret_cast<bVector3 *>(dst), reinterpret_cast<const bVector3 *>(v1), reinterpret_cast<const bVector3 *>(v2));
}

}; // namespace ICE

#endif
