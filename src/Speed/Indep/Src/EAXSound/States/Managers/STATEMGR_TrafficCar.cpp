#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_TrafficCar.hpp"

#include "Speed/Indep/Src/EAXSound/EAXTrafficCar.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pvehicle.h"
#include "Speed/Indep/Src/Misc/Profiler.hpp"

void ScreenPrintf(int x, int y, float scale, unsigned int color, const char *fmt, ...);

int DEBUG_TRAFFIC_CAR_CONNECTIONS = 0;

CSTATEMGR_TrafficCar::CSTATEMGR_TrafficCar() {}

CSTATEMGR_TrafficCar::~CSTATEMGR_TrafficCar() {}

void CSTATEMGR_TrafficCar::EnterWorld(eSndGameMode esgm) {
    int SFXIDs = 0xF;

    for (int n = 0; n < 6; n++) {
        CSTATE_Base *NewTrafficCar = CreateState(0, SFXIDs);
        NewTrafficCar->Setup(SFXIDs);
    }

    CSTATEMGR_Base::EnterWorld(esgm);
}

void CSTATEMGR_TrafficCar::UpdateParams(float t) {
    ProfileNode profile_node("TODO", 0);

    CSTATEMGR_CarState::UpdateParams(t);
    if (DEBUG_TRAFFIC_CAR_CONNECTIONS != 0) {
        DebugDisplayTrafficConnections();
    }
}

void CSTATEMGR_TrafficCar::DebugDisplayTrafficConnections() {
    int y = -0xAA;
    int x = 0xFA;

    for (EAXTrafficCar *car = static_cast<EAXTrafficCar *>(m_pHeadStateObj); car;
         car = static_cast<EAXTrafficCar *>(car->m_pNextState)) {
        unsigned int color;

        if (!car->GetPhysCar() || !car->GetPhysCar()->GetAttributes()->IsValid()) {
            color = 0x5FFF3F3F;
            ScreenPrintf(x, y, 1.0f, color, "unattached");
        } else {
            color = 0xFFFF3F3F;
            Attrib::Gen::pvehicle vehicle(*car->GetPhysCar()->GetAttributes());
            ScreenPrintf(x, y, 1.0f, color, "%s", vehicle.CollectionName());
        }

        y += 0xF;
    }
}
