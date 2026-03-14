#include "./EAXSOund.hpp"
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
#include "Speed/Indep/Src/Main/Scheduler.h"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/Misc/QueuedFile.hpp"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/World/WorldConn.h"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bDebug.hpp"

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
    virtual void ReleaseResource();
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

namespace Sound {
enum Context {
    kRaceContext_QuickRace = GRace::kRaceContext_QuickRace,
    kRaceContext_TimeTrial = GRace::kRaceContext_TimeTrial,
    kRaceContext_Career = GRace::kRaceContext_Career,
    kRaceContext_Count = GRace::kRaceContext_Count,
};
}

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

struct stSndDataLoadParams {
    /* 0x00 */ char _pad0[0x34];
    /* 0x34 */ int Handle;
    /* 0x38 */ int bResolvedAsync;
    /* 0x3c */ int bResolvedSync;
    /* 0x40 */ char _pad40[0x20];
    /* 0x60 */ int t_req;
    /* 0x64 */ int t_load;

    void Clear();
};

struct stAssetDescription;
struct stSndAssetQueue;

enum eBANK_SLOT_TYPE {
    eBANK_SLOT_NONE = -1,
    eBANK_SLOT_AI_AEMS_ENGINE = 0,
    eBANK_SLOT_AI_GINA_ENGINE = 1,
    eBANK_SLOT_AI_GIND_ENGINE = 2,
    eBANK_SLOT_PATHFINDER = 3,
    eBANK_SLOT_MAX_NUM = 4
};

enum eEVTSYS {
    EVTSYS_MAIN = 0,
    EVTSYS_FE = 1,
};

enum eSNDDATAPATH {
    SNDPATH_EVTSYS = 0,
    SNDPATH_GLOBAL = 1,
    SNDPATH_INGAME = 2,
    SNDPATH_ENGINE = 3,
    SNDPATH_FXEDIT = 4,
};

enum eSNDDATATYPE {
    EAXSND_DT_INVALID = 0,
    EAXSND_DT_AEMS_MAINMEM = 1,
    EAXSND_DT_GENERIC_DATA = 2,
    EAXSND_DT_AEMS_ASYNCSPUMEM = 3,
};

struct stBankSlot {
    /* 0x00 */ eBANK_SLOT_TYPE Type;
    /* 0x04 */ int BANKmemLocation;
    /* 0x08 */ char *MAINmemLocation;
    /* 0x0c */ char *pLastAlloc;
    /* 0x10 */ int MAINmemSize;
    /* 0x14 */ int BANKMemSize;
    /* 0x18 */ int LoadFailed;
    /* 0x1c */ unsigned char Index;
    /* 0x20 */ stSndDataLoadParams *pAssetParams;

    void Clear();
};

struct EAXAemsManager : public AudioMemBase {
    static int m_RequiredSlots[4];
    static int m_SlotSizes[4][2];

    /* 0x004 */ char _pad0[0x9c - 0x4];
    /* 0x09c */ int m_nCurLoadedBankIndex;
    /* 0x0a0 */ int m_nEndOfList;
    /* 0x0a4 */ char _pad0xA4[0x10];
    /* 0x0b4 */ int m_SPUMainAllocsEnd;
    /* 0x0b8 */ int m_SPU_UpperAddress;
    /* 0x0bc */ char _pad0a[0xF8 - 0xbc]; // padding to m_pEvtSystems
    /* 0x0f8 */ void **m_pEvtSystems_start;
    /* 0x0fc */ void **m_pEvtSystems_end;
    /* 0x100 */ void **m_pEvtSystems_end_of_storage;
    /* 0x104 */ char _pad1[0x10]; // skip to m_nCallbackEvtSys
    /* 0x114 */ int m_nCallbackEvtSys;
    /* 0x118 */ stSndDataLoadParams *m_pCurLoadSDLP;
    /* 0x11c */ stSndDataLoadParams *m_pCurUNLOADSDLP;
    /* 0x120 */ stSndDataLoadParams *m_pAsyncLoadSDLP;
    /* 0x124 */ int m_ItemsPendingAsyncResolve;
    /* 0x128 */ char _pad0x128[0x4];
    /* 0x12c */ int m_IsWaitingForFileCB;

