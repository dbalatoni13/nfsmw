#ifndef STATEMGR_HELI_HPP
#define STATEMGR_HELI_HPP

#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_Base.hpp"

// total size: 0x1C
// Decl: 8
class CSTATEMGR_Helicopter : public CSTATEMGR_Base {
  public:
    CSTATEMGR_Helicopter();
    ~CSTATEMGR_Helicopter() override;

    // Overrides: CSTATEMGR_Base
    void EnterWorld(eSndGameMode esgm) override;
};

#endif
