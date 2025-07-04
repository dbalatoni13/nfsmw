#ifndef BWARE_BMATH_H
#define BWARE_BMATH_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <math.h>

#if TARGET_GC
#include "dolphin/mtx44_ext.h"
#else
#error Choose a platform
#endif

float bSin(unsigned short angle);
float bSin(float angle);
float bCos(unsigned short angle);
void bSinCos(float *presult_sin, float *presult_cos, unsigned short angle);
unsigned short bASin(float x);
unsigned short bATan(float x, float y);

inline unsigned short bACos(float x) {
    return 16384 - bASin(x);
}

inline float bTan(unsigned short angle) {
    return bSin(angle) / bCos(angle);
}

inline float bSqrt(float x) {
    const float bSqrtEPS = 5e-11f;

    float y0;
    float y1;
    float t0;
    float t1;
    float t2;

    float half = 0.5f;
    float one = 1.0f;

    if (x > bSqrtEPS) {
        asm("frsqrte %0, %1" : "=f"(y0) : "f"(x));
        t0 = y0 * y0;
        // t1 = y0 * half;
        asm("fmuls %0, %1, %2" : "=f"(t1) : "f"(y0), "f"(half));
        // t2 = -(x * t0 - one);
        asm("fnmsubs %0, %1, %2, %3" : "=f"(t2) : "f"(x), "f"(t0), "f"(one));
        y0 += (t2 * t1);
        // asm("fmadds %0, %1, %2, %3" : "=f"(y0) : "f"(t2), "f"(t1), "f"(y0));

        t0 = y0 * y0;
        // t1 = y0 * half;
        asm("fmuls %0, %1, %2" : "=f"(t1) : "f"(y0), "f"(half));
        // t2 = -(x * t0 - one);
        asm("fnmsubs %0, %1, %2, %3" : "=f"(t2) : "f"(x), "f"(t0), "f"(one));
        y0 += (t2 * t1);
        // asm("fmadds %0, %1, %2, %3" : "=f"(y0) : "f"(t2), "f"(t1), "f"(y0));

        y0 *= x;
    } else {
        y0 = 0.0f;
    }

    return y0;
}

inline int bMax(int a, int b) {
    return a > b ? a : b;
}

inline float bMax(float a, float b) {
    float c = a - b;
    float d;
    return c < 0.0f ? b : a;
}

inline int bAbs(int a) {
    if (a < 0) {
        return -a;
    }
    return a;
}

inline unsigned short bDegToAng(float degrees) {
    return static_cast<int>(degrees * 65536.0f) / 360;
}

inline unsigned short bRadToAng(float radians) {
    return static_cast<int>(radians * (65536.0f / (2 * static_cast<float>(M_PI))));
}

inline float bAngToRad(unsigned short angle) {}

inline float bDegToRad(float degrees) {}

inline float bAngToDeg(unsigned short angle) {}

inline float bRadToDeg(float radians) {}

inline float bAngToRad(short angle) {}

inline float bAngToDeg(short angle) {}

struct bVector2 {
    // total size: 0x8
    float x; // offset 0x0, size 0x4
    float y; // offset 0x4, size 0x4

    bVector2() {}

    bVector2 operator+() {}

    bVector2(float _x, float _y);

    bVector2(const bVector2 &v) {}

    bVector2 operator-(const bVector2 &v);

    bVector2 &operator=(const bVector2 &v) {}

    bVector2 &operator*=(float scale) {}

    bVector2 &operator/=(float inv_scale) {}

    int operator==(const bVector2 &v) {}

    float &operator[](int index) {}

    bVector2 operator+(const bVector2 &v) {}

    bVector2 operator-() {}

    bVector2 operator*(float f) {}

    bVector2 &operator-=(const bVector2 &v) {}

    bVector2 &operator+=(const bVector2 &v) {}
};

bVector2 *bNormalize(bVector2 *dest, const bVector2 *v);
bVector2 *bNormalize(bVector2 *dest, const bVector2 *v, float length);

inline bVector2 *bFill(bVector2 *dest, float x, float y) {
    dest->x = x;
    dest->y = y;
    return dest;
}

inline bVector2::bVector2(float _x, float _y) {
    bFill(this, _x, _y);
}

inline bVector2 bVector2::operator-(const bVector2 &v) {
    float x1 = this->x;
    float y1 = this->y;
    float x2 = v.x;
    float y2 = v.y;
    float _x = x1 - x2;
    float _y = y1 - y2;
    return bVector2(_x, _y);
}

