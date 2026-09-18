#include "EEnableAIPhysics.hpp"

#include "Speed/Indep/Src/Interfaces/Simables/IEngine.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISuspension.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Sim/SimTypes.h"

EEnableAIPhysics::EEnableAIPhysics(uintptr_t phSimable, float pSpeed, unsigned int pEnable) : Event(0x20), fhSimable(phSimable), fSpeed(pSpeed), fEnable(pEnable) {
}

EEnableAIPhysics::~EEnableAIPhysics() {
    ISimable *simable = ISimable::FindInstance((HSIMABLE) fhSimable);

    if (simable) {
        IVehicle *vehicle;

        if (simable->QueryInterface(&vehicle)) {
            if (fEnable) {
                vehicle->SetBehaviorOverride(BEHAVIOR_MECHANIC_SUSPENSION, UCrc32("SuspensionSimple"));
            } else {
                vehicle->RemoveBehaviorOverride(BEHAVIOR_MECHANIC_SUSPENSION);
            }

            vehicle->CommitBehaviorOverrides();

            ISuspension *suspension;

            if (vehicle->QueryInterface(&suspension)) {
                suspension->MatchSpeed(fSpeed);
            }

            IEngine *engine;

            if (vehicle->QueryInterface(&engine)) {
                engine->MatchSpeed(fSpeed);
            }
        }
    }
}

const char *EEnableAIPhysics::GetEventName() const {
    return "EEnableAIPhysics";
}

void EEnableAIPhysics_MakeEvent_Callback(const void *staticData) {
    new EEnableAIPhysics(((EEnableAIPhysics::StaticData *) staticData)->fhSimable, ((EEnableAIPhysics::StaticData *) staticData)->fSpeed, ((EEnableAIPhysics::StaticData *) staticData)->fEnable);
}