    EAXAemsManager();
    virtual ~EAXAemsManager();
    void Init();
    int AddEventSystem(eEVTSYS eESIndex, eSNDDATAPATH eSDP);
    int InitiateLoad();
    void SetupNextLoad();
    void Update();
    bool AreResourceLoadsPending();
    void *GetCallbackEventSys();
    static void EvtSysLoadCallback(int param, int error_status);
    void UnloadSndData(Attrib::StringKey filename);
    void UnloadSndData(int index);
    void RemoveAEMSBank();
    static void AddAemsBank();
    void CheckForCompleteAsyncLoad();
    void ResolvePendingAsyncLoads();
    void InitSPUram();
    void InitializeSlots(bool bDoPFSlot);
    void RegisterSlots(eBANK_SLOT_TYPE Type, int NumSlots, int SizePerSlotSPU, int SizePerSlotMainMem, bool bDoPFSlot);
    void ResetBankLoadParams();
    void RemoveBankListing(int index);
    int AddBankListing(stAssetDescription &asset);
    void QueueFileLoad(stSndAssetQueue &queueitem, eBANK_SLOT_TYPE SlotType);
    static void *AsyncResidentAllocCB(int size);
    static void *ResidentAllocCB(void *pbank, int residentsize, int totalsize);
    static void DataLoadCB(int param, int error_status);
    void DestroySlots(bool bDoPFSlot);
    int IsAssetInList(Attrib::StringKey filename);
    int IsAssetLoaded(Attrib::StringKey filename);
    void CompleteAsyncLoad();
    void ResolveCurrentDataMemory();
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

struct CSTATEMGR_AICar {
    static void QueueSlots();
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
};

struct EAX_CarState : public UTL::Collections::Listable<EAX_CarState, 10> {
    char _pad_context[0x20C];
    Sound::Context mContext;
    bool mSimUpdating; // offset 0x214
    char _pad_sim[3];
    bool mAssetsLoaded; // offset 0x218
    char _pad_assets[3];
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

