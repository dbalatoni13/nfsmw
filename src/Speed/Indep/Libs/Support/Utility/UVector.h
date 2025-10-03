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

    // TODO these shouldn't be member functions
    const UVector3 &operator=(const UVector3 &From) {
        x = From.x;
        y = From.y;
        z = From.z;
        return *this;
    }

    UVector3 &operator/=(const float scalar) {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
    }

    void operator*=(float scalar) {
        UMath::Scale(*this, scalar, *this);
    }

    UVector3 &operator+=(const UVector3 &v) {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    UVector3 &operator-=(const UVector3 &v) {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }

    UVector3 operator+(const UVector3 &v) {
        UVector3 result;
        result.x = x + v.x;
        result.y = y + v.y;
        result.z = z + v.z;
        return result;
    }

    UVector3 operator+(const float b) {
        UVector3 v;
        v.x = this->x + b;
        v.y = this->y + b;
        v.z = this->z + b;
        return v;
    }

    UVector3 operator-(const UVector3 &b) {
        UVector3 v;
        v.x = this->x - b.x;
        v.y = this->y - b.y;
        v.z = this->z - b.z;
        return v;
    }

    UVector3 operator-(const float b) {
        UVector3 v;
        v.x = this->x - b;
        v.y = this->y - b;
        v.z = this->z - b;
        return v;
    }

    UVector3 operator/(const UVector3 &b) {
        UVector3 v;
        v.x = this->x / b.x;
        v.y = this->y / b.y;
        v.z = this->z / b.z;
        return v;
    }

    UVector3 operator/(const float b) {
        UVector3 v;
        v.x = this->x / b;
        v.y = this->y / b;
        v.z = this->z / b;
        return v;
    }

    UVector3 operator=(const float b) {
        UVector3 v;
        v.x = b;
        v.y = b;
        v.z = b;
        return v;
    }

    // UVector3 &UVector3::operator=(const struct Vector4 &b);
};

inline UVector3 operator*(const UVector3 &v1, float scalar) {
    UMath::Vector3 result;
    UMath::Scale(v1, scalar, result);
    return UVector3(result);
}

#endif
