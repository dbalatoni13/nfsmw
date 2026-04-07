#ifndef EAXSOUND_STATES_MANAGERS_STATEMGR_TRAFFICCAR_H
#define EAXSOUND_STATES_MANAGERS_STATEMGR_TRAFFICCAR_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_CarState.hpp"

// total size: 0x24
struct CSTATEMGR_TrafficCar : public CSTATEMGR_CarState {
    CSTATEMGR_TrafficCar();
    ~CSTATEMGR_TrafficCar() override;

    void EnterWorld(eSndGameMode esgm) override;
    void UpdateParams(float t) override;

  private:
    void DebugDisplayTrafficConnections();
};

#endif
