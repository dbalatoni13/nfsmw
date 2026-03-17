#include "./EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/EAXAemsManager.h"
#include "Speed/Indep/Src/EAXSound/EAXCarState.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_NISStream.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_Pathfinder.hpp"
#include "Speed/Indep/Src/EAXSound/Stream/EAXS_StreamManager.h"
#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_Base.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_NISReving.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Gameplay/GRace.h"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Generated/Messages/MControlPathfinder.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/engine.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/engineaudio.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pvehicle.h"
#include "Speed/Indep/Src/Main/Scheduler.h"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/Misc/QueuedFile.hpp"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/World/WorldConn.h"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bDebug.hpp"
#include <new>

enum SpeechModuleIndex {
    NISSFX_MODULE = 0,
    COPSPEECH_MODULE = 1,
    NUM_SPEECH_MODULES = 2,
};

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

namespace Speech {
struct Cache {
    void Dump();
    void Init(int size);
    void Validate();
};
struct Module {
    virtual void Init(int channel);
    virtual void LoadBanks();
    virtual int TestSentenceRuleCallback(int eventID, int ruleID, int parmValue);
    virtual int SetSentenceRuleCallback(int eventID, int ruleID, int parmValue);
    virtual int EventRuleCallback(int eventID);
    virtual int GetNumBanks();
    virtual unsigned int GetBankOffset(int bnum);
    virtual void Update();
    virtual const char *GetFilename();
    virtual bool QueStream(int stream_type, void (*callback)(), bool trigger_play_after_callback);
    virtual unsigned int SampleRequestCallback(void *data);
    virtual bool IsStreamQueued();
    virtual char *GetCSIptr();
    virtual int GetChannel();
    virtual char *GetEventDat();
    virtual bool IsDataLoaded();
    virtual bool PlayStream(int stream_id);
    virtual void UnknownVirtual();
    virtual void ReleaseResource();
    void PurgeSpeech();
};
struct Manager {
    static void ClearPlayback();
    static void Init(int mode);
    static void Destroy();
    static void Deduce();
    static void Update(float dt);
    static Module *GetSpeechModule(int id);
    static void AttachSFXOBJ(SpeechModuleIndex module, SFX_Base *psb, eSFXOBJ_MAIN_TYPES type);
};
} // namespace Speech

extern Speech::Cache gSpeechCache;

#include "Speed/Indep/Src/EAXSound/EAXCommon.hpp"
#include "Speed/Indep/Src/EAXSound/EAXFrontEnd.hpp"

struct EAXSND8Wrapper : public AudioMemBase {
    char *m_pSoundHeap;   // offset 0x04
    char *m_pStreamBuff;  // offset 0x08
    int m_nHeapSize;      // offset 0x0C
    int m_nStreamSize;    // offset 0x10
    eSndAudioMode m_eCurrentAudioMode; // offset 0x14
    eSndAudioMode m_eLastAudioMode;    // offset 0x18

    EAXSND8Wrapper();
    virtual ~EAXSND8Wrapper();
    bool Initialize();
    eSndAudioMode GetDefaultPlatformAudioMode();
    eSndAudioMode SetAudioModeFromMemoryCard(eSndAudioMode mode);
    eSndAudioMode SetAudioRenderMode(eSndAudioMode mode);
    void ReInit();
    eSndAudioMode SetSnd8RenderMode(eSndAudioMode mode);
    void Update();
    void STUPID();
};

struct CarSoundConn : public Sim::Connection, public UTL::Collections::Listable<CarSoundConn, 10> {
    bool mConnected; // offset 0x14, size 0x1
    EAX_CarState *mState; // offset 0x18, size 0x4
    WorldConn::Reference mTarget; // offset 0x1C, size 0x10

    CarSoundConn(const Sim::ConnectionData &data);
    ~CarSoundConn() override;
    void OnReceive(Sim::Packet *) override;
    void OnClose() override;
    Sim::ConnStatus OnStatusCheck() override;
    void UpdateState(float dT);
    static Sim::Connection *Construct(const Sim::ConnectionData &data);
    static void SetAssetsLoaded(CarSoundConn *conn);
};

struct EAX_HeliState : public UTL::Collections::Listable<EAX_HeliState, 10> {
    bMatrix4 mMatrix; // offset 0x4
    bVector3 mVel0; // offset 0x44
    bVector3 mVel1; // offset 0x54
    bVector3 mAccel; // offset 0x64
    float mFWSpeed; // offset 0x74
    int mMovementMode; // offset 0x78
    int mPlayerZone; // offset 0x7C
    Attrib::Instance mAttributes; // offset 0x80
    Sound::Context mContext; // offset 0x94
    bool mSimUpdating; // offset 0x98
    char _pad_sim[3];
    const unsigned int mWorldID; // offset 0x9C

    EAX_HeliState(const Attrib::Collection *atr, unsigned int wuid);
};

struct HeliSoundConn : public Sim::Connection, public UTL::Collections::Listable<HeliSoundConn, 10> {
    EAX_HeliState *mState; // offset 0x18, size 0x4
    WorldConn::Reference mTarget; // offset 0x18, size 0x10

    HeliSoundConn(const Sim::ConnectionData &data);
    ~HeliSoundConn() override;
    void OnReceive(Sim::Packet *) override;
    void OnClose() override;
    Sim::ConnStatus OnStatusCheck() override;
    void UpdateState(float dT);
    static Sim::Connection *Construct(const Sim::ConnectionData &data);
};

struct CSTATEMGR_CarState : public CSTATEMGR_Base {
    static void ResolveCarBanks();
    static void ResetCarBanks();
    static void DestroyCar(EAX_CarState *pCar);
};

struct CSTATEMGR_Main : public CSTATEMGR_Base {
    CSTATEMGR_Main();
};

struct CSTATEMGR_Music : public CSTATEMGR_Base {
    CSTATEMGR_Music();
};

struct CSTATEMGR_PlayerCar : public CSTATEMGR_Base {
    CSTATEMGR_PlayerCar();
};

struct CSTATEMGR_AICar : public CSTATEMGR_Base {
    CSTATEMGR_AICar();
    static void QueueSlots();
};

struct CSTATEMGR_CopCar : public CSTATEMGR_Base {
    CSTATEMGR_CopCar();
};

struct CSTATEMGR_TrafficCar : public CSTATEMGR_Base {
    CSTATEMGR_TrafficCar();
};

struct CSTATEMGR_Enviro : public CSTATEMGR_Base {
    CSTATEMGR_Enviro();
};

struct CSTATEMGR_Collision : public CSTATEMGR_Base {
    CSTATEMGR_Collision();
};

struct CSTATEMGR_DriveBy : public CSTATEMGR_Base {
    CSTATEMGR_DriveBy();
};

struct CSTATEMGR_Helicopter : public CSTATEMGR_Base {
    CSTATEMGR_Helicopter();
};

struct CSTATEMGR_Truck : public CSTATEMGR_Base {
    CSTATEMGR_Truck();
};

enum eRaceMixType {
    eRACE_CIRCUIT = 0,
    eRACE_DRAG = 1,
    eRACE_TWODRG = 2,
    eRACE_TWOCIRC = 3,
};

struct NFSMixMaster {
    bool m_bMapReady;
    void CreateMainMainMap(eRaceMixType type);
    void DestroyMainMainMap();
    void InitMixMap(int mode);
    void ProcessMixMap(float dt, int cam_state);
    void AssignSFXCallbacks(int *(*GetPointerCB)(int), void (*SetSFXOutCB)(int, int *), bool (*SetSFXInputCB)(int, int *),
                            int (*GetStateRefCountCB)(int), void (*MixReadyCB)());
};

template <>
UTL::Collections::Listable<CarSoundConn, 10>::List UTL::Collections::Listable<CarSoundConn, 10>::_mTable =
    UTL::Collections::Listable<CarSoundConn, 10>::List();
template <>
UTL::Collections::Listable<HeliSoundConn, 10>::List UTL::Collections::Listable<HeliSoundConn, 10>::_mTable =
    UTL::Collections::Listable<HeliSoundConn, 10>::List();
template <>
UTL::Collections::Listable<EAX_CarState, 10>::List UTL::Collections::Listable<EAX_CarState, 10>::_mTable =
    UTL::Collections::Listable<EAX_CarState, 10>::List();
template <>
UTL::Collections::Listable<EAX_HeliState, 10>::List UTL::Collections::Listable<EAX_HeliState, 10>::_mTable =
    UTL::Collections::Listable<EAX_HeliState, 10>::List();

extern unsigned int SoundRandomSeed;
extern float g_SliderValue;
extern float g_fMasterSFXVolume;
extern int g_iMasterSFXVolume;
extern int g_DMIX_DummyInputBlock[];
extern char *g_pcsCSISAllocString;
extern char csCSISdebug[];
extern bool bIsMapInQueuedFileLoad;
extern unsigned int g_ActiveCtlStates;
extern unsigned int g_laststartanimid;
extern bool g_bWasLastNISaStart;
extern bool IsAudioStreamingEnabled;
extern bool IsSpeechEnabled;
extern int _RUN_SOUND_STATE;
extern unsigned int g_ActiveSFXStates;
extern unsigned int g_PrevActiveCtlStates;
extern unsigned int g_PrevActiveSFXStates;
extern unsigned int g_CtlStateActions[18];
extern const char *csfxedit[];
extern bool bHasStartNewGameOccured;
extern bool gbHasStartNewGamePlayBeenProcessed;
extern bool bIsAnFEToIngameTransition;
extern bool bHasDataLoadOccured;
extern int gIsPauseForPause;
extern int gnHasStartLoadFEBeenProcessed;
extern int gb_DORESTART_RACE;
extern int gb_Is321;
extern int SkipFE;
extern int SkipFELanguage;
extern int DisableSoundUpdate;
extern bool gbAudioInterruptsWorldDataRead;
extern bool gbWorldDataBlocksAudioRead;
extern bool bReadCallbackToggle;
extern bool bStreamBlockState;
extern bool bAudioInterrupt;
extern bool bStreamReadTiming;
extern unsigned int uStreamBlockTicks;
extern unsigned int uStreamReadTicks;
extern unsigned int uAudioInterruptTicks;
extern int GameFlowSndState[];
extern void SNDSYS_service();
extern int GetCurrentLanguage();
void RegisterStates();
void RegisterSFX();

