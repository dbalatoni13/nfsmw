#ifndef EAX_SOUND_HPP
#define EAX_SOUND_HPP // Decl: 37

#include "Speed/Indep/Src/EAXSound/AemsDef.hpp"
#include "Speed/Indep/Src/EAXSound/AudioMemBase.hpp"
#include "Speed/Indep/Src/EAXSound/EAXAudioParams.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSoundEnums.hpp"
#include "Speed/Indep/Src/EAXSound/SoundPause.h"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/EAXSound/SFX_base.hpp"
#include "Speed/Indep/Src/EAXSound/STICH_Playback.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/audiosystem.h"
#include "Speed/Indep/Src/Main/Event.h"
#include "Speed/Indep/Src/Misc/Hermes.h"

#define MAX_EAXMODS 32; // Decl: 39

#define MAX_NUM_DEBUG_BANKS 15 // Decl: 41

#define SND_HIGH_DETAIL 44100 // Decl: 43

#define SND_LOW_DETAIL 22050   // Decl: 44
#define MAX_EAX_AITUNERCARS 10 // Decl: 46

#define SNDPRINTF_BUFFER_SIZE 512 // Decl: 467
#define SNDPRINTF_CHANNEL 9       // Decl: 468
#define NO_SNDPRINTF              // Decl: 471
#define SndPrintf if (0)          // Decl: 478

// total size: 0xBC
class EAXSound : public AudioMemBase {
  public:
    EAXSound(void);
    virtual ~EAXSound(void);

    void Update(float t);
    bool AreResourceLoadsPending();

    void START_321Countdown();

    SFX_Base *GetSFXBase_Object(int nID);

    void StartSND11();
    void StopSND11();

    void QueueNISStream(unsigned int anim_id, int camera_track_number, void (*setmstimecb)(unsigned int, int));
    bool IsNISStreamQueued();
    void NISFinished();

    void PlayUISoundFX(eMenuSoundTriggers etriggertype);

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
    eSNDPAUSE_REASON m_LastPauseReason;          // offset 0x48, size 0x4
    int m_transStartTime;                        // offset 0x4C, size 0x4
    int m_startingLoopVolume;                    // offset 0x50, size 0x4
    bool m_bAudioIsPaused;                       // offset 0x54, size 0x1
    bool m_X360_UI_Override;                     // offset 0x58, size 0x1
    char *m_pcsCsisName;                         // offset 0x5C, size 0x4
    struct stSongInfo *m_pNewSongInfoSt;         // offset 0x60, size 0x4
    eEAXGameState m_streamManagerState;          // offset 0x64, size 0x4
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

void InitializeSoundDriver();

bool g_EAXIsPaused(void);

void SetSoundControlState(bool bON, eSNDCTLSTATE esndstate, const char *Reason);

extern EAXSound *g_pEAXSound;

// TODO move these to Ecstasy
extern int32 eDisableFixUpTables;
extern int32 eDirtySolids;
extern int32 eDirtyTextures;
extern int32 eDirtyAnimations;

#endif
