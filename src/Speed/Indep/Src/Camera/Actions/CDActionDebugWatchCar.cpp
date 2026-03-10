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
    if (!director->GetAction()) {
        return nullptr;
    }
    return new CDActionDebugWatchCar(director);
}

CDActionDebugWatchCar::CDActionDebugWatchCar(CameraAI::Director *director)
    : CameraAI::Action(), //
      IDebugWatchCar(this) {
    mTarget = WorldConn::Reference(0);
    mhSimable = nullptr;
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

    mAnchor = new CameraAnchor(0);

    AquireTarget();

    if (mTarget.IsValid()) {
        bMatrix4 mat(*mTarget.GetMatrix());
        mAnchor->Update(0.0f, mat, *mTarget.GetVelocity(), *mTarget.GetAcceleration());
    }

    mMover = new TrackCarCameraMover(static_cast<int>(director->GetViewID()), mAnchor, true);
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
