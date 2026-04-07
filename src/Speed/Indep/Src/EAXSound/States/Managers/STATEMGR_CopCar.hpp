#ifndef EAXSOUND_STATES_MANAGERS_STATEMGR_COPCAR_H
#define EAXSOUND_STATES_MANAGERS_STATEMGR_COPCAR_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_CarState.hpp"

// total size: 0x28
struct CSTATEMGR_CopCar : public CSTATEMGR_CarState {
  private:
    int mNumCopsInProximity; // offset 0x24, size 0x4

  public:
    CSTATEMGR_CopCar();
    ~CSTATEMGR_CopCar() override;

    void UpdateParams(float t) override;
    void EnterWorld(eSndGameMode esgm) override;

    int GetNumCloseCops() {
        return mNumCopsInProximity;
    }
};

#endif
