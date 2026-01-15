#ifndef SUPPORT_UTILITY_UTYPES_H
#define SUPPORT_UTILITY_UTYPES_H

#include "types.h"
#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bMath.hpp"

namespace UMath {

struct Vector2 {
    // total size: 0x8
    float x; // offset 0x0, size 0x4
    float y; // offset 0x4, size 0x4

    static const Vector2 kZero;

    float &operator[](int index) {
        return (&x)[index];
    }
};

struct Vector3 {
    // total size: 0xC
    float x; // offset 0x0, size 0x4
    float y; // offset 0x4, size 0x4
    float z; // offset 0x8, size 0x4

    static const Vector3 kZero;

    operator const bVector3 &() const {
        return *reinterpret_cast<const bVector3 *>(this);
    }

    const float &operator[](int index) const {
        return (&x)[index];
    }

    float &operator[](int index) {
        return (&x)[index];
    }
};

struct ALIGN_16 Vector4 {
    // total size: 0x10
    float x; // offset 0x0, size 0x4
    float y; // offset 0x4, size 0x4
    float z; // offset 0x8, size 0x4
    float w; // offset 0xC, size 0x4

    static const Vector4 kZero;
    static const Vector4 kIdentity;

    operator const bVector4 &() const {
        return *reinterpret_cast<const bVector4 *>(this);
    }

    const float &operator[](int index) const {
        return (&x)[index];
    }

    float &operator[](int index) {
        return (&x)[index];
    }
};

struct Matrix3 {
    Vector3 v0, v1, v2;

    static const Matrix3 kZero;
    static const Matrix3 kIdentity;

    // float *GetElements() {}

    // const float *GetElements() const {}

    const Vector3 &operator[](int index) const {
        return (&v0)[index];
    }

    Vector3 &operator[](int index) {
        return (&v0)[index];
    }
};

struct ALIGN_16 Matrix4 {
    Vector4 v0, v1, v2, v3;

    static const Matrix4 kZero;
    static const Matrix4 kIdentity;

    float *GetElements() {
        return &v0.x;
    }

    const float *GetElements() const {
        return &v0.x;
    }

    const Vector4 &operator[](int index) const {
        return (&v0)[index];
    }

    Vector4 &operator[](int index) {
        return (&v0)[index];
    }
};

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

inline UMath::Vector3 Vector3Make(float x, float y, float z) {
    Vector3 c;

    c.x = x;
    c.y = y;
    c.z = z;

    return c;
}

// TODO PS2
inline Vector4 Vector4Make(const Vector3 &c, float w) {
    Vector4 res;
    res.x = c.x;
    res.y = c.y;
    res.z = c.z;
    res.w = w;
    return res;
}

typedef Vector4 Quaternion;

} // namespace UMath

#endif
