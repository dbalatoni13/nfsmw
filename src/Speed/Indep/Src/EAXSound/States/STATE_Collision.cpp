#include "Speed/Indep/Src/EAXSound/States/STATE_Collision.hpp"

STATETYPE_IMPLEMENT(0x70000, CSTATE_Collision, CSTATE_Base);

CSTATE_Collision::CSTATE_Collision() : m_pCollisionEvent(nullptr) {}

CSTATE_Collision::~CSTATE_Collision() {
    if (this->m_pCollisionEvent != nullptr) {
        this->m_pCollisionEvent->SetOwner(nullptr);
        this->m_pCollisionEvent->Release();
        this->m_pCollisionEvent = nullptr;
    }
}

void CSTATE_Collision::Attach(void *pAttachment) {
    this->m_pCollisionEvent = static_cast<Sound::CollisionEvent *>(pAttachment);
    CSTATE_Base::Attach(pAttachment);
}

bool CSTATE_Collision::Detach() {
    CSTATE_Base::Detach();
    if (this->m_pCollisionEvent != nullptr) {
        this->m_pCollisionEvent->SetOwner(nullptr);
        this->m_pCollisionEvent->Release();
        this->m_pCollisionEvent = nullptr;
    }
    return true;
}
