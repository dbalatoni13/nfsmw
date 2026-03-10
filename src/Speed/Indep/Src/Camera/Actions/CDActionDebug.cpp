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
    return new ("CDActionDebug") CDActionDebug(director);
}

CDActionDebug::CDActionDebug(CameraAI::Director *director)
    : CameraAI::Action(), //
      mActionQ(1, 0x98c7a2f5, "DebugWorld", false) {
    mDone = false;
    mPrev = director->GetAction()->GetName();

    CameraMover *m = director->GetMover();
    bVector3 pos;
    bVector3 dir;

    if (m != nullptr) {
        pos = *m->GetCamera()->GetPosition();
        dir = *m->GetCamera()->GetDirection();
    } else {
        pos.x = 0.0f;
        pos.y = 0.0f;
        pos.z = 0.0f;
        dir.x = 0.0f;
        dir.y = 0.0f;
        dir.z = 1.0f;
    }

        int viewId = static_cast<int>(director->GetViewID());
    mMover = new DebugWorldCameraMover(viewId, &pos, &dir, static_cast<JoystickPort>(viewId));
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
