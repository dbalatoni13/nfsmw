#ifndef EAX_SOUND_HPP
#define EAX_SOUND_HPP // Decl: 37

#include "Speed/Indep/Src/EAXSound/AemsDef.hpp"
#include "Speed/Indep/Src/EAXSound/AudioMemBase.hpp"
#include "Speed/Indep/Src/EAXSound/EAXAudioParams.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSoundEnums.hpp"
#include "Speed/Indep/Src/EAXSound/SoundPause.h"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/EAXSound/SFX_base.hpp"
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

class cSTICH_PlayBack;

// total size: 0xBC
// Decl: 131
class EAXSound : public AudioMemBase {
  public:
    EAXSound(void);
    virtual ~EAXSound();

    void Update(float t);

    void InitializeDriver();

    void RestoreDriver();

    void InitializeSoundBootLoad();

    void Destroy();

    void QueueNISButtonThrough(uint32 anim_id, int camera_track_number);
    void QueueNISStream(uint32 anim_id, int camera_track_number, void (*setmstimecb)(unsigned int, int));
    bool IsNISStreamQueued();
    void PlayNIS();
    void NISFinished();
    bool AreResourceLoadsPending();
    void START_321Countdown();

    static SndBase *GetSndBase_Object(int nID);
    SFX_Base *GetSFXBase_Object(int nID);

    void SetSndBaseObject(SndBase *psb, eMAINMAPSTATES estate, int ntype, int instance);
    void SetSFXBaseObject(SFX_Base *psb, eMAINMAPSTATES estate, int ntype, int instance);

    static int *GetPointerCallback(int nid);

    static void SetSFXOutCallback(int nid, int *ptr);
    static bool SetSFXInputCallback(int nid, int *ptr);

    static int GetStateRefCount(int nstate);

    static void MixMapReadyCallback();

    void StartSND11();
    void StopSND11();

    void InitSndCars();

    void SetCarSoundPlayback(bool _On) {} // Decl: 197
    bool IsPlayingCarSounds() {}          // Decl: 198

    struct stSongInfo *GetNewSongInfo();

    void UpdateSongInfo();
    void InitEATRAX();
    void PlayEATraxSong(int nindex);

    void PlayFEMusic(int nIndex);
    void PauseFEMusic(const char *pMovieString);
    void ResumeFEMusic();

    bool IsAudioStreamReading() {}         // Decl: 211
    bool IsAudioStreamingBlockedByWDR() {} // Decl: 212
    bool DidAudioInterruptWDR() {}         // Decl: 213

    void ReInitMasterVolumes();
    int GetMasterVolume(eMasterMixChannel eMasterMixChannel);

    void UpdateVolumes(struct AudioSettings *paudiosettings, float NewValue);
    void StartNewGamePlay();
    void InitializeFrontEnd();
    void InitializeInGame();
    void LoadInGameSoundBanks(void (*callback)(), int32 callback_param);
    void LoadFrontEndSoundBanks(void (*callback)(), int32 callback_param);
    void UnloadFrontEndSoundBanks();
    void UnLoadInGameSoundBanks();
    void EnterPauseMenu(eSNDPAUSE_REASON pause_reason);
    void ExitPauseMenu(eSNDPAUSE_REASON pause_reason);
    void CloseSound();

    struct NFSMixMaster *GetMixMaster() {}

    void SetSndGameMode(eSndGameMode eGameMode) {
        this->m_prevSndGameMode = this->m_eSndGameMode;
        this->m_eSndGameMode = eGameMode;
    } // Decl: 243

    eSndGameMode GetSndGameMode() {
        return this->m_eSndGameMode;
    } // Decl: 244
    eSndGameMode GetPrevSndGameMode() {
        return this->m_prevSndGameMode;
    } // Decl: 245

    void SetDebugStreamState(int nstate) {} // Decl: 247

    struct EAXFrontEnd *GetFrontEnd() {
        return this->m_pFESnd;
    } // Decl: 250

    EAXCar *ConnectCarSnd(EAX_CarState *pcar);
    EAXCar *GetPlayerTunerCar(int nindex);
    EAXCar *GetAITunerCar(int nindex);
    void DestroyEAXCar(EAX_CarState *pCar);
    struct CSTATE_Helicopter *SpawnHelicopter(struct EAX_HeliState *pHeli);

    void DestroyEAXHeli(struct EAX_HeliState *pHeli);
    unsigned int Random(int range);
    float Random(float range);

    char *GetCsisName() {} // Decl: 276
    void SetCsisName(SndBase *psndbase);
    void SetCsisName(char *pcsAllocName);

    void PlayUISoundFX(eMenuSoundTriggers etriggertype);
    void StopUISoundFX(eMenuSoundTriggers etriggertype);

    struct EAXS_StreamManager *GetStreamManager() {
        return this->m_pStreamManager;
    } // Decl: 282

    int IsSpeechDone() {}      // Decl: 284
    void StopSpeechStream() {} // Decl: 285

    e3DPlayerMix GetPlayerMixMode() {
        return this->m_ePlayerMixMode;
    } // Decl: 287
    void SetPlayerMixMode(e3DPlayerMix emix) {} // Decl: 288
    void PauseAudioStreams() {}                 // Decl: 289
    void ResumeAudioStreams() {}                // Decl: 290
    void ChangeLanguage(int new_language) {}    // Decl: 291

    void CommitAssets();

    cSTICH_PlayBack *GetStichPlayer() {
        return this->m_pSTICH_Playback;
    } // Decl: 307

    eSndAudioMode GetDefaultPlatformAudioMode();

