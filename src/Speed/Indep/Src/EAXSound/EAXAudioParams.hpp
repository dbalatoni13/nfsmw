//
//
//
//
#ifndef _EAXAUDIOPARAMS_HPP
#define _EAXAUDIOPARAMS_HPP // Decl: 6

#define SNDUPG_ENGINE_BASE 0x00           // Decl: 12
#define SNDUPG_ENGINE_STREET 0x01         // Decl: 13
#define SNDUPG_ENGINE_PRO (0x01) << 1     // Decl: 14
#define SNDUPG_ENGINE_EXTREME (0x01) << 2 // Decl: 15

// Decl: 17
enum eGAMEMODE {
    SNDGM_FREEROAM = 0,
    SNDGM_RACE = 1,
    SNDGM_FRONTEND = 2,
    SNDGM_SPLITSCREEN = 3,
};

// Decl: 26
enum eSndAudioMode {
    AUDIO_MODE_MONO = 0,
    AUDIO_MODE_STEREO = 1,
    AUDIO_MODE_PROLOGIC = 2,
    AUDIO_MODE_MIN = 0,
    AUDIO_MODE_MAX = 2,
};

// Decl: 52
enum eSndGameMode {
    SND_MODE_NONE = 0,
    SND_FRONTEND = 1,
    SND_DRAGRACE = 2,
    SND_DRIFTRACE = 3,
    SND_SMOKESHOW = 4,
    SND_STREETRACE = 5,
    SND_CHALLENGERACE = 6,
    SND_FREEROAM = 7,
    SND_CARSHOW = 8,
    SND_LOADING_SCREEN = 9,
    SND_PURSUITBREAKER = 10,
};

#endif
