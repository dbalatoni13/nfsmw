#include "Speed/Indep/Src/Physics/Behavior.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pvehicle.h"

IMPLEMENT_FACTORY(Behavior);

Behavior::Behavior(const BehaviorParams &params, unsigned int num_interfaces)
    : Sim::Object(num_interfaces + 1),
      mPaused(false),
      mOwner(params.fowner),
      mIOwner(params.fowner),
      mMechanic(params.fMechanic),
      mSignature(params.fSig),
      mPriority(0),
      mProfile(0) {
    const Attrib::Instance &attributes = params.fowner->GetAttributes();
    unsigned int num = attributes.Get(Attrib::Hash::pvehicle::BEHAVIOR_ORDER).GetLength();
    while (mPriority < num) {
        const Attrib::StringKey *order =
            static_cast<const Attrib::StringKey *>(attributes.GetAttributePointer(Attrib::Hash::pvehicle::BEHAVIOR_ORDER, mPriority));
        if (order == nullptr) {
            order = static_cast<const Attrib::StringKey *>(Attrib::DefaultDataArea(sizeof(Attrib::StringKey)));
        }
        if (UCrc32(*order) == mMechanic) {
            break;
        }
        mPriority++;
    }
}

void Behavior::Pause(bool pause) {
    if (mPaused != pause) {
        mPaused = pause;
        if (pause) {
            OnPause();
        } else {
            OnUnPause();
        }
    }
}

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
