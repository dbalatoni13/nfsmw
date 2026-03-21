#include "DrawVehicle.h"

HMODEL DrawVehicle::GetModelHandle() const {
    return static_cast<const IModel *>(this)->GetInstanceHandle();
}

const IModel *DrawVehicle::GetModel() const {
    return this;
}

IModel *DrawVehicle::GetModel() {
    return this;
}

void DrawVehicle::OnProcessFrame(float dT) {}

void DrawVehicle::GetLinearVelocity(UMath::Vector3 &velocity) const {
    GetOwner()->GetLinearVelocity(velocity);
}

void DrawVehicle::GetAngularVelocity(UMath::Vector3 &velocity) const {
    GetOwner()->GetAngularVelocity(velocity);
}

void DrawVehicle::GetTransform(UMath::Matrix4 &transform) const {
    GetOwner()->GetTransform(transform);
}

UCrc32 DrawVehicle::GetPartName() const {
    return UCrc32::kNull;
}

WUID DrawVehicle::GetWorldID() const {
    return GetOwner()->GetWorldID();
}

const CollisionGeometry::Bounds *DrawVehicle::GetCollisionGeometry() const {
    return mGeometry;
}

const Attrib::Instance &DrawVehicle::GetAttributes() const {
    return GetOwner()->GetAttributes();
}

ISimable *DrawVehicle::GetSimable() const {
    return GetOwner();
}

IModel *DrawVehicle::GetRootModel() const {
    return const_cast<DrawVehicle *>(this);
}

IModel *DrawVehicle::GetParentModel() const {
    return nullptr;
}

bool DrawVehicle::IsRootModel() const {
    return true;
}

EventSequencer::IEngine *DrawVehicle::GetEventSequencer() {
    return GetOwner()->GetEventSequencer();
}

float DrawVehicle::GetCausalityTime() const {
    return mCausalityTime;
}

bool DrawVehicle::Attach(UTL::COM::IUnknown *pOther) {
    return mAttachments->Attach(pOther);
}

bool DrawVehicle::Detach(UTL::COM::IUnknown *pOther) {
    return mAttachments->Detach(pOther);
}

bool DrawVehicle::IsAttached(const UTL::COM::IUnknown *pOther) const {
    return mAttachments->IsAttached(pOther);
}

void DrawVehicle::OnAttached(IAttachable *pOther) {}

void DrawVehicle::OnDetached(IAttachable *pOther) {}

const IAttachable::List *DrawVehicle::GetAttachments() const {
    return reinterpret_cast<const IAttachable::List *>(mAttachments);
}

void DrawVehicle::SetCausality(HCAUSE from, float time) {
    mCausality = from;
    mCausalityTime = time;
}

HCAUSE DrawVehicle::GetCausality() const {
    return mCausality;
}

bool DrawVehicle::IsHidden() const {
    return !GetVehicle()->IsActive();
}

void DrawVehicle::HideModel() {
    GetVehicle()->Deactivate();
}

void DrawVehicle::ReleaseModel() {
    delete static_cast<IModel *>(this);
}

void DrawVehicle::ReleaseChildModels() {
    if (mAttachments) {
        delete mAttachments;
    }
    mAttachments = new Sim::Attachments(static_cast<IAttachable *>(this));
}

void DrawVehicle::StopEffects() {
    for (EffectList::iterator iter = mEffects.begin(); iter != mEffects.end(); ++iter) {
        delete *iter;
    }
    mEffects.clear();
}

void DrawVehicle::StopEffect(UCrc32 identifire) {
    for (EffectList::iterator iter = mEffects.begin(); iter != mEffects.end(); ++iter) {
        if ((*iter)->Identifire == identifire) {
            delete *iter;
            mEffects.erase(iter);
            break;
        }
    }
}
