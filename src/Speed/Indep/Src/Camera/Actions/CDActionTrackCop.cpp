#include "Speed/Indep/Src/Camera/Actions/CDActionTrackCop.hpp"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"

static UTL::COM::Factory<CameraAI::Director *, CameraAI::Action, UCrc32>::Prototype _CDActionTrackCop("TRACKCOP", CDActionTrackCop::Construct);

const Attrib::StringKey &CDActionTrackCop::GetName() const {
    static Attrib::StringKey name("TRACKCOP");
    return name;
}

Attrib::StringKey CDActionTrackCop::GetNext() const {
    return Attrib::StringKey();
}

bool CDActionTrackCop::Attach(UTL::COM::IUnknown *pOther) {
    return mAttachments->Attach(pOther);
}

bool CDActionTrackCop::Detach(UTL::COM::IUnknown *pOther) {
    return mAttachments->Detach(pOther);
}

bool CDActionTrackCop::IsAttached(const UTL::COM::IUnknown *pOther) const {
    return mAttachments->IsAttached(pOther);
}

const IAttachable::List *CDActionTrackCop::GetAttachments() const {
    return &mAttachments->GetList();
}

CameraAI::Action *CDActionTrackCop::Construct(CameraAI::Director *director) {
    // TODO
    return nullptr;
}

CDActionTrackCop::CDActionTrackCop(CameraAI::Director *director, IPlayer *player)
    : CameraAI::Action(), //
      IAttachable(this) {
    // TODO
}

CDActionTrackCop::~CDActionTrackCop() {
    // TODO
}

void CDActionTrackCop::OnDetached(IAttachable *pOther) {
    // TODO
}

void CDActionTrackCop::OnCarDetached() {
    // TODO
}

void CDActionTrackCop::AquireCar() {
    // TODO
}

void CDActionTrackCop::Update(float dT) {
    // TODO
}

bool CDActionTrackCop::GetTrafficBasis(UMath::Matrix4 &matrix, UMath::Vector3 &velocity) {
    // TODO
    return false;
}
