#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_Speech.hpp"

#include "Speed/Indep/Src/EAXSound/EAXCarState.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/SndCamera.hpp"
#include "Speed/Indep/Src/EAXSound/SoundCollision.hpp"
#include "Speed/Indep/Src/EAXSound/Stream/SpeechManager.hpp"

SndBase::TypeInfo *SFXObj_Speech::GetTypeInfo() const {
    return &s_TypeInfo;
}

const char *SFXObj_Speech::GetTypeName() const {
    return s_TypeInfo.typeName;
}

SndBase *SFXObj_Speech::CreateObject(unsigned int allocator) {
    if (allocator == 0) {
        return new (GetStaticTypeInfo()->typeName, false) SFXObj_Speech();
    }
    return new (GetStaticTypeInfo()->typeName, true) SFXObj_Speech();
}

SFXObj_Speech::SFXObj_Speech()
    : CARSFX() {
    fPosition = bVector3(0.0f, 0.0f, 0.0f);
    m_pClosestCar = nullptr;
    m_pActorPos = nullptr;
    *reinterpret_cast<int *>(&bFresh) = 0;
}

SFXObj_Speech::~SFXObj_Speech() {
    Destroy();
    g_pEAXSound->SetSFXBaseObject(nullptr, eMM_MAIN, 1, 0);
}

void SFXObj_Speech::InitSFX() {
    SndBase::InitSFX();
    m_pActorPos->AssignPositionVector(&fPosition);
    g_pEAXSound->SetSFXBaseObject(this, eMM_MAIN, 1, 0);
}

void SFXObj_Speech::Destroy() {}

int SFXObj_Speech::GetController(int Index) {
    if (Index == 0) {
        return 3;
    }
    return -1;
}

void SFXObj_Speech::AttachController(SFXCTL *psfxctl) {
    if (psfxctl->GetObjectIndex() == 3) {
        m_pActorPos = static_cast<SFXCTL_3DObjPos *>(psfxctl);
    }
}

void SFXObj_Speech::UpdateParams(float t) {
    bool is_cellcall_playing;

    is_cellcall_playing = Speech::Manager::IsCopSpeechPlaying(kSPCH1_EventID_CellCall);
    SetDMIX_Input(4, is_cellcall_playing ? 0x7FFF : 0);
    SetDMIX_Input(0, Speech::Manager::IsPlaying(COPSPEECH_MODULE) && !is_cellcall_playing ? 0x7FFF : 0);

    if (*reinterpret_cast<int *>(&bFresh) == 0) {
        if (GetDMIX_InputValue(2) != 0) {
            EAX_CarState *closest;

            closest = GetClosestCopCarToCamera();
            if (closest) {
                m_pClosestCar = closest;
                fPosition = *closest->GetPosition();
                *reinterpret_cast<int *>(&bFresh) = 1;
            } else {
                m_pClosestCar = nullptr;
                fPosition = *SndCamera::GetWorldCarPos3(0);
            }
        }
    } else if (GetDMIX_InputValue(2) == 0) {
        m_pClosestCar = nullptr;
        *reinterpret_cast<int *>(&bFresh) = 0;
    } else {
        EAX_CarState *copcar;

        copcar = GetClosestCopCarToCamera();
        if (m_pClosestCar == copcar) {
            fPosition = *m_pClosestCar->GetPosition();
        }
    }
}

SndBase::TypeInfo *SFXCTL_3DVoiceActorPos::GetTypeInfo() const {
    return &s_TypeInfo;
}

const char *SFXCTL_3DVoiceActorPos::GetTypeName() const {
    return s_TypeInfo.typeName;
}

SndBase *SFXCTL_3DVoiceActorPos::CreateObject(unsigned int allocator) {
    if (allocator == 0) {
        return new (GetStaticTypeInfo()->typeName, false) SFXCTL_3DVoiceActorPos();
    }
    return new (GetStaticTypeInfo()->typeName, true) SFXCTL_3DVoiceActorPos();
}

void SFXObj_Speech::ProcessUpdate() {}

SFXCTL_3DVoiceActorPos::~SFXCTL_3DVoiceActorPos() {}
bVector3 v3NULL(0.0f, 0.0f, 0.0f);

SndBase::TypeInfo SFXObj_Speech::s_TypeInfo = {
    0x00000010,
    "SFXObj_Speech",
    &SndBase::s_TypeInfo,
    SFXObj_Speech::CreateObject,
};

SndBase::TypeInfo SFXCTL_3DVoiceActorPos::s_TypeInfo = {
    0x00000030,
    "SFXCTL_3DVoiceActorPos",
    &SFXCTL_3DObjPos::s_TypeInfo,
    SFXCTL_3DVoiceActorPos::CreateObject,
};

