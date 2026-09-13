#ifndef EAX_SOUND_HPP
#define EAX_SOUND_HPP // Decl: 37

#define MAX_EAXMODS 32; // Decl: 39

#define MAX_NUM_DEBUG_BANKS 15 // Decl: 41

#define SND_HIGH_DETAIL 44100 // Decl: 43

#define SND_LOW_DETAIL 22050   // Decl: 44
#define MAX_EAX_AITUNERCARS 10 // Decl: 46

#include "Speed/Indep/Src/EAXSound/AemsDef.hpp"
#include "Speed/Indep/Src/EAXSound/AudioMemBase.hpp"
#include "Speed/Indep/Src/EAXSound/EAXAudioParams.hpp"
#include "Speed/Indep/Src/EAXSound/EAXFrontEnd.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSND8Wrapper.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSoundEnums.hpp"
#include "Speed/Indep/Src/EAXSound/SoundPause.h"
#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_Enums.hpp"
#include "Speed/Indep/Src/EAXSound/Dynamic_Mixer/NFSMixMaster.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/EAXSound/Dynamic_Mixer/NFSLiveLink.hpp"
#include "Speed/Indep/Src/EAXSound/SFX_base.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/audiosystem.h"
#include "Speed/Indep/Src/Main/Event.h"
#include "Speed/Indep/Src/Misc/Hermes.h"
#include "Speed/Indep/Src/EAXSound/EAXFrontEnd.hpp"

class AudioSettings;

class cSTICH_PlayBack;

// TODO where do these go?
////////////

enum eSFXOBJ_COLLISON_TYPES {
    eSFXOBJ_COLLISION = 0,
    eSFXOBJ_SCRAPES = 1,
};

typedef eSFXOBJ_COLLISON_TYPES reflection_typedef_eSFXOBJ_COLLISON_TYPES;

enum eSFXOBJ_ENVIRONMENT_TYPES {
    SFXOBJ_WORLDOBJECT = 0,
};

typedef eSFXOBJ_ENVIRONMENT_TYPES reflection_typedef_eSFXOBJ_ENVIRONMENT_TYPES;

enum eSFXOBJ_PLANES {
    SFXOBJ_PLANES = 0,
};

typedef eSFXOBJ_PLANES reflection_typedef_eSFXOBJ_PLANES;

enum eSFXOBJ_TRAINS {
    SFXOBJ_TRAIN = 0,
};

typedef eSFXOBJ_TRAINS reflection_typedef_eSFXOBJ_TRAINS;

enum eSFXOBJ_PLAYER_TYPES {
    eCARSFX_AEMS_ENGINE = 0,
    eCARSFX_SINGLE_GINSU = 1,
    eCARSFX_DUAL_GINSU = 2,
    eCARSFX_SHIFTS = 3,
    eCARSFX_TURBOS = 4,
    eCARSFX_NITRO = 5,
    eCARSFX_SPRKCH = 6,
    eCARSFX_SKIDS = 7,
    eCARSFX_ROADNZ = 8,
    eCARSFX_WINDNZ = 9,
    eCARSFX_HYDRAULICS = 10,
    eCARSFX_RAIN = 11,
    eCARSFX_WINDWEATHER = 12,
    eCARSFX_BOTTOMOUT = 13,
    eCARSFX_DRIVEBY = 14,
    eCARSFX_CARDAMAGE = 15,
    eSFXOBJ_REVERB = 16,
    eCARSFX_SIREN = 17,
    eCARSFX_PRECOLWOOSH = 18,
    eCARSFX_TRUCKFX = 19,
    eCARSFX_HORN = 20,
    eSFX_MAXCARSFXOBJGROUPS = 21,
};

typedef eSFXOBJ_PLAYER_TYPES reflection_typedef_eSFXOBJ_PLAYER_TYPES;

enum eVOL_WINDWEATHER {
    eVOL_WINDWEATHER_MAIN = 0,
};

