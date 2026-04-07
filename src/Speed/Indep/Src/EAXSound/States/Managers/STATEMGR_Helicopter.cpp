 #include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_Helicopter.hpp"

CSTATEMGR_Helicopter::CSTATEMGR_Helicopter()
    : CSTATEMGR_Base() {}

CSTATEMGR_Helicopter::~CSTATEMGR_Helicopter() {}

void CSTATEMGR_Helicopter::EnterWorld(eSndGameMode esgm) {
    int SFXIDs;
    CSTATE_Base *NewMainSTate;

    SFXIDs = 1;
    NewMainSTate = CreateState(0, SFXIDs);
    NewMainSTate->Setup(SFXIDs);
    CSTATEMGR_Base::EnterWorld(esgm);
}
