#include "EVehicleReset.hpp"

#include "Speed/Indep/Src/Interfaces/SimEntities/IEntity.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"

EVehicleReset::EVehicleReset(uintptr_t phSimable, UMath::Vector3 pFrom, UMath::Vector3 pTo)
    : Event(0x30), fhSimable(phSimable), fFrom(pFrom), fTo(pTo) {
}

EVehicleReset::~EVehicleReset() {
    HSIMABLE hsimable = (HSIMABLE) fhSimable;
    ISimable *isimable = ISimable::FindInstance(hsimable);

    if (isimable) {
        Sim::IEntity *entity = isimable->GetEntity();

        if (entity) {
            IPlayer *iplayer;

            if (entity->QueryInterface(&iplayer)) {
                if (iplayer->InGameBreaker()) {
                    iplayer->ToggleGameBreaker();
                }
            }
        }
    }
}

const char *EVehicleReset::GetEventName() const {
    return "EVehicleReset";
}

void EVehicleReset_MakeEvent_Callback(const void *staticData) {
    new EVehicleReset(((EVehicleReset::StaticData *) staticData)->fhSimable, ((EVehicleReset::StaticData *) staticData)->fFrom, ((EVehicleReset::StaticData *) staticData)->fTo);
}
