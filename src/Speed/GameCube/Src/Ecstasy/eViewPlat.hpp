#ifndef GAMECUBE_ECSTASY_EVIEW_PLAT_H
#define GAMECUBE_ECSTASY_EVIEW_PLAT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Ecstasy/EcstasyData.hpp"
#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include <dolphin.h>

struct eView;

struct eViewPlatInfo {
    // total size: 0x174
    bMatrix4 WorldViewMatrix;          // offset 0x0, size 0x40
    bMatrix4 ViewScreenMatrix;         // offset 0x40, size 0x40
    bMatrix4 WorldClipMatrix;          // offset 0x80, size 0x40
    bMatrix4 WorldScreenMatrix;        // offset 0xC0, size 0x40
    Mtx44 *LightPerspectiveProjection; // offset 0x100, size 0x4
    eView *ReflectionView;             // offset 0x104, size 0x4
    eView *SpecularView;               // offset 0x108, size 0x4
    float aspect;                      // offset 0x10C, size 0x4
    float fovscl;                      // offset 0x110, size 0x4
    bVector4 ClippingPlanes[6];        // offset 0x114, size 0x60

    inline eViewPlatInfo();
    inline bMatrix4 *GetWorldViewMatrix();
    inline bMatrix4 *GetViewScreenMatrix();
    inline bMatrix4 *GetWorldClipMatrix();
    inline bMatrix4 *GetWorldScreenMatrix();

    void SetWorldViewMatrix(bMatrix4 *view);
    void SetViewScreenMatrix(bMatrix4 *proj);

    Mtx44 *GetLightPerspectiveProjection() {
        return LightPerspectiveProjection;
    };
    void SetLightPerspectiveProjection(Mtx44 *projection) {
        LightPerspectiveProjection = projection;
    }
};

enum eVisibleState;

enum EVIEWMODE {
    EVIEWMODE_TWOV = 4,
    EVIEWMODE_TWOH = 3,
    EVIEWMODE_ONE_RVM = 2,
    EVIEWMODE_ONE = 1,
    EVIEWMODE_NONE = 0,
};

class eViewPlatInterface {
    // total size: 0x4
    eViewPlatInfo *PlatInfo; // offset 0x0, size 0x4

  public:
    eViewPlatInfo *GetPlatInfo() {
        return PlatInfo;
    }
    void SetPlatInfo(eViewPlatInfo *info) {
        PlatInfo = info;
    }

    static eViewPlatInfo *GimmeMyViewPlatInfo(int view_id);
    eVisibleState GetVisibleStateGB(const bVector3 *aabb_min, const bVector3 *aabb_max, bMatrix4 *local_world);
    eVisibleState GetVisibleStateSB(const bVector3 *aabb_min, const bVector3 *aabb_max, bMatrix4 *local_world);
};

struct eRenderTarget {
    // total size: 0x40
    TARGET_ID ID;                     // offset 0x0, size 0x4
    const char *Name;                 // offset 0x4, size 0x4
    int Active;                       // offset 0x8, size 0x4
    int ScissorX;                     // offset 0xC, size 0x4
    int ScissorY;                     // offset 0x10, size 0x4
    int ScissorW;                     // offset 0x14, size 0x4
    int ScissorH;                     // offset 0x18, size 0x4
    int FrameAddress;                 // offset 0x1C, size 0x4
    int FrameWidth;                   // offset 0x20, size 0x4
    int FrameHeight;                  // offset 0x24, size 0x4
    FILTER_ID CopyFilterID;           // offset 0x28, size 0x4
    struct _GXColor BackgroundColour; // offset 0x2C, size 0x4
    int ClearBackground;              // offset 0x30, size 0x4
    bMatrix4 *WorldClip;              // offset 0x34, size 0x4
    bMatrix4 *WorldView;              // offset 0x38, size 0x4
    bMatrix4 *ViewScreen;             // offset 0x3C, size 0x4

    FILTER_ID GetCopyFilter() {
        return CopyFilterID;
    }

    TextureInfo *GetTextureInfo();
};

#endif
