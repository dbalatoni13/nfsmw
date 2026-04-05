#ifndef EAXSOUND_STATES_MANAGERS_STATEMGR_DRIVEBY_H
#define EAXSOUND_STATES_MANAGERS_STATEMGR_DRIVEBY_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_Base.hpp"

// total size: 0x20
struct CSTATEMGR_DriveBy : public CSTATEMGR_Base {
    int WooshCheckFrameCntr; // offset 0x1C, size 0x4

    CSTATEMGR_DriveBy();
};

#endif
