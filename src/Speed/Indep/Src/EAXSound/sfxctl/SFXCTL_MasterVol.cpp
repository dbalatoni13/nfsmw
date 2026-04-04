#include "Speed/Indep/Src/EAXSound/sfxctl/sfxctl_mastervol.hpp"
#include "Speed/Indep/Libs/Support/Miscellaneous/StringHash.h"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Generated/Messages/MPursuitBreaker.h"

class AV_PLAYER;
class FRAME;

class MoviePlayer {
  public:
    int GetStatus() { return fStatus; }

  private:
    char mSettings[0x124];        // offset 0x0, size 0x124 (Dwarf: MoviePlayer::Settings)
    unsigned int fCurFrameNum;    // offset 0x124, size 0x4
    int fStatus;                  // offset 0x128, size 0x4
    int fLiveStatus;              // offset 0x12C, size 0x4
    unsigned int mTicker;         // offset 0x130, size 0x4
    bool mTickerFirstTime;        // offset 0x134, size 0x1
    bool mMoviePaused;            // offset 0x138, size 0x1
    int mili_seconds;             // offset 0x13C, size 0x4
    int seconds;                  // offset 0x140, size 0x4
    int minutes;                  // offset 0x144, size 0x4
    float milliseconds;           // offset 0x148, size 0x4
    float prevMilliseconds;       // offset 0x14C, size 0x4
    AV_PLAYER *fPlayer;           // offset 0x150, size 0x4
    FRAME *CurFrame;              // offset 0x154, size 0x4
};

extern MoviePlayer *gMoviePlayer;
extern unsigned int g_ActiveSFXStates;
extern int GameFlowSndState[];

namespace RadarDetector {
extern float mStaticRange;
}

SndBase::TypeInfo *SFXCTL_MasterVol::GetTypeInfo() const { return &s_TypeInfo; }

const char *SFXCTL_MasterVol::GetTypeName() const { return s_TypeInfo.typeName; }

SndBase *SFXCTL_MasterVol::CreateObject(unsigned int allocator) {
    if (allocator == 0) {
        return new (SFXCTL_MasterVol::GetStaticTypeInfo()->typeName, false) SFXCTL_MasterVol();
    }
    return new (SFXCTL_MasterVol::GetStaticTypeInfo()->typeName, true) SFXCTL_MasterVol();
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

SndBase::TypeInfo *SFXCTL_GameState::GetTypeInfo() const { return &s_TypeInfo; }

const char *SFXCTL_GameState::GetTypeName() const { return s_TypeInfo.typeName; }

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
