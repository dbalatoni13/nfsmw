#ifndef EAXSOUND_EAXSOUND_H
#define EAXSOUND_EAXSOUND_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/AudioMemBase.hpp"
#include "Speed/Indep/Src/EAXSound/SFX_base.hpp"
#include "Speed/Indep/Src/EAXSound/STICH_Playback.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/audiosystem.h"
#include "Speed/Indep/Src/Main/Event.h"
#include "Speed/Indep/Src/Misc/Hermes.h"

// yes that is the correct name for the file

enum eSNDPAUSE_REASON {
    eSNDPAUSE_SMS_MESSAGE = 11,
    eSNDPAUSE_PHOTOFINISH = 10,
    eSNDPAUSE_NISON = 9,
    eSNDPAUSE_QUITTOFE = 8,
    eSNDPAUSE_STARTNEWGAME = 7,
    eSNDPAUSE_MOVIE = 6,
    eSNDPAUSE_ONLINE = 5,
    eSNDPAUSE_MEMCARD = 4,
    eSNDPAUSE_SIMSTATE_ACTIVE = 3,
    eSNDPAUSE_SIMSTATE_INIT = 2,
    eSNDPAUSE_SIMSTATE_IDLE = 1,
    eSNDPAUSE_PAUSEMENU = 0,
    ePAUSE_ERROR = -1,
};

enum eEAXGameState {
    EAXGS_EXIT_FE = 11,
    EAXGS_EXIT_GAME = 10,
    EAXGS_PAUSE_FE_MUSIC = 9,
    EAXGS_PLAY_FE_MUSIC = 8,
    EAXGS_RETURNTOGAME = 7,
    EAXGS_RESUME = 6,
    EAXGS_PAUSE = 5,
    EAXGS_ENTERINGPAUSE = 4,
    EAXGS_INGAME = 3,
    EAXGS_ENTERINGGAME = 2,
    EAXGS_FRONTEND = 1,
    EAXGS_INITIALIZING = 0,
};

enum e3DPlayerMix {
    EAXS3D_TWO_PLAYER_MIX = 1,
    EAXS3D_SINGLE_PLAYER_MIX = 0,
};

enum eSndGameMode {
    SND_PURSUITBREAKER = 10,
    SND_LOADING_SCREEN = 9,
    SND_CARSHOW = 8,
    SND_FREEROAM = 7,
    SND_CHALLENGERACE = 6,
    SND_STREETRACE = 5,
    SND_SMOKESHOW = 4,
    SND_DRIFTRACE = 3,
    SND_DRAGRACE = 2,
    SND_FRONTEND = 1,
    SND_MODE_NONE = 0,
};
enum eAemsStreamBanks {
    MAX_AEMSSTREAMBANKS = 10,
    EAX_AEMS_SPEECH_JAPANESE = 9,
    EAX_AEMS_SPEECH_SPANISH = 8,
    EAX_AEMS_SPEECH_ITALIAN = 7,
    EAX_AEMS_SPEECH_GERMAN = 6,
    EAX_AEMS_SPEECH_FRENCH = 5,
    EAX_AEMS_SPEECH_ENGLISH = 4,
    EAX_AEMS_SPECIAL_CASE = 3,
    EAX_AEMS_AMB_MONOPOINT = 2,
    EAX_AEMS_AMB_STEREO = 1,
    EAX_AEMS_MUSIC_UG0_S = 0,
};

enum eSNDCTLSTATE {
    MAX_SNDCTL_STATES = 18,
    SNDSTATE_SYSTEM_HUD = 17,
    SNDSTATE_ERROR = 16,
    SNDSTATE_FADEOUT = 15,
    SNDSTATE_MINILOAD = 14,
    SNDSTATE_GAMESTARTRACE = 13,
    SNDSTATE_STOP_MUSIC = 12,
    SNDSTATE_FMV = 11,
    SNDSTATE_NIS_ARREST = 10,
    SNDSTATE_NIS_BLK = 9,
    SNDSTATE_NIS_321 = 8,
    SNDSTATE_NIS_INTRO = 7,
    SNDSTATE_NIS_STORY = 6,
    SNDSTATE_FE_SMS_MESSAGE = 5,
    SNDSTATE_FE_UPSCREEN = 4,
    SNDSTATE_FE = 3,
    SNDSTATE_INGAME = 2,
    SNDSTATE_PAUSE = 1,
    SNDSTATE_OFF = 0,
};