typedef eVOL_WINDWEATHER reflection_typedef_eVOL_WINDWEATHER;

enum eSFXOBJ_TRAFFIC_TYPES {
    eCARSFX_TRAFFIC_ENG = 0,
    eCARSFX_TRAFFIC_WOOSH = 1,
    eCARSFX_TRAFFIC_HORN = 2,
    eCARSFX_TRAFFIC_SKIDS = 3,
};

typedef eSFXOBJ_TRAFFIC_TYPES reflection_typedef_eSFXOBJ_TRAFFIC_TYPES;

enum eSFXOBJ_DRIVEBY {
    eSFXOBJ_WOOSH = 0,
};

typedef eSFXOBJ_DRIVEBY reflection_typedef_eSFXOBJ_DRIVEBY;

enum eSFXOBJ_HELI {
    SFXOBJ_HELICOPTER = 0,
};

typedef eSFXOBJ_HELI reflection_typedef_eSFXOBJ_HELI;

enum eVOL_SPEECH {
    eAZI_CLOSESTCOP_AZI = 0,
    eVOL_FE_SPEECH = 1,
    eVOL_CELL_CALL = 2,
    eVOL_MAIN_COP_1 = 3,
    eVOL_MAIN_COP_2 = 4,
    eVOL_MAIN_COP_3 = 5,
    eVOL_REG_COP_1 = 6,
    eVOL_REG_COP_2 = 7,
    eVOL_REG_COP_3 = 8,
    eVOL_REG_COP_4 = 9,
    eVOL_HELI_COP = 10,
    eVOL_DISPATCH = 11,
    eFLT_SPEECHFILTER_FLT = 12,
    eVOL_CMN_RADIO_VOL = 13,
    eVOL_MISC_SPEECH = 14,
};

typedef eVOL_SPEECH reflection_typedef_eVOL_SPEECH;

enum eSFXOUT_SPEECH {
    eTRG_SPEECH_ISPLAYING = 0,
    eSCL_SPEECH_INENSITY = 1,
    eTRG_SPEECH_PANNING = 2,
    eSCL_SPEECH_CLARITY = 3,
    eTRG_CELLCALL_PLAYING = 4,
    eSCL_BUSTEDMETER = 5,
};

typedef eSFXOUT_SPEECH reflection_typedef_eSFXOUT_SPEECH;

enum eVOL_PRE_COL_WOOSH {
    eAZI_PRE_COL_WOOSH_AZI = 0,
    eVOL_PRE_COL_WOOSH_WOOSH = 1,
    TRIG_PRE_COL_WOOSH_TRIG = 2,
    eVRB_PRE_COL_WOOSH_VERB = 3,
};

typedef eVOL_PRE_COL_WOOSH reflection_typedef_eVOL_PRE_COL_WOOSH;

enum eVOL_BOTTOMOUT {
    eAZI_BOTTOMOUT_AZI = 0,
    eVOL_BOTTOMOUT_JUMPLAND = 1,
    eVOL_BOTTOMOUT_BOTTOMOUT = 2,
    eVOL_BOTTOMOUT_JUMPCAMCRASH = 3,
    eVRB_BOTTOMOUT_VERB = 4,
};

typedef eVOL_BOTTOMOUT reflection_typedef_eVOL_BOTTOMOUT;

enum eVOL_CARDAMAGE {
    eAZI_CARDAMAGE_AZI = 0,
    eVOL_CARDAMAGE_TRUNK_BOUNCE = 1,
    eVOL_CARDAMAGE_WINDOW = 2,
    eVRB_CARDAMAGE_VERB = 3,
};

typedef eVOL_CARDAMAGE reflection_typedef_eVOL_CARDAMAGE;

