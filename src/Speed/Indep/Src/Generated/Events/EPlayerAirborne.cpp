#include "EPlayerAirborne.hpp"

#include "Speed/Indep/Src/Camera/CameraAI.hpp"

EPlayerAirborne::EPlayerAirborne(HSIMABLE phSimable) : Event(0x10), fhSimable(phSimable) {
}

EPlayerAirborne::~EPlayerAirborne() {
    ISimable *isimable = ISimable::FindInstance(fhSimable);

    if (isimable) {
        if (isimable->IsPlayer()) {
            CameraAI::MaybeDoJumpCam(isimable);
        }
    }
}

const char *EPlayerAirborne::GetEventName() const {
    return "EPlayerAirborne";
}

void EPlayerAirborne_MakeEvent_Callback(const void *staticData) {
    new EPlayerAirborne(((EPlayerAirborne::StaticData *) staticData)->fhSimable);
}
