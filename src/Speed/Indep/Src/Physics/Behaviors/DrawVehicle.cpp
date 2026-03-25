#include "DrawVehicle.h"
#include "Speed/Indep/Src/Physics/SmokeableInfo.hpp"
#include "Speed/Indep/Src/Physics/SmackableTrigger.h"

namespace RenderConn {
struct Pkt_VehicleFragment_Open : Sim::Packet {
    WUID mVehicleWorldID;
    WUID mWorldID;
    UCrc32 mPartName;
    UCrc32 mColName;

    Pkt_VehicleFragment_Open(WUID vehicleWorldID, WUID worldID, UCrc32 partName, UCrc32 colName)
        : mVehicleWorldID(vehicleWorldID), //
          mWorldID(worldID),               //
          mPartName(partName),             //
          mColName(colName) {}

    UCrc32 ConnectionClass() override {
        static UCrc32 hash = "VehicleFragmentConn";
        return hash;
    }

    unsigned int Size() override {
        return sizeof(*this);
    }

    unsigned int Type() override {
        return SType();
    }

    static unsigned int SType() {
        static UCrc32 hash = "VehicleFragment_Open";
        return hash.GetValue();
    }
};

struct Pkt_VehicleFragment_Service : Sim::Packet {
    int mInView;
    float mDistanceToView;
};
}

static int Vehicle_Part_Count;

DrawVehicle::DrawVehicle(const BehaviorParams &bp)
    : VehicleBehavior(bp, 2), //
      IRenderable(bp.fowner), //
      IModel(bp.fowner), //
      IAttachable(bp.fowner), //
      mGeometry(nullptr), //
      mAttachments(nullptr), //
      mCausality(nullptr), //
      mCausalityTime(0.0f) {
    mAttachments = new Sim::Attachments(static_cast<IAttachable *>(this));

    CollisionGeometry::IBoundable *boundable = nullptr;
    if (GetOwner()->QueryInterface(&boundable)) {
        mGeometry = boundable->GetGeometryNode();
    }
}

