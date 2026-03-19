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

namespace RadarDetector {
extern float mStaticRange;
}

SFXCTL_MasterVol::SFXCTL_MasterVol() {}

SFXCTL_MasterVol::~SFXCTL_MasterVol() {}

SndBase *SFXCTL_MasterVol::CreateObject(unsigned int allocator) {
    SFXCTL_MasterVol *object = static_cast<SFXCTL_MasterVol *>(
      gAudioMemoryManager.AllocateMemory(sizeof(SFXCTL_MasterVol), SFXCTL_MasterVol::s_TypeInfo.typeName, allocator != 0));
    return new (object) SFXCTL_MasterVol();
}

SndBase::TypeInfo *SFXCTL_MasterVol::GetTypeInfo() const { return &s_TypeInfo; }

char *SFXCTL_MasterVol::GetTypeName() const { return s_TypeInfo.typeName; }

void SFXCTL_MasterVol::InitSFX() { SFXCTL::InitSFX(); }

void SFXCTL_MasterVol::UpdateParams(float t) {
    (void)t;

    const char *eaxSound = static_cast<const char *>(static_cast<const void *>(g_pEAXSound));
    const AudioSettings *audioSettings = *static_cast<const AudioSettings *const *>(static_cast<const void *>(eaxSound + 0x34));

    if (audioSettings != nullptr) {
        float fMasterVol = audioSettings->MasterVol;
        int nvolindex = static_cast<int>(fMasterVol * 32767.0f);
        int nmastervol = NFSMixShape::GetCurveOutput(NFSMixShape::SHAPE_UP_EQPWR_SQ, nvolindex, false);
        float fvol = static_cast<float>(nmastervol) * 3.051851e-05f;
        int nvol;

        SetDMIX_Input(0, static_cast<int>((1.0f - audioSettings->GetMasteredFEMusicVol() * fvol) * 32767.0f));
        SetDMIX_Input(1, static_cast<int>((1.0f - audioSettings->GetMasteredIGMusicVol() * fvol) * 32767.0f));
        SetDMIX_Input(2, static_cast<int>((1.0f - audioSettings->GetMasteredSpeechVol() * fvol) * 32767.0f));
        SetDMIX_Input(3, static_cast<int>((1.0f - audioSettings->GetMasteredSoundEffectsVol() * fvol) * 32767.0f));
        SetDMIX_Input(4, static_cast<int>((1.0f - audioSettings->GetMasteredCarVol() * fvol) * 32767.0f));
        nvol = static_cast<int>((1.0f - audioSettings->GetMasteredCarVol() * fvol) * 32767.0f);
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

SFXCTL_GameState::SFXCTL_GameState() {}

SndBase *SFXCTL_GameState::CreateObject(unsigned int allocator) {
    SFXCTL_GameState *object = static_cast<SFXCTL_GameState *>(
      gAudioMemoryManager.AllocateMemory(sizeof(SFXCTL_GameState), SFXCTL_GameState::s_TypeInfo.typeName, allocator != 0));
    return new (object) SFXCTL_GameState();
}

void SFXCTL_GameState::UpdateMixerOutputs() {
    SetDMIX_Input(0, 0);
    SetDMIX_Input(1, 0);
    SetDMIX_Input(2, 0);
    SetDMIX_Input(3, 0);
    SetDMIX_Input(4, 0);
    SetDMIX_Input(5, 0);
    SetDMIX_Input(6, 0);
}

SndBase::TypeInfo *SFXCTL_GameState::GetTypeInfo() const { return &s_TypeInfo; }

char *SFXCTL_GameState::GetTypeName() const { return s_TypeInfo.typeName; }