enum eVOL_ENGINE {
    eAZI_ENGINE_AZI = 0,
    eVOL_ENGINE_AEMS = 1,
    eVOL_ENGINE_GINSU = 2,
    eVOL_ENGINE_TRANNY = 3,
    ePCH_ENGINE_PITCH = 4,
    eFLT_ENGINE_FILTER = 5,
    eVRB_ENGINE_AEMS_VERB = 6,
    eVRB_ENGINE_GINSU_VERB = 7,
};

typedef eVOL_ENGINE reflection_typedef_eVOL_ENGINE;

enum eVOL_NITROUS {
    eAZI_NITROUS_AZ = 0,
    eVOL_NITROUS_MAIN = 1,
    eVOL_NITROUS_PURGE = 2,
    ePCH_NITROUS_PCH = 3,
    eFLT_NITROUS_FLTR = 4,
    eVRB_NITROUS_VERB = 5,
};

typedef eVOL_NITROUS reflection_typedef_eVOL_NITROUS;

enum eVOL_RAIN {
    eVOL_RAIN_OUTSIDE = 0,
    eVOL_RAIN_INSIDE = 1,
    eTRG_RAINING_TRG = 2,
};

typedef eVOL_RAIN reflection_typedef_eVOL_RAIN;

enum eTRIG_ROADNOISE {
    eTRIG_ROADNOISE_TRANSITION = 0,
};

typedef eTRIG_ROADNOISE reflection_typedef_eTRIG_ROADNOISE;

enum eVOL_SHIFTING {
    eAZI_SHIFTING_AZI = 0,
    eVOL_SHIFTING_UP = 1,
    eVOL_SHIFTING_DOWN = 2,
    eVOL_SHIFTING_ENGAGE = 3,
    eVOL_SHIFTING_DISENGAGE = 4,
    eVOL_SHIFTING_ON = 5,
    eVOL_SHIFTING_OFF = 6,
    eVOL_SHIFTING_UP_TRIG = 7,
    eVOL_WHINE = 8,
    eVRB_SHIFTING_VERB = 9,
    eVOL_BRAKEPEDAL_HIT = 10,
};

typedef eVOL_SHIFTING reflection_typedef_eVOL_SHIFTING;

enum eVOL_SIREN {
    eAZI_SIREN_AZI = 0,
    eVOL_SIREN_MAIN = 1,
    eVOL_SIREN_BED = 2,
    ePCH_SIREN_PITCH = 3,
    eVRB_SIREN_VERB = 4,
    eTRG_SIREN_TRIG = 5,
};

typedef eVOL_SIREN reflection_typedef_eVOL_SIREN;

enum eVOL_SKIDS {
    eAZI_SKIDS_RIGHT_AZI = 0,
    eAZI_SKIDS_LEFT_AZI = 1,
    eVOL_SKIDS_RIGHT_FORWARD = 2,
    eVOL_SKIDS_RIGHT_BACK = 3,
    eVOL_SKIDS_RIGHT_SIDE = 4,
    eVOL_SKIDS_LEFT_FORWARD = 5,
    eVOL_SKIDS_LEFT_BACK = 6,
    eVOL_SKIDS_LEFT_SIDE = 7,
    ePCH_SKIDS_PITCH = 8,
    eRVB_SKIDS_REVERB = 9,
};

typedef eVOL_SKIDS reflection_typedef_eVOL_SKIDS;

enum eVOL_SPARKCHATTER {
    eAZI_SPARKCHATTER_AZI = 0,
    eVOL_SPARKCHATTER_MAIN = 1,
    eTRG_SPARKCHATTER_TRIG = 2,
    eVRB_SPARKCHATTER_VERB = 3,
};

typedef eVOL_SPARKCHATTER reflection_typedef_eVOL_SPARKCHATTER;

enum eVOL_STEREO {
    eAZI_STEREO_AZI = 0,
    eVOL_STEREO_MAIN = 1,
};

typedef eVOL_STEREO reflection_typedef_eVOL_STEREO;

