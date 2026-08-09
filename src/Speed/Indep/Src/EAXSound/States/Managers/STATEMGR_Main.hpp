#ifndef STATEMGR_MAIN_HPP
#define STATEMGR_MAIN_HPP

#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_Base.hpp"

// total size: 0x1C
// Decl: 8
class CSTATEMGR_Main : public CSTATEMGR_Base {
  public:
    CSTATEMGR_Main();
    ~CSTATEMGR_Main() override;

    // Overrides: CSTATEMGR_Base
    void EnterWorld(eSndGameMode esgm) override;
};

#endif
