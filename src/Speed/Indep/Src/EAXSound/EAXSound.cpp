#include "./EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_NISStream.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_Pathfinder.hpp"
#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_Base.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_NISReving.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Misc/Config.h"

namespace Speech {
struct Cache {
    void Dump();
};
} // namespace Speech

extern Speech::Cache gSpeechCache;

// Minimal definitions for delete — virtual destructors required
struct EAXCommon {
    int m_nAemsPlayHandle[7];        // offset 0x0, size 0x1C
    void *m_pPlayCommonSampleHandle; // offset 0x1C, size 0x4
    void *m_pRadar;                  // offset 0x20, size 0x4
    SFX_Base *m_pSFXOBJ_FEHUD;      // offset 0x24, size 0x4
    void *mMsgMiscSound;            // offset 0x28, size 0x4
    virtual ~EAXCommon();
};

struct EAXFrontEnd {
    char _pad[0x10C];
    virtual ~EAXFrontEnd();
};

struct EAXSND8Wrapper : public AudioMemBase {
    virtual ~EAXSND8Wrapper();
    eSndAudioMode GetDefaultPlatformAudioMode();
    eSndAudioMode SetAudioModeFromMemoryCard(eSndAudioMode mode);
};

struct EAXAemsManager : public AudioMemBase {
    bool AreResourceLoadsPending();
};

struct CarSoundConn : public Sim::Connection, public UTL::Collections::Listable<CarSoundConn, 10> {
    bool mConnected; // offset 0x14, size 0x1
};

struct CSTATEMGR_CarState : public CSTATEMGR_Base {
    static void ResolveCarBanks();
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

unsigned int bRandom(int range, unsigned int *seed);
float bRandom(float range, unsigned int *seed);
int bSPrintf(char *destString, const char *fmt, ...);
void SoundPause(bool on, eSNDPAUSE_REASON reason);

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

bool EAXSound::IsNISStreamQueued() {
    if (!IsSoundEnabled || !IsAudioStreamingEnabled) {
        return true;
    }
    SFXObj_NISStream *pnis = static_cast<SFXObj_NISStream *>(GetSFXBase_Object(0x40000050));
    return pnis->IsNISStreamReady();
}

// NISFinished goes here when implemented

void EAXSound::PlayNIS() {
    if (IsSoundEnabled && IsAudioStreamingEnabled) {
        SFXObj_NISStream *pnis = static_cast<SFXObj_NISStream *>(GetSFXBase_Object(0x40000050));
        pnis->StartNIS();
        SoundPause(true, eSNDPAUSE_NISON);
    }
}

// PlayUISoundFX, StopUISoundFX go here when implemented

void EAXSound::SetCsisName(SndBase *psndbase) {
    char *dest = csCSISdebug;
    const char *fmt = " %s, 0x%x ";
    int ninst = psndbase->GetUniqueID();
    char *name = psndbase->GetTypeName();
    bSPrintf(dest, fmt, name, ninst);
    SetCsisName(dest);
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

void EAXSound::InitEATRAX() {
    UpdateSongInfo();
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