inline float bLength(const bVector2 *v) {
    float x = v->x;
    float y = v->y;
    return bSqrt(x * x + y * y);
}

inline bVector2 bNormalize(const bVector2 &v) {
    bVector2 dest;
    bNormalize(&dest, &v);
    return dest;
}

inline float bDot(const bVector2 *v1, const bVector2 *v2) {
    return v1->x * v2->x + v1->y * v2->y;
}

struct bVector3 {
    // total size: 0x10
    float x;   // offset 0x0, size 0x4
    float y;   // offset 0x4, size 0x4
    float z;   // offset 0x8, size 0x4
    float pad; // offset 0xC, size 0x4

    bVector3() {}

    bVector3 operator+() {}

    bVector3(float _x, float _y, float _z) {
        x = _x;
        y = _y;
        z = _z;
    }

    bVector3(const bVector3 &v);
    bVector3 &operator*=(float scale);
    bVector3 &operator+=(const bVector3 &v);
    bVector3 operator+(const bVector3 &v);
    bVector3 &operator=(const bVector3 &v);
    bVector3 operator-(const bVector3 &v);

    bVector3 &operator/=(float inv_scale) {}

    int operator==(const bVector3 &v) {}

    float &operator[](int index) {}

    bVector3 operator-() {}

    bVector3 operator*(float f) {}

    bVector3 &operator-=(const bVector3 &v) {}
};

bVector3 *bNormalize(bVector3 *dest, const bVector3 *v);

inline bVector3 *bFill(bVector3 *dest, float x, float y, float z) {
    dest->x = x;
    dest->y = y;
    dest->z = z;
    return dest;
}

inline bVector3 *bCopy(bVector3 *dest, const bVector3 *v) {
    float x = v->x;
    float y = v->y;
    float z = v->z;
    bFill(dest, x, y, z);
    return dest;
}

inline bVector3 *bScale(bVector3 *dest, const bVector3 *v, float scale) {
    float x = v->x;
    float y = v->y;
    float z = v->z;

    dest->x = x * scale;
    dest->y = y * scale;
    dest->z = z * scale;
    return dest;
}

inline bVector3 *bAdd(bVector3 *dest, const bVector3 *v1, const bVector3 *v2) {
    float x1 = v1->x;
    float y1 = v1->y;
    float z1 = v1->z;
    float x2 = v2->x;
    float y2 = v2->y;
    float z2 = v2->z;

    bFill(dest, x1 + x2, y1 + y2, z1 + z2);
    return dest;
}

inline bVector3 bAdd(const bVector3 &v1, const bVector3 &v2) {
    bVector3 dest;
    bAdd(&dest, &v1, &v2);
    return dest;
}

inline bVector3 *bSub(bVector3 *dest, const bVector3 *v1, const bVector3 *v2) {
    float x1 = v1->x;
    float y1 = v1->y;
    float z1 = v1->z;
    float x2 = v2->x;
    float y2 = v2->y;
    float z2 = v2->z;

    bFill(dest, x1 - x2, y1 - y2, z1 - z2);
    return dest;
}

inline bVector3 bSub(const bVector3 &v1, const bVector3 &v2) {
    bVector3 dest;
    bSub(&dest, &v1, &v2);
    return dest;
}

inline bVector3::bVector3(const bVector3 &v) {
    bCopy(this, &v);
}

inline bVector3 &bVector3::operator*=(float scale) {
    bScale(this, this, scale);
    return *this;
}

inline bVector3 &bVector3::operator+=(const bVector3 &v) {
    bAdd(this, this, &v);
    return *this;
}

inline bVector3 bVector3::operator+(const bVector3 &v) {
    return bAdd(*this, v);
}

inline bVector3 bVector3::operator-(const bVector3 &v) {
    return bSub(*this, v);
}

inline bVector3 &bVector3::operator=(const bVector3 &v) {
    bCopy(this, &v);
    return *this;
}

inline bVector3 *bNeg(bVector3 *dest, const bVector3 *v) {
    float x;
    float y;
    float z;
}

inline float bDot(const bVector3 *v1, const bVector3 *v2) {
    return v1->x * v2->x + v1->y * v2->y + v1->z * v2->z;
}

inline int bEqual(const bVector3 *v1, const bVector3 *v2, float epsilon) {}

inline float bLength(const bVector3 *v) {
    return bSqrt(bDot(v, v));
}

