#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_Collision.hpp"

CSTATEMGR_Collision::CSTATEMGR_Collision() {}

CSTATEMGR_Collision::~CSTATEMGR_Collision() {}

void CSTATEMGR_Collision::EnterWorld(eSndGameMode esgm) {
    int ColSFXID = 1;

    for (int n = 0; n < 10; n++) {
        CSTATE_Base *NewState = CreateState(0, ColSFXID);
        NewState->Setup(ColSFXID);
    }

    CSTATEMGR_Base::EnterWorld(esgm);
}

void CSTATEMGR_Collision::UpdateParams(float t) {
    CSTATEMGR_Base::UpdateParams(t);
}
