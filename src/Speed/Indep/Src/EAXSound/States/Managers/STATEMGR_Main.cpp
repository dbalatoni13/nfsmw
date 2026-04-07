 #include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_Main.hpp"

CSTATEMGR_Main::CSTATEMGR_Main()
    : CSTATEMGR_Base() {}

CSTATEMGR_Main::~CSTATEMGR_Main() {}

void CSTATEMGR_Main::EnterWorld(eSndGameMode esgm) {
    int SFXIDs;
    int SFXCTRLS;
    CSTATE_Base *NewMainSTate;

    SFXIDs = 0xFF;
    SFXCTRLS = 3;
    NewMainSTate = CreateState(0, SFXIDs);
    NewMainSTate->Setup(SFXIDs);
    NewMainSTate->ForceCreateSFXCtrls(SFXCTRLS);
    CSTATEMGR_Base::EnterWorld(esgm);
    NewMainSTate->Attach(nullptr);
}
