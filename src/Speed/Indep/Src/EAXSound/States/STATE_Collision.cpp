#include "Speed/Indep/Src/EAXSound/States/STATE_Collision.hpp"

CSTATE_Base::StateInfo CSTATE_Collision::s_StateInfo = {
    0x00070000,
    "CSTATE_Collision",
    &CSTATE_Base::s_StateInfo,
    CSTATE_Collision::CreateState,
};

CSTATE_Collision::StateInfo *CSTATE_Collision::GetStateInfo() const {
    return &s_StateInfo;
}

const char *CSTATE_Collision::GetStateName() const {
    return s_StateInfo.stateName;
}

CSTATE_Base *CSTATE_Collision::CreateState(unsigned int allocator) {
    return new CSTATE_Collision;
}

CSTATE_Collision::CSTATE_Collision()
    : m_pCollisionEvent(nullptr)
{}

CSTATE_Collision::~CSTATE_Collision() {}

void CSTATE_Collision::Attach(void *pAttachment) {
    m_pCollisionEvent = static_cast<Sound::CollisionEvent *>(pAttachment);
    CSTATE_Base::Attach(pAttachment);
}

bool CSTATE_Collision::Detach() {
    CSTATE_Base::Detach();
    if (m_pCollisionEvent) {
        m_pCollisionEvent->SetOwner(nullptr);
        m_pCollisionEvent->Release();
        m_pCollisionEvent = nullptr;
    }
    return true;
}
