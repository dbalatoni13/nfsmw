#include "Speed/Indep/Src/Camera/Actions/CDActionIce.hpp"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Camera/ICE/ICEMover.hpp"

static UTL::COM::Factory<CameraAI::Director *, CameraAI::Action, UCrc32>::Prototype _CDActionIce("ICE", CDActionIce::Construct);

const Attrib::StringKey &CDActionIce::GetName() const {
    static Attrib::StringKey name("ICE");
    return name;
}

Attrib::StringKey CDActionIce::GetNext() const {
    if (!mDone) {
        return Attrib::StringKey("");
    }
    return mPrev;
}

CameraMover *CDActionIce::GetMover() {
    return mMover;
}

bool CDActionIce::Attach(UTL::COM::IUnknown *pOther) {
    return mAttachments->Attach(pOther);
}

bool CDActionIce::Detach(UTL::COM::IUnknown *pOther) {
    return mAttachments->Detach(pOther);
}

bool CDActionIce::IsAttached(const UTL::COM::IUnknown *pOther) const {
    return mAttachments->IsAttached(pOther);
}

const IAttachable::List *CDActionIce::GetAttachments() const {
    return &mAttachments->GetList();
}

CameraAI::Action *CDActionIce::Construct(CameraAI::Director *director) {
    // TODO
    return nullptr;
}

CDActionIce::CDActionIce(CameraAI::Director *director, IPlayer *player)
    : CameraAI::Action(), //
      IAttachable(this),   //
      mActionQ(false) {
    // TODO
}

CDActionIce::~CDActionIce() {
    // TODO
}

void CDActionIce::OnDetached(IAttachable *pOther) {
    // TODO
}

void CDActionIce::ReleaseCar(bool detach) {
    // TODO
}

void CDActionIce::AquireCar() {
    // TODO
}

void CDActionIce::Update(float dT) {
    // TODO
}
