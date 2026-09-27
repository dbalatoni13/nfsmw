#include "Speed/Indep/Src/EAXSound/Dynamic_Mixer/NFSMixShape.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_MasterVol.hpp"
#include "Speed/Indep/Src/EAXSound/SndCamera.hpp"
#include "Speed/Indep/Src/Frontend/MoviePlayer/MoviePlayer.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeRadarDetector.hpp"
#include "Speed/Indep/Src/Generated/Messages/MPursuitBreaker.h"
#include "Speed/Indep/Src/Misc/DemoDisc.hpp"

int GameFlowSndState[15];

DEFINE_CREATABLE(0, SFXCTL_MasterVol, SFXCTL);

SFXCTL_MasterVol::SFXCTL_MasterVol() {
    bMemSet(GameFlowSndState, 0, sizeof(GameFlowSndState));
}

SFXCTL_MasterVol::~SFXCTL_MasterVol() {}

void SFXCTL_MasterVol::InitSFX() {
    float fvol = g_pEAXSound->m_pCurAudioSettings->GetMasteredSoundEffectsVol();
    int nvol = static_cast<int>(fvol);
}

int g_IG_Music_Scale = 0x7FFF; // Decl: 58

// UNSOLVED
void SFXCTL_MasterVol::UpdateParams(float t) {
    float fvol;

    if (g_pEAXSound->m_pCurAudioSettings != nullptr) {
        float fMasterVol;
        fMasterVol = g_pEAXSound->m_pCurAudioSettings->MasterVol;

        if (TheDemoDiscManager.IsActive()) {
            fMasterVol *= static_cast<float>(TheDemoDiscManager.GetMasterVolumeScale()) / 32767.0f;
        }

        int nvolindex = static_cast<int>(fMasterVol * 32767.0f);
        int nmastervol = NFSMixShape::GetCurveOutput(SHAPE_UP_EQPWR, nvolindex, false);
        fvol = static_cast<float>(nmastervol) / 32767.0f;

        int nvol = static_cast<int>(
            (1.0f - g_pEAXSound->m_pCurAudioSettings->MasterVol * g_pEAXSound->m_pCurAudioSettings->GetMasteredFEMusicVol() * fvol) * 32767.0f);
        this->SetDMIX_Input(0, nvol);

        nvol = static_cast<int>(
            (1.0f - g_pEAXSound->m_pCurAudioSettings->MasterVol * g_pEAXSound->m_pCurAudioSettings->GetMasteredIGMusicVol() * fvol) * 32767.0f);
        this->SetDMIX_Input(1, nvol);

        nvol = static_cast<int>(
            (1.0f - g_pEAXSound->m_pCurAudioSettings->MasterVol * g_pEAXSound->m_pCurAudioSettings->GetMasteredSpeechVol() * fvol) * 32767.0f);
        this->SetDMIX_Input(2, nvol);

        nvol = static_cast<int>(
            (1.0f - g_pEAXSound->m_pCurAudioSettings->MasterVol * g_pEAXSound->m_pCurAudioSettings->GetMasteredSoundEffectsVol() * fvol) * 32767.0f);
        this->SetDMIX_Input(3, nvol);

        nvol = static_cast<int>((1.0f - g_pEAXSound->m_pCurAudioSettings->MasterVol * g_pEAXSound->m_pCurAudioSettings->GetMasteredCarVol() * fvol) *
                                32767.0f);
        this->SetDMIX_Input(4, nvol);

        nvol = static_cast<int>((1.0f - g_pEAXSound->m_pCurAudioSettings->MasterVol * g_pEAXSound->m_pCurAudioSettings->GetMasteredCarVol() * fvol) *
                                32767.0f);
        this->SetDMIX_Input(5, nvol);
    }

    if (g_pEAXSound->GetSndGameMode() == SND_FRONTEND) {
        if (g_EAXIsPaused()) {
            this->SetDMIX_Input(6, 0x7fff);
        } else {
            this->SetDMIX_Input(6, 0);
        }
    } else {
        if (g_EAXIsPaused()) {
            this->SetDMIX_Input(6, 0x7fff);
        } else {
            this->SetDMIX_Input(6, 0);
        }
    }

    if (gMoviePlayer != nullptr && gMoviePlayer->GetStatus() == 5) {
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

    int RandarRange = static_cast<int>(RadarDetector::mStaticRange * 32767.0f);
    this->SetDMIX_Input(10, RandarRange);
}

DEFINE_CREATABLE(0x10, SFXCTL_GameState, SFXCTL);

void SFXCTL_GameState::UpdateMixerOutputs() {
    for (int i = 0; i < 14; i++) {
        extern uint32 g_ActiveSFXStates; // Decl: 230
        if (g_ActiveSFXStates & (1 << i)) {
            this->SetDMIX_Input(i, 0x7fff);
        } else {
            this->SetDMIX_Input(i, 0);
        }
    }
}
