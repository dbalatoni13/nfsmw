#ifndef _FEMATH_H_
#define _FEMATH_H_

#include <types.h>
#include "Speed/Indep/Src/FEng/FEngStandard.h"

class FEMatrix4;

typedef float f32;

// File: speed/indep/src/feng/FEMath.h
// total size: 0x10
// Decl: speed/indep/src/feng/FEMath.h:48
class FEQuaternion {
  public:
    f32 x, y, z, w; // offset 0x0, size 0x4, Decl: speed/indep/src/feng/FEMath.h:51

    FEQuaternion() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) {} // Decl: speed/indep/src/feng/FEMath.h:54
    FEQuaternion(float X, float Y, float Z, float W) {}

    void Conjugate() { // Decl: speed/indep/src/feng/FEMath.h:56
        x = -x;
        y = -y;
        z = -z;
    }

    FEQuaternion &operator=(const FEQuaternion &q) {
        x = q.x;
        y = q.y;
        z = q.z;
        w = q.w;
        return *this;
    }

    FEQuaternion &operator+=(const FEQuaternion &q) {
        x += q.x;
        y += q.y;
        z += q.z;
        w += q.w;
        return *this;
    }

    FEQuaternion &operator-=(const FEQuaternion &q) {
        x -= q.x;
        y -= q.y;
        z -= q.z;
        w -= q.w;
        return *this;
    }

    FEQuaternion &operator*=(const FEQuaternion &q) {
        *this = *this * q;
        return *this;
    }

    FEQuaternion operator*(const FEQuaternion &q1) {
        FEQuaternion qRet;
        qRet.x = (y * q1.z - z * q1.y) + (q1.w * x + q1.x * w);
        qRet.y = (z * q1.x - x * q1.z) + (q1.w * y + q1.y * w);
        qRet.z = (x * q1.y - y * q1.x) + (q1.w * z + q1.z * w);
        qRet.w = q1.w * w - (q1.x * x + q1.y * y + q1.z * z);
        return qRet;
    }

    void GetMatrix(FEMatrix4 *rotmat); // Decl: speed/indep/src/feng/FEMath.h:74
};

inline f32 QuaternionDot(const FEQuaternion &q0, const FEQuaternion &q1) {
    return q0.x * q1.x + q0.y * q1.y + q0.z * q1.z + q0.w * q1.w;
}

inline FEQuaternion operator*(const FEQuaternion &q, f32 fScaler) {
    FEQuaternion r;
    r.x = q.x * fScaler;
    r.y = q.y * fScaler;
    r.z = q.z * fScaler;
    r.w = q.w * fScaler;
    return r;
}

inline FEQuaternion operator+(const FEQuaternion &q0, const FEQuaternion &q1) {
    FEQuaternion q;
    q.x = q0.x + q1.x;
    q.y = q0.y + q1.y;
    q.z = q0.z + q1.z;
    q.w = q0.w + q1.w;
    return q;
}

inline FEQuaternion operator-(const FEQuaternion &q0, const FEQuaternion &q1) {
    FEQuaternion q;
    q.x = q0.x - q1.x;
    q.y = q0.y - q1.y;
    q.z = q0.z - q1.z;
    q.w = q0.w - q1.w;
    return q;
}

inline f32 QuaternionNorm(const FEQuaternion &q) {
    return q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
}

inline float QuaternionMagnitude(const FEQuaternion &q) {
    return FEngSqrt(QuaternionNorm(q));
}

inline void NormalizeQuaternion(FEQuaternion &q) {
    float fMagnitude = 1.0f / QuaternionMagnitude(q);
    if (fMagnitude > 0.00001f) {
        q.x *= fMagnitude;
        q.y *= fMagnitude;
        q.z *= fMagnitude;
        q.w *= fMagnitude;
    } else {
        q.x = 0.0f;
        q.y = 0.0f;
        q.z = 0.0f;
        q.w = 1.0f;
    }
}

// total size: 0x8
// Decl: speed/indep/src/feng/FEMath.h:82
class FEVector2 {
  public:
    f32 x, y; // offset 0x0, size 0x4, Decl: speed/indep/src/feng/FEMath.h:84

    FEVector2() { // Decl: speed/indep/src/feng/FEMath.h:86
        x = y = 0.0f;
    }
    FEVector2(float v) : x(v), y(v) {}
    FEVector2(float vx, float vy) : x(vx), y(vy) {}
    FEVector2(float *pf) : x(pf[0]), y(pf[1]) {}
    FEVector2(const FEVector2 &v) {
        *this = v;
    }

