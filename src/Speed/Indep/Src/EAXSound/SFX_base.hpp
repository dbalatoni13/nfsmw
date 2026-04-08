#ifndef EAXSOUND_SFX_BASE_H
#define EAXSOUND_SFX_BASE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/SndBase.hpp"

enum eSFXOBJ_MAIN_TYPES {
    SFXOBJ_MUSIC = 0,
    SFXOBJ_SPEECH = 1,
    SFXOBJ_AMBIENCE = 2,
    SFXOBJ_MOVIES = 3,
    SFXOBJ_COMMON = 4,
    SFXOBJ_NISPROJ_STRMS = 5,
    SFXOBJ_MOMENT_STRMS = 6,
    SFXOBJ_FEHUD = 7,
};

// total size: 0x24
struct SFX_Base : public SndBase {
    ~SFX_Base() override {}
};

#endif
