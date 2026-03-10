#include "Speed/Indep/Src/Camera/Actions/CDActionDrive.hpp"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Generated/Messages/MJumpCut.h"

static float kCinematicMomementSeconds;

static UTL::COM::Factory<CameraAI::Director *, CameraAI::Action, UCrc32>::Prototype _CDActionDrive("DRIVE", CDActionDrive::Construct);

const Attrib::StringKey &CDActionDrive::GetName() const {
    static Attrib::StringKey name("DRIVE");
    return name;
}

Attrib::StringKey CDActionDrive::GetNext() const {
    return Attrib::StringKey();
}

void CDActionDrive::SetSpecial(float val) {
    if (val > 0.0f) {
        kCinematicMomementSeconds = val;
        mCinematicMomementTimerInc = true;
    }
}

bool CDActionDrive::Attach(UTL::COM::IUnknown *pOther) {
    return mAttachments->Attach(pOther);
}

bool CDActionDrive::Detach(UTL::COM::IUnknown *pOther) {
    return mAttachments->Detach(pOther);
}

bool CDActionDrive::IsAttached(const UTL::COM::IUnknown *pOther) const {
    return mAttachments->IsAttached(pOther);
}

const IAttachable::List *CDActionDrive::GetAttachments() const {
    return &mAttachments->GetList();
}

CameraAI::Action *CDActionDrive::Construct(CameraAI::Director *director) {
    // TODO
    return nullptr;
}

CDActionDrive::CDActionDrive(CameraAI::Director *director, IPlayer *player)
    : CameraAI::Action(), //
      IAttachable(this), //
      mMaxCollisionTime(0.5f) {
    // TODO
}

CDActionDrive::~CDActionDrive() {
    // TODO
}

void CDActionDrive::Reset() {
    mGear = 0;
    mCinematicMomementTimer = 0.0f;
    mGameBreakerScale = 0.0f;
    mDampCollisionTime = 0.0f;
    mGroundCollisionTime = 0.0f;
    mObjectCollisionTime = 0.0f;
    mPulseTimer = 0.0f;
    mCinematicMomementTimerInc = false;
}

void CDActionDrive::OnDetached(IAttachable *pOther) {
    // TODO
}

void CDActionDrive::OnCarDetached() {
    // TODO
}

void CDActionDrive::OnCollision(const Sim::Collision::Info &cinfo) {
    // TODO
}

void CDActionDrive::AquireCar() {
    // TODO
}

void CDActionDrive::Update(float dT) {
    // TODO
}

bool CDActionDrive::GetTrafficBasis(UMath::Matrix4 &matrix, UMath::Vector3 &velocity) {
    // TODO
    return false;
}

void CDActionDrive::MessageJumpCut(const MJumpCut &message) {
    // TODO
}