enum eVOL_TRAFFIC_ENG {
    eAZI_TRAFFIC_AZI = 0,
    eVOL_TRAFFIC_ENG_MAIN = 1,
    eVOL_TRAFFIC_HORN = 2,
    eVOL_TRAFFIC_WOOSH = 3,
    ePCH_TRAFFIC_PITCH = 4,
    eTRG_TRAFFIC_WOOSH = 5,
    eTRG_TRAFFIC_WOOSH_BIG = 6,
};

typedef eVOL_TRAFFIC_ENG reflection_typedef_eVOL_TRAFFIC_ENG;

enum eVOL_TURBO {
    eAZI_TURBO_AZI = 0,
    eVOL_TURBO_SPOOLING = 1,
    eVOL_TURBO_BLOWOFF1 = 2,
    eVOL_TURBO_BLOWOFF2 = 3,
};

typedef eVOL_TURBO reflection_typedef_eVOL_TURBO;

enum eVOL_WINDNOISE {
    eAZI_WINDNOISE_LEFT_AZI = 0,
    eAZI_WINDNOISE_RIGHT_AZI = 1,
    eVOL_WINDNOISE_LEFT_VOL = 2,
    eVOL_WINDNOISE_RIGHT_VOL = 3,
    eVOL_WINDNOISE_RUMBLE = 4,
    ePCH_WINDNOISE_PITCH = 5,
};

typedef eVOL_WINDNOISE reflection_typedef_eVOL_WINDNOISE;

enum eVOL_AMBIENCE {
    eVOL_BACKGROUND = 0,
};

typedef eVOL_AMBIENCE reflection_typedef_eVOL_AMBIENCE;

enum eVOL_FEHUD {
    eAZI_FE_AZI = 0,
    eVOL_COMMON = 1,
    eVOL_FRONTEND = 2,
    eVOL_RADAR = 3,
    eTRG_NEW_ZONE = 4,
};

typedef eVOL_FEHUD reflection_typedef_eVOL_FEHUD;

enum eSFX_HELI {
    HELI_AZIMUTH = 0,
    HELI_VOLUME = 1,
    HELI_PITCH = 2,
};

typedef eSFX_HELI reflection_typedef_eSFX_HELI;

enum eVOL_NISSTREAMS {
    eAZI_NIS_AZI = 0,
    eVOL_NIS_INTRO = 1,
    eVOL_NIS_END = 2,
    eVOL_GAMEBREAKER_NOS = 3,
    eVOL_COLLISION_SWEETENER = 4,
    eVOL_STORM = 5,
    eTRG_NISPLAYTOGGLE = 6,
    eTRG_END_NIS = 7,
};

typedef eVOL_NISSTREAMS reflection_typedef_eVOL_NISSTREAMS;

enum eREVERB_TRIGGERS {
    REVERB_ENTER_TUNNEL = 0,
    REVERB_ENTER_NEWZONE = 1,
};

typedef eREVERB_TRIGGERS reflection_typedef_eREVERB_TRIGGERS;

enum eVOL_COMMONFX {
    eVOL_COMMON_CAMERASNAP = 0,
    eVOL_COMMON_CAMERACHARGE = 1,
    eVOL_COMMON_UVES = 2,
    eVOL_COMMON_PURSUIT_START = 3,
    eVOL_COMMON_PURSUIT_STOP = 4,
};

typedef eVOL_COMMONFX reflection_typedef_eVOL_COMMONFX;

enum eVOL_TRUCKSOUNDS {
    eAZI_TRUCK_AZI = 0,
    eVOL_TRUCK_SFX = 1,
    ePCH_TRUCK_PITCH = 2,
};

typedef eVOL_TRUCKSOUNDS reflection_typedef_eVOL_TRUCKSOUNDS;

enum eVOL_WORLDOBJ {
    eAZI_WORLDOBJ_AZI = 0,
    eVOL_WORLDOBJ_FOUNTAIN = 1,
    ePCH_WORLDOBJ_PITCH = 2,
    eCUT_WORLDOBJ_CUTOFF = 3,
};

