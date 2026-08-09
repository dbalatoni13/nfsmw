#include "./EAXSOund.hpp"
#include "Speed/Indep/Src/Generated/Events/EAudioWorldTest.hpp"
#include "Speed/Indep/Src/Misc/Config.h"

// TODO look at dwarf
void EAXSound::START_321Countdown() {
    if (IsSoundEnabled != 0 && IsAudioStreamingEnabled != 0) {
        // TODO magic
        SFX_Base *obj = GetSFXBase_Object(0x40010010);
        if (obj != nullptr) {
        }
        SFX_Base *nis = GetSFXBase_Object(0x40010050);
        if (nis != nullptr) {
            // nis->StartNIS();
        }
    }
}

EAXSound::EAXSound() {
    this->m_pcsCsisName = "SOUND";
    this->bPlayCarSounds = true;
    this->mAttributes = nullptr;
    this->mLocalAttr = nullptr;
    this->m_pEAXSND8Wrapper = nullptr;
    this->m_bAudioIsPaused = false;
    this->m_X360_UI_Override = false;
    this->m_eSndGameMode = SND_MODE_NONE;
    this->m_prevSndGameMode = SND_MODE_NONE;
    this->m_pStreamManager = nullptr;
    this->m_nStereoUpgradeLevel = 0;
    this->m_bIsPaused = false;
    this->m_pCurAudioSettings = nullptr;
    this->m_nSpeechLoadBankIndex = 0;
    this->m_ePlayerMixMode = EAXS3D_SINGLE_PLAYER_MIX;
    this->m_pNewSongInfoSt = nullptr;
    this->EngineLoadingBlocked = false;
    this->FrameCnt = 0;
    this->m_bIsSpecialUGMovie = false;
    this->m_pCmnSnd = nullptr;
    this->m_pFESnd = nullptr;
    this->m_pSTICH_Playback = nullptr;
    this->m_pNFSLiveLink = nullptr;
    this->m_pNFSMixMaster = nullptr;
    this->bPlayCameraSnapShot = false;
    this->mmsgMRestartRace = nullptr;
    this->m_bPause_MainFNG = false;
    this->mEventID = 0;
    this->mData.fEventID = EAudioWorldTest::kEventID;
    int nloop = 0;
}

EAXSound::~EAXSound() {
    // gSpeechCache.Dump();
}

void EAXSound::StopSND11() {}

void EAXSound::StartSND11() {}
