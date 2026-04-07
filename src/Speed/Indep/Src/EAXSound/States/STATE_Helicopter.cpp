 #include "Speed/Indep/Src/EAXSound/States/STATE_Helicopter.hpp"

 #include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"

CSTATE_Helicopter::StateInfo CSTATE_Helicopter::s_StateInfo = {
    0x000B0000,
    "CSTATE_Helicopter",
    &CSTATE_Base::s_StateInfo,
    CSTATE_Helicopter::CreateState,
};

CSTATE_Helicopter::StateInfo *CSTATE_Helicopter::GetStateInfo() const {
    return &s_StateInfo;
}

const char *CSTATE_Helicopter::GetStateName() const {
    return s_StateInfo.stateName;
}

CSTATE_Base *CSTATE_Helicopter::CreateState(unsigned int allocator) {
    if (allocator == 0) {
        return new (AudioMemBase::operator new(sizeof(CSTATE_Helicopter), GetStaticStateInfo()->stateName, false))
            CSTATE_Helicopter;
    }
    return new (AudioMemBase::operator new(sizeof(CSTATE_Helicopter), GetStaticStateInfo()->stateName, true))
        CSTATE_Helicopter;
}

CSTATE_Helicopter::CSTATE_Helicopter()
    : CSTATE_Base()
    , m_pHeliState(nullptr)
{}

CSTATE_Helicopter::~CSTATE_Helicopter() {}

void CSTATE_Helicopter::UpdateParams(float t) {
    SndBase *CurSFXCtl;
    SndBase *CurSFXOBj;

    if (g_EAXIsPaused()) {
        t_DeltaTime = 0.0f;
    } else {
        t_DeltaTime = t;
        t_CurTime += t;
    }

    if (IsAttached() && m_pHeliState->IsSimUpdating()) {
        CurSFXCtl = m_pHeadSFXCTL;
        while (CurSFXCtl) {
            CurSFXCtl->UpdateParams(t);
            CurSFXCtl->UpdateMixerOutputs();
            CurSFXCtl = CurSFXCtl->m_pNextSFX;
        }

        CurSFXOBj = m_pHeadSFXObj;
        while (CurSFXOBj) {
            CurSFXOBj->UpdateParams(t);
            CurSFXOBj->UpdateMixerOutputs();
            CurSFXOBj = CurSFXOBj->m_pNextSFX;
        }
    }
}

void CSTATE_Helicopter::Attach(void *pAttachment) {
    m_pHeliState = static_cast<EAX_HeliState *>(pAttachment);
    CSTATE_Base::Attach(pAttachment);
}

bool CSTATE_Helicopter::Detach() {
    bool bVar1 = CSTATE_Base::Detach();
    if (bVar1) {
        m_pHeliState = nullptr;
    }
    return bVar1;
}