typedef eVOL_WORLDOBJ reflection_typedef_eVOL_WORLDOBJ;

enum eDEPTH_REVERB {
    eDEPTH_PLAYER = 0,
};

typedef eDEPTH_REVERB reflection_typedef_eDEPTH_REVERB;

enum eINVERTED_MIX_CTRL {
    eINVERTED_MIX_CTRL_NONE = 0,
};

typedef eINVERTED_MIX_CTRL reflection_typedef_eINVERTED_MIX_CTRL;

enum eMISC_SOUNDS {
    eMISC_SOUNDS_RADAR = 0,
    eMISC_SOUNDS_UVES = 1,
    eMISC_SOUNDS_CAMERA = 2,
    eMISC_SOUNDS_PURSUIT_START = 3,
    eMISC_SOUNDS_PURSUIT_END = 4,
    eMISC_SOUNDS_MAX = 5,
};

typedef eMISC_SOUNDS reflection_typedef_eMISC_SOUNDS;

// typedef eGameFlowSndState reflection_typedef_eGameFlowSndState;

////////////

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

    // void SetCarSoundPlayback(bool _On) {} // Decl: 197
    // bool IsPlayingCarSounds() {}          // Decl: 198

    Sound::stSongInfo *GetNewSongInfo();

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

    void UpdateVolumes(AudioSettings *paudiosettings, float NewValue);
    void StartNewGamePlay();
    void InitializeFrontEnd();
    void InitializeInGame();
    void LoadInGameSoundBanks(void (*callback)(int), int32 callback_param);
    void LoadFrontEndSoundBanks(void (*callback)(int), int32 callback_param);
    void UnloadFrontEndSoundBanks();
    void UnLoadInGameSoundBanks();
    void EnterPauseMenu(eSNDPAUSE_REASON pause_reason);
    void ExitPauseMenu(eSNDPAUSE_REASON pause_reason);
    void CloseSound();

    NFSMixMaster *GetMixMaster() {
        return this->m_pNFSMixMaster;
    }

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

    // char *GetCsisName() {} // Decl: 276
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

    static CSTATEMGR_Base *GetStateMgr(eMAINMAPSTATES estate) {
        return m_pStateMgr[estate];
    }

    int ncompiletest; // offset 0x4, size 0x4, Decl: 144

    int m_nCopAIStateParam; // offset 0x8, size 0x4, Decl: 195

    bool bPlayCameraSnapShot;  // offset 0xC, size 0x1, Decl: 318
    bool bPlayCarSounds;       // offset 0x10, size 0x1, Decl: 319
    bool m_bIsSpecialUGMovie;  // offset 0x14, size 0x1, Decl: 320
    bool EngineLoadingBlocked; // offset 0x18, size 0x1, Decl: 321
    bool m_bIsPaused;          // offset 0x1C, size 0x1, Decl: 322
    bool m_bLostFocus;         // offset 0x20, size 0x1, Decl: 323
    float t_Paused;            // offset 0x24, size 0x4, Decl: 324
    float t_CurTime;           // offset 0x28, size 0x4, Decl: 325

    int FrameCnt; // offset 0x2C, size 0x4, Decl: 328

    int m_nDebugStreamState;            // offset 0x30, size 0x4, Decl: 331
    AudioSettings *m_pCurAudioSettings; // offset 0x34, size 0x4

#ifndef EA_BUILD_A124
    bool m_bPause_MainFNG; // offset 0x38, size 0x1, Decl: 333
#endif

  private:
    void *m_pMemoryPoolMem;     // offset 0x3C, size 0x4, Decl: 339
    int m_memoryPoolSize;       // offset 0x40, size 0x4, Decl: 340
    int m_numMemoryAllocations; // offset 0x44, size 0x4, Decl: 341

    eSNDPAUSE_REASON m_LastPauseReason; // offset 0x48, size 0x4, Decl: 345

    int m_transStartTime;     // offset 0x4C, size 0x4, Decl: 347
    int m_startingLoopVolume; // offset 0x50, size 0x4, Decl: 348

    bool m_bAudioIsPaused; // offset 0x54, size 0x1, Decl: 350
