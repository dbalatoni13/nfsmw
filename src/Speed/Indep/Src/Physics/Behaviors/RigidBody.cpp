#include "RigidBody.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/rigidbodyspecs.h"
#include "Speed/Indep/Src/Generated/Events/ECollision.hpp"
#include "Speed/Indep/Src/Generated/Hash.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Main/EventSequencer.h"
#include "Speed/Indep/Src/Physics/Dynamics.h"
#include "Speed/Indep/Src/Physics/Dynamics/Articulation.h"
#include "Speed/Indep/Src/Physics/Dynamics/Collision.h"
#include "Speed/Indep/Src/Sim/Collision.h"
#include "Speed/Indep/Src/Sim/SimSubSystem.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/World/WCollisionTri.h"
#include "Speed/Indep/bWare/Inc/bTypes.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

// total size: 0x30
class CollisionPacket {
  public:
    UMath::Vector3 lever;                  // offset 0x0, size 0xC
    const Attrib::Collection *bodysurface; // offset 0xC, size 0x4
    UMath::Vector3 normal;                 // offset 0x10, size 0xC
    float penetration;                     // offset 0x1C, size 0x4
    UMath::Vector3 arm;                    // offset 0x20, size 0xC
    const Attrib::Collection *surface;     // offset 0x2C, size 0x4

    bool operator<(const CollisionPacket &b) const {
        return this->penetration > b.penetration;
    }
};

static const bool Tweak_PrintRBWarnings = false;
static const bool Tweak_DisableGroundCollisions = false;
static const bool Tweak_DisableWorldCollisions = false;
static const bool Tweak_DisableObjectCollisions = false;

BIND_SIM_SUBSYSTEM(RigidBody, RigidBody::InitRigidBodySystem, RigidBody::ShutdownRigidBodySystem);
IMPLEMENT_SCRATCHPTR(RigidBody::Volatile);
BIND_BEHAVIOR_SPECS(rigidbodyspecs);

bTList<RigidBody> TheRigidBodies;

unsigned int RigidBody::mCount = 0;
bool RigidBody::mOnSP = false;

BIND_BEHAVIOR_FACTORY(RigidBody);

static const float AV_LIMIT = 0.0f; // TODO value
static const float V_LIMIT = 0.0f;  // TODO value

// TODO clear the magic numbers in Volatile::state and status

Behavior *RigidBody::Construct(const BehaviorParams &params) {
    const RBComplexParams bp(params.fparams.Fetch<RBComplexParams>(UCrc32(UCRC32_BASE)));
    return new RigidBody(params, bp);
}

RigidBody::Volatile::Volatile() {}

// UNSOLVED but functionally matching
RigidBody::Mesh::Mesh(const SimSurface &material, const UMath::Vector4 *verts, unsigned int count, UCrc32 name, bool persistant)
    : mVerts(nullptr),                          //
      mNumVertices(count),                      //
      mFlags(0),                                //
      mMaterial(material.GetConstCollection()), //
      mName(name) {
    if (persistant) {
        this->mVerts = const_cast<UMath::Vector4 *>(verts);
    } else {
        this->mVerts =
            reinterpret_cast<UMath::Vector4 *>(gFastMem.Alloc(static_cast<unsigned short>(count) * sizeof(*this->mVerts), "RigidBody::Mesh::Verts"));
        bMemCpy(this->mVerts, verts, static_cast<unsigned short>(count) * sizeof(*this->mVerts));
        this->mFlags |= Mesh::FREEABLE;
    }
}

RigidBody::Mesh::~Mesh() {
    if (this->mFlags & Mesh::FREEABLE) {
        gFastMem.Free(this->mVerts, this->mNumVertices * sizeof(*this->mVerts), "RigidBody::Mesh::Verts");
    }
}

void RigidBody::Mesh::Enable(bool enable) {
    if (!enable) {
        this->mFlags |= Mesh::DISABLED;
    } else {
        this->mFlags &= ~Mesh::DISABLED;
    }
}

bool RigidBody::MeshList::Create(const SimSurface &material, const UMath::Vector4 *verts, unsigned int count, UCrc32 name, bool persistant) {
    Mesh *mesh = new Mesh(material, verts, count, name, persistant);
    AddTail(mesh);
    this->mVertCount += count;
    this->mSize++;
    return true;
}

// STRIPPED
void RigidBody::PrimList::CalcRadius() {}

bool RigidBody::PrimList::Create(const UMath::Vector3 &dim, const UMath::Vector3 &offset, const SimSurface &material,
                                 Dynamics::Collision::Geometry::Shape shape, const UMath::Vector4 &orient, unsigned int flags, const UCrc32 &name) {
    Primitive *geom = new Primitive(dim, offset, material, shape, orient, flags, name);

    if (geom->GetShape() == Dynamics::Collision::Geometry::SPHERE) {
        this->AddHead(geom);
    } else {
        this->AddTail(geom);
    }

    float r = UMath::Length(dim) + UMath::Length(offset);
    this->mRadius = UMath::Max(this->mRadius, r);
    this->mSize++;
    return true;
}

void RigidBody::Primitive::Enable(bool enable) {
    if (!enable) {
        this->mFlags |= Primitive::DISABLED;
    } else {
        this->mFlags &= ~Primitive::DISABLED;
    }
}

// TODO why is the offset of bodyMatrix wrong on Xenon?
void RigidBody::Primitive::Prepare(const Volatile &data) {
    UMath::Rotate(GetOffset(), data.bodyMatrix, this->mPrevPosition);
    UMath::Add(this->mPrevPosition, data.position, this->mPrevPosition);
}

bool RigidBody::Primitive::SetCollision(const Volatile &data, Dynamics::Collision::Geometry &obb) const {
    UMath::Vector3 position;
    UMath::Vector3 dP;

    UMath::Rotate(GetOffset(), data.bodyMatrix, position);
    UMath::Add(position, data.position, position);
    UMath::Sub(position, this->mPrevPosition, dP);

    if (this->mShape == Dynamics::Collision::Geometry::SPHERE) {
        obb.Set(UMath::Matrix4::kIdentity, position, this->GetDimension(), Dynamics::Collision::Geometry::SPHERE, dP);
    } else {
        UMath::Matrix4 mat;
        UMath::Vector4 rot;
        UMath::Mult(data.orientation, this->mOrientation, rot);
        UMath::QuaternionToMatrix4(rot, mat);
        obb.Set(mat, position, this->GetDimension(), this->mShape, dP);
    }
    return true;
}

void RigidBody::InitRigidBodySystem() {}

void RigidBody::ShutdownRigidBodySystem() {}

// STRIPPED
void RigidBody::DebugSystem() {}

IMPLEMENT_SAP_GRID(RigidBody);

static char RBGrid_Memory[6912];

RBGrid *RBGrid::Add(unsigned int index, RigidBody &owner, const UMath::Vector3 &position, float radius) {
    if (index < 64) {
        void *addr = &RBGrid_Memory[index * sizeof(RBGrid)];
        return new (addr) RBGrid(owner, position, radius);
    } else {
        return nullptr;
    }
}

void RBGrid::Remove(RBGrid *grid) {
    if (grid != nullptr) {
        grid->~RBGrid();
    }
}

