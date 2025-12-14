#ifndef CAMERA_ICE_ICEMATH_H
#define CAMERA_ICE_ICEMATH_H

#include "Speed/Indep/Src/Ecstasy/eMath.hpp"
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

inline void MulVector(Vector3 *dst, const Matrix4 *m, const Vector3 *v) {
    eMulVector(reinterpret_cast<bVector3 *>(dst), reinterpret_cast<const bMatrix4 *>(m), reinterpret_cast<const bVector3 *>(v));
}

}; // namespace ICE

#endif
