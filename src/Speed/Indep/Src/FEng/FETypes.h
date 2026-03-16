#ifndef FENG_FETYPES_H
#define FENG_FETYPES_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

float FEngSqrt(float x);

// total size: 0x8
struct FEVector2 {
    float x; // offset 0x0, size 0x4
    float y; // offset 0x4, size 0x4

    inline FEVector2() : x(0.0f), y(0.0f) {}
    inline FEVector2(float v) : x(v), y(v) {}
    inline FEVector2(float vx, float vy) : x(vx), y(vy) {}
    inline FEVector2(float* pf) : x(pf[0]), y(pf[1]) {}
    inline FEVector2(const FEVector2& v) { *this = v; }

    inline FEVector2 operator+(const FEVector2& v) const { return FEVector2(x + v.x, y + v.y); }
    inline FEVector2 operator-(const FEVector2& v) const { return FEVector2(x - v.x, y - v.y); }
    inline FEVector2 operator*(float f) const { return FEVector2(x * f, y * f); }
    inline FEVector2 operator/(float f) const { return FEVector2(x / f, y / f); }
    inline FEVector2& operator=(const FEVector2& v) { x = v.x; y = v.y; return *this; }
    inline FEVector2& operator+=(FEVector2 v) { x += v.x; y += v.y; return *this; }
    inline FEVector2& operator-=(FEVector2 v) { x -= v.x; y -= v.y; return *this; }
    inline FEVector2& operator*=(float f) { x *= f; y *= f; return *this; }

    inline float Dot(const FEVector2& v) const { return x * v.x + y * v.y; }
    inline float Length() const { return FEngSqrt(Dot(*this)); }
    inline float Normalize() {
        float ret = Length();
        if (ret >= 0.1f) {
            float oof = 1.0f / ret;
            x *= oof;
            y *= oof;
        }
        return ret;
    }
};

// total size: 0xC
struct FEVector3 {
    float x; // offset 0x0, size 0x4
    float y; // offset 0x4, size 0x4
    float z; // offset 0x8, size 0x4

    inline FEVector3() : x(0.0f), y(0.0f), z(0.0f) {}
    inline FEVector3(float v) : x(v), y(v), z(v) {}
    inline FEVector3(float vx, float vy, float vz) : x(vx), y(vy), z(vz) {}
    inline FEVector3(float* pf) : x(pf[0]), y(pf[1]), z(pf[2]) {}
    inline FEVector3(const FEVector3& v) { *this = v; }

    inline FEVector3& operator=(const FEVector3& v) { x = v.x; y = v.y; z = v.z; return *this; }
    inline FEVector3 operator-(const FEVector3& v) const { return FEVector3(x - v.x, y - v.y, z - v.z); }
    inline FEVector3& operator*=(float f) { x *= f; y *= f; z *= f; return *this; }
    inline FEVector3& operator+=(const FEVector3& v) { x += v.x; y += v.y; z += v.z; return *this; }
};

// total size: 0x10
struct FEColor {
    int b; // offset 0x0, size 0x4
    int g; // offset 0x4, size 0x4
    int r; // offset 0x8, size 0x4
    int a; // offset 0xC, size 0x4

    inline FEColor() {}
    FEColor(unsigned long Col);
    operator unsigned long() const;
    FEColor& operator=(const FEColor& rhs);
    FEColor& operator+=(const FEColor& rhs);
    FEColor operator-(const FEColor& rhs) const;
};

struct FEMatrix4;

// total size: 0x10
struct FEQuaternion {
    float x; // offset 0x0, size 0x4
    float y; // offset 0x4, size 0x4
    float z; // offset 0x8, size 0x4
    float w; // offset 0xC, size 0x4

    inline FEQuaternion() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) {}
    inline FEQuaternion(float X, float Y, float Z, float W) : x(X), y(Y), z(Z), w(W) {}
    inline FEQuaternion& operator=(const FEQuaternion& q) { x = q.x; y = q.y; z = q.z; w = q.w; return *this; }
    inline void Conjugate() { x = -x; y = -y; z = -z; }
    inline FEQuaternion& operator*=(const FEQuaternion& q) { *this = *this * q; return *this; }
    inline FEQuaternion operator*(const FEQuaternion& q1) {
        FEQuaternion qRet;
        qRet.x = (y * q1.z - z * q1.y) + (q1.w * x + q1.x * w);
        qRet.y = (z * q1.x - x * q1.z) + (q1.w * y + q1.y * w);
        qRet.z = (x * q1.y - y * q1.x) + (q1.w * z + q1.z * w);
        qRet.w = q1.w * w - (q1.x * x + q1.y * y + q1.z * z);
        return qRet;
    }
    void GetMatrix(FEMatrix4* pMatrix);
};

inline FEQuaternion operator+(const FEQuaternion& q0, const FEQuaternion& q1) {
    FEQuaternion q;
    q.x = q0.x + q1.x;
    q.y = q0.y + q1.y;
    q.z = q0.z + q1.z;
    q.w = q0.w + q1.w;
    return q;
}

