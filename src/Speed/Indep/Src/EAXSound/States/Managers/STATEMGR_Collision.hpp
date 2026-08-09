#ifndef STATEMGR_COLLISION_HPP
#define STATEMGR_COLLISION_HPP // Decl: 2

#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_Base.hpp"

#define MAX_NUM_COLLISIONS 20 // Decl: 8

// total size: 0x1C
// Decl: 14
class CSTATEMGR_Collision : public CSTATEMGR_Base {
  public:
    CSTATEMGR_Collision();
    ~CSTATEMGR_Collision() override;

    // Overrides: CSTATEMGR_Base
    void EnterWorld(eSndGameMode esgm) override;
    void UpdateParams(float t) override;
    CSTATE_Base *GetFreeState(void *ObjectPtr) override;
};

#endif
