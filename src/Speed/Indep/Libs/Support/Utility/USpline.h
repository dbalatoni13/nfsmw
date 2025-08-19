#ifndef SUPPORT_UTILITY_USPLINE_H
#define SUPPORT_UTILITY_USPLINE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <list>

#include "Speed/Indep/Libs/Support/Utility/UMath.h"

class SplinePointList : public std::list<UMath::Vector4> {};

class USpline {
    enum SplineType {
        OVERHAUSER_EXTRAPOLATED = 2,
        OVERHAUSER_LOOP = 1,
        OVERHAUSER_LINE = 0,
    };

    // total size: 0x6C
    UMath::Matrix4 fSplineMat;     // offset 0x0, size 0x40
    UMath::Vector4 fLookAt[2];     // offset 0x40, size 0x20
    SplineType fSplineType;        // offset 0x60, size 0x4
    SplinePointList fSplinePtList; // offset 0x64, size 0x8
};

#endif
