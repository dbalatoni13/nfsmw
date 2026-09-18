#include "Speed/Indep/Src/EAXSound/sfxctl/sfxctl_mastervol.hpp"
#include "Speed/Indep/Libs/Support/Miscellaneous/StringHash.h"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSndUtil.h"
#include "Speed/Indep/Src/EAXSound/SndCamera.hpp"
#include "Speed/Indep/Src/EAXSound/Dynamic_Mixer/NFSMixShape.hpp"
#include "Speed/Indep/Src/Frontend/MoviePlayer/MoviePlayer.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Generated/Messages/MPursuitBreaker.h"

extern MoviePlayer *gMoviePlayer;
extern unsigned int g_ActiveSFXStates;
extern int GameFlowSndState[];

namespace RadarDetector {
extern float mStaticRange;
}

SndBase::TypeInfo SFXCTL_MasterVol::s_TypeInfo = {(int)0x00000000, "SFXCTL_MasterVol", &SFXCTL::s_TypeInfo, SFXCTL_MasterVol::CreateObject};

SndBase::TypeInfo *SFXCTL_MasterVol::GetTypeInfo() const { return &s_TypeInfo; }

const char *SFXCTL_MasterVol::GetTypeName() const { return s_TypeInfo.typeName; }

SndBase *SFXCTL_MasterVol::CreateObject(unsigned int allocator) {
    if (allocator == 0) {
        return new (SFXCTL_MasterVol::s_TypeInfo.typeName, false) SFXCTL_MasterVol();
    }
    return new (SFXCTL_MasterVol::s_TypeInfo.typeName, true) SFXCTL_MasterVol();
}

SFXCTL_MasterVol::SFXCTL_MasterVol() {
    bMemSet(GameFlowSndState, '\0', 0x3C);
}

SFXCTL_MasterVol::~SFXCTL_MasterVol() {}

void SFXCTL_MasterVol::InitSFX() {
    float fvol;
    int nvol;

    if (false) {
        fvol = g_pEAXSound->m_pCurAudioSettings->GetMasteredSoundEffectsVol();
        nvol = static_cast<int>(fvol);
    }
}

void SFXCTL_MasterVol::UpdateParams(float t) {
    float fvol;
    int RandarRange;

    (void)t;
    if (g_pEAXSound->GetCurAudioSettings()) {
        float fMasterVol;
        int nvolindex;
        int nmastervol;
        int nvol;

        fMasterVol = g_pEAXSound->GetCurAudioSettings()->MasterVol;
        nvolindex = static_cast<int>(fMasterVol * 32767.0f);
        nmastervol = NFSMixShape::GetCurveOutput(static_cast<eMIXTABLEID>(1), nvolindex, false);
        fMasterVol = static_cast<float>(nmastervol) * 3.051851e-05f;

        fvol = 1.0f - g_pEAXSound->GetCurAudioSettings()->GetMasteredFEMusicVol() * fMasterVol;
        nvol = static_cast<int>(fvol * 32767.0f);
        this->SetDMIX_Input(0, nvol);
        fvol = 1.0f - g_pEAXSound->GetCurAudioSettings()->GetMasteredIGMusicVol() * fMasterVol;
        nvol = static_cast<int>(fvol * 32767.0f);
        this->SetDMIX_Input(1, nvol);
        fvol = 1.0f - g_pEAXSound->GetCurAudioSettings()->GetMasteredSpeechVol() * fMasterVol;
        nvol = static_cast<int>(fvol * 32767.0f);
        this->SetDMIX_Input(2, nvol);
        fvol = 1.0f - g_pEAXSound->GetCurAudioSettings()->GetMasteredSoundEffectsVol() * fMasterVol;
        nvol = static_cast<int>(fvol * 32767.0f);
        this->SetDMIX_Input(3, nvol);
        fvol = 1.0f - g_pEAXSound->GetCurAudioSettings()->GetMasteredCarVol() * fMasterVol;
        nvol = static_cast<int>(fvol * 32767.0f);
        this->SetDMIX_Input(4, nvol);
        fvol = 1.0f - g_pEAXSound->GetCurAudioSettings()->GetMasteredCarVol() * fMasterVol;
        nvol = static_cast<int>(fvol * 32767.0f);
        this->SetDMIX_Input(5, nvol);
    }

    if (g_pEAXSound->GetSndGameMode() == SND_FRONTEND) {
        if (g_EAXIsPaused()) {
            this->SetDMIX_Input(6, 0x7fff);
        } else {
            this->SetDMIX_Input(6, 0);
        }
    } else {
        RandarRange = g_EAXIsPaused();
        if (RandarRange != 0) {
            this->SetDMIX_Input(6, 0x7fff);
        } else {
            this->SetDMIX_Input(6, RandarRange);
        }
    }

    if (gMoviePlayer && gMoviePlayer->GetStatus() == 5) {
        this->SetDMIX_Input(7, 0x7fff);
    } else {
        this->SetDMIX_Input(7, 0);
    }

    if (g_pEAXSound->GetSndGameMode() == SND_PURSUITBREAKER) {
        this->SetDMIX_Input(8, 0x7fff);
        if (g_pEAXSound->GetPrevSndGameMode() != SND_PURSUITBREAKER) {
            MPursuitBreaker(true).Post(UCrc32("PursuitBreaker"));
        }
    } else {
        this->SetDMIX_Input(8, 0);
        if (g_pEAXSound->GetPrevSndGameMode() == SND_PURSUITBREAKER) {
            MPursuitBreaker(false).Post(UCrc32("PursuitBreaker"));
        }
    }

    if (SndCamera::GetCurCamState(0) == DMIX_NFS_JUMP_CAM) {
        this->SetDMIX_Input(9, 0x7fff);
    } else {
        this->SetDMIX_Input(9, 0);
    }
    this->SetDMIX_Input(10, static_cast<int>(RadarDetector::mStaticRange * 32767.0f));
}

SndBase::TypeInfo SFXCTL_GameState::s_TypeInfo = {(int)0x00000010, "SFXCTL_GameState", &SFXCTL::s_TypeInfo, SFXCTL_GameState::CreateObject};

SndBase::TypeInfo *SFXCTL_GameState::GetTypeInfo() const { return &s_TypeInfo; }

const char *SFXCTL_GameState::GetTypeName() const { return s_TypeInfo.typeName; }

SndBase *SFXCTL_GameState::CreateObject(unsigned int allocator) {
    if (allocator == 0) {
        return new (SFXCTL_GameState::s_TypeInfo.typeName, false) SFXCTL_GameState();
    }
    return new (SFXCTL_GameState::s_TypeInfo.typeName, true) SFXCTL_GameState();
}

void SFXCTL_GameState::UpdateMixerOutputs() {
    for (int i = 0; i <= 13; i++) {
        if (g_ActiveSFXStates & (1 << i)) {
            this->SetDMIX_Input(i, 0x7fff);
        } else {
            this->SetDMIX_Input(i, 0);
        }
    }
}
