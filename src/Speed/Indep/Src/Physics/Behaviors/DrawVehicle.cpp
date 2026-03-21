#include "DrawVehicle.h"
#include "Speed/Indep/Src/Physics/SmackableTrigger.h"

namespace RenderConn {
struct Pkt_VehicleFragment_Service : Sim::Packet {
    int mInView;
    float mDistanceToView;
};
}

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

DrawVehicle::Effect::~Effect() {}

const Attrib::Instance &DrawVehicle::Part::GetAttributes() const {
    return mAttributes;
}

void DrawVehicle::Part::GetTransform(UMath::Matrix4 &transform) const {
    if (static_cast<const IModel *>(this)->GetSimable()) {
        static_cast<const IModel *>(this)->GetSimable()->GetTransform(transform);
    } else if (mTrigger != nullptr) {
        mTrigger->GetObjectMatrix(transform);
    } else {
        transform = UMath::Matrix4::kIdentity;
    }
}

void DrawVehicle::Part::RemoveTrigger() {
    if (mTrigger) {
        delete mTrigger;
        mTrigger = nullptr;
    }
}

bool DrawVehicle::Part::OnDraw(Sim::Packet *service) {
    RenderConn::Pkt_VehicleFragment_Service *pkt = static_cast<RenderConn::Pkt_VehicleFragment_Service *>(service);
    *reinterpret_cast<float *>(reinterpret_cast<char *>(this) + 0x68) = pkt->mDistanceToView;
    *reinterpret_cast<int *>(reinterpret_cast<char *>(this) + 0x6C) = pkt->mInView;
    return true;
}

void DrawVehicle::Part::OnBeginDraw() {}

void DrawVehicle::Part::PlaceTrigger(const UMath::Matrix4 &matrix, bool enable) {
    CreateTrigger(matrix);
    if (!enable && mTrigger != nullptr) {
        mTrigger->Disable();
    }
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

UCrc32 DrawVehicle::GetName() const {
    return GetVehicle()->GetVehicleAttributes().MODEL();
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
