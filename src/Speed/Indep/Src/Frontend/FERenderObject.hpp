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
    void *operator new(size_t size);
    void operator delete(void *ptr);

    ePoly EPoly;               // offset 0x8, size 0x94
    TextureInfo *pTexture;     // offset 0x9C, size 0x4
    TextureInfo *pTextureMask; // offset 0xA0, size 0x4
};

// total size: 0x64
class FERenderObject : public bTNode<FERenderObject> {
  public:
    FERenderObject(FEObject *pOwner, TextureInfo *pTexture);
    ~FERenderObject();
    static void *operator new(size_t size) {
        return bOMalloc(mpobFERenderObjectSlotPool);
    }
    static void operator delete(void *p) {
        bFree(mpobFERenderObjectSlotPool, p);
    }
    void SetTransform(bMatrix4 *pMatrix);
    FERenderEPoly *AddPoly(float x0, float y0, float x1, float y1, float z, float s0, float t0, float s1, float t1, uint32 *in_colors,
                           FEPackageRenderInfo *pkg_render_info);
    void AddPoly(float x0, float y0, float x1, float y1, float z, float s0, float t0, float s1, float t1, uint32 *in_colors, TextureInfo *texture,
                 FEPackageRenderInfo *pkg_render_info);
    void AddPoly(float x0, float y0, float x1, float y1, float z, float s0, float t0, float s1, float t1, uint32 *in_colors, FEClipInfo *pClipInfo,
                 FEPackageRenderInfo *pkg_render_info);
    void AddPolyWithMask(float x0, float y0, float x1, float y1, float z, float s0, float t0, float s1, float t1, float s2, float t2, float s3,
                         float t3, uint32 *colors, TextureInfo *texture, TextureInfo *textureMask);
    void AddPolyWithRotatedMask(float x0, float y0, float x1, float y1, float z, float s0, float t0, float s1, float t1, float s2, float t2, float s3,
                                float t3, float s4, float t4, float s5, float t5, uint32 *colors, TextureInfo *texture, TextureInfo *textureMask);

    uint32 CountPolys() {}
    FEObject *GetObject() const {}
    void SetTexture(TextureInfo *texture) {
        mpobTexture = texture;
    }
    void Render();
    void ReadyToRender() {
        mulNumTimesRendered++;
        mulFlags |= 2;
    }
    bool IsReadyToRender() {
        return (mulFlags & 2) != 0;
    }
    void DeleteMeAfterRendering() {}
    bool ShouldIBeDeletedAfterRendering() {}
    void Clear(FEPackageRenderInfo *pkg_render_info);
    static void Initialize();
    static void Terminate();

  private:
    uint32 ClipGeneral(FEClipInfo *pClipInfo, bVector3 *v, bVector2 *uv, bVector4 *colors, bVector3 *nv, bVector2 *nuv, bVector4 *ncolors);
    uint32 ClipAligned(FEClipInfo *pClipInfo, bVector3 *v, bVector2 *uv, bVector4 *colors, bVector3 *nv, bVector2 *nuv, bVector4 *ncolors);

    FEObject *mpobOwner;               // offset 0x8, size 0x4
    uint32 mulFlags;                   // offset 0xC, size 0x4
    uint32 mulNumTimesRendered;        // offset 0x10, size 0x4
    TextureInfo *mpobTexture;          // offset 0x14, size 0x4
    bTList<FERenderEPoly> mobPolyList; // offset 0x18, size 0x8
    uint32 mPolyCount;                 // offset 0x20, size 0x4
    bMatrix4 mstTransform;             // offset 0x24, size 0x40
};

#endif
