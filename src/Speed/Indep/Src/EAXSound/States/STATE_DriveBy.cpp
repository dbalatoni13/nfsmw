#include "Speed/Indep/Src/EAXSound/States/STATE_DriveBy.hpp"

CSTATE_Base::StateInfo CSTATE_DriveBy::s_StateInfo = {
    0x00080000,
    "CSTATE_DriveBy",
    &CSTATE_Base::s_StateInfo,
    CSTATE_DriveBy::CreateState,
};

CSTATE_DriveBy::StateInfo *CSTATE_DriveBy::GetStateInfo() const {
    return &s_StateInfo;
}

const char *CSTATE_DriveBy::GetStateName() const {
    return s_StateInfo.stateName;
}

CSTATE_Base *CSTATE_DriveBy::CreateState(unsigned int allocator) {
    if (allocator == 0) {
        return new (AudioMemBase::operator new(sizeof(CSTATE_DriveBy), GetStaticStateInfo()->stateName, false))
            CSTATE_DriveBy;
    }
    return new (AudioMemBase::operator new(sizeof(CSTATE_DriveBy), GetStaticStateInfo()->stateName, true))
        CSTATE_DriveBy;
}

CSTATE_DriveBy::CSTATE_DriveBy()
    : CSTATE_Base() {
    m_DriveByInfo.eDriveByType = DRIVE_BY_LAMPPOST;
    m_DriveByInfo.pEAXCar = nullptr;
    m_DriveByInfo.ClosingVelocity = 0.0f;
    m_DriveByInfo.UniqueID = 0;
}

CSTATE_DriveBy::~CSTATE_DriveBy() {}

void CSTATE_DriveBy::Attach(void *pAttachment) {
    m_DriveByInfo = *static_cast<stDriveByInfo *>(pAttachment);
    CSTATE_Base::Attach(pAttachment);
}

bool CSTATE_DriveBy::Detach() {
    return CSTATE_Base::Detach();
}
