#include "Speed/Indep/Src/EAXSound/States/STATE_DriveBy.hpp"

STATETYPE_IMPLEMENT(0x80000, CSTATE_DriveBy, CSTATE_Base);

static const int DEBUG_DRIVE_BY_WOOSH = 0; // size: 0x4, Decl: 5

CSTATE_DriveBy::CSTATE_DriveBy() : CSTATE_Base() {}

CSTATE_DriveBy::~CSTATE_DriveBy() {}

void CSTATE_DriveBy::Attach(void *pAttachment) {
    this->m_DriveByInfo = *static_cast<stDriveByInfo *>(pAttachment);
    CSTATE_Base::Attach(pAttachment);
}

bool CSTATE_DriveBy::Detach() {
    if (!CSTATE_Base::Detach()) {
        return false;
    }
    return true;
}
