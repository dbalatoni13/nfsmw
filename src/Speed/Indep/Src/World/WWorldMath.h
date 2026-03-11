#ifndef WORLD_WWORLDMATH_H
#define WORLD_WWORLDMATH_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bMath.hpp"

void VU0_v4crossprodxyz(const UMath::Vector4 &a, const UMath::Vector4 &b, UMath::Vector4 &r);

namespace WWorldMath {

inline float pow2(float a) { return a * a; }

inline float wmin(const float &a, const float &b) {
    if (a < b) return a;
    return b;
}

inline float wmax(const float &a, const float &b) {
    if (a > b) return a;
    return b;
}

inline bool InCircle(float x, float y, float cx, float cy, float r) {
    return pow2(cx - x) + pow2(cy - y) < pow2(r);
}

inline float InvSqrt(const float f) {
    return VU0_rsqrt(f);
}

inline bool PtsEqual(const UMath::Vector3 &p0, const UMath::Vector3 &p1, float tolerance) {
    const float kTolerance = tolerance;
    return UMath::Abs(p0.x - p1.x) < kTolerance && UMath::Abs(p0.y - p1.y) < kTolerance && UMath::Abs(p0.z - p1.z) < kTolerance;
}

inline void Crossxyz(const UMath::Vector4 &a, const UMath::Vector4 &b, UMath::Vector4 &r) {
    VU0_v4crossprodxyz(a, b, r);
}

bool IntersectCircle(float x1, float y1, float x2, float y2, float cx, float cy, float r, float &u1, float &u2);
bool MakeSegSpaceMatrix(const UMath::Vector3 &startPt, const UMath::Vector3 &endPt, UMath::Matrix4 &mat);
float GetPlaneY(const UMath::Vector3 &normal, const UMath::Vector3 &pointOnPlane, const UMath::Vector3 &testPoint);
void NearestPointLine2D(const UMath::Vector4 &pt, const UMath::Vector4 *line, UMath::Vector4 &nearPt);
void NearestPointLine2D3(const UMath::Vector3 &pt, const UMath::Vector3 &p0, const UMath::Vector3 &p1, UMath::Vector3 &nearPt);
bool IntersectSegPlane(const UMath::Vector3 &P1, const UMath::Vector3 &P2, const UMath::Vector3 &PtOnPlane, const UMath::Vector3 &Normal, UMath::Vector3 &intersectionPt, float &t);
bool SegmentIntersect(const UMath::Vector4 *line1, const UMath::Vector4 *line2, UMath::Vector4 *intersectPt);

};

#endif
