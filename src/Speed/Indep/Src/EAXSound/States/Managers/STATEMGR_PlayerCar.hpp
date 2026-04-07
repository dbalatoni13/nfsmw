#ifndef EAXSOUND_STATES_MANAGERS_STATEMGR_PLAYERCAR_H
#define EAXSOUND_STATES_MANAGERS_STATEMGR_PLAYERCAR_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_Base.hpp"

// total size: 0x1C
struct CSTATEMGR_PlayerCar : public CSTATEMGR_Base {
    static bool IsTruck;

    CSTATEMGR_PlayerCar();
    ~CSTATEMGR_PlayerCar() override;

    void EnterWorld(eSndGameMode esgm) override;
    void UpdateParams(float t) override;
};

#endif
