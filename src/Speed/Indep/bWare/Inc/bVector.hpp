#pragma once

#include "math.h"

struct bVector2 {
  // total size: 0x8
  float x; // offset 0x0, size 0x4
  float y; // offset 0x4, size 0x4

  bVector2() {}

  bVector2 operator+() {}

  bVector2(float _x, float _y, float _z) {}

  bVector2(const bVector2 &v) {}

  bVector2 &operator=(const bVector2 &v) {}

  bVector2 &operator*=(float scale) {}

  bVector2 &operator/=(float inv_scale) {}

  int operator==(const bVector2 &v) {}

  float &operator[](int index) {}

  bVector2 operator+(const bVector2 &v) {}

  bVector2 operator-(const bVector2 &v) {}

  bVector2 operator-() {}

  bVector2 operator*(float f) {}

  bVector2 &operator-=(const bVector2 &v) {}

  bVector2 &operator+=(const bVector2 &v) {}
};

bVector2 *bNormalize(bVector2 *dest, const bVector2 *v);
bVector2 *bNormalize(bVector2 *dest, const bVector2 *v, float length);

inline float bLength(const bVector2 *v) {
  float x = v->x;
  float y = v->y;
  return sqrtf(x * x + y * y);
}

struct bVector3 {
  // total size: 0x10
  float x;   // offset 0x0, size 0x4
  float y;   // offset 0x4, size 0x4
  float z;   // offset 0x8, size 0x4
  float pad; // offset 0xC, size 0x4

  bVector3() {}

  bVector3 operator+() {}

  bVector3(float _x, float _y, float _z) {}

  bVector3(const bVector3 &v) {}

  bVector3 &operator=(const bVector3 &v) {}

  bVector3 &operator*=(float scale) {}

  bVector3 &operator/=(float inv_scale) {}

  int operator==(const bVector3 &v) {}

  float &operator[](int index) {}

  bVector3 operator+(const bVector3 &v) {}

  bVector3 operator-(const bVector3 &v) {}

  bVector3 operator-() {}

  bVector3 operator*(float f) {}

  bVector3 &operator-=(const bVector3 &v) {}

  bVector3 &operator+=(const bVector3 &v) {}
};

inline bVector3 *bFill(bVector3 *dest, float x, float y, float z) {}

inline bVector3 *bCopy(bVector3 *dest, const bVector3 *v) {
  // Local variables
  float x;
  float y;
  float z;
}

inline bVector3 *bAdd(bVector3 *dest, const bVector3 *v1, const bVector3 *v2) {
  // Local variables
  float x1;
  float y1;
  float z1;
  float x2;
  float y2;
  float z2;
}

inline bVector3 *bSub(bVector3 *dest, const bVector3 *v1, const bVector3 *v2) {
  // Local variables
  float x1;
  float y1;
  float z1;
  float x2;
  float y2;
  float z2;
}

inline bVector3 *bNeg(bVector3 *dest, const bVector3 *v) {
  // Local variables
  float x;
  float y;
  float z;
}

inline float bDot(const bVector3 *v1, const bVector3 *v2) { return v1->x * v2->x + v1->y * v2->y + v1->z * v2->z; }

inline int bEqual(const bVector3 *v1, const bVector3 *v2, float epsilon) {}

inline float bLength(const bVector3 *v) { return sqrtf(bDot(v, v)); }

inline bVector3 *bScale(bVector3 *dest, const bVector3 *v, float scale) {
  // Local variables
  float x;
  float y;
  float z;
}

inline bVector3 *bScale(bVector3 *dest, const bVector3 *v1, const bVector3 *v2) {
  // Local variables
  float x;
  float y;
  float z;
}

inline bVector3 *bMin(bVector3 *dest, const bVector3 *v1, const bVector3 *v2) {}

inline bVector3 *bMax(bVector3 *dest, const bVector3 *v1, const bVector3 *v2) {}

inline bVector3 bAdd(const bVector3 &v1, const bVector3 &v2) {
  // Local variables
  bVector3 dest;
}

inline bVector3 bSub(const bVector3 &v1, const bVector3 &v2) {
  // Local variables
  bVector3 dest;
}

inline bVector3 bNeg(const bVector3 &v) {
  // Local variables
  bVector3 dest;
}

inline bVector3 bCross(const bVector3 &v1, const bVector3 &v2) {
  // Local variables
  bVector3 dest;
}

inline float bDot(const bVector3 &v1, const bVector3 &v2) { return bDot(&v1, &v2); }

inline int bEqual(const bVector3 &v1, const bVector3 &v2, float epsilon) { return bEqual(&v1, &v2, epsilon); }

inline float bLength(const bVector3 &v) { return bLength(&v); }

inline float bDistBetween(const bVector3 &v1, const bVector3 &v2) {}

inline bVector3 bScale(const bVector3 &v, float scale) {
  // Local variables
  bVector3 dest;
}

inline bVector3 bScale(const bVector3 &v1, const bVector3 &v2) {
  // Local variables
  bVector3 dest;
}

inline bVector3 bScaleAdd(const bVector3 &v1, const bVector3 &v2, float scale) {
  // Local variables
  bVector3 dest;
}

inline bVector3 bNormalize(const bVector3 &v) {
  // Local variables
  bVector3 dest;
}

