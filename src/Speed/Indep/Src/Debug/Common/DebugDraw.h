#ifndef SPEED_INDEP_DEBUG_COMMON_DEBUGDRAW_H
#define SPEED_INDEP_DEBUG_COMMON_DEBUGDRAW_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UVectorMath.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
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
    DebugDraw();

    static void Init() {
        if (!fgDbgDraw) {
            fgDbgDraw = new ("DebugDraw", 0) DebugDraw;
        }
    }

    static DebugDraw &Get() {
        Init();
        return *fgDbgDraw;
    }

    const UMath::Vector4 &GetCameraPos() {
        return fCachedCameraPos;
    }

    const UMath::Vector4 &GetCameraFwdVec() {
        return fCachedCameraFwdVec;
    }

    const UMath::Vector4 &GetCameraUpVec() {
        return fCachedCameraUpVec;
    }

    bool InView(const UMath::Vector3 &pt, float radius);
    bool OutOfRange(const UMath::Vector3 &pt, float radius);
    bool OutOfRange(const UMath::Vector4 &pt, float radius);
    float GetDrawRange();
    short int GetTimeType();
    void AllocLists();
    void DeAllocLists();
    void LineSeg(const UMath::Matrix4 &mat, const UMath::Vector4 &pt0,
                 const UMath::Vector4 &pt1, unsigned int c, short int lifeSpan);
    void LineSeg(const UMath::Vector3 &pt0, const UMath::Vector3 &pt1,
                 unsigned int c, short int lifeSpan);
    void Sphere(const UMath::Matrix4 &mat, float radius, unsigned int c,
                short int lifeSpan, bool bShadow, int texture);
    void Sphere(const UMath::Vector3 &basePt, float radius, unsigned int c,
                short int lifeSpan, bool bShadow, int texture);
    void Cylinder(const UMath::Matrix4 &mat, float radius, float height,
                  unsigned int c, short int lifeSpan, bool bShadow, int texture);
    void Cylinder(const UMath::Vector3 &basePt, float radius, float height,
                  unsigned int c, short int lifeSpan, bool bShadow, int texture);
    void CylinderSection(const UMath::Matrix4 &mat, float radTop, float radBot,
                         float height, unsigned int c, short int lifeSpan, int texture);
    void Box(const UMath::Matrix4 &mat, float width, float height, float depth,
             unsigned int c, short int lifeSpan, bool bShadow, int texture, bool frombase);
    void Vector(const UMath::Vector3 &pt, const UMath::Vector3 &vec, float scale,
                unsigned int c, short int lifeSpan, int texture);
    void Vector(const UMath::Vector4 &base, const UMath::Vector4 &vec, float scale,
                unsigned int c, short int lifeSpan, int texture);
    void DrawAll();
    void CacheCameraInfo();
};

void eDrawKLine(const ePolyVertex *verts, unsigned int numLines);

#endif