    FEVector2 operator+(const FEVector2 &v) const {
        return FEVector2(x + v.x, y + v.y);
    }
    FEVector2 operator-(const FEVector2 &v) const {
        return FEVector2(x - v.x, y - v.y);
    }
    FEVector2 operator*(float f) const {
        return FEVector2(x * f, y * f);
    }
    FEVector2 operator/(float f) const {
        return FEVector2(x / f, y / f);
    }
    FEVector2 &operator=(const FEVector2 &v) {
        x = v.x;
        y = v.y;
        return *this;
    }
    FEVector2 &operator+=(FEVector2 v) { // Decl: speed/indep/src/feng/FEMath.h:98
        x += v.x;
        y += v.y;
        return *this;
    }
    FEVector2 &operator-=(FEVector2 v) { // Decl: speed/indep/src/feng/FEMath.h:99
        x -= v.x;
        y -= v.y;
        return *this;
    }
    FEVector2 &operator*=(float f) {
        x *= f;
        y *= f;
        return *this;
    }
    FEVector2 &operator*=(FEVector2 &v) {} // Decl: speed/indep/src/feng/FEMath.h:101
    FEVector2 &operator/=(float f) {}
    FEVector2 &operator/=(FEVector2 &v) {} // Decl: speed/indep/src/feng/FEMath.h:103

    int operator==(const FEVector2 &v) const {}
    f32 operator*(const FEVector2 &v) const {}

    const f32 operator[](int i) const {}
    const f32 &operator[](int i) {} // Decl: speed/indep/src/feng/FEMath.h:109

    float Dot(const FEVector2 &v) const {
        return x * v.x + y * v.y;
    }

    f32 Length() const {
        return FEngSqrt(Dot(*this));
    }
    f32 Normalize() { // Decl: speed/indep/src/feng/FEMath.h:114
        f32 ret = Length();
        if (ret >= 0.1f) {
            f32 oof = 1.0f / ret;
            x *= oof;
            y *= oof;
        }
        return ret;
    }
};

// total size: 0xC
// Decl: speed/indep/src/feng/FEMath.h:123
class FEVector3 {
  public:
    f32 x, y, z; // offset 0x0, size 0x4, Decl: speed/indep/src/feng/FEMath.h:125

    FEVector3() { // Decl: speed/indep/src/feng/FEMath.h:127
        x = y = z = 0.0f;
    }
    FEVector3(float v) : x(v), y(v), z(v) {}
    FEVector3(float vx, float vy, float vz) : x(vx), y(vy), z(vz) {}
    FEVector3(float *pf) : x(pf[0]), y(pf[1]), z(pf[2]) {}
    FEVector3(const FEVector3 &v) {
        *this = v;
    }

    FEVector3 operator+(const FEVector3 &v) const {}
    FEVector3 operator-(const FEVector3 &v) const {
        return FEVector3(x - v.x, y - v.y, z - v.z);
    }
    FEVector3 operator*(float f) const {}
    FEVector3 operator*(const FEVector3 &v) const {}
    FEVector3 operator/(float f) const {}
    FEVector3 &operator=(const FEVector3 &v) {
        x = v.x;
        y = v.y;
        z = v.z;
        return *this;
    }
    FEVector3 &operator+=(const FEVector3 &v) {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }
    FEVector3 &operator-=(const FEVector3 &v) {}
    FEVector3 &operator*=(float f) {
        x *= f;
        y *= f;
        z *= f;
        return *this;
    }
    FEVector3 &operator*=(const FEVector3 &v) {}
    FEVector3 &operator/=(float f) {}

    int operator==(const FEVector3 &v) const {}

    const f32 operator[](int i) const {}

    const f32 &operator[](int i) {} // Decl: speed/indep/src/feng/FEMath.h:148

    f32 Dot(const FEVector3 &v) const {}

    f32 Length() const {}

    f32 Normalize() {} // Decl: speed/indep/src/feng/FEMath.h:153

    FEVector3 CrossProduct(const FEVector3 &v) const {}

    operator f32 *() {}
};

// total size: 0x10
// Decl: speed/indep/src/feng/FEMath.h: unknown
class FEVector4 {
  public:
    float x, y, z, w; // offset 0x0, size 0x4

    FEVector4() {}
    FEVector4(float v) {}
    FEVector4(float xx, float yy, float zz, float ww) {}
    FEVector4(const FEVector4 &Src) {}
    FEVector4 &operator=(const FEVector4 &Src) {}
};

// total size: 0x40
// Decl: speed/indep/src/feng/FEMath.h:181
class FEMatrix4 {
  public:
    FEMatrix4() {} // Decl: speed/indep/src/feng/FEMath.h:183
    FEMatrix4(const FEMatrix4 &m) {
        *this = m;
    }

    FEMatrix4 &operator=(const FEMatrix4 &m) {
        FEngMemCpy(this, &m, sizeof(FEMatrix4));
        return *this;
    }

    void Identify(); // Decl: speed/indep/src/feng/FEMath.h:189

    float m11, m12, m13, m14; // offset 0x0, size 0x4, Decl: speed/indep/src/feng/FEMath.h:191
    float m21, m22, m23, m24; // offset 0x10, size 0x4, Decl: speed/indep/src/feng/FEMath.h:192
    float m31, m32, m33, m34; // offset 0x20, size 0x4, Decl: speed/indep/src/feng/FEMath.h:193
    float m41, m42, m43, m44; // offset 0x30, size 0x4, Decl: speed/indep/src/feng/FEMath.h:194
};

void FEMultMatrix(FEMatrix4 *dest, const FEMatrix4 *a, const FEMatrix4 *b);
void FEMultMatrix(FEVector3 *dest, const FEMatrix4 *m, const FEVector3 *v);

#endif
