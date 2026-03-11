#include "Speed/Indep/Src/Camera/Actions/CDActionDebug.hpp"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Camera/Camera.hpp"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"

extern bool Tweak_EnableICEAuthoring;

static UTL::COM::Factory<CameraAI::Director *, CameraAI::Action, UCrc32>::Prototype _CDActionDebug("DEBUG", CDActionDebug::Construct);

const Attrib::StringKey &CDActionDebug::GetName() const {
    static Attrib::StringKey name("DEBUG");
    return name;
}

Attrib::StringKey CDActionDebug::GetNext() const {
    if (mDone) {
        return mPrev;
    }
    return Attrib::StringKey("");
}

CameraAI::Action *CDActionDebug::Construct(CameraAI::Director *director) {
    return new (static_cast<const char *>(0)) CDActionDebug(director);
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
    CameraMover *m = mMover;
    delete m;
}

void CDActionDebug::Update(float dT) {
    while (!mActionQ.IsEmpty()) {
        ActionRef aRef = mActionQ.GetAction();
        float data = aRef.Data();
        if (aRef.ID() == 0x15 && !Tweak_EnableICEAuthoring) {
            mDone = true;
        }
        mActionQ.PopAction();
    }
}

bool CDActionDebug::GetTrafficBasis(UMath::Matrix4 &matrix, UMath::Vector3 &velocity) {
    bMatrix4 camera_to_world;
    eInvertTransformationMatrix(&camera_to_world, mMover->GetCamera()->GetCameraMatrix());
    ConversionUtil::RightToLeftMatrix4(camera_to_world, matrix);
    ConversionUtil::RightToLeftVector3(*mMover->GetCamera()->GetVelocityPosition(), velocity);
    return true;
}

namespace ConversionUtil {
template void Copy4<UMath::Vector4, UMath::Vector4>(UMath::Vector4 &, const UMath::Vector4 &);
template void RightToLeftVector3<UMath::Vector3, UMath::Vector3>(const UMath::Vector3 &, UMath::Vector3 &);
template void RightToLeftMatrix4<UMath::Matrix4, UMath::Matrix4>(const UMath::Matrix4 &, UMath::Matrix4 &);
} // namespace ConversionUtil
