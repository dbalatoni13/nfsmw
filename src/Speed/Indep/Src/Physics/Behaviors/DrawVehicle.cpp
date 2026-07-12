#include "Speed/Indep/Src/Physics/Behaviors/DrawVehicle.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Physics/SmokeableInfo.hpp"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/Render/RenderConn.h"

static int Vehicle_Part_Count = 0;

DrawVehicle::Part::Part(IModel *parent, WUID vehicleID, const CollisionGeometry::Bounds *geoms, const Attrib::Collection *spec, UCrc32 partname)
    : Sim::Model(parent, geoms, partname, 2), //
      ITriggerableModel(this),                //
      mTrigger(nullptr),                      //
      mAttributes(spec, 0, nullptr) {
    this->mOffScreenTask = false;
    this->mOffScreenTime = 0.0f;
    this->mVehicleID = vehicleID;
    Vehicle_Part_Count++;
}

DrawVehicle::Part::~Part() {
    RemoveTrigger();
    if (this->mOffScreenTask) {
        this->mOffScreenTask = false;
        this->mOffScreenTime = 0.0f;
    }
    Vehicle_Part_Count--;
}

void DrawVehicle::Part::GetTransform(UMath::Matrix4 &transform) const {
    ISimable *isimable = this->GetSimable();
    if (isimable != nullptr) {
        isimable->GetTransform(transform);
    } else if (this->mTrigger != nullptr) {
        this->mTrigger->GetObjectMatrix(transform);
    } else {
        transform = UMath::Matrix4::kIdentity;
    }
}

void DrawVehicle::Part::OnProcessFrame(float dT) {
    if (this->mOffScreenTask) {
        float offscreen_allow = this->mAttributes.KILL_OFF_SCREEN();
        if (0.0f < offscreen_allow) {
            if (!this->InView()) {
                this->mOffScreenTime += dT;
            } else {
                this->mOffScreenTime = 0.0f;
            }
            if (mOffScreenTime > offscreen_allow) {
                this->ReleaseModel();
            }
        }
    }
}

void DrawVehicle::Part::RemoveTrigger() {
    if (this->mTrigger != nullptr) {
        delete this->mTrigger;
        this->mTrigger = nullptr;
    }
}

void DrawVehicle::Part::CreateTrigger(const UMath::Matrix4 &matrix) {
    UMath::Vector3 dim;
    GetCollisionGeometry()->GetHalfDimensions(dim);
    if (UMath::LengthSquare(dim) <= 0.0f) {
        this->RemoveTrigger();
    } else if (mTrigger == nullptr) {
        this->mTrigger = new SmackableTrigger(this->GetInstanceHandle(), false, matrix, dim, 0);
    } else {
        this->mTrigger->Move(matrix, dim, false);
        this->mTrigger->Enable();
    }
}

void DrawVehicle::Part::OnBeginSimulation() {
    if (this->mTrigger != nullptr) {
        this->mTrigger->Disable();
    }

    RenderConn::Pkt_VehicleFragment_Open pkt(this->GetWorldID(), this->mVehicleID, this->GetPartName(), this->GetCollisionGeometry()->fNameHash);
    BeginDraw(UCRC32_ECSTASY, &pkt);

    if (!this->mOffScreenTask) {
        float offscreen_allow;
        offscreen_allow = this->mAttributes.KILL_OFF_SCREEN();
        if (0.0f < offscreen_allow) {
            this->mOffScreenTime = 0.0f;
            this->mOffScreenTask = true;
        }
    }

    StartSequencer(UCrc32(this->mAttributes.EventSequencer()));
}

bool DrawVehicle::Part::OnDraw(Sim::Packet *pkt) {
    RenderConn::Pkt_VehicleFragment_Service *vfs = static_cast<RenderConn::Pkt_VehicleFragment_Service *>(pkt);
    this->UpdateVisibility(vfs->InView(), vfs->DistanceToView());
    return true;
}

void DrawVehicle::Part::OnBeginDraw() {}

void DrawVehicle::Part::OnEndDraw() {
    this->RemoveTrigger();
    if (this->mOffScreenTask) {
        this->mOffScreenTask = false;
    }
    this->ReleaseSequencer();
    this->StopEffects();
}

void DrawVehicle::Part::PlaceTrigger(const UMath::Matrix4 &matrix, bool enable) {
    this->CreateTrigger(matrix);
    if (!enable && this->mTrigger != nullptr) {
        this->mTrigger->Disable();
    }
}

DrawVehicle::DrawVehicle(const BehaviorParams &bp)
    : VehicleBehavior(bp, 2),                   //
      IRenderable(bp.fowner),                   //
      IModel(this),                             //
      IAttachable(this),                        //
      mGeometry(nullptr),                       //
      mAttachments(new Sim::Attachments(this)), //
      mCausality(nullptr),                      //
      mCausalityTime(0.0f) {
    CollisionGeometry::IBoundable *boundable;
    if (this->GetOwner()->QueryInterface(&boundable)) {
        this->mGeometry = const_cast<CollisionGeometry::Bounds *>(boundable->GetGeometryNode());
    }
}

