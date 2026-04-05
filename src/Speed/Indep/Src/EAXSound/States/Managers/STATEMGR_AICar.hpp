#ifndef EAXSOUND_STATES_MANAGERS_STATEMGR_AICAR_H
#define EAXSOUND_STATES_MANAGERS_STATEMGR_AICAR_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_CarState.hpp"

// total size: 0x24
class CSTATEMGR_AICar : public CSTATEMGR_CarState {
  public:
    CSTATEMGR_AICar();
    static void QueueSlots();
};

#endif