RigidBody::RigidBody(const BehaviorParams &bp, const RBComplexParams &params)
    : Behavior(bp, 0),                                           //
      IRigidBody(bp.fowner),                                     //
      ICollisionBody(bp.fowner),                                 //
      IDynamicsEntity(bp.fowner),                                //
      IBoundable(bp.fowner),                                     //
      mSpecs(this, 0),                                           //
      mWCollider(nullptr),                                       //
      mGeoms(params.fgeoms),                                     //
      mGroundNormal(UMath::Vector4Make(0.0f, 1.0f, 0.0f, 0.0f)), //
      mDimension(UMath::Vector3Make(1.0f, 1.0f, 1.0f)),          //
      mCollisionMask(params.fCollisionMask),                     //
      mDetachForce(0.0f),                                        //
      mInvWorldTensor(UMath::Matrix4::kIdentity) {
    mSimableType = this->GetOwner()->GetSimableType();
    this->MakeDebugable(DBG_RIGIDBODY);
    TheRigidBodies.AddTail(this);

    this->mData->force = UMath::Vector3::kZero;
    this->mData->torque = UMath::Vector3::kZero;
    this->mData->index = AssignSlot();
    this->mData->torque = UMath::Vector3::kZero;
    this->mData->mass = params.finitMass;
    this->mData->leversInContact = 0;
    this->mData->position = params.finitPos;
    this->mData->linearVel = params.factive ? params.finitVel : UMath::Vector3::kZero;
    this->mData->angularVel = params.factive ? params.finitAngVel : UMath::Vector3::kZero;
    this->mData->bodyMatrix = UMath::Matrix4::kIdentity;
    this->mData->inertiaTensor = params.finitMoment;
    this->mData->status = 0;
    this->mData->statusPrev = 0;
    this->mData->oom = 1.0f / this->mData->mass;
    this->mData->state = 0;
    this->mData->radius = UMath::Length(params.fdimension);

    this->SetOrientation(params.finitMat);
    this->mGrid = RBGrid::Add(this->mData->index, *this, params.finitPos, UMath::Length(params.fdimension));
    this->mCOG = UMath::Vector4To3(this->mSpecs.CG());
    this->mDimension.x = UMath::Max(0.001f, params.fdimension.x);
    this->mDimension.y = UMath::Max(0.001f, params.fdimension.y);
    this->mDimension.z = UMath::Max(0.001f, params.fdimension.z);

    if (!params.factive) {
        this->mData->state = 1;
    }

    if (LengthSquarexyz(this->mSpecs.DRAG()) > 0.0f) {
        this->mData->SetStatus(0x20);
    } else {
        this->mData->RemoveStatus(0x20);
    }

    if (LengthSquarexyz(this->mSpecs.DRAG_ANGULAR()) > 0.0f) {
        this->mData->SetStatus(0x800);
    } else {
        this->mData->RemoveStatus(0x800);
    }

    if (this->mSimableType == SIMABLE_VEHICLE) {
        this->mData->SetStatus(0x40);
    }

    WUID world_id = this->GetOwner()->GetWorldID();
    this->mWCollider = WCollider::Create(world_id, WCollider::kColliderShape_Cylinder, 0x1C, this->mCollisionMask);
    this->CreateGeometries();
    this->mMaps[this->mData->index] = this;
    this->mCount++;
    this->mData->SetStatus(0x200);
}

RigidBody::~RigidBody() {
    if (this->mGrid != nullptr) {
        RBGrid::Remove(this->mGrid);
        this->mGrid = nullptr;
    }
    this->mMaps[this->mData->index] = nullptr;
    this->mCount--;
    if (this->mWCollider != nullptr) {
        WCollider::Destroy(this->mWCollider);
    }
    this->mWCollider = nullptr;
    this->mMeshes.DeleteAllElements();
    this->mPrimitives.DeleteAllElements();
    Dynamics::Articulation::Release(this);
    TheRigidBodies.Remove(this);
}

void RigidBody::Reset() {
    this->mWCollider->Clear();
    this->UpdateCollider();
}

void RigidBody::SetOrientation(const UMath::Matrix4 &orientMat) {
    Volatile &data = *this->mData;
    UMath::Matrix4ToQuaternion(orientMat, data.orientation);
    UMath::Copy(orientMat, data.bodyMatrix);
    UMath::Set(data.bodyMatrix, 3, UMath::Vector4::kIdentity);
    data.inertiaTensor.GetInverseWorldTensor(data.bodyMatrix, this->mInvWorldTensor);
}

void RigidBody::SetOrientation(const UMath::Vector4 &newOrientation) {
    Volatile &data = *this->mData;
    UMath::Copy(newOrientation, data.orientation);
    UMath::QuaternionToMatrix4(data.orientation, data.bodyMatrix);
    data.inertiaTensor.GetInverseWorldTensor(data.bodyMatrix, this->mInvWorldTensor);
}

bool RigidBody::DistributeMass() {
    Volatile &data = *this->mData;
    UMath::Clear(mCOG);

    float total_volume = 0.0f;
    for (Primitive *primitive = this->mPrimitives.GetHead(); primitive != this->mPrimitives.EndOfList(); primitive = primitive->GetNext()) {
        if (primitive->GetFlags() & Primitive::VSOBJECTS) {
            const UMath::Vector3 &dimension = primitive->GetDimension();
            float volume = dimension.x * dimension.y * dimension.z * 8.0f;
            UMath::ScaleAdd(primitive->GetOffset(), volume, this->mCOG, this->mCOG);
            total_volume += volume;
        }
    }
    if (total_volume > 0.0f) {
        UMath::Scale(this->mCOG, 1.0f / total_volume, this->mCOG);
        return true;
    }
    return false;
}

void RigidBody::EnableCollisionGeometries(UCrc32 name, bool enable) {
    if (name == UCrc32::kNull) {
        return;
    }
    for (Primitive *primitive = this->mPrimitives.GetHead(); primitive != this->mPrimitives.EndOfList(); primitive = primitive->GetNext()) {
        if (primitive->GetName() == name) {
            primitive->Enable(enable);
        }
    }
    for (Mesh *mesh = this->mMeshes.GetHead(); mesh != this->mMeshes.EndOfList(); mesh = mesh->GetNext()) {
        if (mesh->GetName() == name) {
            mesh->Enable(enable);
        }
    }
}

void RigidBody::SetPosition(const UMath::Vector3 &pos) {
    this->mData->position = pos;
}

void RigidBody::SetLinearVelocity(const UMath::Vector3 &vel) {
    this->mData->linearVel = vel;
}

void RigidBody::SetAngularVelocity(const UMath::Vector3 &vel) {
    this->mData->angularVel = vel;
}

void RigidBody::SetRadius(float radius) {
    this->mData->radius = radius;
}

void RigidBody::AttachedToWorld(bool b, float detachforce) {
    if (b) {
        this->mData->SetStatus(Volatile::IS_ATTACHED_TO_WORLD);
    } else {
        this->mData->RemoveStatus(Volatile::IS_ATTACHED_TO_WORLD);
    }
    this->mDetachForce = detachforce;
}

void RigidBody::Detach() {
    if (this->mData->GetStatus(Volatile::IS_ATTACHED_TO_WORLD)) {
        this->mData->RemoveStatus(Volatile::IS_ATTACHED_TO_WORLD);
        EventSequencer::IEngine *engine = this->GetOwner()->GetEventSequencer();
        if (engine != nullptr) {
            engine->ProcessStimulus(UCRC32_WORLD_DETACHED, Sim::GetTime(), nullptr, EventSequencer::QUEUE_ALLOW);
        }
    }
}

void RigidBody::SetInertiaTensor(const UMath::Vector3 &moment) {
    this->mData->inertiaTensor = moment;
}

void RigidBody::SetMass(float newMass) {
    Volatile &data = *this->mData;
    float ratio = data.mass;
    if ((ratio != newMass) && (newMass > 0.0f)) {
        data.mass = newMass;
        data.oom = 1.0f / newMass;
        UMath::Scale(data.inertiaTensor, newMass / ratio, data.inertiaTensor);
    }
}

bool RigidBody::AddCollisionPrimitive(UCrc32 name, const UMath::Vector3 &dim, float radius, const UMath::Vector3 &offset, const SimSurface &material,
                                      const UMath::Vector4 &orient, CollisionGeometry::BoundFlags boundflags) {
    bool result = false;
    unsigned int flags = 0;
    if (boundflags & CollisionGeometry::kBounds_Disabled) {
        flags = 8;
    }
    if ((boundflags &
         (CollisionGeometry::kBounds_PrimVsGround | CollisionGeometry::kBounds_PrimVsObjects | CollisionGeometry::kBounds_PrimVsWorld))) {
        if (boundflags & CollisionGeometry::kBounds_PrimVsGround) {
            flags |= 4;
        }
        if (boundflags & CollisionGeometry::kBounds_PrimVsWorld) {
            flags |= 1;
        }
        if (boundflags & CollisionGeometry::kBounds_PrimVsObjects) {
            flags |= 2;
        }
        if (boundflags & CollisionGeometry::kBounds_Box) {
            if (this->AddCollisionBox(dim, offset, material, orient, flags, name)) {
                result = true;
            }
        } else if (boundflags & CollisionGeometry::kBounds_Sphere) {
            if (this->AddCollisionSphere(radius, offset, material, flags, name)) {
                result = true;
            }
        }
    }
    return result;
}

bool RigidBody::AddCollisionMesh(UCrc32 name, const UMath::Vector4 *verts, unsigned int count, const struct SimSurface &material,
                                 CollisionGeometry::BoundFlags flags, bool persistant) {
    return this->mMeshes.Create(material, verts, count, name, persistant);
}