unsigned int bRandom(int range, unsigned int *seed);
float bRandom(float range, unsigned int *seed);
int bSPrintf(char *destString, const char *fmt, ...);
int bStrICmp(const char *s1, const char *s2);
void SoundPause(bool on, eSNDPAUSE_REASON reason);
void SetSoundControlState(bool on, eSNDCTLSTATE state, const char *caller);

namespace SndCamera {
void UpdateCameras();
extern int m_CurCamState[2];
}

bool g_EAXIsPaused() {
    return (g_ActiveCtlStates & 0x3483b) != 0;
}

void g_LoadSndAsset(Attrib::StringKey filename, eSNDDATAPATH path, eSNDDATATYPE type) {
    stAssetDescription asset;
    asset.Clear();
    asset.eDataType = type;
    asset.FileName = filename;
    asset.DataPath = path;
    asset.bLoadToTop = false;
    gAEMSMgr.AddBankListing(asset);
}

void EAXSound::START_321Countdown() {
    int id;
    SFXObj_Pathfinder *ppf;
    SFXObj_NISStream *pnis;

    if (IsSoundEnabled && IsAudioStreamingEnabled) {
        id = 0x40010010;
        ppf = static_cast<SFXObj_Pathfinder *>(GetSFXBase_Object(id));
        if (ppf) {
            ppf->Set321(true);
        }
        id = 0x40000050;
        pnis = static_cast<SFXObj_NISStream *>(GetSFXBase_Object(id));
        if (pnis) {
            pnis->StartNIS();
        }
    }
}

bool EAXSound::AreResourceLoadsPending() {
    return gAEMSMgr.AreResourceLoadsPending();
}

void EAXSound::QueueNISButtonThrough(unsigned int anim_id, int camera_track_number) {
    int id;
    SFXObj_NISStream *pnis;
    Speech::Module *cop_speech;

    if (!IsSoundEnabled || !IsAudioStreamingEnabled) {
        return;
    }

    id = 0x40000050;
    pnis = static_cast<SFXObj_NISStream *>(GetSFXBase_Object(id));
    if (camera_track_number == -1) {
        if (g_laststartanimid != 0x0D0E5A9D && g_laststartanimid != 0xCBFF6594) {
            pnis->QueueNISStream(g_laststartanimid, -1, true, false);
        }
    } else if (g_bWasLastNISaStart) {
        pnis->QueueNISStream(anim_id, camera_track_number, true, false);
    } else {
        pnis->StopStream();
    }

    Speech::Manager::ClearPlayback();
    id = COPSPEECH_MODULE;
    cop_speech = Speech::Manager::GetSpeechModule(id);
    if (cop_speech != nullptr) {
        cop_speech->ReleaseResource();
    }
}

void EAXSound::QueueNISStream(unsigned int anim_id, int camera_track_number, void (*setmstimecb)(unsigned int, int)) {
    int id;
    SFXObj_NISStream *pnis;

    if (IsSoundEnabled && IsAudioStreamingEnabled) {
        id = 0x40000050;
        pnis = static_cast<SFXObj_NISStream *>(GetSFXBase_Object(id));
        pnis->QueueNISStream(anim_id, camera_track_number, setmstimecb, false);
    }
}

bool EAXSound::IsNISStreamQueued() {
    int id;
    SFXObj_NISStream *pnis;

    if (!IsSoundEnabled || !IsAudioStreamingEnabled) {
        return true;
    }
    id = 0x40000050;
    pnis = static_cast<SFXObj_NISStream *>(GetSFXBase_Object(id));
    return pnis->IsNISStreamReady();
}

// NISFinished goes here when implemented

void EAXSound::NISFinished() {
    int id;
    SFXObj_NISStream *pnis;

    if (IsSoundEnabled && IsAudioStreamingEnabled) {
        id = 0x40000050;
        pnis = static_cast<SFXObj_NISStream *>(GetSFXBase_Object(id));
        pnis->NISActivityDone();
        SoundPause(false, eSNDPAUSE_NISON);
        SetSoundControlState(false, SNDSTATE_NIS_STORY, "EAXSound::NISFinished");
        SetSoundControlState(false, SNDSTATE_NIS_INTRO, "EAXSound::NISFinished");
        SetSoundControlState(false, SNDSTATE_NIS_BLK, "EAXSound::NISFinished");
        SetSoundControlState(false, SNDSTATE_NIS_ARREST, "EAXSound::NISFinished");
    }
}

void EAXSound::PlayNIS() {
    int id;
    SFXObj_NISStream *pnis;

    if (IsSoundEnabled && IsAudioStreamingEnabled) {
        id = 0x40000050;
        pnis = static_cast<SFXObj_NISStream *>(GetSFXBase_Object(id));
        pnis->StartNIS();
        SoundPause(true, eSNDPAUSE_NISON);
    }
}

// PlayUISoundFX, StopUISoundFX go here when implemented

void EAXSound::PlayUISoundFX(eMenuSoundTriggers etriggertype) {
    if (IsSoundEnabled) {
        if (etriggertype < UISND_COMMON_MAX_NUM) {
            if (m_pCmnSnd != nullptr) {
                m_pCmnSnd->Play(etriggertype);
            }
        } else if (etriggertype < UISND_FRONTEND_MAX_NUM) {
            etriggertype = static_cast<eMenuSoundTriggers>(etriggertype - UISND_COMMON_MAX_NUM);
            if (m_pFESnd != nullptr) {
                m_pFESnd->Play(etriggertype);
            }
        }
    }
}

void EAXSound::StopUISoundFX(eMenuSoundTriggers etriggertype) {
    if (IsSoundEnabled) {
        if (etriggertype < UISND_COMMON_MAX_NUM) {
            if (m_pCmnSnd != nullptr) {
                m_pCmnSnd->Stop(etriggertype);
            }
        } else {
            etriggertype = static_cast<eMenuSoundTriggers>(etriggertype - UISND_COMMON_MAX_NUM);
            if (m_pFESnd != nullptr) {
                m_pFESnd->Stop(etriggertype);
            }
        }
    }
}

void EAXSound::SetCsisName(SndBase *psndbase) {
    int ninst = psndbase->GetUniqueID();
    bSPrintf(csCSISdebug, " %s, 0x%x ", psndbase->GetTypeName(), ninst);
    SetCsisName(csCSISdebug);
}

void EAXSound::SetCsisName(char *pcsAllocName) {
    g_pcsCSISAllocString = pcsAllocName;
}

EAXSound::EAXSound() {
    int nloop;

    m_pcsCsisName = "SOUND";
    bPlayCarSounds = true;
    mAttributes = nullptr;
    mLocalAttr = nullptr;
    m_pEAXSND8Wrapper = nullptr;
    m_bAudioIsPaused = false;
    m_X360_UI_Override = false;
    m_eSndGameMode = SND_MODE_NONE;
    m_prevSndGameMode = SND_MODE_NONE;
    m_pStreamManager = nullptr;
    m_nStereoUpgradeLevel = 0;
    m_bIsPaused = false;
    m_pCurAudioSettings = nullptr;
    m_nSpeechLoadBankIndex = 0;
    m_ePlayerMixMode = EAXS3D_SINGLE_PLAYER_MIX;
    m_pNewSongInfoSt = nullptr;
    EngineLoadingBlocked = false;
    FrameCnt = 0;
    m_bIsSpecialUGMovie = false;
    m_pCmnSnd = nullptr;
    m_pFESnd = nullptr;
    m_pSTICH_Playback = nullptr;
    m_pNFSLiveLink = nullptr;
    m_pNFSMixMaster = nullptr;
    bPlayCameraSnapShot = false;
    mmsgMRestartRace = nullptr;
    m_bPause_MainFNG = false;
    mEventID = 0;
    mData.fEventID = 0xF2D10992; // TODO magic
    nloop = 0;
}

EAXSound::~EAXSound() {
    gSpeechCache.Dump();

    if (mAttributes != nullptr) {
        delete mAttributes;
        mAttributes = nullptr;
    }

    if (mLocalAttr != nullptr) {
        delete mLocalAttr;
        mLocalAttr = nullptr;
    }

    if (m_pCmnSnd != nullptr) {
        delete m_pCmnSnd;
        m_pCmnSnd = nullptr;
    }

    if (m_pFESnd != nullptr) {
        delete m_pFESnd;
        m_pFESnd = nullptr;
    }

    if (m_pEAXSND8Wrapper != nullptr) {
        delete m_pEAXSND8Wrapper;
        m_pEAXSND8Wrapper = nullptr;
    }

    if (g_pNISRevMgr != nullptr) {
        delete g_pNISRevMgr;
    }
}

int *EAXSound::GetPointerCallback(int nid) {
    SndBase *pbs = GetSndBase_Object(nid);
    if (pbs != nullptr) {
        return pbs->GetOutputBlockPtr() != nullptr ? pbs->GetOutputBlockPtr() : nullptr;
    }
    return g_DMIX_DummyInputBlock;
}

