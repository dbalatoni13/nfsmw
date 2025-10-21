#include "./EAXSOund.hpp"
#include "Speed/Indep/Src/Misc/Config.h"

void EAXSound::START_321Countdown() {
    if (IsSoundEnabled && IsAudioStreamingEnabled) {
        SFX_Base *obj = GetSFXBase_Object(0x40010010);
        if (obj) {
        }
        SFX_Base *nis = GetSFXBase_Object(0x40010050);
        if (nis) {
            // nis->StartNIS();
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
    mData.fEventID = 0xF2D10992;
    mEventID = 0;
}

EAXSound::~EAXSound() {
    // gSpeechCache.Dump();
}

void EAXSound::StopSND11() {}

void EAXSound::StartSND11() {}
