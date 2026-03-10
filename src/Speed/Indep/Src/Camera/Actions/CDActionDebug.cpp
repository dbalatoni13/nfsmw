#include "Speed/Indep/Src/Camera/Actions/CDActionDebug.hpp"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"

static UTL::COM::Factory<CameraAI::Director *, CameraAI::Action, UCrc32>::Prototype _CDActionDebug("DEBUG", CDActionDebug::Construct);

const Attrib::StringKey &CDActionDebug::GetName() const {
    static Attrib::StringKey name("DEBUG");
    return name;
}

Attrib::StringKey CDActionDebug::GetNext() const {
    if (!mDone) {
        return Attrib::StringKey("");
    }
    return mPrev;
}

CameraAI::Action *CDActionDebug::Construct(CameraAI::Director *director) {
    // TODO
    return nullptr;
}

CDActionDebug::CDActionDebug(CameraAI::Director *director)
    : CameraAI::Action(), //
      mActionQ(false) {
    // TODO
}

CDActionDebug::~CDActionDebug() {
    // TODO
}

void CDActionDebug::Update(float dT) {
    // TODO
}

bool CDActionDebug::GetTrafficBasis(UMath::Matrix4 &matrix, UMath::Vector3 &velocity) {
    // TODO
    return false;
}
