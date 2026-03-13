#ifndef FENG_FETYPES_H
#define FENG_FETYPES_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

// total size: 0x8
struct FEVector2 {
    float x; // offset 0x0, size 0x4
    float y; // offset 0x4, size 0x4

    inline FEVector2() : x(0.0f), y(0.0f) {}
    inline FEVector2(float v) : x(v), y(v) {}
    inline FEVector2(float vx, float vy) : x(vx), y(vy) {}
    inline FEVector2(float* pf) : x(pf[0]), y(pf[1]) {}
    inline FEVector2(const FEVector2& v) : x(v.x), y(v.y) {}

    inline FEVector2 operator+(const FEVector2& v) const { return FEVector2(x + v.x, y + v.y); }
    inline FEVector2 operator-(const FEVector2& v) const { return FEVector2(x - v.x, y - v.y); }
    inline FEVector2 operator*(float f) const { return FEVector2(x * f, y * f); }
    inline FEVector2 operator/(float f) const { return FEVector2(x / f, y / f); }
    FEVector2& operator=(const FEVector2& v);
    inline FEVector2& operator+=(FEVector2 v) { x += v.x; y += v.y; return *this; }
    inline FEVector2& operator-=(FEVector2 v) { x -= v.x; y -= v.y; return *this; }
    inline FEVector2& operator*=(float f) { x *= f; y *= f; return *this; }
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
};

// total size: 0x10
struct FEColor {
    int b; // offset 0x0, size 0x4
    int g; // offset 0x4, size 0x4
    int r; // offset 0x8, size 0x4
    int a; // offset 0xC, size 0x4

    inline FEColor() : b(0), g(0), r(0), a(0) {}
    FEColor(unsigned long Col);
    operator unsigned long() const;
    FEColor& operator=(const FEColor& rhs);
    FEColor& operator+=(const FEColor& rhs);
    FEColor operator-(const FEColor& rhs) const;
};

// total size: 0x10
struct FEQuaternion {
    float x; // offset 0x0, size 0x4
    float y; // offset 0x4, size 0x4
    float z; // offset 0x8, size 0x4
    float w; // offset 0xC, size 0x4

    inline FEQuaternion() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
    inline FEQuaternion(float X, float Y, float Z, float W) : x(X), y(Y), z(Z), w(W) {}
    inline FEQuaternion& operator=(const FEQuaternion& q) { x = q.x; y = q.y; z = q.z; w = q.w; return *this; }
};

// total size: 0x10
struct FERect {
    float left;   // offset 0x0, size 0x4
    float top;    // offset 0x4, size 0x4
    float right;  // offset 0x8, size 0x4
    float bottom; // offset 0xC, size 0x4

    inline FERect() : left(0.0f), top(0.0f), right(0.0f), bottom(0.0f) {}
    inline FERect(float Value) : left(Value), top(Value), right(Value), bottom(Value) {}
    inline FERect(float l, float t, float r, float b) : left(l), top(t), right(r), bottom(b) {}
};

// total size: 0x44
struct FEObjData {
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

// total size: 0x90
struct FEMultiImageData : public FEImageData {
    FEVector2 TopLeftUV[3];     // offset 0x54, size 0x18
    FEVector2 BottomRightUV[3]; // offset 0x6C, size 0x18
    FEVector3 PivotRot;         // offset 0x84, size 0xC
};

#endif
