#ifndef SUPPORT_UTILITY_UVECTOR_H
#define SUPPORT_UTILITY_UVECTOR_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "UMath.h"

struct UVector3 : public UMath::Vector3 {
    UVector3() {
        Clear(*this);
    }

    float &operator[](int index) {
        return (&x)[index];
    }

    UVector3(const Vector3 &From) {
        x = From.x;
        y = From.y;
        z = From.z;
    }

    UVector3(const UMath::Vector4 &From) {
        x = From.x;
        y = From.y;
        z = From.z;
    }

    UVector3(const float f) {
        x = f;
        y = f;
        z = f;
    }

    UVector3(float fx, float fy, float fz) {
        x = fx;
        y = fy;
        z = fz;
    }

    float Magnitude() const {
        return UMath::Length(*this);
    }

    const UVector3 &operator=(const Vector3 &From) {
        x = From.x;
        y = From.y;
        z = From.z;
        return *this;
    }

    // unused
    void operator/=(const float scalar) {
        x /= scalar;
        y /= scalar;
        z /= scalar;
    }

    void operator*=(float scalar) {
        UMath::Scale(*this, scalar, *this);
    }

    void operator*=(UMath::Matrix4 &m) {
        UMath::Vector3 r;
        UMath::Rotate(*this, m, r);
        *this = r;
    }

    void operator+=(const UMath::Vector3 &v) {
        UMath::Add(*this, v, *this);
    }

    void operator-=(const UMath::Vector3 &v) {
        UMath::Sub(*this, v, *this);
    }
};

inline UVector3 operator*(const UVector3 &v1, float scalar) {
    UMath::Vector3 result;
    UMath::Scale(v1, scalar, result);
    return UVector3(result);
}

inline UVector3 operator+(const UVector3 &v1, const UVector3 &v2) {
    UMath::Vector3 result;
    UMath::Add(v1, v2, result);
    return result;
}

inline UVector3 operator-(const UVector3 &v1, const UMath::Vector3 &b) {
    UMath::Vector3 result;
    UMath::Sub(v1, b, result);
    return result;
}

#endif
