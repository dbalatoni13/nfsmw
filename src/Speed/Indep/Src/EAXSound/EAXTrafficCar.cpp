#include "Speed/Indep/Src/EAXSound/EAXTrafficCar.hpp"
#include "Speed/Indep/Src/EAXSound/States/STATE_Base.hpp"

STATETYPE_IMPLEMENT(0x50000, EAXTrafficCar, CSTATE_Base);

EAXTrafficCar::EAXTrafficCar() : CSTATE_Base() {}

EAXTrafficCar::~EAXTrafficCar() {}

void EAXTrafficCar::Attach(void *pAttachment) {
    this->IsLargeTrafficCar = false;
    this->m_pCar = static_cast<EAX_CarState *>(pAttachment);
    CSTATE_Base::Attach(pAttachment);
}

bool EAXTrafficCar::Detach() {
    CSTATE_Base::Detach();
    this->m_pCar = nullptr;
    return true;
}