void EAXSound::SetSFXOutCallback(int nid, int *ptr) {
    SndBase *pbs = GetSndBase_Object(nid);
    if (pbs != nullptr) {
        pbs->SetOutputsPtr(ptr);
    }
    int *pclear = ptr;
    for (int n = 0; n < 15; n++) {
        *pclear = 0;
        pclear++;
    }
}

bool EAXSound::SetSFXInputCallback(int nid, int *ptr) {
    SndBase *pbs = GetSndBase_Object(nid);
    if (pbs != nullptr) {
        pbs->SetInputsPtr(ptr);
        if (pbs->GetStateBase() != nullptr) {
            return pbs->GetStateBase()->IsAttached();
        }
    }
    return false;
}

int EAXSound::GetStateRefCount(int nstate) {
    if (m_pStateMgr[nstate] == nullptr) {
        return 0;
    }
    return m_pStateMgr[nstate]->GetStateObjCount();
}

// GetSFXBase_Object, GetSndBase_Object go here when implemented

void EAXSound::SetSFXBaseObject(SFX_Base *psb, eMAINMAPSTATES estate, int ntype, int instance) {
    switch (estate) {
    case eMM_MAIN:
        switch (ntype) {
        case SFXOBJ_SPEECH:
            Speech::Manager::AttachSFXOBJ(COPSPEECH_MODULE, psb, SFXOBJ_SPEECH);
            return;
        case SFXOBJ_NISPROJ_STRMS:
        case SFXOBJ_MOMENT_STRMS:
            Speech::Manager::AttachSFXOBJ(NISSFX_MODULE, psb, static_cast<eSFXOBJ_MAIN_TYPES>(ntype));
            return;
        case SFXOBJ_FEHUD:
            if (m_pFESnd != nullptr) {
                m_pFESnd->AttachSFXOBJ(psb, SFXOBJ_FEHUD);
            }
            if (m_pCmnSnd != nullptr) {
                m_pCmnSnd->AttachSFXOBJ(psb, SFXOBJ_FEHUD);
            }
            return;
        }
        break;
    case eMM_COLLISION:
        Speech::Manager::AttachSFXOBJ(NISSFX_MODULE, psb, static_cast<eSFXOBJ_MAIN_TYPES>(ntype));
        return;
    }
}

SFX_Base *EAXSound::GetSFXBase_Object(int nID) {
    SFX_Base *ReturnObj = nullptr;
    int nState = (static_cast<unsigned int>(nID) >> 16) & 0xFF;
    if (m_pStateMgr[nState] != nullptr) {
        int nInstanceID = (static_cast<unsigned int>(nID) >> 11) & 0x1F;
        int SFXID_Number = (static_cast<unsigned int>(nID) >> 4) & 0x7F;
        if (m_pStateMgr[nState]->GetStateObj(nInstanceID) == nullptr) {
            return nullptr;
        }
        if ((nID & 0xE0000000) == 0x40000000) {
            ReturnObj = static_cast<SFX_Base *>(m_pStateMgr[nState]->GetStateObj(nInstanceID)->GetSFXObject(SFXID_Number));
        }
    }
    return ReturnObj;
}

SndBase *EAXSound::GetSndBase_Object(int nID) {
    SndBase *ReturnObj = nullptr;
    int nState = (static_cast<unsigned int>(nID) >> 16) & 0xFF;
    if (m_pStateMgr[nState] != nullptr) {
        int nInstanceID = (static_cast<unsigned int>(nID) >> 11) & 0x1F;
        int SFXID_Number = (static_cast<unsigned int>(nID) >> 4) & 0x7F;
        if (m_pStateMgr[nState]->GetStateObj(nInstanceID) == nullptr) {
            return nullptr;
        }
        if ((nID & 0xE0000000) == 0x40000000) {
            ReturnObj = m_pStateMgr[nState]->GetStateObj(nInstanceID)->GetSFXObject(SFXID_Number);
        }
        if ((nID & 0xE0000000) == 0x60000000) {
            ReturnObj = m_pStateMgr[nState]->GetStateObj(nInstanceID)->GetSFXCTLObject(SFXID_Number);
        }
    }
    if (ReturnObj == nullptr) {
        return nullptr;
    }
    return ReturnObj;
}

float EAXSound::GetCurMusicVolume() {
    if (m_eSndGameMode == SND_FRONTEND) {
        return m_pCurAudioSettings->GetMasteredFEMusicVol();
    }
    return m_pCurAudioSettings->GetMasteredIGMusicVol();
}

void EAXSound::ReInitMasterVolumes() {
    int i;
}

void EAXSound::UpdateVolumes(AudioSettings *paudiosettings, float NewValue) {
    m_pCurAudioSettings = paudiosettings;
    ReInitMasterVolumes();
    g_fMasterSFXVolume = paudiosettings->AmbientVol;
    g_iMasterSFXVolume = static_cast<int>(paudiosettings->AmbientVol * 32767.0f);
    g_SliderValue = NewValue;
}

unsigned int EAXSound::Random(int range) {
    return bRandom(range, &SoundRandomSeed);
}

float EAXSound::Random(float range) {
    return bRandom(range, &SoundRandomSeed);
}

void EAXSound::UpdateSongInfo() {}

void EAXSound::InitializeDriver() {
    if (!IsSoundEnabled) {
        return;
    }

    m_pEAXSND8Wrapper = new (gAudioMemoryManager.AllocateMemory(0x1C, "EAXSND8Wrapper", false)) EAXSND8Wrapper();
    if (m_pEAXSND8Wrapper == nullptr) {
        IsSoundEnabled = false;
        return;
    }

    if (!m_pEAXSND8Wrapper->Initialize()) {
        IsSoundEnabled = false;
        return;
    }

    gAEMSMgr.InitSPUram();
    m_pNFSMixMaster = new (gAudioMemoryManager.AllocateMemory(0x74, "NFSMixMaster", false)) NFSMixMaster();
    m_pSTICH_Playback = new (gAudioMemoryManager.AllocateMemory(0x1C, "STICH_PlayBack", false)) cSTICH_PlayBack();
    g_pNISRevMgr = new (gAudioMemoryManager.AllocateMemory(0x98, "NISRevMan", false)) NIS_RevManager();

    for (int n = 0; n < 13; n++) {
        m_pStateMgr[n] = nullptr;
    }

    m_pStateMgr[0] = new (gAudioMemoryManager.AllocateMemory(0x1C, "SND: CSTATEMGR_MAIN", false)) CSTATEMGR_Main();
    m_pStateMgr[0]->Initialize(static_cast<eMAINMAPSTATES>(0));

    m_pStateMgr[1] = new (gAudioMemoryManager.AllocateMemory(0x1C, "SND: CSTATEMGR_Music", false)) CSTATEMGR_Music();
    m_pStateMgr[1]->Initialize(static_cast<eMAINMAPSTATES>(1));

    m_pStateMgr[2] = new (gAudioMemoryManager.AllocateMemory(0x1C, "SND: CSTATEMGR_PlyrCar", false)) CSTATEMGR_PlayerCar();
    m_pStateMgr[2]->Initialize(static_cast<eMAINMAPSTATES>(2));

    m_pStateMgr[3] = new (gAudioMemoryManager.AllocateMemory(0x24, "SND: CSTATEMGR_AICar", false)) CSTATEMGR_AICar();
    m_pStateMgr[3]->Initialize(static_cast<eMAINMAPSTATES>(3));

    m_pStateMgr[4] = new (gAudioMemoryManager.AllocateMemory(0x28, "SND: CSTATEMGR_CarState", false)) CSTATEMGR_CopCar();
    m_pStateMgr[4]->Initialize(static_cast<eMAINMAPSTATES>(4));

    m_pStateMgr[5] = new (gAudioMemoryManager.AllocateMemory(0x24, "SND: CSTATEMGR_TrafficCar", false)) CSTATEMGR_TrafficCar();
    m_pStateMgr[5]->Initialize(static_cast<eMAINMAPSTATES>(5));

    m_pStateMgr[6] = new (gAudioMemoryManager.AllocateMemory(0x24, "SND: CSTATEMGR_ENVIRO", false)) CSTATEMGR_Enviro();
    m_pStateMgr[6]->Initialize(static_cast<eMAINMAPSTATES>(6));

    m_pStateMgr[7] = new (gAudioMemoryManager.AllocateMemory(0x1C, "SND: CSTATEMGR_Collision", false)) CSTATEMGR_Collision();
    m_pStateMgr[7]->Initialize(static_cast<eMAINMAPSTATES>(7));

    m_pStateMgr[8] = new (gAudioMemoryManager.AllocateMemory(0x20, "SND: CSTATEMGR_DriveBy", false)) CSTATEMGR_DriveBy();
    m_pStateMgr[8]->Initialize(static_cast<eMAINMAPSTATES>(8));

    m_pStateMgr[11] =
        new (gAudioMemoryManager.AllocateMemory(0x1C, "SND: CSTATEMGR_Helicopter", false)) CSTATEMGR_Helicopter();
    m_pStateMgr[11]->Initialize(static_cast<eMAINMAPSTATES>(11));

    m_pStateMgr[12] = new (gAudioMemoryManager.AllocateMemory(0x24, "SND: CSTATEMGR_Truck", false)) CSTATEMGR_Truck();
    m_pStateMgr[12]->Initialize(static_cast<eMAINMAPSTATES>(12));

    m_pNFSMixMaster->AssignSFXCallbacks(EAXSound::GetPointerCallback, EAXSound::SetSFXOutCallback,
                                        EAXSound::SetSFXInputCallback, EAXSound::GetStateRefCount,
                                        EAXSound::MixMapReadyCallback);
}

