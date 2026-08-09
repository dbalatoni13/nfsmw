#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_Helicopter.hpp"

CSTATEMGR_Helicopter::CSTATEMGR_Helicopter() : CSTATEMGR_Base() {}

CSTATEMGR_Helicopter::~CSTATEMGR_Helicopter() {}

void CSTATEMGR_Helicopter::EnterWorld(eSndGameMode esgm) {
    int SFXIDs = 1;
    CSTATE_Base *NewMainSTate = this->CreateState(0, SFXIDs);
    NewMainSTate->Setup(SFXIDs);
    CSTATEMGR_Base::EnterWorld(esgm);
}