    bool ValidateStreamChunks(const char *filepath, int start, int end);

    eSndAudioMode SetAudioRenderMode(eSndAudioMode mode);

    eSndAudioMode SetAudioModeFromMemoryCard(eSndAudioMode mode);

    AudioSettings *GetCurAudioSettings() {
        return this->m_pCurAudioSettings;
    }

    float GetCurMusicVolume();

    void PlayCameraSnapShot() {} // Decl: 317

    bool PauseFadeComplete() {} // Decl: 326

  private:
    void DebugAndProfile();
    void AttachPlayerCars();

  public:
    Attrib::Gen::audiosystem &GetAttributes() {
        return *this->mAttributes;
    } // Decl: 394
    Attrib::Gen::audiosystem &GetLocalAttr() {
        return *this->mLocalAttr;
    } // 395

    void ReStartRace(bool bIs321);

    void RefreshLocalAttr();

  private:
    int ncompiletest;                          // offset 0x4, size 0x4
    int m_nCopAIStateParam;                    // offset 0x8, size 0x4
    bool bPlayCameraSnapShot;                  // offset 0xC, size 0x1
    bool bPlayCarSounds;                       // offset 0x10, size 0x1
    bool m_bIsSpecialUGMovie;                  // offset 0x14, size 0x1
    bool EngineLoadingBlocked;                 // offset 0x18, size 0x1
    bool m_bIsPaused;                          // offset 0x1C, size 0x1
    bool m_bLostFocus;                         // offset 0x20, size 0x1
    float t_Paused;                            // offset 0x24, size 0x4
    float t_CurTime;                           // offset 0x28, size 0x4
    int FrameCnt;                              // offset 0x2C, size 0x4
    int m_nDebugStreamState;                   // offset 0x30, size 0x4
    struct AudioSettings *m_pCurAudioSettings; // offset 0x34, size 0x4
    bool m_bPause_MainFNG;                     // offset 0x38, size 0x1
    void *m_pMemoryPoolMem;                    // offset 0x3C, size 0x4
    int m_memoryPoolSize;                      // offset 0x40, size 0x4
    int m_numMemoryAllocations;                // offset 0x44, size 0x4
    eSNDPAUSE_REASON m_LastPauseReason;        // offset 0x48, size 0x4
    int m_transStartTime;                      // offset 0x4C, size 0x4
    int m_startingLoopVolume;                  // offset 0x50, size 0x4
    bool m_bAudioIsPaused;                     // offset 0x54, size 0x1
    bool m_X360_UI_Override;                   // offset 0x58, size 0x1
    char *m_pcsCsisName;                       // offset 0x5C, size 0x4
    stSongInfo *m_pNewSongInfoSt;              // offset 0x60, size 0x4
    eEAXGameState m_streamManagerState;        // offset 0x64, size 0x4
    char *m_pEAX_SysHeap;                      // offset 0x68, size 0x4
    EAXFrontEnd *m_pFESnd;                     // offset 0x6C, size 0x4
    struct EAXCommon *m_pCmnSnd;               // offset 0x70, size 0x4
    struct NFSLiveLink *m_pNFSLiveLink;        // offset 0x74, size 0x4
    e3DPlayerMix m_ePlayerMixMode;             // offset 0x78, size 0x4
    int m_nStereoUpgradeLevel;                 // offset 0x7C, size 0x4
    int m_nGameMode;                           // offset 0x80, size 0x4
    eSndGameMode m_eSndGameMode;               // offset 0x84, size 0x4
    eSndGameMode m_prevSndGameMode;            // offset 0x88, size 0x4
    int m_nNumCarsInGame;                      // offset 0x8C, size 0x4
    struct EAXSND8Wrapper *m_pEAXSND8Wrapper;  // offset 0x90, size 0x4
    EAXS_StreamManager *m_pStreamManager;      // offset 0x94, size 0x4
    struct NFSMixMaster *m_pNFSMixMaster;      // offset 0x98, size 0x4
    cSTICH_PlayBack *m_pSTICH_Playback;        // offset 0x9C, size 0x4
    eAemsStreamBanks m_eSpeechLoadBank;        // offset 0xA0, size 0x4
    int m_nSpeechLoadBankIndex;                // offset 0xA4, size 0x4
    Attrib::Gen::audiosystem *mAttributes;     // offset 0xA8, size 0x4
    Attrib::Gen::audiosystem *mLocalAttr;      // offset 0xAC, size 0x4
    Hermes::HHANDLER mmsgMRestartRace;         // offset 0xB0, size 0x4
    int mEventID;                              // offset 0xB4, size 0x4
    Event::StaticData mData;                   // offset 0xB8, size 0x4
};

// total size: 0x18
// Decl: 422
struct SND_Params {
    SND_Params()
        : Vol(0x7FFF),     //
          Pitch(0x1000) {} // Decl: 423

    SND_Params(int _ID, int _Vol, int _Pitch, int _Az, int _Mag, int _RVerb)
        : ID(_ID),         //
          Vol(_Vol),       //
          Pitch(_Pitch),   //
          Az(_Az),         //
          Mag(_Mag),       //
          RVerb(_RVerb) {} // Decl: 433

    int ID;    // offset 0x0, size 0x4, Decl: 443
    int Vol;   // offset 0x4, size 0x4, Decl: 444
    int Pitch; // offset 0x8, size 0x4, Decl: 445
    int Az;    // offset 0xC, size 0x4, Decl: 446
    int Mag;   // offset 0x10, size 0x4, Decl: 447
    int RVerb; // offset 0x14, size 0x4, Decl: 448
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
