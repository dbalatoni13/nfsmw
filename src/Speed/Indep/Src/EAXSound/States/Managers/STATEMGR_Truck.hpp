#ifndef EAXSOUND_STATES_MANAGERS_STATEMGR_TRUCK_H
#define EAXSOUND_STATES_MANAGERS_STATEMGR_TRUCK_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_CarState.hpp"

// total size: 0x24
struct CSTATEMGR_Truck : public CSTATEMGR_CarState {
    CSTATEMGR_Truck();
    ~CSTATEMGR_Truck() override;

    void EnterWorld(eSndGameMode esgm) override;
};

#endif
