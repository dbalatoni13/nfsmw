#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_Music.hpp"

CSTATEMGR_Music::CSTATEMGR_Music() : CSTATEMGR_Base() {}

CSTATEMGR_Music::~CSTATEMGR_Music() {}

void CSTATEMGR_Music::EnterWorld(eSndGameMode esgm) {
    int SFXIDs = 3;
    CSTATE_Base *NewMainSTate = this->CreateState(0, SFXIDs);
    NewMainSTate->Setup(SFXIDs);
    NewMainSTate->Attach(nullptr);
    CSTATEMGR_Base::EnterWorld(esgm);
}
