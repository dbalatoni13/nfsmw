#ifndef SPEED_INDEP_DEBUG_COMMON_DEBUGDRAW_H
#define SPEED_INDEP_DEBUG_COMMON_DEBUGDRAW_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UVectorMath.h"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"

struct ePolyVertex {
    float x;
    float y;
    float z;
    unsigned int colour;
    float u;
    float v;
    float q;
    float t;
};

struct DrawPrimTri {
    short int fLifeSpan;
    short int fTimeType;
};

struct DrawPrimLin {
    short int fLifeSpan;
    short int fTimeType;
};

struct DebugDraw {
  private:
    static DebugDraw *fgDbgDraw;
    bool fZTest;
    bool fEnabled;
    bool fCheckDrawOverlap;
    short int fTimeType;
    int fNumTriPrims;
    int fNumLinPrims;
    DrawPrimTri *fTriPrimList;
    unsigned int *fTriColourList;
    UMath::Vector4 *fTriVertList;
    TextureInfo *fTextureInfo;
    DrawPrimLin *fLinPrimList;
    unsigned int *fLinColourList;
    UMath::Vector4 *fLinVertList;
    UMath::Vector4 fCachedCameraPos;
    UMath::Vector4 fCachedCameraUpVec;
    UMath::Vector4 fCachedCameraFwdVec;

  public:
    void DrawAll();
    void CacheCameraInfo();
};

void eDrawKLine(const ePolyVertex *verts, unsigned int numLines);

#endif
