#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_Truck.hpp"

CSTATEMGR_Truck::CSTATEMGR_Truck() {}

CSTATEMGR_Truck::~CSTATEMGR_Truck() {}

void CSTATEMGR_Truck::EnterWorld(eSndGameMode esgm) {
    int SFXIDs = 0x184001;

    for (int n = 0; n < 2; n++) {
        CSTATE_Base *NewTruck = CreateState(0, SFXIDs);
        NewTruck->Setup(SFXIDs);
    }

    CSTATEMGR_Base::EnterWorld(esgm);
}