// total size: 0xBC
class EAXSound : public AudioMemBase {
  public:
    EAXSound(void);
    virtual ~EAXSound(void);

    void START_321Countdown();

    SFX_Base *GetSFXBase_Object(int nID);

    void StartSND11();
    void StopSND11();

    void NISFinished();

  private:
    int ncompiletest;                            // offset 0x4, size 0x4
    int m_nCopAIStateParam;                      // offset 0x8, size 0x4
    bool bPlayCameraSnapShot;                    // offset 0xC, size 0x1
    bool bPlayCarSounds;                         // offset 0x10, size 0x1
    bool m_bIsSpecialUGMovie;                    // offset 0x14, size 0x1
    bool EngineLoadingBlocked;                   // offset 0x18, size 0x1
    bool m_bIsPaused;                            // offset 0x1C, size 0x1
    bool m_bLostFocus;                           // offset 0x20, size 0x1
    float t_Paused;                              // offset 0x24, size 0x4
    float t_CurTime;                             // offset 0x28, size 0x4
    int FrameCnt;                                // offset 0x2C, size 0x4
    int m_nDebugStreamState;                     // offset 0x30, size 0x4
    struct AudioSettings *m_pCurAudioSettings;   // offset 0x34, size 0x4
    bool m_bPause_MainFNG;                       // offset 0x38, size 0x1
    void *m_pMemoryPoolMem;                      // offset 0x3C, size 0x4
    int m_memoryPoolSize;                        // offset 0x40, size 0x4
    int m_numMemoryAllocations;                  // offset 0x44, size 0x4
    enum eSNDPAUSE_REASON m_LastPauseReason;     // offset 0x48, size 0x4
    int m_transStartTime;                        // offset 0x4C, size 0x4
    int m_startingLoopVolume;                    // offset 0x50, size 0x4
    bool m_bAudioIsPaused;                       // offset 0x54, size 0x1
    bool m_X360_UI_Override;                     // offset 0x58, size 0x1
    char *m_pcsCsisName;                         // offset 0x5C, size 0x4
    struct stSongInfo *m_pNewSongInfoSt;         // offset 0x60, size 0x4
    enum eEAXGameState m_streamManagerState;     // offset 0x64, size 0x4
    char *m_pEAX_SysHeap;                        // offset 0x68, size 0x4
    struct EAXFrontEnd *m_pFESnd;                // offset 0x6C, size 0x4
    struct EAXCommon *m_pCmnSnd;                 // offset 0x70, size 0x4
    struct NFSLiveLink *m_pNFSLiveLink;          // offset 0x74, size 0x4
    e3DPlayerMix m_ePlayerMixMode;               // offset 0x78, size 0x4
    int m_nStereoUpgradeLevel;                   // offset 0x7C, size 0x4
    int m_nGameMode;                             // offset 0x80, size 0x4
    eSndGameMode m_eSndGameMode;                 // offset 0x84, size 0x4
    eSndGameMode m_prevSndGameMode;              // offset 0x88, size 0x4
    int m_nNumCarsInGame;                        // offset 0x8C, size 0x4
    struct EAXSND8Wrapper *m_pEAXSND8Wrapper;    // offset 0x90, size 0x4
    struct EAXS_StreamManager *m_pStreamManager; // offset 0x94, size 0x4
    struct NFSMixMaster *m_pNFSMixMaster;        // offset 0x98, size 0x4
    cSTICH_PlayBack *m_pSTICH_Playback;          // offset 0x9C, size 0x4
    eAemsStreamBanks m_eSpeechLoadBank;          // offset 0xA0, size 0x4
    int m_nSpeechLoadBankIndex;                  // offset 0xA4, size 0x4
    Attrib::Gen::audiosystem *mAttributes;       // offset 0xA8, size 0x4
    Attrib::Gen::audiosystem *mLocalAttr;        // offset 0xAC, size 0x4
    Hermes::HHANDLER mmsgMRestartRace;           // offset 0xB0, size 0x4
    int mEventID;                                // offset 0xB4, size 0x4
    Event::StaticData mData;                     // offset 0xB8, size 0x4
};

bool g_EAXIsPaused(void);

void SetSoundControlState(bool bON, eSNDCTLSTATE esndstate, const char *Reason);

extern EAXSound *g_pEAXSound;

// TODO move these to Ecstasy
extern int eDisableFixUpTables;
extern int eDirtySolids;
extern int eDirtyTextures;
extern int eDirtyAnimations;

#endif
