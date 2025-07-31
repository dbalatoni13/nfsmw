#ifndef SUPPORT_UTILITY_UVECTOR_MATH_H
#define SUPPORT_UTILITY_UVECTOR_MATH_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

namespace UMath {

struct Vector2 {
    static const Vector2 kZero;

    float x, y;

    Vector2() {}

    float &operator[](int index) {
        return (&x)[index];
    }

    Vector2(const float f) {
        x = f;
        y = f;
    }

    Vector2(const float fx, const float fy) {
        x = fx;
        y = fy;
    }

    Vector2 operator+(const Vector2 &b) {
        Vector2 v;
        v.x = this->x + b.x;
        v.y = this->y + b.y;
        return v;
    }

    Vector2 operator+(const float b) {
        Vector2 v;
        v.x = this->x + b;
        v.y = this->y + b;
        return v;
    }

    Vector2 operator-(const Vector2 &b) {
        Vector2 v;
        v.x = this->x - b.x;
        v.y = this->y - b.y;
        return v;
    }

    Vector2 operator-(const float b) {
        Vector2 v;
        v.x = this->x - b;
        v.y = this->y - b;
        return v;
    }

    Vector2 operator*(const Vector2 &b) {
        Vector2 v;
        v.x = this->x * b.x;
        v.y = this->y * b.y;
        return v;
    }

    Vector2 operator*(const float b) {
        Vector2 v;
        v.x = this->x * b;
        v.y = this->y * b;
        return v;
    }

    Vector2 operator/(const Vector2 &b) {
        Vector2 v;
        v.x = this->x / b.x;
        v.y = this->y / b.y;
        return v;
    }

    Vector2 operator/(const float b) {
        Vector2 v;
        v.x = this->x / b;
        v.y = this->y / b;
        return v;
    }

    Vector2 operator=(const float b) {
        Vector2 v;
        v.x = b;
        v.y = b;
        return v;
    }
};

const Vector2 Vector2::kZero = Vector2();

struct Vector3 {
    static const Vector3 kZero;

    float x, y, z;

    Vector3() {}

    float &operator[](int index) {
        return (&x)[index];
    }

    Vector3(const Vector3 &From) {
        x = From.x;
        y = From.y;
        z = From.z;
    }

    Vector3(const float f) {
        x = f;
        y = f;
        z = f;
    }

    Vector3(const float fx, const float fy, const float fz) {
        x = fx;
        y = fy;
        z = fz;
    }

    /* Vector3& operator=(const Vector3& From)
{
    x = From.x;
    y = From.y;
    z = From.z;
    return *this;
} */

    Vector3 &operator*=(const Vector3 &b) {
        x *= b.x;
        y *= b.y;
        z *= b.z;
        return *this;
    }

    Vector3 &operator/=(const float scalar) {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
    }

    Vector3 &operator*=(const float scalar) {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }

    Vector3 &operator+=(const Vector3 &v) {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    Vector3 &operator-=(const Vector3 &v) {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }

    Vector3 operator+(const Vector3 &v) {
        Vector3 result;
        result.x = x + v.x;
        result.y = y + v.y;
        result.z = z + v.z;
        return result;
    }

    Vector3 operator+(const float b) {
        Vector3 v;
        v.x = this->x + b;
        v.y = this->y + b;
        v.z = this->z + b;
        return v;
    }

    Vector3 operator-(const Vector3 &b) {
        Vector3 v;
        v.x = this->x - b.x;
        v.y = this->y - b.y;
        v.z = this->z - b.z;
        return v;
    }

    Vector3 operator-(const float b) {
        Vector3 v;
        v.x = this->x - b;
        v.y = this->y - b;
        v.z = this->z - b;
        return v;
    }

    Vector3 operator*(const Vector3 &b) {
        Vector3 v;
        v.x = this->x * b.x;
        v.y = this->y * b.y;
        v.z = this->z * b.z;
        return v;
    }

    Vector3 operator*(const float b) {
        Vector3 v;
        v.x = this->x * b;
        v.y = this->y * b;
        v.z = this->z * b;
        return v;
    }

    Vector3 operator/(const Vector3 &b) {
        Vector3 v;
        v.x = this->x / b.x;
        v.y = this->y / b.y;
        v.z = this->z / b.z;
        return v;
    }

    Vector3 operator/(const float b) {
        Vector3 v;
        v.x = this->x / b;
        v.y = this->y / b;
        v.z = this->z / b;
        return v;
    }

    Vector3 operator=(const float b) {
        Vector3 v;
        v.x = b;
        v.y = b;
        v.z = b;
        return v;
    }

    // Vector3 &Vector3::operator=(const struct Vector4 &b);
};

const Vector3 Vector3::kZero = Vector3();

struct Vector4 {
    static const Vector4 kZero;
    static const Vector4 kIdentity;

