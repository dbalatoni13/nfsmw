#ifndef FRONTEND_FERENDEROBJECT_H
#define FRONTEND_FERENDEROBJECT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/Src/FEng/FEObject.h"
#include "Speed/Indep/Src/Frontend/cFEngRender.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"

struct FEClipInfo;
struct FEPackageRenderInfo;

extern SlotPool *mpobFERenderObjectSlotPool;

// total size: 0xA4
class FERenderEPoly : public bTNode<FERenderEPoly> {
  public:
    void *operator new(unsigned int size);
    void operator delete(void *p);

    ePoly EPoly;               // offset 0x8, size 0x94
    TextureInfo *pTexture;     // offset 0x9C, size 0x4
    TextureInfo *pTextureMask; // offset 0xA0, size 0x4
};

// total size: 0x64
class FERenderObject : public bTNode<FERenderObject> {
  public:
    FERenderObject(FEObject *obj, TextureInfo *tex);
    ~FERenderObject();
    void operator delete(void *p) {
        bFree(mpobFERenderObjectSlotPool, p);
    }
    void SetTransform(bMatrix4 *pMatrix);
    void Render();
    void Clear(FEPackageRenderInfo *pkg_render_info);
    FERenderEPoly *AddPoly(float x0, float y0, float x1, float y1, float z,
                           float s0, float t0, float s1, float t1,
                           unsigned int *in_colors, FEPackageRenderInfo *pkg_render_info);
    void AddPoly(float x0, float y0, float x1, float y1, float z,
                 float s0, float t0, float s1, float t1,
                 unsigned int *in_colors, TextureInfo *texture, FEPackageRenderInfo *pkg_render_info);
    void AddPoly(float x0, float y0, float x1, float y1, float z,
                 float s0, float t0, float s1, float t1,
                 unsigned int *in_colors, FEClipInfo *pClipInfo, FEPackageRenderInfo *pkg_render_info);
    void AddPolyWithRotatedMask(float x0, float y0, float x1, float y1, float z,
                                float s0, float t0, float s1, float t1,
                                float ms0, float mt0, float ms1, float mt1,
                                float ms2, float mt2, float ms3, float mt3,
                                unsigned int *in_colors, TextureInfo *texture, TextureInfo *mask);
    unsigned int ClipGeneral(FEClipInfo *pClipInfo, bVector3 *v, bVector2 *uv, bVector4 *colors,
                             bVector3 *nv, bVector2 *nuv, bVector4 *ncolors);
    unsigned int ClipAligned(FEClipInfo *pClipInfo, bVector3 *v, bVector2 *uv, bVector4 *colors,
                             bVector3 *nv, bVector2 *nuv, bVector4 *ncolors);
    static void Initialize();

    void ReadyToRender() {
        mulNumTimesRendered++;
        mulFlags |= 2;
    }

    bool IsReadyToRender() {
        return (mulFlags & 2) != 0;
    }

    void SetTexture(TextureInfo *texture) {
        mpobTexture = texture;
    }

    FEObject *mpobOwner;               // offset 0x8, size 0x4
    unsigned int mulFlags;             // offset 0xC, size 0x4
    unsigned int mulNumTimesRendered;  // offset 0x10, size 0x4
    TextureInfo *mpobTexture;          // offset 0x14, size 0x4
    bTList<FERenderEPoly> mobPolyList; // offset 0x18, size 0x8
    unsigned int mPolyCount;           // offset 0x20, size 0x4
    bMatrix4 mstTransform;             // offset 0x24, size 0x40
};

#endif
