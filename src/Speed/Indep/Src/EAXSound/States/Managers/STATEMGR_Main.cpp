#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_Main.hpp"

CSTATEMGR_Main::CSTATEMGR_Main() : CSTATEMGR_Base() {}

CSTATEMGR_Main::~CSTATEMGR_Main() {}

void CSTATEMGR_Main::EnterWorld(eSndGameMode esgm) {
    int SFXIDs = 0xFF;
    int SFXCTRLS = 3;
    CSTATE_Base *NewMainSTate = CreateState(0, SFXIDs);
    NewMainSTate->Setup(SFXIDs);
    NewMainSTate->ForceCreateSFXCtrls(SFXCTRLS);
    CSTATEMGR_Base::EnterWorld(esgm);
    NewMainSTate->Attach(nullptr);
}
