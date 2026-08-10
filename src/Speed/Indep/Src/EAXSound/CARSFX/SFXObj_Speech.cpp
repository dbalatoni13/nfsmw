#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_Speech.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/SndCamera.hpp"
#include "Speed/Indep/Src/EAXSound/Stream/SpeechManager.hpp"
#include "Speed/Indep/Src/World/ParameterMaps.hpp"

ParameterAccessor SPAMAccessorSpeech("Speech");

DEFINE_CREATABLE(0x10, SFXObj_Speech, SndBase);

SFXObj_Speech::SFXObj_Speech() : CARSFX() {
    this->m_pActorPos = nullptr;
    this->fPosition = bVector3(0.0f, 0.0f, 0.0f);
    this->bFresh = false;
    this->m_pClosestCar = nullptr;
}

SFXObj_Speech::~SFXObj_Speech() {
    this->Destroy();
    g_pEAXSound->SetSFXBaseObject(nullptr, eMM_MAIN, 1, 0);
}

void SFXObj_Speech::InitSFX() {
    SndBase::InitSFX();
    this->m_pActorPos->AssignPositionVector(&this->fPosition);
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
        this->m_pActorPos = static_cast<SFXCTL_3DObjPos *>(psfxctl);
    }
}

void SFXObj_Speech::UpdateParams(float t) {
    bool is_cellcall_playing = Speech::Manager::IsCopSpeechPlaying(kSPCH1_EventID_CellCall);
    this->SetDMIX_Input(4, is_cellcall_playing ? 0x7FFF : 0);
    this->SetDMIX_Input(0, Speech::Manager::IsPlaying(COPSPEECH_MODULE) && !is_cellcall_playing ? 0x7FFF : 0);

    if (!this->bFresh) {
        if (this->GetDMIX_InputValue(2) != 0) {
            EAX_CarState *closest = GetClosestCopCarToCamera();
            if (closest != nullptr) {
                this->m_pClosestCar = closest;
                this->fPosition = *closest->GetPosition();
                this->bFresh = true;
            } else {
                this->m_pClosestCar = nullptr;
                this->fPosition = *SndCamera::GetWorldCarPos3(0);
            }
        }
    } else {
        if (GetDMIX_InputValue(2) == 0) {
            this->bFresh = false;
            this->m_pClosestCar = nullptr;
        } else if (GetDMIX_InputValue(2) != 0) {
            EAX_CarState *copcar = GetClosestCopCarToCamera();
            if (this->m_pClosestCar == copcar) {
                this->fPosition = *this->m_pClosestCar->GetPosition();
            }
        }
    }
}

DEFINE_CREATABLE(0x30, SFXCTL_3DVoiceActorPos, SFXCTL_3DObjPos);
