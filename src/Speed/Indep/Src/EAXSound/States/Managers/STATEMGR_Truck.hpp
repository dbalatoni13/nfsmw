#ifndef STATEMGR_TRUCK_HPP
#define STATEMGR_TRUCK_HPP

#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_CarState.hpp"

// total size: 0x24
// Decl: 14
class CSTATEMGR_Truck : public CSTATEMGR_CarState {
  public:
    CSTATEMGR_Truck();
    ~CSTATEMGR_Truck() override;

    // Overrides: CSTATEMGR_Base
    void EnterWorld(eSndGameMode esgm) override;
};

#endif
