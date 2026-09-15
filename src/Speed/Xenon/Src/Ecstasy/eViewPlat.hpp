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

// total size: 0x40
struct eRenderTarget {
    char dummy[0x40];
};

#endif
