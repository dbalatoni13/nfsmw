#ifndef WORLD_WWORLDMATH_H
#define WORLD_WWORLDMATH_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

namespace WWorldMath {

inline float pow2(float a) {
    return a * a;
}

inline float wmin(const float &a, const float &b) {
    if (a < b)
        return a;
    return b;
}

inline float wmax(const float &a, const float &b) {
    if (a > b)
        return a;
    return b;
}

inline bool InCircle(float x, float y, float cx, float cy, float r) {
    return (cx - x) * (cx - x) + (cy - y) * (cy - y) < r * r;
}

inline float InvSqrt(const float f) {
    return VU0_rsqrt(f);
}

inline bool PtsEqual(const UMath::Vector3 &p0, const UMath::Vector3 &p1, float tolerance) {
    const float kTolerance = tolerance;
    return fabsf(p0.x - p1.x) < kTolerance && fabsf(p0.y - p1.y) < kTolerance && fabsf(p0.z - p1.z) < kTolerance;
}

inline float PtDir4(const UMath::Vector4 &p1, const UMath::Vector4 &p2, const UMath::Vector3 &tp) {
    return (p1.x - p2.x) * (tp.z - p2.z) - (tp.x - p2.x) * (p1.z - p2.z);
}

inline bool InTri(const UMath::Vector3 &pt, const UMath::Vector4 *pts) {
    if (PtDir4(pts[0], pts[1], pt) <= 0.0f) {
        if (PtDir4(pts[1], pts[2], pt) <= 0.0f && PtDir4(pts[2], pts[0], pt) <= 0.0f) {
            return true;
        } else {
            return false;
        }
    } else {
        if (PtDir4(pts[1], pts[2], pt) >= 0.0f && PtDir4(pts[2], pts[0], pt) >= 0.0f) {
            return true;
        } else {
            return false;
        }
    }
}

inline bool InTriR(const UMath::Vector3 &pt, const UMath::Vector4 *pts) {
    return PtDir4(pts[0], pts[1], pt) <= 0.0f && PtDir4(pts[1], pts[2], pt) <= 0.0f && PtDir4(pts[2], pts[0], pt) <= 0.0f;
}

inline bool InTriL(const UMath::Vector3 &pt, const UMath::Vector4 *pts) {
    return 0.0f <= PtDir4(pts[0], pts[1], pt) && 0.0f <= PtDir4(pts[1], pts[2], pt) && 0.0f <= PtDir4(pts[2], pts[0], pt);
}

float GetPlaneY(const UMath::Vector3 &normal, const UMath::Vector3 &pointOnPlane, const UMath::Vector3 &pt);
bool IntersectCircle(float x1, float y1, float x2, float y2, float cx, float cy, float r, float &u1, float &u2);
bool MakeSegSpaceMatrix(const UMath::Vector3 &startPt, const UMath::Vector3 &endPt, UMath::Matrix4 &mat);
float GetPlaneY(const UMath::Vector3 &normal, const UMath::Vector3 &pointOnPlane, const UMath::Vector3 &testPoint);
void NearestPointLine2D(const UMath::Vector4 &pt, const UMath::Vector4 *line, UMath::Vector4 &nearPt);
void NearestPointLine2D3(const UMath::Vector3 &pt, const UMath::Vector3 &p0, const UMath::Vector3 &p1, UMath::Vector3 &nearPt);
bool IntersectSegPlane(const UMath::Vector3 &P1, const UMath::Vector3 &P2, const UMath::Vector3 &PtOnPlane, const UMath::Vector3 &Normal,
                       UMath::Vector3 &intersectionPt, float &t);
bool SegmentIntersect(const UMath::Vector4 *line1, const UMath::Vector4 *line2, UMath::Vector4 *intersectPt);

}; // namespace WWorldMath

#endif