#ifndef EA_BUILD_A124
    bool m_X360_UI_Override; // offset 0x58, size 0x1
#endif

    char *m_pcsCsisName;                 // offset 0x5C, size 0x4, Decl: 359
    Sound::stSongInfo *m_pNewSongInfoSt; // offset 0x60, size 0x4
    eEAXGameState m_streamManagerState;  // offset 0x64, size 0x4, Decl: 361

    char *m_pEAX_SysHeap; // offset 0x68, size 0x4, Decl: 363

    EAXFrontEnd *m_pFESnd;       // offset 0x6C, size 0x4, Decl: 365
    EAXCommon *m_pCmnSnd;        // offset 0x70, size 0x4, Decl: 366
    NFSLiveLink *m_pNFSLiveLink; // offset 0x74, size 0x4, Decl: 367

    e3DPlayerMix m_ePlayerMixMode; // offset 0x78, size 0x4, Decl: 370

    int m_nStereoUpgradeLevel; // offset 0x7C, size 0x4, Decl: 373
    int m_nGameMode;           // offset 0x80, size 0x4, Decl: 374

    eSndGameMode m_eSndGameMode;    // offset 0x84, size 0x4, Decl: 376
    eSndGameMode m_prevSndGameMode; // offset 0x88, size 0x4, Decl: 377
    int m_nNumCarsInGame;           // offset 0x8C, size 0x4, Decl: 378

    EAXSND8Wrapper *m_pEAXSND8Wrapper;    // offset 0x90, size 0x4
    EAXS_StreamManager *m_pStreamManager; // offset 0x94, size 0x4, Decl: 386
    NFSMixMaster *m_pNFSMixMaster;        // offset 0x98, size 0x4

  public:
    static CSTATEMGR_Base *m_pStateMgr[eMM_MAX_MAIN_MIXSTATES];

  private:
    cSTICH_PlayBack *m_pSTICH_Playback; // offset 0x9C, size 0x4, Decl: 404

    eAemsStreamBanks m_eSpeechLoadBank; // offset 0xA0, size 0x4, Decl: 407
    int m_nSpeechLoadBankIndex;         // offset 0xA4, size 0x4, Decl: 408

    Attrib::Gen::audiosystem *mAttributes; // offset 0xA8, size 0x4, Decl: 410
    Attrib::Gen::audiosystem *mLocalAttr;  // offset 0xAC, size 0x4, Decl: 411

    Hermes::HHANDLER mmsgMRestartRace; // offset 0xB0, size 0x4, Decl: 413

    int mEventID;            // offset 0xB4, size 0x4
    Event::StaticData mData; // offset 0xB8, size 0x4
};

// total size: 0x18
// Decl: 422
struct SND_Params {
    // TODO it's sus that only these are initialized
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

#define SNDPRINTF_BUFFER_SIZE 512 // Decl: 467
#define SNDPRINTF_CHANNEL 9       // Decl: 468
#define NO_SNDPRINTF              // Decl: 471
#define SndPrintf if (0)          // Decl: 478

void InitializeSoundDriver();

bool g_EAXIsPaused(void);

void SetSoundControlState(bool bON, eSNDCTLSTATE esndstate, const char *Reason);

void SoundPause(bool bpause, eSNDPAUSE_REASON esndpause);

void FESoundControl(bool bOn, const char *name);

extern EAXSound *g_pEAXSound;
extern bool gbAudioInterruptsWorldDataRead;
extern bool gbWorldDataBlocksAudioRead;

// TODO move these to Ecstasy
extern int32 eDisableFixUpTables;
extern int32 eDirtySolids;
extern int32 eDirtyTextures;
extern int32 eDirtyAnimations;

#endif
