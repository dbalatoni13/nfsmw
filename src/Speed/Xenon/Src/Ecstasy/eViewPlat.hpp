#ifndef XENON_ECSTASY_EVIEW_PLAT_H
#define XENON_ECSTASY_EVIEW_PLAT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Ecstasy/EcstasyData.hpp"
#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

struct eView;

// total size: 0x174
struct eViewPlatInfo {
    char dummy[0x174];
};

enum eVisibleState;

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

// total size: 0x40
struct eRenderTarget {
    char dummy[0x40];
};

#endif
