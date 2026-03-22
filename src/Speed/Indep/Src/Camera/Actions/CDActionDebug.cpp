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
    bVector3 start_position(0.0f, 0.0f, 0.0f);
    bVector3 start_direction(0.0f, 0.0f, 1.0f);
    Action *prev_action = director->GetAction();

    if (prev_action) {
        mPrev = prev_action->GetName();
        CameraMover *prev_mover = prev_action->GetMover();
        if (prev_mover) {
            start_position = *prev_mover->GetPosition();
            start_direction = *prev_mover->GetDirection();
        }
    }

    int viewId = static_cast<int>(director->GetViewID());
    mMover = new DebugWorldCameraMover(viewId, &start_position, &start_direction, static_cast<JoystickPort>(viewId));
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