inline bVector3 *bScale(bVector3 *dest, const bVector3 *v1, const bVector3 *v2) {
    float x;
    float y;
    float z;
}

inline bVector3 *bMin(bVector3 *dest, const bVector3 *v1, const bVector3 *v2) {}

inline bVector3 *bMax(bVector3 *dest, const bVector3 *v1, const bVector3 *v2) {}

inline bVector3 bNeg(const bVector3 &v) {
    bVector3 dest;
}

inline bVector3 bCross(const bVector3 &v1, const bVector3 &v2) {
    bVector3 dest;
}

inline float bDot(const bVector3 &v1, const bVector3 &v2) {
    return bDot(&v1, &v2);
}

inline int bEqual(const bVector3 &v1, const bVector3 &v2, float epsilon) {
    return bEqual(&v1, &v2, epsilon);
}

inline float bLength(const bVector3 &v) {
    return bLength(&v);
}

inline float bDistBetween(const bVector3 &v1, const bVector3 &v2) {}

inline bVector3 bScale(const bVector3 &v, float scale) {
    bVector3 dest;
}

inline bVector3 bScale(const bVector3 &v1, const bVector3 &v2) {
    bVector3 dest;
}

inline bVector3 bScaleAdd(const bVector3 &v1, const bVector3 &v2, float scale) {
    bVector3 dest;
}

inline bVector3 bNormalize(const bVector3 &v) {
    bVector3 dest;
}

inline bVector3 bNormalize(const bVector3 &v, float length) {
    bVector3 dest;
}

inline bVector3 bMin(const bVector3 &v1, const bVector3 &v2) {
    bVector3 dest;
}

inline bVector3 bMax(const bVector3 &v1, const bVector3 &v2) {
    bVector3 dest;
}

struct bVector4 {
    // total size: 0x10
    float x; // offset 0x0, size 0x4
    float y; // offset 0x4, size 0x4
    float z; // offset 0x8, size 0x4
    float w; // offset 0xC, size 0x4

    bVector4() {}

    bVector4 operator+() {}

    bVector4(float _x, float _y, float _z, float _w) {}

    bVector4(const bVector4 &v) {}

    bVector4 &operator=(const bVector4 &v);

    bVector4 &operator-=(const bVector4 &v) {}

    bVector4 &operator+=(const bVector4 &v) {}

    bVector4 &operator*=(float scale) {}

    bVector4 &operator/=(float inv_scale) {}

    int operator==(const bVector4 &v) {}

    float &operator[](int index) {}

    const float &operator[](int index) const {}

    bVector4 operator+(const bVector4 &v) {
        bVector4 *pv;
        float x1;
        float y1;
        float z1;
        float w1;
        float x2;
        float y2;
        float z2;
        float w2;
        float _x;
        float _y;
        float _z;
        float _w;
    }

    bVector4 operator-(const bVector4 &v) {
        bVector4 *pv;
        float x1;
        float y1;
        float z1;
        float w1;
        float x2;
        float y2;
        float z2;
        float w2;
        float _x;
        float _y;
        float _z;
        float _w;
    }

    bVector4 operator-() {
        float x1;
        float y1;
        float z1;
        float w1;
    }

    bVector4 operator*(const float f) {
        bVector4 t;
    }
};

bVector4 *bNormalize(bVector4 *dest, const bVector4 *v);

inline bVector4 *bFill(bVector4 *dest, float x, float y, float z, float w) {
    dest->x = x;
    dest->y = y;
    dest->z = z;
    dest->w = w;

    return dest;
}

inline bVector4 *bCopy(bVector4 *dest, const bVector4 *v) {
    float x = v->x;
    float y = v->y;
    float z = v->z;
    float w = v->w;

    bFill(dest, x, y, z, w);
    return dest;
}

inline bVector4 *bCopy(bVector4 *dest, const bVector3 *v) {
    float x;
    float y;
    float z;
}

inline bVector4 *bCopy(bVector4 *dest, const bVector3 *v, float w) {
    float x = v->x;
    float y = v->y;
    float z = v->z;

    dest->x = x;
    dest->y = y;
    dest->z = z;
    dest->w = w;

    return dest;
}

inline bVector4 *bAdd(bVector4 *dest, const bVector4 *v1, const bVector4 *v2) {
    float x1;
    float y1;
    float z1;
    float w1;
    float x2;
    float y2;
    float z2;
    float w2;
}

