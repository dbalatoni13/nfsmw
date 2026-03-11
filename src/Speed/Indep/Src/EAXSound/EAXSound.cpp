#include "./EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_NISStream.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_Pathfinder.hpp"
#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_Base.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_NISReving.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Gameplay/GRace.h"
#include "Speed/Indep/Src/Generated/Messages/MControlPathfinder.h"
#include "Speed/Indep/Src/Misc/Config.h"

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
};
struct Module;
struct Manager {
    static void ClearPlayback();
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
    eSndAudioMode GetDefaultPlatformAudioMode();
    eSndAudioMode SetAudioModeFromMemoryCard(eSndAudioMode mode);
    void Update();
    void STUPID();
};

struct EAXAemsManager : public AudioMemBase {
    bool AreResourceLoadsPending();
};

struct CarSoundConn : public Sim::Connection, public UTL::Collections::Listable<CarSoundConn, 10> {
    bool mConnected; // offset 0x14, size 0x1
    EAX_CarState *mState; // offset 0x18, size 0x4

    void OnReceive(Sim::Packet *) override;
    Sim::ConnStatus OnStatusCheck() override;
};

struct EAX_HeliState;

struct HeliSoundConn : public Sim::Connection, public UTL::Collections::Listable<HeliSoundConn, 10> {
    EAX_HeliState *mState; // offset 0x18, size 0x4

    void OnReceive(Sim::Packet *) override;
    Sim::ConnStatus OnStatusCheck() override;
};

struct CSTATEMGR_CarState : public CSTATEMGR_Base {
    static void ResolveCarBanks();
    static void DestroyCar(EAX_CarState *pCar);
};

struct EAX_CarState {
    char _pad_context[0x210];
    GRace::Context mContext;
    char _pad_assets[0x04];
    bool mAssetsLoaded; // offset 0x218
};

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

unsigned int bRandom(int range, unsigned int *seed);
float bRandom(float range, unsigned int *seed);
int bSPrintf(char *destString, const char *fmt, ...);
void SoundPause(bool on, eSNDPAUSE_REASON reason);
void SetSoundControlState(bool on, eSNDCTLSTATE state, const char *caller);

bool g_EAXIsPaused() {
    return (g_ActiveCtlStates & 0x3483b) != 0;
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

// QueueNISButtonThrough, QueueNISStream go here when implemented

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
            if (m_pFESnd != nullptr) {
                m_pFESnd->Play(static_cast<eMenuSoundTriggers>(etriggertype - UISND_COMMON_MAX_NUM));
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
            if (m_pFESnd != nullptr) {
                m_pFESnd->Stop(static_cast<eMenuSoundTriggers>(etriggertype - UISND_COMMON_MAX_NUM));
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

// InitializeDriver, RefreshLocalAttr, etc. go here when implemented

void EAXSound::MixMapReadyCallback() {
    bIsMapInQueuedFileLoad = false;
    for (int n = 0; n < eMM_MAX_MAIN_MIXSTATES; n++) {
        if (m_pStateMgr[n] != nullptr) {
            m_pStateMgr[n]->SafeConnectOrphanObjects();
        }
    }
    g_pEAXSound->AttachPlayerCars();
}

// AttachPlayerCars goes here when implemented

void EAXSound::InitEATRAX() {
    UpdateSongInfo();
}

void EAXSound::PlayFEMusic(int nIndex) {
    MControlPathfinder msg(true, 0, 0, 0);
    msg.Send(UCrc32("Pathfinder5"));
}

// Update goes here when implemented

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

// LoadFrontEndSoundBanks, etc. go here when implemented

void EAXSound::StopSND11() {}

void EAXSound::StartSND11() {}

// LoadInGameSoundBanks, etc. go here when implemented

eSndAudioMode EAXSound::GetDefaultPlatformAudioMode() {
    return m_pEAXSND8Wrapper->GetDefaultPlatformAudioMode();
}

eSndAudioMode EAXSound::SetAudioModeFromMemoryCard(eSndAudioMode mode) {
    return m_pEAXSND8Wrapper->SetAudioModeFromMemoryCard(mode);
}

void SoundPause(bool bpause, eSNDPAUSE_REASON esndpause) {}