void RigidBody::CreateGeometries() {
    SimSurface material = SimSurface(this->mSpecs.BASE_MATERIAL());
    if ((this->mGeoms != nullptr) && (this->mGeoms->fCollection != nullptr) && this->mGeoms->fCollection->AddTo(this, this->mGeoms, material, true)) {
        return;
    }
    if (this->mSpecs.DEFAULT_COL_BOX() == Attrib::StringKey("SPHERE")) {
        this->AddCollisionSphere(UMath::Max(this->mDimension.x, UMath::Max(this->mDimension.y, this->mDimension.z)), UMath::Vector3::kZero, material,
                                 3, UCrc32::kNull);
    } else {
        this->AddCollisionBox(this->mDimension, UMath::Vector3::kZero, material, UMath::Vector4::kIdentity, 7, UCrc32::kNull);
    }
}

void RigidBody::PlaceObject(const UMath::Matrix4 &orientMat, const UMath::Vector3 &initPos) {
    Volatile &data = *this->mData;
    data.state = 0;
    this->SetOrientation(orientMat);
    data.RemoveStatus(Volatile::IS_ATTACHED_TO_WORLD);
    UMath::Clear(data.force);
    UMath::Clear(data.torque);
    UMath::Clear(data.linearVel);
    UMath::Clear(data.angularVel);
    data.position = initPos;
    data.leversInContact = 0;
    if (this->mSimableType == SIMABLE_VEHICLE) {
        data.SetStatus(0x40);
    } else {
        data.RemoveStatus(0x40);
    }
    this->mData->statusPrev = this->mData->status;
    this->mWCollider->Clear();
    this->UpdateCollider();
    this->mData->Validate();
}

float RigidBody::GetOrientToGround() const {
    const Volatile &data = *this->mData;

    return data.bodyMatrix.v1.x * this->mGroundNormal.x + data.bodyMatrix.v1.y * this->mGroundNormal.y + data.bodyMatrix.v1.z * this->mGroundNormal.z;
}

void RigidBody::GetPointVelocity(const UMath::Vector3 &position, UMath::Vector3 &velocity) const {
    const Volatile &data = *this->mData;
    UMath::Vector3 worldArm;
    UMath::Vector3 cg;

    UMath::Rotate(this->mCOG, data.bodyMatrix, cg);
    UMath::Add(cg, data.position, cg);
    UMath::Sub(position, cg, worldArm);
    UMath::Cross(data.angularVel, worldArm, velocity);
    UMath::Add(velocity, data.linearVel, velocity);
}

void RigidBody::OnEndFrame(float dT) {
    Volatile &data = *this->mData;
    if (data.state == 0) {
        this->UpdateCollider();
        data.RemoveStatus(Volatile::HAS_HAD_WORLD_COLLISION);
        if (!this->mWCollider->IsEmpty()) {
            this->DoWorldCollisions(dT);
        }
        this->DoDrag();
        data.Validate();
        if (this->ShouldSleep()) {
            data.state = 1;
        }
    } else {
        Behavior::GetOwner()->GetWPos().Update(data.position, this->mGroundNormal, true, nullptr, true);
    }
    data.RemoveStatus(0x400);
}

void RigidBody::DisableTriggering() {
    this->mData->SetStatus(1);
}

bool RigidBody::IsTriggering() const {
    return !this->mData->GetStatus(1);
}

void RigidBody::EnableTriggering() {
    this->mData->RemoveStatus(1);
}

void RigidBody::DisableModeling() {
    this->mData->state = 2;
}

void RigidBody::EnableModeling() {
    if (this->mData->state == 2) {
        Volatile &data = *this->mData;
        data.Validate();
        data.state = 0;
        Behavior::GetOwner()->GetWPos().Update(data.position, this->mGroundNormal, false, nullptr, true);
    }
}

void RigidBody::OnBeginFrame(float dT) {
    Volatile &data = *this->mData;
    data.SetStatus(0x400);
    if (data.state != 2) {
        for (Primitive *collider = this->mPrimitives.GetHead(); collider != this->mPrimitives.EndOfList(); collider = collider->GetNext()) {
            collider->Prepare(data);
        }
        this->DoIntegration(dT);
    }
    UMath::Clear(data.force);
    UMath::Clear(data.torque);
    if (data.state == 0) {
        data.statusPrev = data.status;
        if (!data.GetStatus(Volatile::IS_ATTACHED_TO_WORLD)) {
            data.force.y += this->mSpecs.GRAVITY() * data.mass;
        }
    }
    data.Validate();
}

struct Drag : public UVector3 {
    Drag(const UVector3 &vel, const UVector3 &C, const UVector3 &A, const float q) {
        float speed = vel.Magnitude();
        UMath::Scale(vel, A, *this);
        UMath::Scale(*this, C, *this);
        UMath::Scale(*this, -0.5f * speed * q, *this);
    }
};

static const bool Tweak_EnableAngularDrag = true;

void RigidBody::DoDrag() {
    Volatile &data = *this->mData;
    if (!data.GetStatus(Volatile::IS_ATTACHED_TO_WORLD)) {
        if (data.GetStatus(0x800) && Tweak_EnableAngularDrag) {
            UMath::Vector3 avel = data.angularVel;
            ConvertWorldToLocal(avel, false);
            UMath::Vector3 cross_section;
            cross_section.x =
                this->mDimension.x * 4.0f * UMath::Sqrt(this->mDimension.y * this->mDimension.y + this->mDimension.z * this->mDimension.z);
            cross_section.y =
                this->mDimension.y * 4.0f * UMath::Sqrt(this->mDimension.x * this->mDimension.x + this->mDimension.z * this->mDimension.z);
            cross_section.z =
                this->mDimension.z * 4.0f * UMath::Sqrt(this->mDimension.x * this->mDimension.x + this->mDimension.y * this->mDimension.y);

            Drag drag((UVector3(avel)), UVector3(UMath::Vector4To3(this->mSpecs.DRAG_ANGULAR())), UVector3(cross_section), 1.225f);
            this->ConvertLocalToWorld(drag, false);
            this->ResolveTorque(drag);
        }
        if (data.GetStatus(0x20)) {
            UMath::Vector3 vel = data.linearVel;
            UMath::Vector3 area;
            UMath::Scale(this->mDimension, this->mDimension, area);
            UMath::Scale(area, 4.0f, area);
            this->ConvertWorldToLocal(vel, false);

            Drag drag((UVector3(vel)), UVector3(UMath::Vector4To3(this->mSpecs.DRAG())), UVector3(area), 1.225f);
            this->ConvertLocalToWorld(drag, false);
            this->ResolveForce(drag);
        }
    }
}

void RigidBody::ConvertLocalToWorld(UMath::Vector3 &val, bool translate) const {
    UMath::Rotate(val, this->mData->bodyMatrix, val);
    if (translate) {
        UMath::Add(val, this->mData->position, val);
    }
}

void RigidBody::ConvertWorldToLocal(UMath::Vector3 &val, bool translate) const {
    UMath::Matrix4 InvBodyMatrix;

    if (translate) {
        UMath::Sub(val, this->mData->position, val);
    }
    UMath::Transpose(this->mData->bodyMatrix, InvBodyMatrix);
    UMath::Rotate(val, InvBodyMatrix, val);
}

void RigidBody::Resolve(const UMath::Vector3 &force, const UMath::Vector3 &torque) {
    Volatile &data = *this->mData;
    if (data.state != 2) {
        data.state = 0;
        data.status |= 0x40;
        this->Detach();
        UMath::Add(force, data.force, data.force);
        UMath::Add(torque, data.torque, data.torque);
    }
}

void RigidBody::ResolveTorque(const UMath::Vector3 &torque) {
    Volatile &data = *this->mData;
    if (data.state != 2) {
        data.state = 0;
        data.status |= 0x40;
        this->Detach();
        UMath::Add(torque, data.torque, data.torque);
    }
}

void RigidBody::ResolveForce(const UMath::Vector3 &force) {
    Volatile &data = *this->mData;
    if (data.state != 2) {
        data.state = 0;
        data.status |= 0x40;
        this->Detach();
        UMath::Add(force, data.force, data.force);
    }
}

void RigidBody::ResolveTorque(const UMath::Vector3 &force, const UMath::Vector3 &p) {
    Volatile &data = *this->mData;
    if (data.state != 2) {
        data.state = 0;
        data.status |= 0x40;
        this->Detach();

        UMath::Vector3 cg;
        UMath::Vector3 torque;
        UMath::Vector3 r;
        UMath::Rotate(this->mCOG, data.bodyMatrix, cg);
        UMath::Add(cg, data.position, cg);
        UMath::Sub(p, cg, r);
        UMath::Cross(r, force, torque);
        UMath::Add(torque, data.torque, data.torque);
    }
}