inline bVector4 *bSub(bVector4 *dest, const bVector4 *v1, const bVector4 *v2) {
    float x1;
    float y1;
    float z1;
    float w1;
    float x2;
    float y2;
    float z2;
    float w2;
}

inline bVector4 *bNeg(bVector4 *dest, const bVector4 *v) {
    float x;
    float y;
    float z;
    float w;
}

inline float bDot(const bVector4 *v1, const bVector4 *v2) {
    return v1->x * v2->x + v1->y * v2->y + v1->z * v2->z + v1->w * v2->w;
}

inline float bLength(const bVector4 *v) {
    return bSqrt(bDot(v, v));
}

inline bVector4 *bScale(bVector4 *dest, const bVector4 *v, float scale) {
    float x = v->x;
    float y = v->y;
    float z = v->z;
    float w = v->w;

    dest->x = x * scale;
    dest->y = y * scale;
    dest->z = z * scale;
    dest->w = w * scale;
    return dest;
}

inline bVector4 *bScale(bVector4 *dest, const bVector4 *v1, const bVector4 *v2) {
    float x;
    float y;
    float z;
    float w;
}

inline bVector4 *bMin(bVector4 *dest, const bVector4 *v1, const bVector4 *v2) {}

inline bVector4 *bMax(bVector4 *dest, const bVector4 *v1, const bVector4 *v2) {}

inline bVector4 bAdd(const bVector4 &v1, const bVector4 &v2) {
    bVector4 dest;
}

inline bVector4 bSub(const bVector4 &v1, const bVector4 &v2) {
    bVector4 dest;
}

inline bVector4 bNeg(const bVector4 &v) {
    bVector4 dest;
}

inline bVector4 bCross(const bVector4 &v1, const bVector4 &v2) {
    bVector4 dest;
}

inline float bDot(const bVector4 &v1, const bVector4 &v2) {}

inline int bEqual(const bVector4 &v1, const bVector4 &v2, float epsilon) {}

inline float bLength(const bVector4 &v) {}

float bDistBetween(const bVector4 *v1, const bVector4 *v2);
inline float bDistBetween(const bVector4 &v1, const bVector4 &v2) {
    return bDistBetween(&v1, &v2);
}

inline bVector4 bScale(const bVector4 &v, float scale) {
    bVector4 dest;
}

inline bVector4 bScale(const bVector4 &v1, const bVector4 &v2) {
    bVector4 dest;
}

inline bVector4 bScaleAdd(const bVector4 &v1, const bVector4 &v2, float scale) {
    bVector4 dest;
}

inline bVector4 bNormalize(const bVector4 &v) {
    bVector4 dest;
}

inline bVector4 bMin(const bVector4 &v1, const bVector4 &v2) {
    bVector4 dest;
}

inline bVector4 bMax(const bVector4 &v1, const bVector4 &v2) {
    bVector4 dest;
}

inline bVector4 &bVector4::operator=(const bVector4 &v) {
    bCopy(this, &v);
    return *this;
}

struct bMatrix4 {
    // total size: 0x40
    bVector4 v0; // offset 0x0, size 0x10
    bVector4 v1; // offset 0x10, size 0x10
    bVector4 v2; // offset 0x20, size 0x10
    bVector4 v3; // offset 0x30, size 0x10

    bMatrix4() {}

    bMatrix4(const bMatrix4 &m) {}

    bMatrix4 &operator=(const bMatrix4 &m);

    bVector4 &operator[](int index) {}

    const bVector4 &operator[](int index) const {}
};

inline bMatrix4 *bCopy(bMatrix4 *dest, const bMatrix4 *v) {
#if TARGET_GC
    MTX44Copy(*reinterpret_cast<const Mtx44 *>(v), *reinterpret_cast<Mtx44 *>(dest));
#else
#error Choose a platform
#endif
    return dest;
}

inline void bIdentity(bMatrix4 *a) {
    MTX44Identity(*reinterpret_cast<Mtx44 *>(a));
}

inline bMatrix4 &bMatrix4::operator=(const bMatrix4 &m) {
    bCopy(this, &m);
    return *this;
}

// UNUSED
inline bMatrix4 *bCopy(bMatrix4 *dest, const bMatrix4 *v, const struct bVector4 *position) {}

inline bMatrix4 *bCopy(bMatrix4 *dest, const bMatrix4 *v, const struct bVector3 *position) {}

#endif