DrawVehicle::~DrawVehicle() {
    if (this->mAttachments != nullptr) {
        delete this->mAttachments;
    }
    for (EffectList::iterator iter = this->mEffects.begin(); iter != this->mEffects.end(); iter++) {
        delete *iter;
    }
}

void DrawVehicle::SetCausality(HCAUSE from, float time) {
    this->mCausality = from;
    this->mCausalityTime = time;
}

HCAUSE DrawVehicle::GetCausality() const {
    return this->mCausality;
}

// UNSOLVED regswap
IModel::Enumerator *DrawVehicle::EnumerateChildren(Enumerator *enumerator) const {
    const IAttachable::List *attachements = this->GetAttachments();
    if (attachements != nullptr) {
        for (IAttachable::List::const_iterator iter = attachements->begin(); iter != attachements->end(); iter++) {
            IAttachable *ia = *iter;
            IModel *model;
            if (ia->QueryInterface(&model) && UTL::COM::ComparePtr(model->GetParentModel(), static_cast<const IModel *>(this))) {
                if (!enumerator->OnModel(model)) {
                    return enumerator;
                }
            }
        }
    }
    return enumerator;
}

IModel *DrawVehicle::GetChildModel(UCrc32 name) const {
    if (name == UCrc32::kNull) {
        return nullptr;
    }

    const IAttachable::List *attachements = this->GetAttachments();
    if (attachements == nullptr) {
        return nullptr;
    }

    for (IAttachable::List::const_iterator iter = attachements->begin(); iter != attachements->end(); iter++) {
        IAttachable *ia = *iter;
        IModel *model;
        if (ia->QueryInterface(&model) && model->GetPartName() == name) {
            return model;
        }
    }
    return nullptr;
}

IModel *DrawVehicle::SpawnModel(UCrc32 name, UCrc32 collisionnode, UCrc32 attributes) {
    // TODO unhardcode
    if (Vehicle_Part_Count >= 61U || UTL::Collections::Listable<IModel, IModel::InstanceLimit>::Count() > 434U || this->mGeometry == nullptr) {
        return nullptr;
    }

    const CollisionGeometry::Bounds *bounds = this->mGeometry->GetChild(collisionnode);
    if (bounds == nullptr) {
        return nullptr;
    }

    const Attrib::Collection *spec = SmokeableSpawner::FindAttributes(attributes);
    if (spec == nullptr) {
        return nullptr;
    }

    return new Part(this, this->GetOwner()->GetWorldID(), bounds, spec, name);
}

void DrawVehicle::ReleaseModel() {
    this->ReleaseChildModels();
}

void DrawVehicle::ReleaseChildModels() {
    if (this->mAttachments != nullptr) {
        delete this->mAttachments;
    }
    this->mAttachments = new Sim::Attachments(this);
}

UCrc32 DrawVehicle::GetName() const {
    return this->GetVehicle()->GetVehicleAttributes().MODEL();
}

bool DrawVehicle::IsHidden() const {
    return !this->GetVehicle()->IsActive();
}

void DrawVehicle::HideModel() {
    this->GetVehicle()->Deactivate();
}

void DrawVehicle::StopEffects() {
    for (EffectList::iterator iter = this->mEffects.begin(); iter != this->mEffects.end(); iter++) {
        Effect *e = *iter;
        delete e;
    }
    this->mEffects.clear();
}

void DrawVehicle::PlayEffect(UCrc32 identifire, const Attrib::Collection *effect, const UMath::Vector3 &position, const UMath::Vector3 &magnitude,
                             bool tracking) {
    if (identifire == UCrc32::kNull) {
        return;
    }

    Effect *modeleffect = nullptr;
    for (EffectList::iterator iter = this->mEffects.begin(); iter != this->mEffects.end(); iter++) {
        Effect *e = *iter;
        if (e->Identifire == identifire) {
            modeleffect = e;
            break;
        }
    }

    if (modeleffect == nullptr) {
        modeleffect = ::new Effect(identifire, this->GetWorldID(), this->GetAttributes().GetConstCollection());
        this->mEffects.push_back(modeleffect);
    }

    modeleffect->Set(effect, position, magnitude, nullptr, tracking, 0);
}

void DrawVehicle::StopEffect(UCrc32 identifire) {
    for (EffectList::iterator iter = this->mEffects.begin(); iter != this->mEffects.end(); iter++) {
        Effect *e = *iter;
        if (e->Identifire == identifire) {
            delete e;
            this->mEffects.erase(iter);
            return;
        }
    }
}
