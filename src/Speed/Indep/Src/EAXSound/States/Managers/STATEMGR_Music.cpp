 #include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_Music.hpp"

CSTATEMGR_Music::CSTATEMGR_Music()
    : CSTATEMGR_Base() {}

CSTATEMGR_Music::~CSTATEMGR_Music() {}

void CSTATEMGR_Music::EnterWorld(eSndGameMode esgm) {
    int SFXIDs;
    CSTATE_Base *NewMainSTate;

    SFXIDs = 3;
    NewMainSTate = CreateState(0, SFXIDs);
    NewMainSTate->Setup(SFXIDs);
    NewMainSTate->Attach(nullptr);
    CSTATEMGR_Base::EnterWorld(esgm);
}