void RigidBody::ResolveForce(const UMath::Vector3 &force, const UMath::Vector3 &p) {
    Volatile &data = *this->mData;
    if (data.state != 2) {
        data.state = 0;
        data.status |= 0x40;
        this->Detach();

        UMath::Vector3 cg;
        UMath::Vector3 torque;
        UMath::Vector3 r;
        UMath::Rotate(this->mCOG, data.bodyMatrix, cg);
        UMath::Add(cg, data.position, cg);
        UMath::Sub(p, cg, r);
        UMath::Cross(r, force, torque);
        UMath::Add(torque, data.torque, data.torque);
        UMath::Add(force, data.force, data.force);
    }
}

void RigidBody::Debug() {}

void RigidBody::DoIntegration(const float dT) {
    Volatile &data = *this->mData;

    if (data.state != STATE_AWAKE) {
        UMath::Clear(data.linearVel);
        UMath::Clear(data.angularVel);
        return;
    }

    data.Validate();

    UMath::Vector4 temp_quat;
    UMath::Vector4 aVel;
    UMath::Vector3 cg0;
    UMath::Vector3 cg1;
    UMath::Vector3 dcg;

    UMath::Rotate(this->mCOG, data.bodyMatrix, cg0);

    UMath::ScaleAdd(data.force, dT * data.oom, data.linearVel, data.linearVel);
    UMath::Scale(data.torque, dT, data.torque);
    UMath::ScaleAdd(data.linearVel, dT, data.position, data.position);
    UMath::Rotate(data.torque, this->mInvWorldTensor, data.torque);
    UMath::Add(data.angularVel, data.torque, data.angularVel);

    data.linearVel.x = UMath::Bound(data.linearVel.x, 300.0f);
    data.linearVel.y = UMath::Bound(data.linearVel.y, 300.0f);
    data.linearVel.z = UMath::Bound(data.linearVel.z, 300.0f);

    data.angularVel.x = UMath::Bound(data.angularVel.x, 30.0f);
    data.angularVel.y = UMath::Bound(data.angularVel.y, 30.0f);
    data.angularVel.z = UMath::Bound(data.angularVel.z, 30.0f);

    if (UMath::LengthSquare(data.angularVel) != 0.0f) {
        UMath::Scale(data.angularVel, dT, reinterpret_cast<UMath::Vector3 &>(aVel));
        aVel.w = 0.0f;
        UMath::Mult(aVel, data.orientation, temp_quat);
        UMath::ScaleAdd(temp_quat, 0.5f, data.orientation, data.orientation);

        float rlen = UMath::LengthSquare(data.orientation);
        UMath::Scale(data.orientation, 1.0f / sqrtf(rlen), data.orientation);

        UMath::QuaternionToMatrix4(data.orientation, data.bodyMatrix);
    }

    data.inertiaTensor.GetInverseWorldTensor(data.bodyMatrix, this->mInvWorldTensor);

    UMath::Rotate(this->mCOG, data.bodyMatrix, cg1);
    UMath::Sub(cg0, cg1, dcg);
    UMath::Add(data.position, dcg, data.position);

    data.Validate();
}

void RigidBody::SetAnimating(bool animating) {
    if (animating) {
        this->mData->SetStatus(0x100);
    } else {
        this->mData->RemoveStatus(0x100);
    }
}

static const bool Tweak_SingleGroundCollisions = true; // TODO use

void RigidBody::ResolveGroundCollision(const CollisionPacket *bcp, const int numContacts) {
    Volatile &data = *this->mData;
    bool first = true;
    Dynamics::Collision::Friction obj_friction;
    obj_friction.mUs = this->mSpecs.GROUND_FRICTION(0);
    obj_friction.mUk = UMath::Min(this->mSpecs.GROUND_FRICTION(1), this->mSpecs.GROUND_FRICTION(0));
    Dynamics::Collision::Plane plane;

    for (int loop = 0; loop < numContacts; ++loop, ++bcp) {
        Dynamics::Collision::Moment mo(this);
        SimSurface surface(bcp->surface);
        surface.DebugOverride();
        UMath::Add(bcp->lever, data.position, plane.point);
        plane.friction = obj_friction;
        plane.friction *= surface.GROUND_FRICTION();
        plane.normal = bcp->normal;

        UMath::Vector3 e;
        UMath::Dot(plane.normal, data.bodyMatrix, e);
        UMath::Scale(e, UMath::Vector4To3(this->mSpecs.GROUND_ELASTICITY()), e);
        mo.SetElasticity(UMath::Length(e));
        mo.SetInertiaScale(UMath::Vector4To3(this->mSpecs.GROUND_MOMENT_SCALE()));
        if (mo.React(plane, 16)) {
            if (first) {
                first = false;
                COLLISION_INFO collisionInfo;
                float force = mo.GetForce().Magnitude();
                collisionInfo.normal = plane.normal;
                collisionInfo.position = plane.point;
                collisionInfo.objA = GetOwner()->GetInstanceHandle();
                collisionInfo.objAVel = data.linearVel;
                collisionInfo.objAsurface = bcp->bodysurface;
                collisionInfo.objBsurface = surface.GetConstCollection();
                collisionInfo.type = COLLISION_INFO::GROUND;
                collisionInfo.sliding = mo.GetFrictionState() == Dynamics::Collision::Friction::Dynamic;
                collisionInfo.slidingVel = mo.GetSlidingVelocity();
                collisionInfo.closingVel = mo.GetClosingVelocity();
                collisionInfo.force = force;
                collisionInfo.impulseA = force * mo.GetOOMass();
                collisionInfo.impulseB = 0.0f;
                collisionInfo.armA = bcp->arm;
                new ECollision(collisionInfo);
            }
            data.angularVel = mo.GetAngularVelocity();
            data.linearVel = mo.GetLinearVelocity();
            data.Validate();
            break;
        }
    }
}

bool RigidBody::CanCollideWith(const RigidBody &other) const {
    if (this->mData->GetStatus(0x100) && other.mData->GetStatus(0x100)) {
        return false;
    }
    return true;
}

bool RigidBody::CanCollideWithGround() const {
    if (Tweak_DisableGroundCollisions || this->mSpecs.NO_GROUND_COLLISIONS() || !this->IsAwake()) {
        return false;
    }
    return true;
}

static const unsigned int Tweak_MaxGroundCollisionResults = 16;
static const unsigned int BOX_CORNERS = 8;