void EAXSound::RefreshLocalAttr() {
    if (mLocalAttr != nullptr) {
        delete mLocalAttr;
        mLocalAttr = nullptr;
    }

    switch ((SkipFE == 0) ? GetCurrentLanguage() : SkipFELanguage) {
    case 1:
        mLocalAttr = new Attrib::Gen::audiosystem(mAttributes->Locales(1).GetCollectionWithDefault(), 0, nullptr);
        break;
    case 2:
        mLocalAttr = new Attrib::Gen::audiosystem(mAttributes->Locales(2).GetCollectionWithDefault(), 0, nullptr);
        break;
    case 3:
        mLocalAttr = new Attrib::Gen::audiosystem(mAttributes->Locales(3).GetCollectionWithDefault(), 0, nullptr);
        break;
    case 4:
        mLocalAttr = new Attrib::Gen::audiosystem(mAttributes->Locales(4).GetCollectionWithDefault(), 0, nullptr);
        break;
    case 10:
        mLocalAttr = new Attrib::Gen::audiosystem(mAttributes->Locales(5).GetCollectionWithDefault(), 0, nullptr);
        break;
    case 0:
    default:
        mLocalAttr = new Attrib::Gen::audiosystem(mAttributes->Locales(0).GetCollectionWithDefault(), 0, nullptr);
        break;
    }
}

void EAXSound::StartNewGamePlay() {
    if (!IsSoundEnabled) {
        return;
    }

    SetSoundControlState(true, SNDSTATE_STOP_MUSIC, "RestartRace");

    if (bHasStartNewGameOccured) {
        if (m_pCmnSnd != nullptr) {
            delete m_pCmnSnd;
            m_pCmnSnd = nullptr;
        }

        if (m_pStreamManager != nullptr) {
            for (int i = 0; i < 4; i++) {
                EAXS_StreamChannel *channel = m_pStreamManager->GetStreamChannel(i);
                if (channel != nullptr && i != 1) {
                    channel->Stop();
                    channel->PurgeStream();
                }
            }
        }

        for (int i = 0; i < 13; i++) {
            if (i == 1) {
                if (m_pStateMgr[i] != nullptr) {
                    m_pStateMgr[i]->DisconnectMixMap();
                }
            } else if (m_pStateMgr[i] != nullptr) {
                m_pStateMgr[i]->ExitWorld();
            }
        }

        if (m_pNFSMixMaster != nullptr && m_pNFSMixMaster->m_bMapReady) {
            m_pNFSMixMaster->DestroyMainMainMap();
        }

        gAEMSMgr.DestroySlots(false);
    }

    if (m_ePlayerMixMode != EAXS3D_TWO_PLAYER_MIX) {
        EAXAemsManager::m_RequiredSlots[3]++;
    }

    CSTATEMGR_AICar::QueueSlots();
    gAEMSMgr.InitializeSlots(!bHasStartNewGameOccured);
    bHasStartNewGameOccured = true;

    if (!bIsMapInQueuedFileLoad) {
        bIsMapInQueuedFileLoad = true;

        GRaceParameters *race = nullptr;
        if (GRaceStatus::Exists()) {
            race = GRaceStatus::Get().GetRaceParameters();
        } else if (GRaceDatabase::Exists()) {
            GRaceCustom *startupRace = GRaceDatabase::Get().GetStartupRace();
            race = startupRace;
        }

        if (race == nullptr) {
            m_prevSndGameMode = m_eSndGameMode;
            m_eSndGameMode = SND_FREEROAM;
            if (m_pNFSMixMaster != nullptr) {
                if (Sim::GetUserMode() == Sim::USER_SPLIT_SCREEN) {
                    m_pNFSMixMaster->CreateMainMainMap(eRACE_TWOCIRC);
                } else {
                    m_pNFSMixMaster->CreateMainMainMap(eRACE_CIRCUIT);
                }
            }
        } else {
            GRace::Type raceType = race->GetRaceType();
            if (raceType == GRace::kRaceType_Drag) {
                m_prevSndGameMode = m_eSndGameMode;
                m_eSndGameMode = SND_DRAGRACE;
                if (m_pNFSMixMaster != nullptr) {
                    if (Sim::GetUserMode() == Sim::USER_SPLIT_SCREEN) {
                        m_pNFSMixMaster->CreateMainMainMap(eRACE_TWODRG);
                    } else {
                        m_pNFSMixMaster->CreateMainMainMap(eRACE_DRAG);
                    }
                }
            } else {
                m_prevSndGameMode = m_eSndGameMode;
                m_eSndGameMode = (raceType == GRace::kRaceType_Challenge) ? SND_CHALLENGERACE : SND_STREETRACE;
                if (m_pNFSMixMaster != nullptr) {
                    if (Sim::GetUserMode() == Sim::USER_SPLIT_SCREEN) {
                        m_pNFSMixMaster->CreateMainMainMap(eRACE_TWOCIRC);
                    } else {
                        m_pNFSMixMaster->CreateMainMainMap(eRACE_CIRCUIT);
                    }
                }
            }
        }
    }

    InitializeInGame();

    if (m_pNFSMixMaster != nullptr) {
        m_pNFSMixMaster->InitMixMap(0);
    }

    SFXObj_Pathfinder *ppf = static_cast<SFXObj_Pathfinder *>(GetSFXBase_Object(0x40010010));
    if (ppf != nullptr) {
        if (m_ePlayerMixMode == EAXS3D_TWO_PLAYER_MIX) {
            ppf->m_Flags |= 2;
        } else {
            ppf->m_Flags &= ~2u;
        }

        if (GRaceStatus::Exists() && GRaceStatus::Get().GetRaceParameters() != nullptr &&
            GRaceStatus::Get().GetRaceParameters()->GetRaceType() != GRace::kRaceType_Drag) {
            ppf->m_Flags |= 1;
        }
    }

    gbHasStartNewGamePlayBeenProcessed = true;
}

void EAXSound::InitializeFrontEnd() {
    if (!IsSoundEnabled) {
        return;
    }

    if (mAttributes == nullptr) {
        mAttributes = new Attrib::Gen::audiosystem(0x7e4b0ed2, 0, nullptr);
    }

    if (mLocalAttr == nullptr && mAttributes != nullptr) {
        mLocalAttr = new Attrib::Gen::audiosystem(mAttributes->Locales(0).GetCollectionWithDefault(), 0, nullptr);
    }

    m_pCmnSnd = new EAXCommon();
    m_pFESnd = new EAXFrontEnd();
    RefreshLocalAttr();
    Speech::Manager::Init(0);
    SoundRandomSeed = bRandom(-1, &SoundRandomSeed);
    m_pCurAudioSettings = &FEDatabase->CurrentUserProfiles[0]->GetOptions()->TheAudioSettings;
    EAXAemsManager::m_RequiredSlots[3]++;
    gAEMSMgr.InitializeSlots(true);

    if (m_pStateMgr[0] != nullptr) {
        m_pStateMgr[0]->EnterWorld(m_eSndGameMode);
    }
    if (m_pStateMgr[1] != nullptr) {
        m_pStateMgr[1]->EnterWorld(m_eSndGameMode);
    }

    InitEATRAX();
}

void EAXSound::InitializeInGame() {
    if (!IsSoundEnabled) {
        return;
    }

    int skipState = -1;
    m_pCmnSnd = new EAXCommon();
    if (m_pCmnSnd != nullptr) {
        m_pCmnSnd->Initialize();
    }

    SoundRandomSeed = bRandom(-1, &SoundRandomSeed);
    if (!bIsAnFEToIngameTransition) {
        skipState = 1;
    }
    bIsAnFEToIngameTransition = false;

    for (int n = 0; n < 13; n++) {
        if (n != skipState && m_pStateMgr[n] != nullptr) {
            m_pStateMgr[n]->EnterWorld(m_eSndGameMode);
        }
    }

    mEventID = Scheduler::Get().fSchedule_OncePerGameLoop->AddTask(-0x0D2EF66E, &mData, 6, true, 0, 0);
}

void EAXSound::MixMapReadyCallback() {
    bIsMapInQueuedFileLoad = false;
    for (int n = 0; n < eMM_MAX_MAIN_MIXSTATES; n++) {
        if (m_pStateMgr[n] != nullptr) {
            m_pStateMgr[n]->SafeConnectOrphanObjects();
        }
    }
    g_pEAXSound->AttachPlayerCars();
}

void EAXSound::AttachPlayerCars() {
    if (m_pStateMgr[eMM_PLAYERCAR] == nullptr) {
        return;
    }

    int attachedCount = 0;
    for (int n = 0; n < 32; n++) {
        CSTATE_Base *state = m_pStateMgr[eMM_PLAYERCAR]->GetStateObj(n);
        if (state == nullptr) {
            continue;
        }

        if (!state->IsAttached()) {
            continue;
        }

        attachedCount++;
        if (Sim::GetUserMode() == Sim::USER_SPLIT_SCREEN) {
            if (attachedCount >= 2) {
                return;
            }
        } else if (attachedCount >= 1) {
            return;
        }
    }
}

void EAXSound::InitEATRAX() {
    UpdateSongInfo();
}

void EAXSound::PlayFEMusic(int nIndex) {
    MControlPathfinder msg(true, 0, 0, 0);
    msg.Send(UCrc32("Pathfinder5"));
}

