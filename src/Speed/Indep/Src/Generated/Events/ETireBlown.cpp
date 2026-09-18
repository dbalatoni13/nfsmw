#include "ETireBlown.hpp"

#include "Speed/Indep/Src/Generated/AttribSys/Classes/pvehicle.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISuspension.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Sim/SimEffect.h"

ETireBlown::ETireBlown(HSIMABLE phSimable, unsigned int pIndex) : Event(0x10), fhSimable(phSimable), fIndex(pIndex) {
}

ETireBlown::~ETireBlown() {
    IVehicle *ivehicle;
    ISimable *isimable = ISimable::FindInstance(fhSimable);
    ISuspension *suspension;

    if (isimable && isimable->QueryInterface(&suspension) && isimable->QueryInterface(&ivehicle)) {
        IVehicleAI *ivai;
        UMath::Vector3 position;
        UMath::Vector3 direction;

        if (ivehicle->QueryInterface(&ivai)) {
            IPursuit *ip = ivai->GetPursuit();

            if (ip) {
                ip->SpikesHit(ivai);
            }
        }

        position = suspension->GetWheelPos(fIndex);

        isimable->GetRigidBody()->GetUpVector(direction);
        Attrib::Instance effect(ivehicle->GetVehicleAttributes().OnTireBlow(), 0, NULL);
        Sim::Effect::Fire(effect.GetConstCollection(), position, direction, isimable->GetWorldID(),
                          isimable->GetAttributes().GetConstCollection(), NULL, 0);
    }
}

const char *ETireBlown::GetEventName() const {
    return "ETireBlown";
}

void ETireBlown_MakeEvent_Callback(const void *staticData) {
    new ETireBlown(((ETireBlown::StaticData *) staticData)->fhSimable, ((ETireBlown::StaticData *) staticData)->fIndex);
}
