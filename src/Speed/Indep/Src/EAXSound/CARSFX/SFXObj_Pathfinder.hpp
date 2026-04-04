#ifndef EAXSOUND_CARSFX_SFXOBJ_PATHFINDER_H
#define EAXSOUND_CARSFX_SFXOBJ_PATHFINDER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Pathfinder5.hpp"

struct SFXCTL_Pathfinder;

struct SFXObj_Pathfinder : CARSFX {
    // total size: 0x120
    stPFParms m_PFParms[2];                 // offset 0x28, size 0xF0
    SFXCTL_Pathfinder *m_pSFXCTL_Pathfinder; // offset 0x118, size 0x4
    unsigned int m_Flags;                   // offset 0x11C, size 0x4

    void Set321(bool bon) {
        m_Flags |= bon;
    }
};

#endif
