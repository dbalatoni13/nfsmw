#include "Speed/Indep/Src/Camera/Actions/CDActionDebugWatchCar.hpp"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"

static UTL::COM::Factory<CameraAI::Director *, CameraAI::Action, UCrc32>::Prototype _CDActionDebugWatchCar("DEBUGWATCHCAR", CDActionDebugWatchCar::Construct);

const Attrib::StringKey &CDActionDebugWatchCar::GetName() const {
    static Attrib::StringKey name("DEBUGWATCHCAR");
    return name;
}

Attrib::StringKey CDActionDebugWatchCar::GetNext() const {
    return mPrev;
}

ISimable *CDActionDebugWatchCar::GetSimable() {
    // TODO
    return nullptr;
}

void CDActionDebugWatchCar::ReleaseTarget() {
    // TODO
}

void CDActionDebugWatchCar::AquireTarget() {
    // TODO
}

CameraAI::Action *CDActionDebugWatchCar::Construct(CameraAI::Director *director) {
    // TODO
    return nullptr;
}

CDActionDebugWatchCar::CDActionDebugWatchCar(CameraAI::Director *director)
    : CameraAI::Action(), //
      IDebugWatchCar(this) {
    // TODO
}

CDActionDebugWatchCar::~CDActionDebugWatchCar() {
    // TODO
}

void CDActionDebugWatchCar::Update(float dT) {
    // TODO
}

bool CDActionDebugWatchCar::GetTrafficBasis(UMath::Matrix4 &matrix, UMath::Vector3 &velocity) {
    // TODO
    return false;
}
