#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_TrafficCar.hpp"
#include "Speed/Indep/Src/EAXSound/EAXTrafficCar.hpp"
#include "Speed/Indep/Src/Misc/Profiler.hpp"

bool DEBUG_TRAFFIC_CAR_CONNECTIONS = false; // size: 0x1, address: 0x80417F30, Decl: 9

CSTATEMGR_TrafficCar::CSTATEMGR_TrafficCar() {
    this->m_CarContext = Sound::CONTEXT_TRAFFIC;
    this->m_fConnectDistance = 65.0f;
}

CSTATEMGR_TrafficCar::~CSTATEMGR_TrafficCar() {}

void CSTATEMGR_TrafficCar::EnterWorld(eSndGameMode esgm) {
    int SFXIDs = 0xF;

    for (int n = 0; n < 6; n++) {
        EAXTrafficCar *NewTrafficCar = static_cast<EAXTrafficCar *>(this->CreateState(0, SFXIDs));
        NewTrafficCar->Setup(SFXIDs);
    }

    CSTATEMGR_Base::EnterWorld(esgm);
}

void CSTATEMGR_TrafficCar::UpdateParams(float t) {
    ProfileNode profile_node("TODO", 0);

    CSTATEMGR_CarState::UpdateParams(t);
    if (DEBUG_TRAFFIC_CAR_CONNECTIONS) {
        this->DebugDisplayTrafficConnections();
    }
}

// TODO ScreenPrintf
void CSTATEMGR_TrafficCar::DebugDisplayTrafficConnections() {
    int y = -0xAA;
    int x = 0xFA;
    EAXTrafficCar *car = static_cast<EAXTrafficCar *>(this->m_pHeadStateObj);

    while (car != nullptr) {
        unsigned int color;

        if (car->GetPhysCar() == nullptr || !car->GetPhysCar()->GetAttributes()->IsValid()) {
            color = 0x5FFF3F3F;
        } else {
            color = 0xFFFF3F3F;

            if (car->GetPhysCar()->GetAttributes()->IsValid()) {
                Attrib::Gen::pvehicle vehicle(*car->GetPhysCar()->GetAttributes());
                vehicle.CollectionName();
            }
        }

        y += 0xF;

        car = static_cast<EAXTrafficCar *>(car->m_pNextState);
    }
}
