#include "EPlayerTriggeredNOS.hpp"

#include "Speed/Indep/Src/EAXSound/EAXCarState.hpp"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/IEngine.h"

EPlayerTriggeredNOS::EPlayerTriggeredNOS(HSIMABLE phSimable) : Event(0x10), fhSimable(phSimable) {
}

EPlayerTriggeredNOS::~EPlayerTriggeredNOS() {
    ISimable *simable = ISimable::FindInstance(fhSimable);

    if (simable) {
        IPlayer *player = simable->GetPlayer();

        if (player) {
            IEngine *engine;

            if (simable->QueryInterface(&engine)) {
                if (engine->HasNOS() && engine->GetNOSCapacity() <= 0.0f) {
                    EAX_CarState *car_state = EAX_CarState::Find(simable->GetWorldID());

                    if (car_state) {
                        car_state->SetNosEmptyFlag(true);
                    }
                }
            }
        }
    }
}

const char *EPlayerTriggeredNOS::GetEventName() const {
    return "EPlayerTriggeredNOS";
}

void EPlayerTriggeredNOS_MakeEvent_Callback(const void *staticData) {
    new EPlayerTriggeredNOS(((EPlayerTriggeredNOS::StaticData *) staticData)->fhSimable);
}
