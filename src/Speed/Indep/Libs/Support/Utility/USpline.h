#ifndef SUPPORT_UTILITY_USPLINE_H
#define SUPPORT_UTILITY_USPLINE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "UTypes.h"
#include <list>

#include "Speed/Indep/Libs/Support/Utility/UMath.h"

class SplinePointList : public std::list<UMath::Vector4> {};

// total size: 0x6C
class USpline {
  public:
    enum SplineType {
        kSpline_Bezier = 0,
        kSpline_CatMulRom = 1,
        kSpline_Count = 2,
        kSpline_Invalid = 2,
    };

    USpline();
    ~USpline();

    void BuildSplineEx(const UMath::Vector3 &start, const UMath::Vector3 &startControl, const UMath::Vector3 &end, const UMath::Vector3 &endControl);
    void EvaluateSpline(float t, UMath::Vector4 &result);
    void EvaluateTangent(float t, UMath::Vector4 &tangent);
    float EvaluateCurvatureXZ(float t);

    static const UMath::Matrix4 &GetBasisMatrix(SplineType splineType);

    UMath::Matrix4 fSplineMat;     // offset 0x0, size 0x40
    UMath::Vector4 fLookAt[2];     // offset 0x40, size 0x20
    SplineType fSplineType;        // offset 0x60, size 0x4
    SplinePointList fSplinePtList; // offset 0x64, size 0x8
};

#endif