    EAX_CarState(const Attrib::Collection *atr, Sound::Context context, unsigned int wuid, HSIMABLE__ *handle);
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

extern EAXAemsManager gAEMSMgr;
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
    char assetBytes[0x1C];
    *reinterpret_cast<int *>(assetBytes + 0x0) = static_cast<int>(type);
    *reinterpret_cast<int *>(assetBytes + 0x4) = static_cast<int>(path);
    *reinterpret_cast<Attrib::StringKey *>(assetBytes + 0x8) = filename;
    *reinterpret_cast<bool *>(assetBytes + 0x18) = false;
    gAEMSMgr.AddBankListing(*reinterpret_cast<stAssetDescription *>(assetBytes));
}

void EAXSound::START_321Countdown() {
    if (IsSoundEnabled && IsAudioStreamingEnabled) {
        SFXObj_Pathfinder *ppf = static_cast<SFXObj_Pathfinder *>(GetSFXBase_Object(0x40010010));
        if (ppf) {
            ppf->Set321(true);
        }
        SFXObj_NISStream *pnis = static_cast<SFXObj_NISStream *>(GetSFXBase_Object(0x40000050));
        if (pnis) {
            pnis->StartNIS();
        }
    }
}

bool EAXSound::AreResourceLoadsPending() {
    return gAEMSMgr.AreResourceLoadsPending();
}

void EAXSound::QueueNISButtonThrough(unsigned int anim_id, int camera_track_number) {
    if (!IsSoundEnabled || !IsAudioStreamingEnabled) {
        return;
    }

    SFXObj_NISStream *pnis = static_cast<SFXObj_NISStream *>(GetSFXBase_Object(0x40000050));
    if (pnis == nullptr) {
        return;
    }

    if (camera_track_number == -1) {
        if (g_laststartanimid == 0x0D0E5A9D || g_laststartanimid == 0xCBFF6594) {
            goto clear_playback;
        }
        anim_id = g_laststartanimid;
    } else if (!g_bWasLastNISaStart) {
        pnis->StopStream();
        goto clear_playback;
    }

    pnis->QueueNISStream(anim_id, camera_track_number, true, false);

clear_playback:
    Speech::Manager::ClearPlayback();
    Speech::Module *module = Speech::Manager::GetSpeechModule(COPSPEECH_MODULE);
    if (module != nullptr) {
        module->ReleaseResource();
    }
}

void EAXSound::QueueNISStream(unsigned int anim_id, int camera_track_number, void (*setmstimecb)(unsigned int, int)) {
    if (IsSoundEnabled && IsAudioStreamingEnabled) {
        SFXObj_NISStream *pnis = static_cast<SFXObj_NISStream *>(GetSFXBase_Object(0x40000050));
        pnis->QueueNISStream(anim_id, camera_track_number, setmstimecb, false);
    }
}

bool EAXSound::IsNISStreamQueued() {
    if (!IsSoundEnabled || !IsAudioStreamingEnabled) {
        return true;
    }
    SFXObj_NISStream *pnis = static_cast<SFXObj_NISStream *>(GetSFXBase_Object(0x40000050));
    return pnis->IsNISStreamReady();
}

// NISFinished goes here when implemented

void EAXSound::NISFinished() {
    if (IsSoundEnabled && IsAudioStreamingEnabled) {
        SFXObj_NISStream *pnis = static_cast<SFXObj_NISStream *>(GetSFXBase_Object(0x40000050));
        pnis->NISActivityDone();
        SoundPause(false, eSNDPAUSE_NISON);
        SetSoundControlState(false, SNDSTATE_NIS_STORY, "EAXSound::NISFinished");
        SetSoundControlState(false, SNDSTATE_NIS_INTRO, "EAXSound::NISFinished");
        SetSoundControlState(false, SNDSTATE_NIS_BLK, "EAXSound::NISFinished");
        SetSoundControlState(false, SNDSTATE_NIS_ARREST, "EAXSound::NISFinished");
    }
}

void EAXSound::PlayNIS() {
    if (IsSoundEnabled && IsAudioStreamingEnabled) {
        SFXObj_NISStream *pnis = static_cast<SFXObj_NISStream *>(GetSFXBase_Object(0x40000050));
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
    if (pbs == nullptr) {
        return g_DMIX_DummyInputBlock;
    }
    int *ptr = pbs->GetOutputBlockPtr();
    if (ptr == nullptr) {
        return nullptr;
    }
    return ptr;
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
    int nState = (static_cast<unsigned int>(nID) >> 16) & 0xFF;
    SFX_Base *ReturnObj = nullptr;
    if (m_pStateMgr[nState] != nullptr) {
        int nInstanceID = (static_cast<unsigned int>(nID) >> 11) & 0x1F;
        int SFXID_Number = (static_cast<unsigned int>(nID) >> 4) & 0x7F;
        CSTATE_Base *pstate = m_pStateMgr[nState]->GetStateObj(nInstanceID);
        if (pstate == nullptr) {
            return nullptr;
        }
        if ((nID & 0xE0000000) == 0x40000000) {
            ReturnObj = static_cast<SFX_Base *>(m_pStateMgr[nState]->GetStateObj(nInstanceID)->GetSFXObject(SFXID_Number));
        }
    }
    return ReturnObj;
}

SndBase *EAXSound::GetSndBase_Object(int nID) {
    int nState = (static_cast<unsigned int>(nID) >> 16) & 0xFF;
    SndBase *ReturnObj = nullptr;
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

void EAXSound::ReInitMasterVolumes() {}

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

    m_pEAXSND8Wrapper = new EAXSND8Wrapper();
    if (m_pEAXSND8Wrapper == nullptr || !m_pEAXSND8Wrapper->Initialize()) {
        IsSoundEnabled = false;
        return;
    }

    gAEMSMgr.InitSPUram();
    m_pNFSMixMaster = new NFSMixMaster();
    m_pSTICH_Playback = new cSTICH_PlayBack();
    g_pNISRevMgr = new NIS_RevManager();

    for (int n = 0; n < 13; n++) {
        m_pStateMgr[n] = nullptr;
    }
}

void EAXSound::RefreshLocalAttr() {
    if (mLocalAttr != nullptr) {
        delete mLocalAttr;
        mLocalAttr = nullptr;
    }

    int language = SkipFELanguage;
    if (SkipFE == 0) {
        language = GetCurrentLanguage();
    }

    unsigned int localeIndex = 0;
    switch (language) {
    case 1:
        localeIndex = 1;
        break;
    case 2:
        localeIndex = 2;
        break;
    case 3:
        localeIndex = 3;
        break;
    case 4:
        localeIndex = 4;
        break;
    case 5:
        localeIndex = 5;
        break;
    default:
        localeIndex = 0;
        break;
    }

    if (mAttributes != nullptr) {
        mLocalAttr = new Attrib::Gen::audiosystem(mAttributes->Locales(localeIndex).GetCollectionWithDefault(), 0, nullptr);
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
    : mContext(context) //
    , mSimUpdating(true) //
    , mAssetsLoaded(false) //
    , mWorldID(wuid) //
    , mHandle(handle) //
    , mTrailerID(0) //
    , mOversteer(0.0f) //
    , mUndersteer(0.0f) //
    , mSlipAngle(0.0f) //
    , mVisualRPM(0.0f) //
    , mTimeSinceSeen(0.0f) //
    , mNISCarID(-1) //
    , mDesiredSpeed(0.0f) //
    , mControlSource(0) {
    (void)atr;

    bMemSet(_pad_context, '\0', sizeof(_pad_context));
    PSMTX44Identity((Mtx44)(_pad_context + 0x14));
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
    Attrib::Gen::audiosystem *attributes =
        *reinterpret_cast<Attrib::Gen::audiosystem **>(reinterpret_cast<char *>(g_pEAXSound) + 0xA8);
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

    *reinterpret_cast<void (**)(int)>(reinterpret_cast<char *>(&gAEMSMgr) + 0x130) = callback;
    *reinterpret_cast<int *>(reinterpret_cast<char *>(&gAEMSMgr) + 0x134) = callback_param;

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

    *reinterpret_cast<void (**)(int)>(reinterpret_cast<char *>(&gAEMSMgr) + 0x130) = callback;
    *reinterpret_cast<int *>(reinterpret_cast<char *>(&gAEMSMgr) + 0x134) = callback_param;

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

    unsigned int hash = Attrib::StringHash32(name);
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
    int index;
    int iVar1 = 0;
    do {
        index = iVar1;
        if (hash == static_cast<unsigned int>(FengList[index])) {
            break;
        }
        int n = index + 1;
        index = -1;
        iVar1 = n;
    } while (iVar1 < 37);

    eSndGameMode gameMode = static_cast<eSndGameMode>(*reinterpret_cast<int *>(reinterpret_cast<char *>(g_pEAXSound) + 0x84));
    if (gameMode == SND_FRONTEND) {
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

                    *reinterpret_cast<int *>(reinterpret_cast<char *>(g_pEAXSound) + 0x38) = static_cast<int>(bOn);
                    SetSoundControlState(bOn, SNDSTATE_PAUSE, name);
                    return;
                }
            }

            if (index == 11) {
                *reinterpret_cast<int *>(reinterpret_cast<char *>(g_pEAXSound) + 0x38) = static_cast<int>(bOn);
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

            if (*reinterpret_cast<int *>(reinterpret_cast<char *>(g_pEAXSound) + 0x38) != 0) {
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
                    Speech::Module *module = Speech::Manager::GetSpeechModule(1);
                    if (module != nullptr) {
                        module = Speech::Manager::GetSpeechModule(1);
                        if (module != nullptr) {
                            module->ReleaseResource();
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
    bool pauseMainFNG = (*reinterpret_cast<int *>(reinterpret_cast<char *>(g_pEAXSound) + 0x38) != 0);
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

    EAXS_StreamManager *streamManager =
        *reinterpret_cast<EAXS_StreamManager **>(reinterpret_cast<char *>(g_pEAXSound) + 0x94);

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