namespace {

struct EAX_CarStateCtorView {
    int _listNode; // offset 0x0
    float mMaxTorque; // offset 0x4
    float mMaxRPM; // offset 0x8
    float mMinRPM; // offset 0xC
    float mRedline; // offset 0x10
    bMatrix4 mMatrix; // offset 0x14
    bVector3 mVel0; // offset 0x54
    int mRacePos; // offset 0x64
    bVector3 mVel1; // offset 0x68
    float mBrake; // offset 0x78
    bVector3 mAccel; // offset 0x7C
    float mEBrake; // offset 0x8C
    float mFWSpeed; // offset 0x90
    int mIsShocked; // offset 0x94
    float mHealth; // offset 0x98
    int mNosEmptyFlag; // offset 0x9C
    int mMovementMode; // offset 0xA0
    int mPlayerZone; // offset 0xA4
    Sound::Wheel mWheel[4]; // offset 0xA8
    unsigned short mSteering; // offset 0x1B8
    unsigned short mAngle; // offset 0x1BA
    Sound::Engine mEngine; // offset 0x1BC
    Sound::Driveline mDriveline; // offset 0x1D8
    int mSirenState; // offset 0x1E0
    int mHotPursuit; // offset 0x1E4
    Attrib::Instance mAttributes; // offset 0x1E8
    Attrib::Instance mEngineInfo; // offset 0x1FC
    Sound::Context mContext; // offset 0x210
    int mSimUpdating; // offset 0x214
    int mAssetsLoaded; // offset 0x218
    unsigned int mWorldID; // offset 0x21C
    HSIMABLE__ *mHandle; // offset 0x220
    unsigned int mTrailerID; // offset 0x224
    float mOversteer; // offset 0x228
    float mUndersteer; // offset 0x22C
    float mSlipAngle; // offset 0x230
    float mVisualRPM; // offset 0x234
    float mTimeSinceSeen; // offset 0x238
    int mNISCarID; // offset 0x23C
    float mDesiredSpeed; // offset 0x240
    int mControlSource; // offset 0x244
};

typedef char EAX_CarStateCtorViewSizeCheck[(sizeof(EAX_CarStateCtorView) == 0x248) ? 1 : -1];

} // namespace

unsigned int GenerateUpgradedEngine(EAX_CarState *pCar, int playerUpgrade);

namespace Physics {
namespace Info {

float MaxTorque(const Attrib::Gen::engine &engine, float &atrpm);

} // namespace Info
} // namespace Physics

void Sound::Wheel::Reset() {
    mWheelSlip = bVector2(0.0f, 0.0f);
    mWheelForceZ = 0.0f;
    mPercentFsFkTransfer = 0.0f;
    mWheelOnGround = 1;
    mLoad = 0.0f;
    mBlownState = 0;
    mPrevBlownState = 0;
}

void Sound::Engine::Reset() {
    mBoostFlag = 0;
    mNOSFlag = 0;
    mNOS = 0.0f;
    mRPMPct = 0.0f;
    mThrottle = 0.0f;
    mBoost = 0.0f;
    mBlownFlag = 0;
}

EAX_HeliState::EAX_HeliState(const Attrib::Collection *atr, unsigned int wuid)
    : mFWSpeed(0.0f) //
    , mMovementMode(1) //
    , mPlayerZone(0) //
    , mAttributes(atr, 0, nullptr) //
    , mContext(Sound::kRaceContext_QuickRace) //
    , mSimUpdating(true) //
    , mWorldID(wuid) {
    mVel0 = bVector3(0.0f, 0.0f, 0.0f);
    mVel1 = mVel0;
    mAccel = bVector3(0.0f, 0.0f, 0.0f);
    PSMTX44Identity((Mtx44)&mMatrix);
}

EAX_CarState::EAX_CarState(const Attrib::Collection *atr, Sound::Context context, unsigned int wuid, HSIMABLE__ *handle)
{
    EAX_CarStateCtorView &state = *static_cast<EAX_CarStateCtorView *>(static_cast<void *>(this));
    static int PlayerUpgrade;

    state.mEBrake = 0.0f;
    state.mMovementMode = Sound::PHYSICS_MOVEMENT;
    state.mPlayerZone = Sound::PLAYER_ZONE_NONE;
    state.mVel0.x = 0.0f;
    state.mVel0.y = 0.0f;
    state.mVel0.z = 0.0f;
    state.mRacePos = 0;
    state.mBrake = 0.0f;
    state.mNosEmptyFlag = 0;

    Sound::Wheel *wheel = state.mWheel;
    int i = 3;
    do {
        new (&wheel->mTerrainType) Attrib::Instance(static_cast<const Attrib::Collection *>(nullptr), 0, nullptr);
        wheel->mTerrainType.SetDefaultLayout(0xFC);
        const Attrib::Collection *nullSpec = SimSurface::kNull.GetConstCollection();
        if (nullSpec) {
            wheel->mTerrainType.Change(nullSpec);
        }

        new (&wheel->mPrevTerrainType) Attrib::Instance(static_cast<const Attrib::Collection *>(nullptr), 0, nullptr);
        wheel->mPrevTerrainType.SetDefaultLayout(0xFC);
        if (nullSpec) {
            wheel->mPrevTerrainType.Change(nullSpec);
        }

        wheel->Reset();
        ++wheel;
    } while (i-- != 0);

    state.mAngle = 0;
    state.mSteering = 0;
    state.mEngine.Reset();
    state.mDriveline.mGearShiftFlag = 0;
    state.mDriveline.mGear = Sound::NEUTRAL;
    state.mHotPursuit = 0;
    state.mSirenState = -1;

    new (&state.mAttributes) Attrib::Instance(atr, 0, nullptr);
    state.mAttributes.SetDefaultLayout(0x50);

    new (&state.mEngineInfo) Attrib::Instance(static_cast<const Attrib::Collection *>(nullptr), 0, nullptr);
    state.mEngineInfo.SetDefaultLayout(0xB0);

    state.mWorldID = wuid;
    state.mContext = context;
    state.mSimUpdating = 1;
    state.mHandle = handle;
    state.mTrailerID = 0;
    state.mNISCarID = -1;
    state.mAssetsLoaded = 0;
    state.mControlSource = Sound::CONTROL_AI;
    state.mDesiredSpeed = 0.0f;
    state.mVel1 = state.mVel0;
    state.mOversteer = 0.0f;
    state.mUndersteer = 0.0f;
    state.mSlipAngle = 0.0f;
    state.mVisualRPM = 0.0f;
    PSMTX44Identity((Mtx44)&state.mMatrix);

    Attrib::Gen::pvehicle &attributes =
        *static_cast<Attrib::Gen::pvehicle *>(static_cast<void *>(&state.mAttributes));
    Attrib::Gen::engineaudio &engineInfo =
        *static_cast<Attrib::Gen::engineaudio *>(static_cast<void *>(&state.mEngineInfo));
    Attrib::Gen::pvehicle vehicleinfo(attributes);
    vehicleinfo.SetDefaultLayout(0x50);

    const Attrib::RefSpec *engineRef = static_cast<const Attrib::RefSpec *>(vehicleinfo.GetAttributePointer(0xF1F5FBC7, 0));
    if (!engineRef) {
        engineRef = static_cast<const Attrib::RefSpec *>(Attrib::DefaultDataArea(0xC));
    }

    Attrib::Gen::engine engine(engineRef->GetCollectionWithDefault(), 0, nullptr);

    if (state.mContext == Sound::kRaceContext_QuickRace) {
        const int *physCurUpgrade = static_cast<const int *>(attributes.GetAttributePointer(0xB12CCB69, 0));
        if (!physCurUpgrade) {
            physCurUpgrade = static_cast<const int *>(Attrib::DefaultDataArea(4));
        }

        int baseUpgrade = 4 - attributes.engine_upgrades();
        if (baseUpgrade < 0) {
            baseUpgrade = 0;
        }
        if (baseUpgrade > 4) {
            baseUpgrade = 4;
        }

        int curupgade_offset = *physCurUpgrade;
        if (curupgade_offset < 0) {
            curupgade_offset = 0;
        }
        if (curupgade_offset > 4) {
            curupgade_offset = 4;
        }

        PlayerUpgrade = 0;
        if (baseUpgrade + curupgade_offset > 0) {
            PlayerUpgrade = baseUpgrade + curupgade_offset;
        }
        if (PlayerUpgrade > 4) {
            PlayerUpgrade = 4;
        }
    }

    unsigned int upgradedEngine = GenerateUpgradedEngine(this, PlayerUpgrade);
    unsigned int (*engineAudioClassKey)() = Attrib::Gen::engineaudio::ClassKey;
    const Attrib::Collection *engineCollection =
        Attrib::FindCollectionWithDefault(engineAudioClassKey(), upgradedEngine);
    engineInfo.Change(engineCollection);

    float max_torque_rpm;
    state.mMaxTorque = Physics::Info::MaxTorque(engine, max_torque_rpm);
    state.mMaxRPM = engine.MAX_RPM();
    state.mMinRPM = engine.IDLE();
    state.mRedline = engine.RED_LINE();

    if (state.mContext == Sound::kRaceContext_Count || (state.mContext > 2 && state.mContext < 7 && state.mContext > 4)) {
        state.mAssetsLoaded = 1;
    }
}

template class UTL::Collections::Listable<EAX_CarState, 10>::List;
template class UTL::Vector<EAX_HeliState *, 16>;

