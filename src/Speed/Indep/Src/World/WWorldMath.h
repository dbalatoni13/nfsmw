#ifndef WORLD_WWORLDMATH_H
#define WORLD_WWORLDMATH_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bMath.hpp"

namespace WWorldMath {

bool IntersectCircle(float x1, float y1, float x2, float y2, float cx, float cy, float r, float &u1, float &u2);
float GetPlaneY(const UMath::Vector3 &normal, const UMath::Vector3 &pointOnPlane, const UMath::Vector3 &testPoint);
float NearestPointLine2D(const UMath::Vector3 &lineStart, const UMath::Vector3 &lineEnd, const UMath::Vector3 &testPoint, UMath::Vector3 &nearPt);

};

#endif