inline FEQuaternion operator-(const FEQuaternion& q0, const FEQuaternion& q1) {
    FEQuaternion q;
    q.x = q0.x - q1.x;
    q.y = q0.y - q1.y;
    q.z = q0.z - q1.z;
    q.w = q0.w - q1.w;
    return q;
}

inline FEQuaternion operator*(const FEQuaternion& q, float fScaler) {
    FEQuaternion r;
    r.x = q.x * fScaler;
    r.y = q.y * fScaler;
    r.z = q.z * fScaler;
    r.w = q.w * fScaler;
    return r;
}

inline float QuaternionDot(const FEQuaternion& q0, const FEQuaternion& q1) {
    return q0.x * q1.x + q0.y * q1.y + q0.z * q1.z + q0.w * q1.w;
}

inline float QuaternionNorm(const FEQuaternion& q) {
    return q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
}

float FEngSqrt(float x);

inline float QuaternionMagnitude(const FEQuaternion& q) {
    return FEngSqrt(QuaternionNorm(q));
}

inline void NormalizeQuaternion(FEQuaternion& q) {
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

// total size: 0x10
struct FERect {
    float left;   // offset 0x0, size 0x4
    float top;    // offset 0x4, size 0x4
    float right;  // offset 0x8, size 0x4
    float bottom; // offset 0xC, size 0x4

    inline FERect() : left(0.0f), top(0.0f), right(0.0f), bottom(0.0f) {}
    inline FERect(float Value) : left(Value), top(Value), right(Value), bottom(Value) {}
    inline FERect(float l, float t, float r, float b) : left(l), top(t), right(r), bottom(b) {}
    inline FERect& operator=(const FERect& r) { left = r.left; top = r.top; right = r.right; bottom = r.bottom; return *this; }
};

// total size: 0x44
struct FEObjData {
    inline FEObjData() {}
    FEColor Col;       // offset 0x0, size 0x10
    FEVector3 Pivot;   // offset 0x10, size 0xC
    FEVector3 Pos;     // offset 0x1C, size 0xC
    FEQuaternion Rot;  // offset 0x28, size 0x10
    FEVector3 Size;    // offset 0x38, size 0xC
};

// total size: 0x54
struct FEImageData : public FEObjData {
    FEVector2 UpperLeft;  // offset 0x44, size 0x8
    FEVector2 LowerRight; // offset 0x4C, size 0x8

    FEImageData();
};

// total size: 0x94
struct FEColoredImageData : public FEImageData {
    FEColor VertexColors[4]; // offset 0x54, size 0x40
};

// total size: 0x90
struct FEMultiImageData : public FEImageData {
    FEVector2 TopLeftUV[3];     // offset 0x54, size 0x18
    FEVector2 BottomRightUV[3]; // offset 0x6C, size 0x18
    FEVector3 PivotRot;         // offset 0x84, size 0xC
};

// total size: 0x8
struct FEPoint {
    float h; // offset 0x0, size 0x4
    float v; // offset 0x4, size 0x4

    inline FEPoint() : h(0.0f), v(0.0f) {}
    inline FEPoint(float Value) {}
    inline FEPoint(float H, float V) : h(H), v(V) {}
    inline FEPoint& operator=(const FEPoint& p) { h = p.h; v = p.v; return *this; }
};

// total size: 0x40
struct FEMatrix4 {
    float m11; // offset 0x0
    float m12; // offset 0x4
    float m13; // offset 0x8
    float m14; // offset 0xC
    float m21; // offset 0x10
    float m22; // offset 0x14
    float m23; // offset 0x18
    float m24; // offset 0x1C
    float m31; // offset 0x20
    float m32; // offset 0x24
    float m33; // offset 0x28
    float m34; // offset 0x2C
    float m41; // offset 0x30
    float m42; // offset 0x34
    float m43; // offset 0x38
    float m44; // offset 0x3C

    inline FEMatrix4& operator=(const FEMatrix4& m) {
        m11 = m.m11; m12 = m.m12; m13 = m.m13; m14 = m.m14;
        m21 = m.m21; m22 = m.m22; m23 = m.m23; m24 = m.m24;
        m31 = m.m31; m32 = m.m32; m33 = m.m33; m34 = m.m34;
        m41 = m.m41; m42 = m.m42; m43 = m.m43; m44 = m.m44;
        return *this;
    }

    void Identify();
};

void FEMultMatrix(FEMatrix4* dest, const FEMatrix4* a, const FEMatrix4* b);
void FEMultMatrix(FEVector3* dest, const FEMatrix4* m, const FEVector3* v);

#ifndef SUPPORT_UTILITY_UVECTOR_MATH_H
inline float IntAsFloat(const int& i) {
    return *reinterpret_cast<const float*>(&i);
}

static const float kFloatScaleUp = IntAsFloat(0x00800000);
static const float kFloatScaleDown = 1.0f / kFloatScaleUp;
#endif

#endif
