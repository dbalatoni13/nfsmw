#ifndef EAXSOUND_CARSFX_SFXOBJ_NISSTREAM_H
#define EAXSOUND_CARSFX_SFXOBJ_NISSTREAM_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX.hpp"

struct SFXObj_NISStream : CARSFX {
    void StartNIS();
    bool IsNISStreamReady() { return m_bNISAudioStreamReady; }

    static bool m_bNISAudioStreamReady;
};

#endif
