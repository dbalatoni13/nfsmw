#ifndef GAMECUBE_ECSTASY_EVIEW_PLAT_H
#define GAMECUBE_ECSTASY_EVIEW_PLAT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Ecstasy/EcstasyData.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include <dolphin.h>

class eView;

// TODO is it really possible that this is in GameCube/.../EcstasyE.cpp?
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

#endif