void EAXSound::Update(float t) {
    if (!IsSoundEnabled || t < 0.0f) {
        return;
    }

    if (gnHasStartLoadFEBeenProcessed != 0) {
        gnHasStartLoadFEBeenProcessed--;
        if (gnHasStartLoadFEBeenProcessed < 0) {
            gnHasStartLoadFEBeenProcessed = 0;
        }
        return;
    }

    if (gbHasStartNewGamePlayBeenProcessed != 0) {
        gbHasStartNewGamePlayBeenProcessed = 0;
        return;
    }

    if (DisableSoundUpdate != 0) {
        return;
    }

    if (!bReadCallbackToggle) {
        if (!bStreamReadTiming) {
            bStreamReadTiming = true;
            unsigned int cur = bGetTicker();
            bGetTickerDifference(uStreamReadTicks, cur);
            uStreamReadTicks = cur;
        }
    } else if (bStreamReadTiming) {
        unsigned int cur = bGetTicker();
        bGetTickerDifference(uStreamReadTicks, cur);
        uStreamReadTicks = cur;
        bStreamReadTiming = false;
    }

    if (gbWorldDataBlocksAudioRead) {
        if (!bStreamBlockState) {
            bStreamBlockState = true;
            uStreamBlockTicks = bGetTicker();
        }
    } else if (bStreamBlockState) {
        unsigned int cur = bGetTicker();
        bGetTickerDifference(uStreamBlockTicks, cur);
        bStreamBlockState = false;
    }

    if (gbAudioInterruptsWorldDataRead) {
        if (!bAudioInterrupt) {
            uAudioInterruptTicks = bGetTicker();
        }
        bAudioInterrupt = true;
    } else if (bAudioInterrupt) {
        bGetTickerDifference(uAudioInterruptTicks);
        bAudioInterrupt = false;
    } else {
        bAudioInterrupt = false;
    }

    if (gb_DORESTART_RACE == 0) {
        if (gb_Is321 != 0) {
            QueueNISButtonThrough(bStringHash("RESTART_FAKE"), -1);
            if (g_pNISRevMgr != nullptr) {
                g_pNISRevMgr->Start321Reving();
            }
            gb_Is321 = 0;
        }

        if (GameFlowSndState[13] != 0) {
            Speech::Manager::Deduce();
        }

        gAEMSMgr.Update();
        SndCamera::UpdateCameras();
        if (m_pEAXSND8Wrapper != nullptr) {
            m_pEAXSND8Wrapper->Update();
        }
        if (g_pNISRevMgr != nullptr) {
            g_pNISRevMgr->Update(t);
        }

        if (m_pNFSMixMaster != nullptr && m_pNFSMixMaster->m_bMapReady) {
            if (m_pSTICH_Playback != nullptr) {
                m_pSTICH_Playback->Update(t);
            }
            Speech::Manager::Update(t);

            if (m_pFESnd != nullptr) {
                m_pFESnd->Update(nullptr);
            }

            SndBase::m_fRunningTime += t;
            SndBase::m_fDeltaTime = t;
            for (int n = 0; n < 13; n++) {
                CSTATEMGR_Base *mgr = m_pStateMgr[n];
                if (mgr != nullptr) {
                    mgr->UpdateParams(g_EAXIsPaused() ? 0.0f : t);
                }
            }

            m_pNFSMixMaster->ProcessMixMap(t, SndCamera::m_CurCamState[0]);

            for (int n = 0; n < 13; n++) {
                CSTATEMGR_Base *mgr = m_pStateMgr[n];
                if (mgr != nullptr) {
                    mgr->ProcessUpdate();
                }
            }

            if (m_pCmnSnd != nullptr) {
                m_pCmnSnd->Update(nullptr);
            }
        }

        m_prevSndGameMode = m_eSndGameMode;
        if (!AreResourceLoadsPending()) {
            UTL::Collections::Listable<CarSoundConn, 10>::ForEach(CarSoundConn::SetAssetsLoaded);
        }
        SNDSYS_service();
    } else {
        if (m_pNFSMixMaster != nullptr) {
            m_pNFSMixMaster->DestroyMainMainMap();
        }
        InitializeInGame();
        if (m_pNFSMixMaster != nullptr) {
            m_pNFSMixMaster->InitMixMap(0);
        }

        Speech::Module *module = Speech::Manager::GetSpeechModule(1);
        if (module != nullptr) {
            module = Speech::Manager::GetSpeechModule(1);
            if (module != nullptr) {
                module->ReleaseResource();
            }
        }

        SFXObj_Pathfinder *ppf = static_cast<SFXObj_Pathfinder *>(GetSFXBase_Object(0x40010010));
        if (ppf != nullptr) {
            if (m_ePlayerMixMode == EAXS3D_TWO_PLAYER_MIX) {
                ppf->m_Flags |= 2;
            } else {
                ppf->m_Flags &= ~2u;
            }
        }

        bMemSet(GameFlowSndState, '\0', 0x3C);
        gb_DORESTART_RACE = 0;
        SetSoundControlState(false, SNDSTATE_PAUSE, "PauseMenu");
    }
}

void EAXSound::CommitAssets() {
    CSTATEMGR_CarState::ResolveCarBanks();
    typedef UTL::Collections::Listable<CarSoundConn, 10> CarList;
    for (CarSoundConn *const *iter = CarList::GetList().begin(); iter != CarList::GetList().end(); ++iter) {
        CarSoundConn *pconn = *iter;
        if (!pconn->mConnected) {
            pconn->mConnected = true;
        }
    }
}

EAXCar *EAXSound::GetPlayerTunerCar(int nindex) {
    CSTATEMGR_Base *mgr = m_pStateMgr[2];
    if (mgr == nullptr) {
        return nullptr;
    }
    return (EAXCar *)mgr->GetStateObj(nindex);
}

CSTATE_Base *EAXSound::SpawnHelicopter(EAX_HeliState *pHeli) {
    if (!IsSoundEnabled) {
        return nullptr;
    }
    CSTATE_Base *newheli = m_pStateMgr[eMM_HELICOPTER]->GetFreeState(pHeli);
    if (newheli != nullptr) {
        newheli->Attach(pHeli);
        return newheli;
    }
    return nullptr;
}

void EAXSound::DestroyEAXHeli(EAX_HeliState *pHeli) {
    if (IsSoundEnabled) {
        CSTATE_Base *newheli = m_pStateMgr[eMM_HELICOPTER]->GetStateObj(pHeli);
        if (newheli != nullptr) {
            newheli->Detach();
        }
    }
}

void EAXSound::DestroyEAXCar(EAX_CarState *pCar) {
    if (!IsSoundEnabled) {
        return;
    }
    CSTATE_Base *attachedcar = nullptr;
    CSTATEMGR_Base *pCVar2;
    switch (pCar->mContext) {
    case GRace::kRaceContext_QuickRace:
        pCVar2 = m_pStateMgr[eMM_PLAYERCAR];
        break;
    case GRace::kRaceContext_TimeTrial:
        pCVar2 = m_pStateMgr[eMM_AIRACECAR];
        break;
    case GRace::kRaceContext_Career:
        pCVar2 = m_pStateMgr[eMM_COPCAR];
        break;
    case GRace::kRaceContext_Count:
        pCVar2 = m_pStateMgr[eMM_TRAFFIC];
        break;
    default:
        pCVar2 = m_pStateMgr[eMM_TRUCK];
        break;
    }
    if (pCVar2 != nullptr) {
        attachedcar = pCVar2->GetStateObj(pCar);
    }
    if (attachedcar != nullptr) {
        attachedcar->Detach();
    }
    CSTATEMGR_CarState::DestroyCar(pCar);
}

void LoadCommonIngameFiles() {
    Attrib::Gen::audiosystem *attributes = g_pEAXSound->GetAttributes();
    if (attributes == nullptr) {
        return;
    }

    g_LoadSndAsset(attributes->AEMS_FEBanks(1), SNDPATH_GLOBAL, EAXSND_DT_AEMS_ASYNCSPUMEM);
    g_LoadSndAsset(attributes->AEMS_EnvBanks(0), SNDPATH_INGAME, EAXSND_DT_AEMS_ASYNCSPUMEM);
    g_LoadSndAsset(attributes->AEMS_MiscBanks(5), SNDPATH_INGAME, EAXSND_DT_AEMS_ASYNCSPUMEM);
    g_LoadSndAsset(attributes->AEMS_MiscBanks(4), SNDPATH_INGAME, EAXSND_DT_AEMS_ASYNCSPUMEM);
    g_LoadSndAsset(attributes->AEMS_MiscBanks(3), SNDPATH_INGAME, EAXSND_DT_AEMS_ASYNCSPUMEM);
    g_LoadSndAsset(attributes->AEMS_MiscBanks(1), SNDPATH_INGAME, EAXSND_DT_AEMS_ASYNCSPUMEM);
    g_LoadSndAsset(attributes->AEMS_RNBanks(0), SNDPATH_INGAME, EAXSND_DT_AEMS_ASYNCSPUMEM);
    g_LoadSndAsset(Attrib::StringKey("SIREN_MB.abk"), SNDPATH_INGAME, EAXSND_DT_AEMS_ASYNCSPUMEM);
    g_LoadSndAsset(attributes->AEMS_StitchBanks(0), SNDPATH_INGAME, EAXSND_DT_AEMS_ASYNCSPUMEM);
    g_LoadSndAsset(attributes->AEMS_StitchBanks(2), SNDPATH_INGAME, EAXSND_DT_AEMS_ASYNCSPUMEM);
    g_LoadSndAsset(attributes->AEMS_StitchBanks(1), SNDPATH_INGAME, EAXSND_DT_AEMS_ASYNCSPUMEM);
    g_LoadSndAsset(attributes->AEMS_MiscBanks(2), SNDPATH_INGAME, EAXSND_DT_AEMS_ASYNCSPUMEM);
    g_LoadSndAsset(attributes->AEMS_WNBanks(0), SNDPATH_INGAME, EAXSND_DT_AEMS_ASYNCSPUMEM);
    g_LoadSndAsset(attributes->AEMS_MiscBanks(6), SNDPATH_ENGINE, EAXSND_DT_AEMS_ASYNCSPUMEM);
    g_LoadSndAsset(attributes->AEMS_MiscBanks(7), SNDPATH_ENGINE, EAXSND_DT_AEMS_ASYNCSPUMEM);

    for (int n = 0; n < 12; n++) {
        g_LoadSndAsset(Attrib::StringKey(csfxedit[n]), SNDPATH_FXEDIT, EAXSND_DT_GENERIC_DATA);
    }
}

