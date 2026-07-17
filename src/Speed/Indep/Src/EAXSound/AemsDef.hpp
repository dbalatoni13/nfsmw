#ifndef AEMS_DEF_HPP
#define AEMS_DEF_HPP // Decl: 27

#define EAXFECOMMONPLAYSTRUCTS // Decl: 32

// total size: 0x14
// Decl: 279
struct PlayCommonSampleSt {
    int nHack;   // offset 0x0, size 0x4
    int id;      // offset 0x4, size 0x4
    int volume;  // offset 0x8, size 0x4
    int pitch;   // offset 0xC, size 0x4
    int azimuth; // offset 0x10, size 0x4
};

typedef PlayCommonSampleSt PlayCommonSampleSt; // Decl: 289

// total size: 0x14
// Decl: 292
struct PlayFrontEndSampleSt {
    int nHack;   // offset 0x0, size 0x4
    int id;      // offset 0x4, size 0x4
    int volume;  // offset 0x8, size 0x4
    int pitch;   // offset 0xC, size 0x4
    int azimuth; // offset 0x10, size 0x4
};

typedef PlayFrontEndSampleSt PlayFrontEndSampleSt; // Decl: 302

#define AEMS_EVENT_NONE -1              // Decl: 305
#define AEMS_EVENT_PLAYCOMMONSAMPLE 0   // Decl: 306
#define AEMS_EVENT_PLAYFRONTENDSAMPLE 1 // Decl: 307

// Decl: 312
enum eEAXGameState {
    EAXGS_INITIALIZING = 0,
    EAXGS_FRONTEND = 1,
    EAXGS_ENTERINGGAME = 2,
    EAXGS_INGAME = 3,
    EAXGS_ENTERINGPAUSE = 4,
    EAXGS_PAUSE = 5,
    EAXGS_RESUME = 6,
    EAXGS_RETURNTOGAME = 7,
    EAXGS_PLAY_FE_MUSIC = 8,
    EAXGS_PAUSE_FE_MUSIC = 9,
    EAXGS_EXIT_GAME = 10,
    EAXGS_EXIT_FE = 11,
};

// Decl: 400
enum eAemsUpgradeLevel {
    AEMS_NO_LEVEL = -1,
    AEMS_LEVEL0 = 0,
    AEMS_LEVEL1 = 1,
    AEMS_LEVEL2 = 2,
    AEMS_LEVEL3 = 3,
    AEMS_MAX_LEVEL = 4,
};

// Decl: 483
enum eAemsStreamBanks {
    EAX_AEMS_MUSIC_UG0_S = 0,
    EAX_AEMS_AMB_STEREO = 1,
    EAX_AEMS_AMB_MONOPOINT = 2,
    EAX_AEMS_SPECIAL_CASE = 3,
    EAX_AEMS_SPEECH_ENGLISH = 4,
    EAX_AEMS_SPEECH_FRENCH = 5,
    EAX_AEMS_SPEECH_GERMAN = 6,
    EAX_AEMS_SPEECH_ITALIAN = 7,
    EAX_AEMS_SPEECH_SPANISH = 8,
    EAX_AEMS_SPEECH_JAPANESE = 9,
    MAX_AEMSSTREAMBANKS = 10,
};

#endif
