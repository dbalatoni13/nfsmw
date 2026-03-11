#ifndef EAXSOUND_CARSFX_SFXOBJ_PATHFINDER_H
#define EAXSOUND_CARSFX_SFXOBJ_PATHFINDER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX.hpp"

struct SFXObj_Pathfinder : CARSFX {
    // total size: 0x120
    char _pad_pathfinder[0x11c - 0x28];
    unsigned int m_Flags; // offset 0x11c, size 0x4

    void Set321(bool bon) {
        m_Flags |= bon;
    }
};

#endif
