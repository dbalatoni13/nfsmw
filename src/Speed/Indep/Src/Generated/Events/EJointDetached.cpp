#include "EJointDetached.hpp"

#include "Speed/Indep/Src/Generated/Hash.hpp"
#include "Speed/Indep/Src/Generated/Messages/MSetTrafficSpeed.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"

EJointDetached::EJointDetached(HSIMABLE phObject) : Event(0x10), fhObject(phObject) {
}

EJointDetached::~EJointDetached() {
    ISimable *object = ISimable::FindInstance(fhObject);

    if (object) {
        object->ProcessStimulus(UCRC32_JOINT_DETACHED);

        if (object->GetSimableType() == SIMABLE_VEHICLE) {
            MSetTrafficSpeed ai_msg(0.0f, 0.0f, true);

            ai_msg.SetID(object->GetWorldID());
            ai_msg.Post("AIAction");
        }
    }
}

const char *EJointDetached::GetEventName() const {
    return "EJointDetached";
}

void EJointDetached_MakeEvent_Callback(const void *staticData) {
    new EJointDetached(((EJointDetached::StaticData *) staticData)->fhObject);
}