void EAXSound::InitializeSoundBootLoad() {
    if (!IsSoundEnabled) {
        return;
    }

    mAttributes = new Attrib::Gen::audiosystem(0x7e4b0ed2, 0, nullptr);
    RefreshLocalAttr();
    gAEMSMgr.Init();
    gSpeechCache.Init(0x1B000);
    CSTATEMGR_Base::ClearClassLists();
    RegisterStates();
    RegisterSFX();
}

void EAXSound::LoadFrontEndSoundBanks(void (*callback)(int), int callback_param) {
    if (!IsSoundEnabled) {
        return;
    }

    if (IsSpeechEnabled) {
        gSpeechCache.Validate();
    }

    gIsPauseForPause = 0;
    bSyncTaskRun();
    gnHasStartLoadFEBeenProcessed = 2;
    m_prevSndGameMode = m_eSndGameMode;
    m_eSndGameMode = SND_FRONTEND;
    gAEMSMgr.ResetBankLoadParams();
    m_pStreamManager = new EAXS_StreamManager();
    if (m_pStreamManager != nullptr) {
        m_pStreamManager->InitializeStreams(SNDGM_FRONTEND);
    }

    if ((g_ActiveCtlStates & 0x20000) == 0) {
        g_ActiveCtlStates = 0;
        g_PrevActiveCtlStates = 0;
        g_ActiveSFXStates = 0;
        g_PrevActiveSFXStates = 0;
    }

    InitializeFrontEnd();

    if (m_pCmnSnd != nullptr) {
        m_pCmnSnd->Initialize();
    }
    if (m_pFESnd != nullptr) {
        m_pFESnd->Initialize();
    }

    if (m_pNFSMixMaster != nullptr) {
        m_pNFSMixMaster->CreateMainMainMap(eRACE_CIRCUIT);
        m_pNFSMixMaster->InitMixMap(0);
    }

    gAEMSMgr.m_ExternalLoadCallback = callback;
    gAEMSMgr.m_ExternalLoadCallbackParam = callback_param;

    ReInitMasterVolumes();
    bSyncTaskRun();

    if (IsSpeechEnabled) {
        gSpeechCache.Validate();
    }
}

void EAXSound::UnloadFrontEndSoundBanks() {
    if (!IsSoundEnabled) {
        return;
    }

    if (IsSpeechEnabled) {
        gSpeechCache.Validate();
    }

    gIsPauseForPause = 0;
    while (AreResourceLoadsPending()) {
        Update(0.0f);
        ServiceQueuedFiles();
    }

    Speech::Manager::Destroy();

    if (m_pFESnd != nullptr) {
        m_pFESnd->DestroyAllDriveOnSnds();
    }

    for (int n = 0; n < 13; n++) {
        if (m_pStateMgr[n] != nullptr) {
            m_pStateMgr[n]->ExitWorld();
        }
    }

    if (m_pStreamManager != nullptr) {
        delete m_pStreamManager;
        m_pStreamManager = nullptr;
    }

    if (m_pNFSMixMaster != nullptr) {
        m_pNFSMixMaster->DestroyMainMainMap();
    }

    if (m_pFESnd != nullptr) {
        delete m_pFESnd;
        m_pFESnd = nullptr;
    }

    if (m_pCmnSnd != nullptr) {
        delete m_pCmnSnd;
        m_pCmnSnd = nullptr;
    }

    while (gAEMSMgr.m_nEndOfList != 0) {
        gAEMSMgr.UnloadSndData(0);
    }

    gAEMSMgr.DestroySlots(false);
    gAEMSMgr.ResetBankLoadParams();
    bSyncTaskRun();

    if (IsSpeechEnabled) {
        gSpeechCache.Validate();
    }
}

void InitializeSoundLoad() {
    g_pEAXSound->InitializeSoundBootLoad();
}

void LoadAemsFrontEnd(void (*callback)(int), int callback_param) {
    if (IsSoundEnabled == 1) {
        g_pEAXSound->LoadFrontEndSoundBanks(callback, callback_param);
    } else {
        callback(callback_param);
    }
}

void UnloadAemsFrontEnd() {
    if (IsSoundEnabled == 1) {
        g_pEAXSound->UnloadFrontEndSoundBanks();
    }
}

void EAXSound::StopSND11() {}

void EAXSound::StartSND11() {}

// LoadInGameSoundBanks, etc. go here when implemented

void EAXSound::LoadInGameSoundBanks(void (*callback)(int), int callback_param) {
    if (!IsSoundEnabled) {
        return;
    }

    if (IsSpeechEnabled) {
        gSpeechCache.Validate();
    }

    gIsPauseForPause = 0;
    bIsAnFEToIngameTransition = true;
    m_pStreamManager = new EAXS_StreamManager();

    bool splitScreen = FEDatabase->IsSplitScreenMode() && FEDatabase->iNumPlayers == 2;
    if (m_pStreamManager != nullptr) {
        if (splitScreen) {
            m_pStreamManager->InitializeStreams(SNDGM_SPLITSCREEN);
            m_ePlayerMixMode = EAXS3D_TWO_PLAYER_MIX;
        } else {
            m_pStreamManager->InitializeStreams(SNDGM_FREEROAM);
            m_ePlayerMixMode = EAXS3D_SINGLE_PLAYER_MIX;
        }
    }

    bHasDataLoadOccured = true;
    bHasStartNewGameOccured = false;
    m_pCurAudioSettings = &FEDatabase->CurrentUserProfiles[0]->GetOptions()->TheAudioSettings;
    if (m_pEAXSND8Wrapper != nullptr) {
        m_pEAXSND8Wrapper->ReInit();
    }

    m_prevSndGameMode = m_eSndGameMode;
    m_eSndGameMode = SND_STREETRACE;
    gAEMSMgr.ResetBankLoadParams();
    CSTATEMGR_CarState::ResetCarBanks();
    RefreshLocalAttr();

    if (m_ePlayerMixMode == EAXS3D_TWO_PLAYER_MIX) {
        IsSpeechEnabled = false;
        Speech::Manager::Init(2);
    } else {
        IsSpeechEnabled = true;
        Speech::Manager::Init(1);
    }

    LoadCommonIngameFiles();

    if ((g_ActiveCtlStates & 0x20000) == 0) {
        g_ActiveCtlStates = 0;
        g_PrevActiveCtlStates = 0;
        g_ActiveSFXStates = 0;
        g_PrevActiveSFXStates = 0;
    }

    gAEMSMgr.m_ExternalLoadCallback = callback;
    gAEMSMgr.m_ExternalLoadCallbackParam = callback_param;

    if (IsSpeechEnabled) {
        gSpeechCache.Validate();
    }
}

void EAXSound::UnLoadInGameSoundBanks() {
    if (!IsSoundEnabled) {
        return;
    }

    IsSpeechEnabled = true;
    gIsPauseForPause = 0;
    while (AreResourceLoadsPending()) {
        Update(0.0f);
        ServiceQueuedFiles();
    }

    bHasDataLoadOccured = false;
    if (mmsgMRestartRace != nullptr) {
        Hermes::Handler::Destroy(mmsgMRestartRace);
        mmsgMRestartRace = nullptr;
    }

    if (IsSpeechEnabled) {
        gSpeechCache.Validate();
    }

    Speech::Manager::Destroy();
    bSyncTaskRun();

    if (m_pNFSMixMaster != nullptr) {
        m_pNFSMixMaster->DestroyMainMainMap();
    }

    if (m_pStateMgr[0] != nullptr) {
        m_pStateMgr[0]->ExitWorld();
    }

    if (m_pStreamManager != nullptr) {
        delete m_pStreamManager;
        m_pStreamManager = nullptr;
    }

    if (m_pCmnSnd != nullptr) {
        delete m_pCmnSnd;
        m_pCmnSnd = nullptr;
    }

    while (gAEMSMgr.m_nEndOfList != 0) {
        gAEMSMgr.UnloadSndData(0);
    }

    gAEMSMgr.DestroySlots(true);
    gAEMSMgr.ResetBankLoadParams();
    bSyncTaskRun();
}

void LoadAemsInGame(void (*callback)(int), int callback_param) {
    if (IsSoundEnabled == 1) {
        g_pEAXSound->LoadInGameSoundBanks(callback, callback_param);
    } else {
        callback(callback_param);
    }
}

void UnloadAemsInGame() {
    if (IsSoundEnabled == 1) {
        g_pEAXSound->UnLoadInGameSoundBanks();
    }
}