inline bVector3 bNormalize(const bVector3 &v, float length) {
  // Local variables
  bVector3 dest;
}

inline bVector3 bMin(const bVector3 &v1, const bVector3 &v2) {
  // Local variables
  bVector3 dest;
}

inline bVector3 bMax(const bVector3 &v1, const bVector3 &v2) {
  // Local variables
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

  bVector4 &operator=(const bVector4 &v) {}

  bVector4 &operator-=(const bVector4 &v) {}

  bVector4 &operator+=(const bVector4 &v) {}

  bVector4 &operator*=(float scale) {}

  bVector4 &operator/=(float inv_scale) {}

  int operator==(const bVector4 &v) {}

  float &operator[](int index) {}

  const float &operator[](int index) const {}

  bVector4 operator+(const bVector4 &v) {
    // Local variables
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
    // Local variables
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
    // Local variables
    float x1;
    float y1;
    float z1;
    float w1;
  }

  bVector4 operator*(const float f) {
    // Local variables
    bVector4 t;
  }
};

inline bVector4 *bFill(bVector4 *dest, float x, float y, float z, float w) {}

inline bVector4 *bCopy(bVector4 *dest, const bVector4 *v) {
  // Local variables
  float x;
  float y;
  float z;
  float w;
}

inline bVector4 *bCopy(bVector4 *dest, const bVector3 *v) {
  // Local variables
  float x;
  float y;
  float z;
}

inline bVector4 *bCopy(bVector4 *dest, const bVector3 *v, float w) {
  // Local variables
  float x;
  float y;
  float z;
}

inline bVector4 *bAdd(bVector4 *dest, const bVector4 *v1, const bVector4 *v2) {
  // Local variables
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
  // Local variables
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
  // Local variables
  float x;
  float y;
  float z;
  float w;
}

inline float bDot(const bVector4 *v1, const bVector4 *v2) { return v1->x * v2->x + v1->y * v2->y + v1->z * v2->z + v1->w + v2->w; }

inline float bLength(const bVector4 *v) { return sqrtf(bDot(v, v)); }

inline bVector4 *bScale(bVector4 *dest, const bVector4 *v, float scale) {
  // Local variables
  float x;
  float y;
  float z;
  float w;
}

inline bVector4 *bScale(bVector4 *dest, const bVector4 *v1, const bVector4 *v2) {
  // Local variables
  float x;
  float y;
  float z;
  float w;
}

inline bVector4 *bMin(bVector4 *dest, const bVector4 *v1, const bVector4 *v2) {}

inline bVector4 *bMax(bVector4 *dest, const bVector4 *v1, const bVector4 *v2) {}

inline bVector4 bAdd(const bVector4 &v1, const bVector4 &v2) {
  // Local variables
  bVector4 dest;
}

inline bVector4 bSub(const bVector4 &v1, const bVector4 &v2) {
  // Local variables
  bVector4 dest;
}

inline bVector4 bNeg(const bVector4 &v) {
  // Local variables
  bVector4 dest;
}

inline bVector4 bCross(const bVector4 &v1, const bVector4 &v2) {
  // Local variables
  bVector4 dest;
}

inline float bDot(const bVector4 &v1, const bVector4 &v2) {}

inline int bEqual(const bVector4 &v1, const bVector4 &v2, float epsilon) {}

inline float bLength(const bVector4 &v) {}

float bDistBetween(const bVector4 *v1, const bVector4 *v2);
inline float bDistBetween(const bVector4 &v1, const bVector4 &v2) { return bDistBetween(&v1, &v2); }

inline bVector4 bScale(const bVector4 &v, float scale) {
  // Local variables
  bVector4 dest;
}

inline bVector4 bScale(const bVector4 &v1, const bVector4 &v2) {
  // Local variables
  bVector4 dest;
}

inline bVector4 bScaleAdd(const bVector4 &v1, const bVector4 &v2, float scale) {
  // Local variables
  bVector4 dest;
}

inline bVector4 bNormalize(const bVector4 &v) {
  // Local variables
  bVector4 dest;
}

inline bVector4 bMin(const bVector4 &v1, const bVector4 &v2) {
  // Local variables
  bVector4 dest;
}

inline bVector4 bMax(const bVector4 &v1, const bVector4 &v2) {
  // Local variables
  bVector4 dest;
}

struct bMatrix4 {
  // total size: 0x40
  bVector4 v0; // offset 0x0, size 0x10
  bVector4 v1; // offset 0x10, size 0x10
  bVector4 v2; // offset 0x20, size 0x10
  bVector4 v3; // offset 0x30, size 0x10

  bMatrix4() {}

  bMatrix4(const bMatrix4 &m) {}

  bMatrix4 &operator=(const bMatrix4 &m) {}

  bVector4 &operator[](int index) {}

  const bVector4 &operator[](int index) const {}
};

inline bMatrix4 *bCopy(bMatrix4 *dest, const bMatrix4 *v) {}

inline bMatrix4 *bCopy(bMatrix4 *dest, const bMatrix4 *v, const struct bVector4 *position) {}

inline bMatrix4 *bCopy(bMatrix4 *dest, const bMatrix4 *v, const struct bVector3 *position) {}

inline void bIdentity(bMatrix4 *a) {}
