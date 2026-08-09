#ifndef STATEMGR_COPCAR_HPP
#define STATEMGR_COPCAR_HPP // Decl: 2

#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_CarState.hpp"

#define MAX_NUM_SND_COP_CARS 4 // Decl: 12

// total size: 0x28
// Decl: 16
struct CSTATEMGR_CopCar : public CSTATEMGR_CarState {
    CSTATEMGR_CopCar();
    ~CSTATEMGR_CopCar() override;

    // Overrides: CSTATEMGR_Base
    void UpdateParams(float t) override;
    void EnterWorld(eSndGameMode esgm) override;

    int GetNumCloseCops() {} // Decl: 24

  private:
    int mNumCopsInProximity; // offset 0x24, size 0x4, Decl: 28
};

#endif