// UNSOLVED, y_vel and tolerance math twice
void RigidBody::DoInstanceCollision2d(const float dT) {
    Volatile &data = *this->mData;
    data.leversInContact = 0;

    UMath::Vector4 deepest_ground_penetration = UMath::Vector4::kZero;
    const float speedXZ = UMath::Lengthxz(data.linearVel);
    UMath::Vector3 world_cog;
    UMath::Rotate(this->mCOG, data.bodyMatrix, world_cog);
    WWorldPos world_pos = this->Behavior::GetOwner()->GetWPos();
    UMath::Vector4 world_normal = this->mGroundNormal;
    unsigned int counter = 0;
    CollisionPacket collision_packets[Tweak_MaxGroundCollisionResults];

    UMath::Vector3 y_extent;
    y_extent.x = data.bodyMatrix[0][1] * this->mDimension.x;
    y_extent.y = data.bodyMatrix[1][1] * this->mDimension.y;
    y_extent.z = data.bodyMatrix[2][1] * this->mDimension.z;

    float depth = UMath::Length(y_extent);
    CollisionPacket *bcp = collision_packets;
    const float ceiling = depth + data.position.y;

    for (Primitive *prim = this->mPrimitives.GetHead(); prim != this->mPrimitives.EndOfList(); prim = prim->GetNext()) {
        if (!prim->IsEnabled() || !(prim->GetFlags() & Primitive::VSGROUND) || prim->GetShape() != Dynamics::Collision::Geometry::BOX) {
            continue;
        }

        static const UMath::Vector4 diagnols[BOX_CORNERS] = {{-1.0f, -1.0f, -1.0f, 1.0f}, {1.0f, -1.0f, -1.0f, 1.0f}, {-1.0f, 1.0f, -1.0f, 1.0f},
                                                             {1.0f, 1.0f, -1.0f, 1.0f},   {-1.0f, -1.0f, 1.0f, 1.0f}, {1.0f, -1.0f, 1.0f, 1.0f},
                                                             {-1.0f, 1.0f, 1.0f, 1.0f},   {1.0f, 1.0f, 1.0f, 1.0f}};

        const Attrib::Collection *bodysuface = prim->GetMaterial();
        const UMath::Vector3 &dim = prim->GetDimension();

        UMath::Matrix4 ortho_scaled_matrix;
        UMath::Matrix4 world_scaled_matrix;
        UMath::QuaternionToMatrix4(prim->GetOrientation(), ortho_scaled_matrix);
        UMath::Scale(ortho_scaled_matrix, dim);
        ortho_scaled_matrix.v3 = UMath::Vector4Make(prim->GetOffset(), 1.0f);
        UMath::Mult(ortho_scaled_matrix, data.bodyMatrix, world_scaled_matrix);

        UMath::Vector4 world_arms[BOX_CORNERS];
        UMath::RotateTranslate(diagnols, world_scaled_matrix, world_arms, BOX_CORNERS);

        for (int loop = 0; loop < BOX_CORNERS && counter < Tweak_MaxGroundCollisionResults; ++loop) {
            UMath::Vector3 world_point;
            const UMath::Vector3 &world_arm = UMath::Vector4To3(world_arms[loop]);

            UMath::Add(world_arm, data.position, world_point);

            float y_vel = (world_arm.x - world_cog.x) * data.angularVel.z + data.linearVel.y;
            y_vel = data.angularVel.x * (world_arm.z - world_cog.z) - y_vel;

            float tolerance = speedXZ * dT + depth;
            tolerance += UMath::Max(y_vel * dT, 0.0f);
            tolerance = UMath::Clamp(ceiling - world_point.y, 0.25f, tolerance);

            world_pos.SetTolerance(tolerance);
            world_pos.Update(world_point, world_normal, true, this->mWCollider, true);

            if (world_pos.OnValidFace() && world_normal.w > UMath::Epsilon) {
                if (world_normal.w > deepest_ground_penetration.w) {
                    deepest_ground_penetration = world_normal;
                }

                counter++;
                UMath::RotateTranslate(UMath::Vector4To3(diagnols[loop]), ortho_scaled_matrix, bcp->arm);
                bcp->lever = world_arm;
                bcp->normal = UMath::Vector4To3(world_normal);
                bcp->penetration = world_normal.w;
                bcp->surface = world_pos.GetSurface();
                bcp->bodysurface = bodysuface;
                bcp++;
            }
        }
    }

    for (Mesh *mesh = this->mMeshes.GetHead(); mesh != this->mMeshes.EndOfList() && counter < Tweak_MaxGroundCollisionResults;
         mesh = mesh->GetNext()) {
        if (!mesh->IsEnabled()) {
            continue;
        }

        const Attrib::Collection *bodysuface = mesh->GetMaterial();
        int limit = mesh->GetNumVertices();
        UMath::Vector4 world_arms[Tweak_MaxGroundCollisionResults];

        UMath::RotateTranslate(mesh->GetOrtho(), data.bodyMatrix, world_arms, limit);

        for (int loop = 0; loop < limit && counter < Tweak_MaxGroundCollisionResults; ++loop) {
            const UMath::Vector3 &ortho = UMath::Vector4To3(mesh->GetOrtho(loop));
            UMath::Vector3 world_point;
            const UMath::Vector3 &world_arm = UMath::Vector4To3(world_arms[loop]);

            UMath::Add(world_arm, data.position, world_point);

            float y_vel = data.angularVel.z * (world_arm.x - world_cog.x) + data.linearVel.y;
            y_vel = data.angularVel.x * (world_arm.z - world_cog.z) - y_vel;

            float tolerance = speedXZ * dT + depth;
            tolerance += UMath::Max(y_vel * dT, 0.0f);
            tolerance = UMath::Clamp(ceiling - world_point.y, 0.25f, tolerance);

            world_pos.SetTolerance(tolerance);
            world_pos.Update(world_point, world_normal, true, this->mWCollider, true);

            if (world_pos.OnValidFace() && world_normal.w > UMath::Epsilon) {
                if (world_normal.w > deepest_ground_penetration.w) {
                    deepest_ground_penetration = world_normal;
                }

                counter++;
                bcp->lever = world_arm;
                bcp->normal = UMath::Vector4To3(world_normal);
                bcp->penetration = world_normal.w;
                bcp->arm = ortho;
                bcp->surface = world_pos.GetSurface();
                bcp->bodysurface = bodysuface;
                bcp++;
            }
        }
    }

    if (counter != 0) {
        data.leversInContact = counter;

        if (counter > 1) {
            std::sort(collision_packets, collision_packets + counter);
        }

        this->ResolveGroundCollision(collision_packets, UMath::Min(Tweak_MaxGroundCollisionResults, counter));

        if (deepest_ground_penetration.w > 0.0f) {
            UMath::ScaleAdd(UMath::Vector4To3(deepest_ground_penetration), deepest_ground_penetration.w, data.position, data.position);
        }
    }
}

void RigidBody::ModifyCollision(const SimSurface &other, const Dynamics::Collision::Plane &plane, Dynamics::Collision::Moment &myMoment) {
    myMoment.SetInertiaScale(UMath::Vector4To3(this->mSpecs.WORLD_MOMENT_SCALE()));
}

bool RigidBody::IsImmobile() const {
    if (this->mSpecs.IMMOBILE_OBJECT_COLLISIONS() || this->mData->GetStatus(Volatile::IS_ATTACHED_TO_WORLD)) {
        return true;
    }
    return false;
}

void RigidBody::ModifyCollision(const RigidBody &other, const Dynamics::Collision::Plane &plane, Dynamics::Collision::Moment &myMoment) {
    Volatile &data = *this->mData;
    if (!other.IsImmobile()) {
        if (this->mSpecs.IMMOBILE_OBJECT_COLLISIONS()) {
            myMoment.MakeImmobile(true, 0.0f);
        } else {
            if (data.GetStatus(Volatile::IS_ATTACHED_TO_WORLD)) {
                myMoment.MakeImmobile(true, mDetachForce);
            }
        }
    }
    myMoment.SetInertiaScale(UMath::Vector4To3(this->mSpecs.OBJ_MOMENT_SCALE()));
    if (this->mSpecs.OBJ_MOMENT_SCALE().w > 0.0f) {
        myMoment.SetMass(myMoment.GetMass() * this->mSpecs.OBJ_MOMENT_SCALE().w);
    }
    if (data.GetStatus(Volatile::IS_ANCHORED)) {
        myMoment.SetFixedCG(true);
    }
}

bool RigidBody::Separate(RigidBody &objA, bool objAImmobile, RigidBody &objB, bool objBImmobile, const UMath::Vector3 &normal, UMath::Vector3 &point,
                         float overlap, bool APenetratesB) {
    if (overlap <= UMath::Epsilon) {
        return false;
    }
    Volatile &dataA = *objA.mData;
    Volatile &dataB = *objB.mData;
    UMath::Vector3 deltaA;
    UMath::Vector3 deltaB;

    if (objBImmobile != objAImmobile) {
        float moveB;
        float moveA;
        if (objBImmobile) {
            moveB = 0.0f;
            moveA = 1.0f;
        } else {
            moveA = 0.0f;
            moveB = 1.0f;
        }
        UMath::Scale(normal, moveA * overlap, deltaA);
        UMath::Scale(normal, moveB * -overlap, deltaB);
    } else {
        if (objAImmobile)
            return false;
        float normal_speedA = -UMath::Dot(dataA.linearVel, normal);
        float normal_speedB = UMath::Dot(dataB.linearVel, normal);
        float closingspeed = normal_speedA + normal_speedB;
        if ((UMath::Abs(closingspeed) > UMath::Epsilon) && (objA.mSimableType == objB.mSimableType) && (objA.mSimableType == SIMABLE_SMACKABLE)) {
            float t = overlap / closingspeed;
            float xA = normal_speedA * t;
            float xB = normal_speedB * t;
            float moveA;
            float moveB;
            if (closingspeed > 0.0f) {
                moveA = xA / overlap;
                moveB = xB / overlap;
            } else {
                moveA = xB / overlap;
                moveB = xA / overlap;
            }
            if (moveA > 1.0f) {
                moveA = 1.0f;
                moveB = 0.0f;
            } else if (moveB > 1.0f) {
                moveB = 1.0f;
                moveA = 0.0f;
            }
            UMath::Scale(normal, moveA * overlap, deltaA);
            UMath::Scale(normal, moveB * -overlap, deltaB);
        } else {
            float moveB = dataA.mass / (dataA.mass + dataB.mass);
            float moveA = 1.0f - moveB;
            UMath::Scale(normal, moveA * overlap, deltaA);
            UMath::Scale(normal, moveB * -overlap, deltaB);
        }
    }
    UMath::Add(dataA.position, deltaA, dataA.position);
    UMath::Add(dataB.position, deltaB, dataB.position);
    if (APenetratesB) {
        UMath::Add(point, deltaA, point);
    } else {
        UMath::Add(point, deltaB, point);
    }
    return true;
}

