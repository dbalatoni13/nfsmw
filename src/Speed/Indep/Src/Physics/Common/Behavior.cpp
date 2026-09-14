#include "Speed/Indep/Src/Physics/Behavior.h"
#include "Speed/Indep/Src/Generated/AttribSys/GenericAccessor.h"
#include "Speed/Indep/Src/Physics/PhysicsObject.h"

Attrib::StringKey BEHAVIOR_MECHANIC_AI("BEHAVIOR_MECHANIC_AI");
Attrib::StringKey BEHAVIOR_MECHANIC_RIGIDBODY("BEHAVIOR_MECHANIC_RIGIDBODY");
Attrib::StringKey BEHAVIOR_MECHANIC_INPUT("BEHAVIOR_MECHANIC_INPUT");
Attrib::StringKey BEHAVIOR_MECHANIC_SUSPENSION("BEHAVIOR_MECHANIC_SUSPENSION");
Attrib::StringKey BEHAVIOR_MECHANIC_ENGINE("BEHAVIOR_MECHANIC_ENGINE");
Attrib::StringKey BEHAVIOR_MECHANIC_DAMAGE("BEHAVIOR_MECHANIC_DAMAGE");
Attrib::StringKey BEHAVIOR_MECHANIC_DRAW("BEHAVIOR_MECHANIC_DRAW");
Attrib::StringKey BEHAVIOR_MECHANIC_AUDIO("BEHAVIOR_MECHANIC_AUDIO");
Attrib::StringKey BEHAVIOR_MECHANIC_EFFECTS("BEHAVIOR_MECHANIC_EFFECTS");
Attrib::StringKey BEHAVIOR_MECHANIC_RESET("BEHAVIOR_MECHANIC_RESET");

Behavior::Behavior(const BehaviorParams &params, unsigned int num_interfaces)
    : Sim::Object(num_interfaces + 1),   //
      mPaused(false),                    //
      mOwner(params.fowner),             //
      mIOwner(params.fowner),            //
      mMechanic(params.fMechanic),       //
      mSignature(params.fSig),           //
      mPriority(0),                      //
      mProfile(nullptr) {
    const Attrib::Instance &attribs = params.fowner->GetAttributes();
    unsigned int count = attribs->Num_BEHAVIOR_ORDER();
    while (this->mPriority < count) {
        if (attribs->BEHAVIOR_ORDER(this->mPriority) == this->mMechanic) {
            break;
        }
        this->mPriority = this->mPriority + 1;
    }
}

void Behavior::Pause(bool pause) {
    if (this->mPaused != pause) {
        this->mPaused = pause;
        if (pause) {
            this->OnPause();
        } else {
            this->OnUnPause();
        }
    }
}
