#ifndef EAXSOUND_STATES_MANAGERS_STATEMGR_COPCAR_H
#define EAXSOUND_STATES_MANAGERS_STATEMGR_COPCAR_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_CarState.hpp"

// total size: 0x28
class CSTATEMGR_CopCar : public CSTATEMGR_CarState {
  public:
    int mNumCopsInProximity; // offset 0x24, size 0x4

    CSTATEMGR_CopCar();
};

#endif