bool RigidBody::ResolveObjectCollision(RigidBody &objA, RigidBody &objB, const Primitive &colliderA, const Primitive &colliderB,
                                       const UMath::Vector3 &collisionNormal, const UMath::Vector3 &collisionPoint, float overlap,
                                       bool APenetratesB) {
    bool result = false;
    const Attrib::Gen::rigidbodyspecs &specsA = *objA.mSpecs;
    const Attrib::Gen::rigidbodyspecs &specsB = *objB.mSpecs;
    bool shouldresolve = true;

    if (!objA.DoPenetration(objB)) {
        shouldresolve = false;
    }

    if (!objB.DoPenetration(objA)) {
        shouldresolve = false;
    }

    if (!shouldresolve) {
        return result;
    }

    Volatile &dataA = *objA.mData;
    Volatile &dataB = *objB.mData;
    Dynamics::Collision::Moment moA(&objA);
    Dynamics::Collision::Moment moB(&objB);
    dataA.SetStatus(8);
    dataB.SetStatus(8);

    const float Ks = UMath::Sqrt(specsA.OBJ_FRICTION(0) * specsB.OBJ_FRICTION(0));
    const float Kd = UMath::Sqrt(specsA.OBJ_FRICTION(1) * specsB.OBJ_FRICTION(1));

    Dynamics::Collision::Plane plane;
    plane.friction = Dynamics::Collision::Friction(UMath::Min(Ks, Kd), Ks);
    plane.normal = collisionNormal;
    plane.point = collisionPoint;

    UMath::Vector3 eA;
    UMath::Dot(plane.normal, dataA.bodyMatrix, eA);
    UMath::Scale(eA, UMath::Vector4To3(specsA.OBJ_ELASTICITY()), eA);
    moA.SetElasticity(UMath::Length(eA));

    UMath::Vector3 eB;
    UMath::Dot(plane.normal, dataB.bodyMatrix, eB);
    UMath::Scale(eB, UMath::Vector4To3(specsB.OBJ_ELASTICITY()), eB);
    moB.SetElasticity(UMath::Length(eB));

    objA.ModifyCollision(objB, plane, moA);
    objB.ModifyCollision(objA, plane, moB);

    if (moA.IsImmobile() && moB.IsImmobile()) {
        if (moA.GetBreakingForce() > 0.0f) {
            moA.MakeImmobile(false, 0.0f);
        }
        if (moB.GetBreakingForce() > 0.0f) {
            moB.MakeImmobile(false, 0.0f);
        }
    }

    if (!RigidBody::Separate(objA, moA.IsImmobile(), objB, moB.IsImmobile(), plane.normal, plane.point, overlap, APenetratesB)) {
        return false;
    }

    moA.SetPosition(dataA.position);
    moB.SetPosition(dataB.position);

    if (moA.React(moB, plane, 32)) {
        COLLISION_INFO collisionInfo;
        float force = moA.GetForce().Magnitude();

        collisionInfo.type = 1;
        collisionInfo.normal = collisionNormal;
        collisionInfo.position = collisionPoint;
        collisionInfo.sliding =
            moA.GetFrictionState() == Dynamics::Collision::Friction::Dynamic || moB.GetFrictionState() == Dynamics::Collision::Friction::Dynamic;
        collisionInfo.slidingVel = moA.GetSlidingVelocity();
        collisionInfo.closingVel = moA.GetClosingVelocity();
        collisionInfo.force = force;
        collisionInfo.objAVel = dataA.linearVel;
        collisionInfo.objBVel = dataB.linearVel;

        if (!moA.IsImmobile()) {
            dataA.angularVel = moA.GetAngularVelocity();
            dataA.linearVel = moA.GetLinearVelocity();
            dataA.state = 0;
            dataA.SetStatus(0x40);
            collisionInfo.impulseA = force * moA.GetOOMass();
        } else {
            collisionInfo.objAImmobile = true;
            collisionInfo.impulseA = force * moB.GetOOMass();
        }

        if (!moB.IsImmobile()) {
            dataB.angularVel = moB.GetAngularVelocity();
            dataB.linearVel = moB.GetLinearVelocity();
            dataB.state = 0;
            dataB.SetStatus(0x40);
            collisionInfo.impulseB = force * moB.GetOOMass();
        } else {
            collisionInfo.objBImmobile = true;
            collisionInfo.impulseB = force * moA.GetOOMass();
        }

        UMath::Matrix4 aInvMat;
        UMath::Matrix4 bInvMat;

        UMath::Transpose(dataA.bodyMatrix, aInvMat);
        UMath::Transpose(dataB.bodyMatrix, bInvMat);

        collisionInfo.objA = objA.GetOwner()->GetInstanceHandle();
        collisionInfo.objAsurface = colliderA.GetMaterial();
        UMath::Sub(collisionPoint, dataA.position, collisionInfo.armA);
        UMath::Rotate(collisionInfo.armA, aInvMat, collisionInfo.armA);

        collisionInfo.objB = objB.GetOwner()->GetInstanceHandle();
        collisionInfo.objBsurface = colliderB.GetMaterial();
        UMath::Sub(collisionPoint, dataB.position, collisionInfo.armB);
        UMath::Rotate(collisionInfo.armB, bInvMat, collisionInfo.armB);

        collisionInfo.objADetached = !collisionInfo.objAImmobile && dataA.GetStatus(Volatile::IS_ATTACHED_TO_WORLD);
        collisionInfo.objBDetached = !collisionInfo.objBImmobile && dataB.GetStatus(Volatile::IS_ATTACHED_TO_WORLD);

        new ECollision(collisionInfo);

        if (collisionInfo.objADetached) {
            objA.Detach();
        }
        if (collisionInfo.objBDetached) {
            objB.Detach();
        }
        result = true;
    }

    return result;
}

static const bool Tweak_TransferGroundImpulses = false; // TODO use

bool RigidBody::ResolveWorldOBBCollision(const UMath::Vector3 &cn, const UMath::Vector3 &cp, COLLISION_INFO *collisionInfo,
                                         const Dynamics::Collision::Geometry *otherGeom, const UMath::Vector3 &linearVel, const SimSurface &rbsurface,
                                         const SimSurface &obbsurface) {
    const float otherMass = 1000000.0f;
    Volatile &data = *this->mData;
    Dynamics::Collision::Moment moA(this);
    UMath::Vector3 inertia =
        Dynamics::Inertia::Box(1000000.0f, 2 * otherGeom->GetDimension().x, 2 * otherGeom->GetDimension().y, 2 * otherGeom->GetDimension().z);
    UMath::Vector3 cg = {};
    UMath::Vector3 angularVel = {};
    Dynamics::Collision::Moment moB(otherGeom->GetOrientation(), otherMass, inertia, cg, linearVel, angularVel, otherGeom->GetPosition());

    Dynamics::Collision::Plane plane;
    plane.friction =
        Dynamics::Collision::Friction(UMath::Min(this->mSpecs.OBJ_FRICTION(0), this->mSpecs.OBJ_FRICTION(1)), this->mSpecs.OBJ_FRICTION(0));
    plane.normal = cn;
    plane.point = UVector3(cp.x, cp.y, cp.z);
    plane.friction *= obbsurface.WORLD_FRICTION();

    UMath::Vector3 e;
    UMath::Dot(plane.normal, data.bodyMatrix, e);
    UMath::Scale(e, UMath::Vector4To3(this->mSpecs.WALL_ELASTICITY()), e);
    moA.SetElasticity(UMath::Length(e));

    this->ModifyCollision(obbsurface, plane, moA);

    moA.MakeImmobile(false, 0.0f);
    moB.MakeImmobile(true, 0.0f);

    if (moA.React(moB, plane, 32)) {
        float force = moA.GetForce().Magnitude();

        collisionInfo->objAVel = data.linearVel;
        collisionInfo->objBVel = linearVel;
        collisionInfo->objAsurface = rbsurface.GetConstCollection();
        collisionInfo->objA = this->GetOwner()->GetInstanceHandle();
        collisionInfo->type = 2;
        collisionInfo->objBsurface = obbsurface.GetConstCollection();
        collisionInfo->normal = cn;
        collisionInfo->position = cp;
        collisionInfo->sliding = moA.GetFrictionState() == Dynamics::Collision::Friction::Dynamic;
        collisionInfo->slidingVel = moA.GetSlidingVelocity();
        collisionInfo->closingVel = moA.GetClosingVelocity();
        collisionInfo->force = force;
        collisionInfo->impulseA = force * moA.GetOOMass();
        collisionInfo->impulseB = 0.0f;

        UMath::Sub(cp, data.position, collisionInfo->armA);
        this->ConvertWorldToLocal(collisionInfo->armA, false);

        data.angularVel = moA.GetAngularVelocity();
        data.linearVel = moA.GetLinearVelocity();
        data.state = 0;
        data.Validate();
        return true;
    }
    return false;
}

