#include "Speed/Indep/Src/Camera/Actions/CDActionShowcase.hpp"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"

static UTL::COM::Factory<CameraAI::Director *, CameraAI::Action, UCrc32>::Prototype _CDActionShowcase("SHOWCASE", CDActionShowcase::Construct);

const Attrib::StringKey &CDActionShowcase::GetName() const {
    static Attrib::StringKey name("SHOWCASE");
    return name;
}

Attrib::StringKey CDActionShowcase::GetNext() const {
    return Attrib::StringKey();
}

bool CDActionShowcase::Attach(UTL::COM::IUnknown *pOther) {
    return mAttachments->Attach(pOther);
}

bool CDActionShowcase::Detach(UTL::COM::IUnknown *pOther) {
    return mAttachments->Detach(pOther);
}

bool CDActionShowcase::IsAttached(const UTL::COM::IUnknown *pOther) const {
    return mAttachments->IsAttached(pOther);
}

const IAttachable::List *CDActionShowcase::GetAttachments() const {
    return &mAttachments->GetList();
}

CameraAI::Action *CDActionShowcase::Construct(CameraAI::Director *director) {
    // TODO
    return nullptr;
}

CDActionShowcase::CDActionShowcase(CameraAI::Director *director, IPlayer *player)
    : CameraAI::Action(), //
      IAttachable(this) {
    // TODO
}

CDActionShowcase::~CDActionShowcase() {
    // TODO
}

void CDActionShowcase::OnDetached(IAttachable *pOther) {
    // TODO
}

void CDActionShowcase::OnCarDetached() {
    // TODO
}

void CDActionShowcase::AquireCar() {
    // TODO
}

void CDActionShowcase::Update(float dT) {
    // TODO
}
