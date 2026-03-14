#include "Speed/Indep/Src/EAXSound/sfxctl/sfxctl_mastervol.hpp"
#include "Speed/Indep/Libs/Support/Miscellaneous/StringHash.h"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Generated/Messages/MPursuitBreaker.h"

struct MoviePlayer {
    char _pad[0x128];
    int fStatus;
};

extern MoviePlayer *gMoviePlayer;

namespace SndCamera {
extern int m_CurCamState[2];
}

namespace RadarDetector {
extern float mStaticRange;
}

SFXCTL_MasterVol::SFXCTL_MasterVol() {}

SFXCTL_MasterVol::~SFXCTL_MasterVol() {}

SndBase *SFXCTL_MasterVol::CreateObject(unsigned int allocator) {
    SFXCTL_MasterVol *object = reinterpret_cast< SFXCTL_MasterVol * >(
      gAudioMemoryManager.AllocateMemory(sizeof(SFXCTL_MasterVol), SFXCTL_MasterVol::s_TypeInfo.typeName, allocator != 0));
    return new (object) SFXCTL_MasterVol();
}

SndBase::TypeInfo *SFXCTL_MasterVol::GetTypeInfo() const { return &s_TypeInfo; }

char *SFXCTL_MasterVol::GetTypeName() const { return s_TypeInfo.typeName; }

void SFXCTL_MasterVol::InitSFX() { SFXCTL::InitSFX(); }

void SFXCTL_MasterVol::UpdateParams(float t) {
    (void)t;

    int *outputs = GetOutputBlockPtr();
    const char *eaxSound = reinterpret_cast<const char *>(g_pEAXSound);
    const char *audioSettings = *reinterpret_cast<const char *const *>(eaxSound + 0x34);

    if (audioSettings != nullptr) {
        const float masterVol = *reinterpret_cast<const float *>(audioSettings + 0x0);
        const int nVol = static_cast<int>(masterVol * 32767.0f);
        const int curve =
            NFSMixShape::GetCurveOutput(static_cast<NFSMixShape::eMIXTABLEID>(1), nVol, false);
        const float curveScale = static_cast<float>(curve) * 3.051851e-05f;

        outputs[0] = static_cast<int>((1.0f - masterVol * *reinterpret_cast<const float *>(audioSettings + 0x8) * curveScale) * 32767.0f);
        outputs[1] = static_cast<int>((1.0f - masterVol * *reinterpret_cast<const float *>(audioSettings + 0xC) * curveScale) * 32767.0f);
        outputs[2] = static_cast<int>((1.0f - masterVol * *reinterpret_cast<const float *>(audioSettings + 0x4) * curveScale) * 32767.0f);
        outputs[3] = static_cast<int>((1.0f - masterVol * *reinterpret_cast<const float *>(audioSettings + 0x10) * curveScale) * 32767.0f);

        const int carVol =
            static_cast<int>((1.0f - masterVol * *reinterpret_cast<const float *>(audioSettings + 0x18) * curveScale) * 32767.0f);
        outputs[4] = carVol;
        outputs[5] = carVol;
    }

    const int gameMode = *reinterpret_cast<const int *>(eaxSound + 0x84);
    if (gameMode == 1) {
        outputs[6] = g_EAXIsPaused() ? 0x7fff : 0;
    } else {
        outputs[6] = g_EAXIsPaused() ? 0x7fff : 0;
    }

    if (gMoviePlayer != nullptr && gMoviePlayer->fStatus == 5) {
        outputs[7] = 0x7fff;
    } else {
        outputs[7] = 0;
    }

    const int previousGameMode = *reinterpret_cast<const int *>(eaxSound + 0x88);
    if (gameMode == 10) {
        outputs[8] = 0x7fff;
        if (previousGameMode != 10) {
            UCrc32 port(stringhash32("PursuitBreaker"));
            MPursuitBreaker message(true);
            message.Post(port);
        }
    } else {
        outputs[8] = 0;
        if (previousGameMode == 10) {
            UCrc32 port(stringhash32("PursuitBreaker"));
            MPursuitBreaker message(false);
            message.Post(port);
        }
    }

    outputs[9] = SndCamera::m_CurCamState[0] == 3 ? 0x7fff : 0;
    outputs[10] = static_cast<int>(RadarDetector::mStaticRange * 32767.0f);
}

SFXCTL_GameState::SFXCTL_GameState() {}

SndBase *SFXCTL_GameState::CreateObject(unsigned int allocator) {
    SFXCTL_GameState *object = reinterpret_cast< SFXCTL_GameState * >(
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