float RigidBody::ResolveWorldCollision(const UMath::Vector3 &cn, const UMath::Vector3 &cp, COLLISION_INFO *collisionInfo,
                                       const Attrib::Collection *objSurface, const SimSurface &worldSurface, const UVector3 &worldVel) {
    Volatile &data = *this->mData;
    const Attrib::Gen::rigidbodyspecs &specs = *this->mSpecs;
    Dynamics::Collision::Moment mo(this);

    Dynamics::Collision::Plane plane;
    plane.friction = Dynamics::Collision::Friction(UMath::Min(specs.WALL_FRICTION(1), specs.WALL_FRICTION(0)), specs.WALL_FRICTION(0));
    plane.normal = cn;
    plane.point = cp;
    plane.friction *= worldSurface.WORLD_FRICTION();

    UMath::Vector3 e;
    UMath::Dot(plane.normal, data.bodyMatrix, e);
    UMath::Scale(e, UMath::Vector4To3(specs.WALL_ELASTICITY()), e);
    mo.SetElasticity(UMath::Length(e));
    this->ModifyCollision(worldSurface, plane, mo);
    mo.SetLinearVelocity(mo.GetLinearVelocity() - worldVel);
    if (mo.React(plane, 16)) {
        float force = mo.GetForce().Magnitude();
        if (collisionInfo != nullptr) {
            collisionInfo->normal = cn;
            collisionInfo->position = cp;
            collisionInfo->objA = this->GetOwner()->GetInstanceHandle();
            collisionInfo->objAVel = data.linearVel;
            collisionInfo->objAsurface = objSurface;
            collisionInfo->type = 2;
            collisionInfo->objBsurface = worldSurface.GetConstCollection();
            collisionInfo->sliding = mo.GetFrictionState() == Dynamics::Collision::Friction::Dynamic;
            collisionInfo->slidingVel = mo.GetSlidingVelocity();
            collisionInfo->closingVel = mo.GetClosingVelocity();
            collisionInfo->force = force;
            collisionInfo->impulseA = force * mo.GetOOMass();
            collisionInfo->impulseB = 0.0f;
            UMath::Sub(cp, data.position, collisionInfo->armA);
            this->ConvertWorldToLocal(collisionInfo->armA, false);
        }
        data.angularVel = mo.GetAngularVelocity();
        data.linearVel = mo.GetLinearVelocity() + worldVel;
        data.Validate();
        return force;
    }
    return 0.0f;
}

void RigidBody::OnObjectOverlap(RigidBody &objA, RigidBody &objB, float dT) {
    Volatile &dataA = *objA.mData;
    Volatile &dataB = *objB.mData;
    float comp = objA.mPrimitives.GetRadius() + objB.mPrimitives.GetRadius();
    comp = UMath::Max(1.0f, comp * comp);

    if (UMath::DistanceSquare(dataA.position, dataB.position) > comp) {
        return;
    }
    if (!objA.CanCollideWith(objB) || !objB.CanCollideWith(objA)) {
        return;
    }
    HSIMABLE hA = objA.GetOwner()->GetInstanceHandle();
    HSIMABLE hB = objB.GetOwner()->GetInstanceHandle();
    Dynamics::Collision::Geometry geomA;
    Dynamics::Collision::Geometry geomB;
    for (Primitive *iter1 = objA.mPrimitives.GetHead(); iter1 != objA.mPrimitives.EndOfList(); iter1 = iter1->GetNext()) {
        Primitive &colliderA = *iter1;
        if (!(colliderA.GetFlags() & 2) || (colliderA.GetFlags() & 8)) {
            continue;
        }
        if (!colliderA.SetCollision(dataA, geomA)) {
            continue;
        }
        for (Primitive *iter2 = objB.mPrimitives.GetHead(); iter2 != objB.mPrimitives.EndOfList(); iter2 = iter2->GetNext()) {
            Primitive &colliderB = *iter2;
            if (!(colliderB.GetFlags() & 2) || (colliderB.GetFlags() & 8)) {
                continue;
            }
            if (colliderB.SetCollision(dataB, geomB) && Dynamics::Collision::Geometry::FindIntersection(&geomA, &geomB, &geomA)) {
                const UMath::Vector3 &collisionNormal = geomA.GetCollisionNormal();
                const UMath::Vector3 &collisionPoint = geomA.GetCollisionPoint();
                float overlap = -geomA.GetOverlap();
                ResolveObjectCollision(objA, objB, colliderA, colliderB, collisionNormal, collisionPoint, overlap, geomA.PenetratesOther());
            }
        }
    }
}

void RigidBody::UpdateCollider() {
    Volatile &data = *this->mData;
    float radius = UMath::Max(this->mPrimitives.GetRadius(), 3.0f);
    this->mWCollider->Refresh(data.position, radius, true);
    Behavior::GetOwner()->GetWPos().Update(data.position, this->mGroundNormal, true, this->mWCollider, true);
}

bool RigidBody::CanCollideWithWorld() const {
    const Volatile &data = *this->mData;

    if (!Tweak_DisableWorldCollisions && data.GetStatus(0x40) && !this->mSpecs.NO_WORLD_COLLISIONS() && !this->IsSleeping()) {
        if (this->mPrimitives.Size() == 0) {
            return false;
        }
        return true;
    }
    return false;
}

bool RigidBody::OnWCollide(const WCollisionMgr::WorldCollisionInfo &cInfo, const UMath::Vector3 &cPoint, void *userdata) {
    Volatile &data = *this->mData;
    data.SetStatus(Volatile::HAS_HAD_WORLD_COLLISION);
#ifdef EA_BUILD_A124
    SimSurface surface(cInfo.fBle.fSurfaceHash.GetValue());
#else
    SimSurface surface(cInfo.fBle.fSurfaceRef);
#endif
    surface.DebugOverride();
    UVector3 wV;
    if (cInfo.fNormal.w > 0.0f) {
        UVector3 dp(cInfo.fNormal);
        dp *= cInfo.fNormal.w;
        UMath::Add(data.position, dp, data.position);
    }
    COLLISION_INFO collisionInfo;

    if (this->ResolveWorldCollision(UVector3(cInfo.fNormal), cPoint, &collisionInfo, reinterpret_cast<Primitive *>(userdata)->GetMaterial(), surface,
                                    wV) > 0.0f) {
        new ECollision(collisionInfo);
    }
    return true;
}

void RigidBody::DoWorldCollisions(const float dT) {
    if (this->CanCollideWithWorld()) {
        this->DoObbCollision(dT);
        this->DoBarrierCollision(dT);
    }
    if (this->CanCollideWithGround() && !this->mWCollider->GetInstanceList().empty()) {
        this->DoInstanceCollision(dT);
    }
}

void RigidBody::DoBarrierCollision(float dT) {
    if (this->mWCollider->GetBarrierList().empty()) {
        return;
    }

    Volatile &data = *this->mData;
    WCollisionMgr::WorldCollisionInfo cInfo;
    Dynamics::Collision::Geometry thisGeom;
    const WCollisionBarrierList &barriers = this->mWCollider->GetBarrierList();
    bool testall = true;

    if (this->mPrimitives.Size() > 1) {
        UVector3 dVdT(data.linearVel);

        dVdT *= dT;

        float rad = this->mPrimitives.GetRadius();
        UVector3 dim(rad, rad, rad);

        thisGeom.Set(data.bodyMatrix, data.position, dim, Dynamics::Collision::Geometry::SPHERE, dVdT);
        testall = WCollisionMgr(this->mCollisionMask, 3).Collide(&thisGeom, &barriers, nullptr, nullptr, false) == true;
    }

    if (testall) {
        for (Primitive *collider = this->mPrimitives.GetHead(); collider != this->mPrimitives.EndOfList(); collider = collider->GetNext()) {
            if (((collider->GetFlags() ^ 1) & 1) != 0) {
                continue;
            }
            if ((collider->GetFlags() & 8) != 0) {
                continue;
            }
            if (!collider->SetCollision(data, thisGeom)) {
                continue;
            }

            bool force_single_sided = (collider->GetFlags() & Primitive::ONESIDED) != 0;
            WCollisionMgr(this->mCollisionMask, 3).Collide(&thisGeom, &barriers, this, collider, force_single_sided);
        }
    }
}

