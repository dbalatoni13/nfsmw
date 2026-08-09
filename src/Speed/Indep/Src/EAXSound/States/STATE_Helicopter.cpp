#include "Speed/Indep/Src/EAXSound/States/STATE_Helicopter.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"

STATETYPE_IMPLEMENT(0xB0000, CSTATE_Helicopter, CSTATE_Base);

CSTATE_Helicopter::CSTATE_Helicopter()
    : CSTATE_Base(), //
      m_pHeliState(nullptr) {}

CSTATE_Helicopter::~CSTATE_Helicopter() {}

void CSTATE_Helicopter::UpdateParams(float t) {
    if (!g_EAXIsPaused()) {
        this->t_DeltaTime = t;
        this->t_CurTime += t;
    } else {
        this->t_DeltaTime = 0.0f;
    }

    if (!this->IsAttached() || !static_cast<EAX_HeliState *>(this->m_pAttachment)->IsSimUpdating()) {
        return;
    }

    SndBase *CurSFXCtl = this->m_pHeadSFXCTL;
    while (CurSFXCtl != nullptr) {
        CurSFXCtl->UpdateParams(t);
        CurSFXCtl->UpdateMixerOutputs();
        CurSFXCtl = CurSFXCtl->m_pNextSFX;
    }

    SndBase *CurSFXOBj = this->m_pHeadSFXObj;
    while (CurSFXOBj != nullptr) {
        CurSFXOBj->UpdateParams(t);
        CurSFXOBj->UpdateMixerOutputs();
        CurSFXOBj = CurSFXOBj->m_pNextSFX;
    }
}

void CSTATE_Helicopter::Attach(void *pAttachment) {
    this->m_pHeliState = static_cast<EAX_HeliState *>(pAttachment);
    CSTATE_Base::Attach(pAttachment);
}

bool CSTATE_Helicopter::Detach() {
    if (!CSTATE_Base::Detach()) {
        return false;
    }
    this->m_pHeliState = nullptr;
    return true;
}
