#include "Speed/Indep/Src/EAXSound/sfxctl/sfxctl_mastervol.hpp"
#include "Speed/Indep/Libs/Support/Miscellaneous/StringHash.h"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Generated/Messages/MPursuitBreaker.h"

struct MoviePlayer {
    char _pad[0x128];
    int fStatus;

    int GetStatus() { return fStatus; }
};

extern MoviePlayer *gMoviePlayer;
extern unsigned int g_ActiveSFXStates;
extern int GameFlowSndState[];

namespace RadarDetector {
extern float mStaticRange;
}

SFXCTL_MasterVol::SFXCTL_MasterVol() {
    bMemSet(GameFlowSndState, '\0', 0x3C);
}

SFXCTL_MasterVol::~SFXCTL_MasterVol() {}

SndBase *SFXCTL_MasterVol::CreateObject(unsigned int allocator) {
    if (allocator == 0) {
        return new (SFXCTL_MasterVol::s_TypeInfo.typeName, false) SFXCTL_MasterVol();
    }
    return new (SFXCTL_MasterVol::s_TypeInfo.typeName, true) SFXCTL_MasterVol();
}

SndBase::TypeInfo *SFXCTL_MasterVol::GetTypeInfo() const { return &s_TypeInfo; }

char *SFXCTL_MasterVol::GetTypeName() const { return s_TypeInfo.typeName; }

void SFXCTL_MasterVol::InitSFX() {}

void SFXCTL_MasterVol::UpdateParams(float t) {
    (void)t;
    if (g_pEAXSound->GetCurrentAudioSettings() != nullptr) {
        int nvolindex = static_cast<int>(g_pEAXSound->GetCurrentAudioSettings()->MasterVol * 32767.0f);
        int nmastervol = NFSMixShape::GetCurveOutput(static_cast<NFSMixShape::eMIXTABLEID>(1), nvolindex, false);
        float fvol = static_cast<float>(nmastervol) * 3.051851e-05f;
        int nvol;

        SetDMIX_Input(0, static_cast<int>((1.0f - g_pEAXSound->GetCurrentAudioSettings()->MasterVol *
                                                   g_pEAXSound->GetCurrentAudioSettings()->FEMusicVol * fvol) *
                                          32767.0f));
        SetDMIX_Input(1, static_cast<int>((1.0f - g_pEAXSound->GetCurrentAudioSettings()->MasterVol *
                                                   g_pEAXSound->GetCurrentAudioSettings()->IGMusicVol * fvol) *
                                          32767.0f));
        SetDMIX_Input(2, static_cast<int>((1.0f - g_pEAXSound->GetCurrentAudioSettings()->MasterVol *
                                                   g_pEAXSound->GetCurrentAudioSettings()->SpeechVol * fvol) *
                                          32767.0f));
        SetDMIX_Input(3, static_cast<int>((1.0f - g_pEAXSound->GetCurrentAudioSettings()->MasterVol *
                                                   g_pEAXSound->GetCurrentAudioSettings()->SoundEffectsVol * fvol) *
                                          32767.0f));
        SetDMIX_Input(4, static_cast<int>((1.0f - g_pEAXSound->GetCurrentAudioSettings()->MasterVol *
                                                   g_pEAXSound->GetCurrentAudioSettings()->CarVol * fvol) *
                                          32767.0f));
        nvol = static_cast<int>((1.0f - g_pEAXSound->GetCurrentAudioSettings()->MasterVol *
                                          g_pEAXSound->GetCurrentAudioSettings()->CarVol * fvol) *
                                32767.0f);
        SetDMIX_Input(5, nvol);
    }

    if (g_pEAXSound->GetSndGameMode() == SND_FRONTEND) {
        if (g_EAXIsPaused()) {
            SetDMIX_Input(6, 0x7fff);
        } else {
            SetDMIX_Input(6, 0);
        }
    } else {
        int nvol = g_EAXIsPaused();
        if (nvol != 0) {
            SetDMIX_Input(6, 0x7fff);
        } else {
            SetDMIX_Input(6, nvol);
        }
    }

    if (gMoviePlayer != nullptr && gMoviePlayer->GetStatus() == 5) {
        SetDMIX_Input(7, 0x7fff);
    } else {
        SetDMIX_Input(7, 0);
    }

    if (g_pEAXSound->GetSndGameMode() == SND_PURSUITBREAKER) {
        SetDMIX_Input(8, 0x7fff);
        if (g_pEAXSound->GetPrevSndGameMode() != SND_PURSUITBREAKER) {
            UCrc32 port("PursuitBreaker");
            MPursuitBreaker message(true);
            message.Post(port);
        }
    } else {
        SetDMIX_Input(8, 0);
        if (g_pEAXSound->GetPrevSndGameMode() == SND_PURSUITBREAKER) {
            UCrc32 port("PursuitBreaker");
            MPursuitBreaker message(false);
            message.Post(port);
        }
    }

    if (SndCamera::GetCurCamState(0) == DMIX_NFS_JUMP_CAM) {
        SetDMIX_Input(9, 0x7fff);
    } else {
        SetDMIX_Input(9, 0);
    }
    SetDMIX_Input(10, static_cast<int>(RadarDetector::mStaticRange * 32767.0f));
}

SndBase *SFXCTL_GameState::CreateObject(unsigned int allocator) {
    if (allocator == 0) {
        return new (SFXCTL_GameState::GetStaticTypeInfo()->typeName, false) SFXCTL_GameState();
    }
    return new (SFXCTL_GameState::GetStaticTypeInfo()->typeName, true) SFXCTL_GameState();
}

void SFXCTL_GameState::UpdateMixerOutputs() {
    for (int i = 0; i <= 13; i++) {
        if (g_ActiveSFXStates & (1 << i)) {
            SetDMIX_Input(i, 0x7fff);
        } else {
            SetDMIX_Input(i, 0);
        }
    }
}

SndBase::TypeInfo *SFXCTL_GameState::GetTypeInfo() const { return &s_TypeInfo; }

char *SFXCTL_GameState::GetTypeName() const { return s_TypeInfo.typeName; }