void RigidBody::DoInstanceCollision(float dT) {
    if (!this->mSpecs.INSTANCE_COLLISIONS_3D()) {
        this->DoInstanceCollision2d(dT);
    } else {
        this->DoInstanceCollision3d(dT);
    }
}

void RigidBody::DoInstanceCollision3d(float dT) {
    Volatile &data = *this->mData;
    WCollisionMgr::WorldCollisionInfo cInfo;
    Dynamics::Collision::Geometry thisGeom;
    const WCollisionInstanceCacheList &instances = this->mWCollider->GetInstanceList();
    for (Primitive *collider = this->mPrimitives.GetHead(); collider != this->mPrimitives.EndOfList(); collider = collider->GetNext()) {
        if ((((collider->GetFlags() ^ 1) & 1) == 0) && !(collider->GetFlags() & 8) && collider->SetCollision(data, thisGeom)) {
            WCollisionMgr(this->mCollisionMask, 3).Collide(&thisGeom, &instances, this, collider);
        }
    }
}

void RigidBody::DoObbCollision(float dT) {
    if (this->mWCollider->GetObbList().empty()) {
        return;
    }

    Volatile &data = *this->mData;
    WCollisionMgr::WorldCollisionInfo cInfo;
    Dynamics::Collision::Geometry thisGeom;
    Dynamics::Collision::Geometry otherGeom;
    const WCollisionObjectList &obbObjectList = this->mWCollider->GetObbList();

    for (WCollisionObjectList::const_iterator it = obbObjectList.begin(); it != obbObjectList.end(); ++it) {
        const WCollisionObject &obbObject = **it;
        UMath::Vector3 objectVel;
        WSurface surface;

        WCollisionMgr(this->mCollisionMask, 3).BuildGeomFromWorldObb(obbObject, dT, otherGeom, objectVel, surface);

        for (Primitive *collider = this->mPrimitives.GetHead(); collider != this->mPrimitives.EndOfList(); collider = collider->GetNext()) {
            if (((collider->GetFlags() ^ Primitive::VSWORLD) & Primitive::VSWORLD) != 0) {
                continue;
            }
            if ((collider->GetFlags() & Primitive::DISABLED) != 0) {
                continue;
            }
            if (!collider->SetCollision(data, thisGeom)) {
                continue;
            }

            if (Dynamics::Collision::Geometry::FindIntersection(&thisGeom, &otherGeom, &thisGeom)) {
                const UMath::Vector3 &collisionNormal = thisGeom.GetCollisionNormal();
                const UMath::Vector3 &collisionPoint = thisGeom.GetCollisionPoint();
                float overlap = -thisGeom.GetOverlap();
                COLLISION_INFO collisionInfo;

                collisionInfo.objA = this->GetOwner()->GetInstanceHandle();

                data.SetStatus(Volatile::HAS_HAD_OBJECT_COLLISION);
                UMath::ScaleAdd(collisionNormal, overlap, data.position, data.position);

                if (this->ResolveWorldOBBCollision(collisionNormal, collisionPoint, &collisionInfo, &otherGeom, objectVel,
                                                   SimSurface(collider->GetMaterial()), surface.GetSimSurface())) {
                    data.SetStatus(Volatile::HAS_HAD_WORLD_COLLISION);
                    new ECollision(collisionInfo);
                }
            }
        }
    }
}

bool RigidBody::ShouldSleep() const {
    const Volatile &data = *this->mData;
    float sumVel = UMath::Length(data.linearVel) + (UMath::Length(data.angularVel) * data.radius);
    if (sumVel < this->mSpecs.SLEEP_VELOCITY() && this->GetNumContactPoints() > 2) {
        return true;
    }
    return false;
}

void RigidBody::Accelerate(const UMath::Vector3 &a, float dT) {
    Volatile &data = *this->mData;
    if (data.state == 0) {
        UMath::ScaleAdd(a, dT, data.linearVel, data.linearVel);
    }
}

bool RigidBody::AddCollisionSphere(float radius, const UMath::Vector3 &offset, const SimSurface &material, unsigned int flags, const UCrc32 &name) {
    return this->mPrimitives.Create(UVector3(radius, radius, radius), offset, material, Dynamics::Collision::Geometry::SPHERE,
                                    UMath::Vector4::kIdentity, flags | 0x10, name);
}

bool RigidBody::AddCollisionBox(const UMath::Vector3 &dim, const UMath::Vector3 &offset, const SimSurface &material, const UMath::Vector4 &orient,
                                unsigned int flags, const UCrc32 &name) {
    return this->mPrimitives.Create(dim + this->mSpecs.COLLISION_BOX_PAD(), offset, material, Dynamics::Collision::Geometry::BOX, orient, flags,
                                    name);
}

bool RigidBody::CanCollideWithObjects() const {
    if (this->mPrimitives.Size() == 0) {
        return false;
    }
    if (this->mSpecs.NO_OBJ_COLLISIONS()) {
        return false;
    }
    if (this->mData->state == 2) {
        return false;
    }
    return true;
}

void RigidBody::OnTaskSimulate(float dT) {}

void RigidBody::OnDebugDraw() {}

unsigned int RigidBody::GetTriggerFlags() const {
    if (this->mData->GetStatus(1) || this->IsSleeping()) {
        return 0;
    }
    return 0x40;
}

void RigidBody::PushSP(void *workspace) {
    mOnSP = true;
    ScratchPtr<RigidBody::Volatile>::Push(workspace);
}

void RigidBody::PopSP() {
    mOnSP = false;
    ScratchPtr<RigidBody::Volatile>::Pop();
}

void RigidBody::Update(const float dT) {
    int overlapx = 0;
    int overlapz = 0;

    for (RigidBody *body = TheRigidBodies.GetHead(); body != TheRigidBodies.EndOfList(); body = body->GetNext()) {
        body->OnBeginFrame(dT);
        body->UpdateGrid(overlapx, overlapz);
    }

    unsigned int numit = SAP::Grid<RigidBody>::Sweep();
    SAP::Grid<RigidBody>::Prune(overlapx < overlapz, RigidBody::OnObjectOverlap, dT);

    for (RigidBody *body = TheRigidBodies.GetHead(); body != TheRigidBodies.EndOfList(); body = body->GetNext()) {
        body->OnEndFrame(dT);
    }

    Dynamics::Articulation::Resolve();
}

void RigidBody::Damp(float amount) {
    Volatile &data = *this->mData;
    float scale = 1.0f - amount;
    UMath::Scale(data.linearVel, scale, data.linearVel);
    UMath::Scale(data.angularVel, scale, data.angularVel);
    UMath::Scale(data.force, scale, data.force);
    UMath::Scale(data.torque, scale, data.torque);
}

void RigidBody::UpdateGrid(int &overlapx, int &overlapz) {
    Volatile &data = *this->mData;
    data.RemoveStatus(Volatile::HAS_HAD_OBJECT_COLLISION | Volatile::HAS_HAD_WORLD_COLLISION);
    const bool collideable = CanCollideWithObjects();
    if (collideable && (this->mGrid == nullptr)) {
        this->mGrid = RBGrid::Add(data.index, *this, data.position, this->mPrimitives.GetRadius());
        return;
    }
    if (!collideable && (this->mGrid != nullptr)) {
        RBGrid::Remove(this->mGrid);
        this->mGrid = nullptr;
        return;
    }
    if (this->mGrid == nullptr) {
        return;
    }
    this->mGrid->SetPosition(data.position, this->mPrimitives.GetRadius());

    if (this->mGrid->GetX().Overlaps()) {
        overlapx++;
    }
    if (this->mGrid->GetZ().Overlaps()) {
        overlapz++;
    }
}

IRigidBody *RigidBody::Get(unsigned int index) {
    if (index < NUM_ELEMENTS(mMaps)) {
        return mMaps[index];
    }
    return nullptr;
}

unsigned int RigidBody::AssignSlot() {
    for (unsigned int i = 0; i < NUM_ELEMENTS(mMaps); ++i) {
        if (mMaps[i] == nullptr) {
            return i;
        }
    }
    return ~0;
}