DrawVehicle::~DrawVehicle() {
    if (mAttachments) {
        delete mAttachments;
    }
    for (EffectList::iterator iter = mEffects.begin(); iter != mEffects.end(); iter++) {
        delete *iter;
    }
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

DrawVehicle::Part::Part(IModel *parent, WUID vehicleID, const CollisionGeometry::Bounds *geoms, const Attrib::Collection *spec, UCrc32 partname)
    : Sim::Model(parent, geoms, partname, 2), //
      ITriggerableModel(this),                //
      mTrigger(nullptr),                      //
      mAttributes(spec, 0, nullptr) {
    mAttributes.SetDefaultLayout(0x28);
    mOffScreenTask = false;
    mOffScreenTime = 0.0f;
    mVehicleID = vehicleID;
    Vehicle_Part_Count = Vehicle_Part_Count + 1;
}

DrawVehicle::Part::~Part() {
    RemoveTrigger();
    if (mOffScreenTask) {
        mOffScreenTask = false;
        mOffScreenTime = 0.0f;
    }
    Vehicle_Part_Count = Vehicle_Part_Count - 1;
}

const Attrib::Instance &DrawVehicle::Part::GetAttributes() const {
    return mAttributes;
}

void DrawVehicle::Part::GetTransform(UMath::Matrix4 &transform) const {
    ISimable *isimable = static_cast<const IModel *>(this)->GetSimable();
    if (isimable) {
        isimable->GetTransform(transform);
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
    int inView = pkt->mInView;
    *reinterpret_cast<float *>(reinterpret_cast<char *>(this) + 0x68) = pkt->mDistanceToView;
    *reinterpret_cast<int *>(reinterpret_cast<char *>(this) + 0x6C) = inView;
    return true;
}

void DrawVehicle::Part::OnBeginDraw() {}

void DrawVehicle::Part::OnEndDraw() {
    RemoveTrigger();
    if (mOffScreenTask) {
        mOffScreenTask = false;
    }
    ReleaseSequencer();
    static_cast<IModel *>(this)->ReleaseModel();
}

void DrawVehicle::Part::PlaceTrigger(const UMath::Matrix4 &matrix, bool enable) {
    CreateTrigger(matrix);
    if (!enable && mTrigger != nullptr) {
        mTrigger->Disable();
    }
}

void DrawVehicle::Part::CreateTrigger(const UMath::Matrix4 &matrix) {
    UMath::Vector3 dim;
    GetCollisionGeometry()->GetHalfDimensions(dim);
    if (VU0_v3lengthsquare(dim) <= 0.0f) {
        RemoveTrigger();
    } else if (mTrigger == nullptr) {
        mTrigger = new SmackableTrigger(GetInstanceHandle(), false, matrix, dim, 0);
    } else {
        mTrigger->Move(matrix, dim, false);
        mTrigger->Enable();
    }
}

void DrawVehicle::Part::OnProcessFrame(float dT) {
    if (mOffScreenTask) {
        const float *offscreen_allow_ptr = reinterpret_cast<const float *>(mAttributes.GetAttributePointer(0xc141f7f8, 0));
        if (!offscreen_allow_ptr) {
            offscreen_allow_ptr = reinterpret_cast<const float *>(Attrib::DefaultDataArea(sizeof(float)));
        }
        float offscreen_allow = *offscreen_allow_ptr;
        if (0.0f < offscreen_allow) {
            if (!static_cast<IModel *>(this)->InView()) {
                mOffScreenTime = mOffScreenTime + dT;
            } else {
                mOffScreenTime = 0.0f;
            }
            if (mOffScreenTime > offscreen_allow) {
                static_cast<IModel *>(this)->ReleaseModel();
            }
        }
    }
}

void DrawVehicle::Part::OnBeginSimulation() {
    if (mTrigger != nullptr) {
        mTrigger->Disable();
    }

    RenderConn::Pkt_VehicleFragment_Open pkt(mVehicleID,
                                             static_cast<IModel *>(this)->GetWorldID(),
                                             static_cast<IModel *>(this)->GetPartName(),
                                             static_cast<IModel *>(this)->GetCollisionGeometry()->fNameHash);
    BeginDraw(UCrc32(0x804c146e), &pkt);

    if (!mOffScreenTask) {
        const float *offscreen_allow_ptr = reinterpret_cast<const float *>(mAttributes.GetAttributePointer(0xc141f7f8, 0));
        if (!offscreen_allow_ptr) {
            offscreen_allow_ptr = reinterpret_cast<const float *>(Attrib::DefaultDataArea(sizeof(float)));
        }
        if (0.0f < *offscreen_allow_ptr) {
            mOffScreenTime = 0.0f;
            mOffScreenTask = true;
        }
    }

    StartSequencer(UCrc32(mAttributes.EventSequencer()));
}

void DrawVehicle::SetCausality(HCAUSE from, float time) {
    mCausality = from;
    mCausalityTime = time;
}

HCAUSE DrawVehicle::GetCausality() const {
    return mCausality;
}

IModel::Enumerator *DrawVehicle::EnumerateChildren(Enumerator *enumerator) const {
    const IAttachable::List *attachments = static_cast<const IAttachable *>(this)->GetAttachments();
    if (attachments) {
        IModel *model = static_cast< IModel * >(const_cast< DrawVehicle * >(this));
        for (IAttachable::List::const_iterator iter = attachments->begin(); iter != attachments->end(); ++iter) {
            IModel *imodel = nullptr;
            if ((*iter)->QueryInterface(&imodel) && imodel->GetParentModel() == model) {
                if (!enumerator->OnModel(imodel)) {
                    return enumerator;
                }
            }
        }
    }
    return enumerator;
}

IModel *DrawVehicle::GetChildModel(UCrc32 name) const {
    if (name != UCrc32::kNull) {
        const IAttachable::List *attachments = static_cast<const IAttachable *>(this)->GetAttachments();
        if (attachments) {
            for (IAttachable::List::const_iterator iter = attachments->begin(); iter != attachments->end(); ++iter) {
                IModel *imodel = nullptr;
                if ((*iter)->QueryInterface(&imodel) && imodel->GetPartName() == name) {
                    return imodel;
                }
            }
        }
    }
    return nullptr;
}

IModel *DrawVehicle::SpawnModel(UCrc32 name, UCrc32 collisionnode, UCrc32 attributes) {
    if (Vehicle_Part_Count < 61 && UTL::Collections::Listable< IModel, 434 >::Count() < 435 && mGeometry) {
        const CollisionGeometry::Bounds *bounds = mGeometry->GetChild(collisionnode);
        if (bounds) {
            const Attrib::Collection *attribs = SmokeableSpawner::FindAttributes(attributes);
            if (attribs) {
                Part *part = new Part(static_cast< IModel * >(this), static_cast< IModel * >(this)->GetWorldID(), bounds, attribs, name);
                if (part) {
                    return part;
                }
            }
        }
    }
    return nullptr;
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
    for (EffectList::iterator iter = mEffects.begin(); iter != mEffects.end(); iter++) {
        Effect *effect = *iter;
        delete effect;
    }
    mEffects.clear();
}

void DrawVehicle::PlayEffect(UCrc32 identifire, const Attrib::Collection *effect, const UMath::Vector3 &position, const UMath::Vector3 &magnitude,
                             bool tracking) {
    if (identifire == UCrc32::kNull) {
        return;
    }

    Effect *drawEffect = nullptr;
    for (EffectList::iterator iter = mEffects.begin(); iter != mEffects.end(); iter++) {
        if ((*iter)->Identifire == identifire) {
            drawEffect = *iter;
            break;
        }
    }

    if (!drawEffect) {
        drawEffect = ::new ("DrawVehicle", 0) Effect(identifire, static_cast< IModel * >(this)->GetWorldID(), GetAttributes().GetConstCollection());
        mEffects.push_back(drawEffect);
    }

    drawEffect->Set(effect, position, magnitude, nullptr, tracking, 0);
}

void DrawVehicle::StopEffect(UCrc32 identifire) {
    EffectList::iterator iter = mEffects.begin();
    while (iter != mEffects.end()) {
        Effect *effect = *iter;
        if (effect->Identifire == identifire) {
            delete effect;
            mEffects.erase(iter);
            return;
        }
        ++iter;
    }
}
