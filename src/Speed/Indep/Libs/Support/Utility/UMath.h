#ifndef SUPPORT_UTILITY_UMATH_H
#define SUPPORT_UTILITY_UMATH_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UVectorMath.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

namespace UMath {

// TODO check if the functions are correct/exist
struct Vector2 {
    // total size: 0x8
    float x; // offset 0x0, size 0x4
    float y; // offset 0x4, size 0x4

    static const Vector2 kZero;

    float &operator[](int index) {
        return (&x)[index];
    }
};

const Vector2 Vector2::kZero = Vector2();

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

const Vector3 Vector3::kZero = {};

struct Vector4 {
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

const Vector4 Vector4::kZero = {};
const Vector4 Vector4::kIdentity = {0.0f, 0.0f, 0.0f, 1.f};

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

struct Matrix4 {
    Vector4 v0, v1, v2, v3;

    static const Matrix4 kZero;
    static const Matrix4 kIdentity;

    // float *GetElements() {}

    // const float *GetElements() const {}

    const Vector4 &operator[](int index) const {
        return (&v0)[index];
    }

    Vector4 &operator[](int index) {
        return (&v0)[index];
    }
};

const Matrix4 Matrix4::kZero = {};
const Matrix4 Matrix4::kIdentity = {{1.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.f}};

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

inline void Scale(const struct Vector3 &a, const float s, struct Vector3 &r) {
    VU0_v3scale(a, s, r);
}

inline void Scale(const struct Vector4 &a, const float s, struct Vector4 &r) {
    VU0_v4scale(a, s, r);
}

} // namespace UMath

#endif