void FESoundControl(bool bOn, const char *name) {
    if (g_pEAXSound == nullptr) {
        return;
    }

    unsigned int uVar2 = Attrib::StringHash32(name);
    Attrib::StringKey FengList[37] = {
        "Pause_Main.fng",
        "Pause_Options.fng",
        "Pause_Controller.fng",
        "InGamePhotoMaster.fng",
        "EA_Trax.fng",
        "FadeScreen.fng",
        "SMS_Message.fng",
        "BUSTED_OVERLAY.fng",
        "SixDaysLater.fng",
        "InGame_MC_Main_GC.fng",
        "InGameAnyMovie.fng",
        "Pause_Performance_Tuning.fng",
        "InGame_MC_Main.fng",
        "InGameDialog.fng",
        "WS_InGameAnyMovie.fng",
        "InGameAnyMovie.fng",
        "InGameAnyTutorial.fng",
        "FEAnyMovie.fng",
        "WS_FEAnyMovie.fng",
        "FEAnyTutorial.fng",
        "InGameAnyTutorial.fng",
        "LS_EALogo.fng",
        "LS_EA_hidef.fng",
        "LS_PSA.fng",
        "MW_LS_IntroFMV.fng",
        "MW_LS_AttractFMV.fng",
        "WS_LS_EALogo.fng",
        "WS_LS_EA_hidef.fng",
        "WS_LS_PSA.fng",
        "WS_LS_IntroFMV.fng",
        "WS_MW_LS_AttractFMV.fng",
        "PostRace_MilestoneRewards.fng",
        "PostRace_Pursuit.fng",
        "Game_StartRace",
        "InGameMilestones.fng",
        "InGameBounty.fng",
        "InGameRaceSheet.fng",
    };

    bStringHash(name);
    int index = -1;
    int iVar1 = 0;
    do {
        index = iVar1;
        if (uVar2 == static_cast<unsigned int>(FengList[index])) {
            break;
        }
        int n = index + 1;
        index = -1;
        iVar1 = n;
    } while (iVar1 < 37);

    if (g_pEAXSound->GetSoundGameMode() == SND_FRONTEND) {
        if (index != 10 && index < 14) {
            return;
        }
        SetSoundControlState(bOn, SNDSTATE_FMV, "name");
        return;
    }

    if (index < 16) {
        if (index < 14) {
            if (index == 8) {
                SetSoundControlState(bOn, SNDSTATE_PAUSE, name);
                SetSoundControlState(bOn, SNDSTATE_OFF, name);
                return;
            }

            if (index < 9) {
                if (index != 3) {
                    if (index > 3) {
                        return;
                    }
                    if (index < 0) {
                        goto FE_UPSCREEN;
                    }

                    g_pEAXSound->SetPauseMainFNG(bOn);
                    SetSoundControlState(bOn, SNDSTATE_PAUSE, name);
                    return;
                }
                SetSoundControlState(bOn, SNDSTATE_PAUSE, name);
                return;
            }

            if (index == 11) {
                g_pEAXSound->SetPauseMainFNG(bOn);
                SetSoundControlState(bOn, SNDSTATE_PAUSE, name);
                return;
            }

            if (index < 12) {
                if (index != 9) {
                    if (index != 10) {
                        goto FE_UPSCREEN;
                    }
                    goto FMV_STATE;
                }
            } else if (index != 12) {
                if (index == 13) {
                    return;
                }
                goto FE_UPSCREEN;
            }

            if (g_pEAXSound->IsPauseMainFNG()) {
                return;
            }
            SetSoundControlState(bOn, SNDSTATE_PAUSE, name);
            return;
        }
    } else {
        if (index > 30) {
            if (index != 33) {
                if (index < 33) {
                    SetSoundControlState(bOn, SNDSTATE_FE_SMS_MESSAGE, name);
                    return;
                }

                if (index < 37) {
                    Speech::Module *pMVar3 = Speech::Manager::GetSpeechModule(1);
                    if (pMVar3 != nullptr) {
                        pMVar3 = Speech::Manager::GetSpeechModule(1);
                        if (pMVar3 != nullptr) {
                            pMVar3->PurgeSpeech();
                        }
                    }
                }
                goto FE_UPSCREEN;
            }
            goto STOP_MUSIC;
        }

        if (index < 21 && (index > 19 || index < 17)) {
            SetSoundControlState(bOn, SNDSTATE_FMV, name);
            goto STOP_MUSIC;
        }
    }

FMV_STATE:
    if (bOn) {
        SetSoundControlState(bOn, SNDSTATE_FMV, name);
        return;
    }
    SetSoundControlState(false, SNDSTATE_FMV, name);
    bOn = true;

STOP_MUSIC:
    SetSoundControlState(bOn, SNDSTATE_STOP_MUSIC, name);
    return;

FE_UPSCREEN:
    SetSoundControlState(bOn, SNDSTATE_FE_UPSCREEN, name);
}

void SetSoundControlState(bool on, eSNDCTLSTATE state, const char *caller) {
    if (_RUN_SOUND_STATE == 0 || g_pEAXSound == nullptr || !IsSoundEnabled) {
        return;
    }

    bool turningOff = !on;
    bool pauseMainFNG = g_pEAXSound->IsPauseMainFNG();
    if (pauseMainFNG && turningOff && state != SNDSTATE_MINILOAD && !(state > SNDSTATE_FE_SMS_MESSAGE && state < SNDSTATE_FMV)) {
        return;
    }

    unsigned int stateBit = 1u << (state & 0x1F);
    if ((g_ActiveCtlStates & stateBit) == 0) {
        if (turningOff) {
            return;
        }
    } else if (!turningOff) {
        return;
    }

    g_PrevActiveCtlStates = g_ActiveCtlStates;
    g_PrevActiveSFXStates = g_ActiveSFXStates;

    if (turningOff) {
        g_ActiveCtlStates &= ~stateBit;
    } else {
        g_ActiveCtlStates |= stateBit;
    }

    g_ActiveSFXStates = 0;
    for (int n = 0; n < 18; n++) {
        if ((g_ActiveCtlStates & (1u << (n & 0x1F))) != 0) {
            g_ActiveSFXStates |= g_CtlStateActions[n];
        }
    }

    EAXS_StreamManager *streamManager = g_pEAXSound->GetStreamManager();

    for (unsigned int s = 0; s < 13; s++) {
        unsigned int sBit = 1u << (s & 0x1F);
        unsigned int cur = g_ActiveSFXStates & sBit;
        if (cur == (g_PrevActiveSFXStates & sBit) || streamManager == nullptr) {
            continue;
        }

        int channel = -1;
        if (s == 1) {
            channel = 0;
        } else if (s == 0) {
            channel = 1;
        } else if (s == 2) {
            channel = 2;
        }

        if (channel >= 0) {
            EAXS_StreamChannel *streamChannel = streamManager->GetStreamChannel(channel);
            if (streamChannel != nullptr) {
                if (cur != 0) {
                    streamChannel->Pause();
                } else {
                    streamChannel->Resume();
                }
            }
        }
    }

    if (state == SNDSTATE_ERROR) {
        g_pEAXSound->Update(0.0f);
    }
}

void CloseSound() {
    if (IsSoundEnabled) {
        g_pEAXSound->CloseSound();
    }
}

void EAXSound::CloseSound() {
    bHasDataLoadOccured = false;

    for (int n = 0; n < 13; n++) {
        if (m_pStateMgr[n] != nullptr) {
            m_pStateMgr[n]->ExitWorld();
        }
    }

    while (AreResourceLoadsPending()) {
        Update(0.0f);
        ServiceQueuedFiles();
    }

    if (m_pNFSMixMaster != nullptr) {
        m_pNFSMixMaster->DestroyMainMainMap();
    }

    bSyncTaskRun();
}

void EAXSound::ReStartRace(bool bfullrestart) {
    if (!IsSoundEnabled) {
        return;
    }

    SetSoundControlState(true, SNDSTATE_STOP_MUSIC, "RestartRace");

    if (m_pStreamManager != nullptr) {
        for (int n = 0; n < 4; n++) {
            EAXS_StreamChannel *channel = m_pStreamManager->GetStreamChannel(n);
            if (channel != nullptr) {
                channel->Stop();
                channel->PurgeStream();
                channel->Resume();
            }
        }
    }

    if (m_pCmnSnd != nullptr) {
        delete m_pCmnSnd;
        m_pCmnSnd = nullptr;
    }

    for (int i = 0; i < 13; i++) {
        if (i == 1) {
            if (m_pStateMgr[1] != nullptr) {
                m_pStateMgr[1]->DisconnectMixMap();
            }
        } else if (m_pStateMgr[i] != nullptr) {
            m_pStateMgr[i]->ExitWorld();
        }
    }

    gb_DORESTART_RACE = 1;
    gb_Is321 = bfullrestart ? 1 : 0;
}

void InitializeSoundDriver() {
    int poolSize = 0x4000;
    if (IsSoundEnabled) {
        poolSize = 0x11F000;
    }

    gAudioMemoryManager.InitMemoryPool(AUD_MAIN_MEM_POOL, poolSize);
    void *mem = gAudioMemoryManager.AllocateMemory(0xBC, "AUD:EAXSound", false);
    g_pEAXSound = new (mem) EAXSound();
    g_pEAXSound->InitializeDriver();
}

eSndAudioMode EAXSound::GetDefaultPlatformAudioMode() {
    return m_pEAXSND8Wrapper->GetDefaultPlatformAudioMode();
}

eSndAudioMode EAXSound::SetAudioModeFromMemoryCard(eSndAudioMode mode) {
    return m_pEAXSND8Wrapper->SetAudioModeFromMemoryCard(mode);
}

void SoundPause(bool bpause, eSNDPAUSE_REASON esndpause) {}
