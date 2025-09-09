#ifndef SUPPORT_UTILITY_UTYPES_H
#define SUPPORT_UTILITY_UTYPES_H

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

} // namespace UMath

#endif
