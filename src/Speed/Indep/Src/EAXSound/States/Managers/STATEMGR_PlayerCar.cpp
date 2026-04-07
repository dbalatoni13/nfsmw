#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_PlayerCar.hpp"

#include "Speed/Indep/Src/EAXSound/EAXCarState.hpp"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/Src/Sim/Simulation.h"

bool CSTATEMGR_PlayerCar::IsTruck = false;

CSTATEMGR_PlayerCar::CSTATEMGR_PlayerCar() {}

CSTATEMGR_PlayerCar::~CSTATEMGR_PlayerCar() {}

void CSTATEMGR_PlayerCar::EnterWorld(eSndGameMode esgm) {
    int SFXIDs = 0x53BFA;
    bool bIsTruck = false;
    typedef UTL::Collections::Listable<EAX_CarState, 10> CarList;
    const CarList::List &carlist = CarList::GetList();

    for (EAX_CarState *const *iter = carlist.begin(); iter != carlist.end(); ++iter) {
        if (bIsTruck) {
            break;
        }

        EAX_CarState *state = *iter;
        if (state->GetContext() == Sound::CONTEXT_PLAYER) {
            Attrib::Gen::pvehicle veh(state->GetAttributes()->GetCollection(), 0, nullptr);
            if (veh.TruckSndFX()) {
                bIsTruck = true;
            }
        }
    }

    if (bIsTruck) {
        IsTruck = true;
        SFXIDs = 0xD3BFA;
    } else {
        IsTruck = false;
    }

    for (int n = 0;; n++) {
        if (Sim::GetUserMode() == Sim::USER_SPLIT_SCREEN) {
            if (n > 1) {
                break;
            }
        } else if (n > 0) {
            break;
        }

        CSTATE_Base *NewPlayerCar = CreateState(0, SFXIDs);
        NewPlayerCar->Setup(SFXIDs);
        NewPlayerCar->ForceCreateSFXCtrls(0x100);
    }

    CSTATEMGR_Base::EnterWorld(esgm);
}

void CSTATEMGR_PlayerCar::UpdateParams(float t) {
    ProfileNode profile_node("TODO", 0);

    CSTATEMGR_Base::UpdateParams(t);
}
