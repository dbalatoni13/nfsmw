#ifndef MISC_QUICKSPLINE_H
#define MISC_QUICKSPLINE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

enum QuickSplineEndPointType {
    QUICKSPLINE_LOOP = 0,
    QUICKSPLINE_LINE = 1,
    QUICKSPLINE_EXTRAPOLATED = 2,
};

enum QuickSplineBasisType {
    QUICKSPLINE_OVERHAUSER = 0,
};

// total size: 0x2C
class QuickSpline {
  public:
    void MemoryImageLoad(bVector4 *control_point_buffer);
    bool HasKink();

    unsigned int GetHash() {
        return nHash;
    }

    friend int LoaderQuickSpline(bChunk *pChunk);

  private:
    void FixupEndpoints();

    unsigned int nHash;                   // offset 0x0, size 0x4
    QuickSplineEndPointType EndPointType; // offset 0x4, size 0x4
    QuickSplineBasisType BasisType;       // offset 0x8, size 0x4
    float MaxParam;                       // offset 0xC, size 0x4
    float MinParam;                       // offset 0x10, size 0x4
    float Length;                         // offset 0x14, size 0x4
    char ControlPointsDirty;              // offset 0x18, size 0x1
    char BufferWasAllocated;              // offset 0x19, size 0x1
    char MinControlPoints;                // offset 0x1A, size 0x1
    unsigned short MaxControlPoints;      // offset 0x1C, size 0x2
    unsigned short NumControlPoints;      // offset 0x1E, size 0x2
    bVector4 *pControlPointBuffer;        // offset 0x20, size 0x4
    bVector4 *pControlPoints;             // offset 0x24, size 0x4
    bMatrix4 *pBasisMatricies;            // offset 0x28, size 0x4
};

#endif