    float x, y, z, w;

    Vector4() {}

    float &operator[](int index) {
        return (&x)[index];
    }

    Vector4(const float in) {
        x = in;
        y = in;
        z = in;
        w = in;
    }

    Vector4(const float inX, const float inY, const float inZ, const float inW) {
        x = inX;
        y = inY;
        z = inZ;
        w = inW;
    }

    Vector4(const Vector3 &v, const float inW) {
        w = inW;
        x = v.x;
        y = v.y;
        z = v.z;
    }

    Vector4 operator+(const Vector4 &b) {
        Vector4 v;
        v.x = this->x + b.x;
        v.y = this->y + b.y;
        v.z = this->z + b.z;
        v.w = this->w + b.z;
        return v;
    }

    Vector4 operator+(const float b) {
        Vector4 v;
        v.x = this->x + b;
        v.y = this->y + b;
        v.z = this->z + b;
        v.w = this->w + b;
        return v;
    }

    Vector4 operator-(const Vector4 &b) {
        Vector4 v;
        v.x = this->x - b.x;
        v.y = this->y - b.y;
        v.z = this->z - b.z;
        v.w = this->w - b.z;
        return v;
    }

    Vector4 operator-(const float b) {
        Vector4 v;
        v.x = this->x - b;
        v.y = this->y - b;
        v.z = this->z - b;
        v.w = this->w - b;
        return v;
    }

    Vector4 operator*(const Vector4 &b) {
        Vector4 v;
        v.x = this->x * b.x;
        v.y = this->y * b.y;
        v.z = this->z * b.z;
        v.w = this->w * b.z;
        return v;
    }

    Vector4 operator*(const float b) {
        Vector4 v;
        v.x = this->x * b;
        v.y = this->y * b;
        v.z = this->z * b;
        v.w = this->w * b;
        return v;
    }

    Vector4 operator/(const Vector4 &b) {
        Vector4 v;
        v.x = this->x / b.x;
        v.y = this->y / b.y;
        v.z = this->z / b.z;
        v.w = this->w / b.z;
        return v;
    }

    Vector4 operator/(const float b) {
        Vector4 v;
        v.x = this->x / b;
        v.y = this->y / b;
        v.z = this->z / b;
        v.w = this->w / b;
        return v;
    }

    Vector4 operator=(const float b) {
        Vector4 v;
        v.x = b;
        v.y = b;
        v.z = b;
        v.w = b;
        return v;
    }

    Vector4 &operator=(const Vector3 &b) {
        x = b.x;
        y = b.y;
        z = b.z;
        return *this;
    }
};

const Vector4 Vector4::kZero = Vector4();
const Vector4 Vector4::kIdentity = Vector4(0.0f, 0.0f, 0.0f, 1.f);

// Vector3 &Vector3::operator=(const Vector4 &b) {
//     x = b.x;
//     y = b.y;
//     z = b.z;
//     return *this;
// }

class Matrix4 {
  public:
    // union {
    //     struct {
    Vector4 v0, v1, v2, v3;
    //     };
    //     float mat[16];
    // };
    Matrix4() {
        // init matrix with identity
        v0 = Vector4(1.f, 0.f, 0.f, 0.f);
        v1 = Vector4(0.f, 1.f, 0.f, 0.f);
        v2 = Vector4(0.f, 0.f, 1.f, 0.f);
        v3 = Vector4(0.f, 0.f, 0.f, 1.f);
    }
};

} // namespace UMath

#endif
