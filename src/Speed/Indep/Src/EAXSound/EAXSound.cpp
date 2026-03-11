#include "./EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_NISStream.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_Pathfinder.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_NISReving.hpp"
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
};

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

void EAXSound::StopSND11() {}

void EAXSound::StartSND11() {}
