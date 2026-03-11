#include "Speed/Indep/Src/Physics/Behavior.h"

#include "Speed/Indep/Src/Generated/AttribSys/GenericAccessor.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Physics/PhysicsObject.h"

Behavior::Behavior(const BehaviorParams &params, unsigned int num_interfaces)
    : Sim::Object(num_interfaces + 1)
    , mPaused(false)
    , mOwner(params.fowner)
    , mIOwner(static_cast<ISimable *>(params.fowner))
    , mMechanic(params.fMechanic)
    , mSignature(params.fSig)
    , mPriority(0)
    , mProfile(nullptr) {
    const Attrib::Instance &attribs = params.fowner->GetAttributes();
    unsigned int count =
        reinterpret_cast<const Attrib::Gen::GenericAccessor *>(&attribs)->Num_BEHAVIOR_ORDER();
    while (mPriority < count) {
        UCrc32 order(
            reinterpret_cast<const Attrib::Gen::GenericAccessor *>(&attribs)->BEHAVIOR_ORDER(mPriority));
        if (order == mMechanic) {
            break;
        }
        mPriority = mPriority + 1;
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