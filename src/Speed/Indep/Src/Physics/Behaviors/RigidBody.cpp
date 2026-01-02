#include "RigidBody.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/rigidbodyspecs.h"
#include "Speed/Indep/Src/Generated/Events/ECollision.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Main/EventSequencer.h"
#include "Speed/Indep/Src/Physics/Dynamics.h"
#include "Speed/Indep/Src/Physics/Dynamics/Collision.h"
#include "Speed/Indep/Src/Sim/Collision.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/World/WCollisionTri.h"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

#include <types.h>

static char RBGrid_Memory[6912]; // size: 0x1B00, address: 0x8048A1A8

bTList<RigidBody> TheRigidBodies;

// TODO clear the magic numbers in Volatile::state and status

Behavior *RigidBody::Construct(const BehaviorParams &params) {
    const RBComplexParams bp(params.fparams.Fetch<RBComplexParams>(UCrc32(0xa6b47fac)));
    return new RigidBody(params, bp);
}

RigidBody::Volatile::Volatile() {}

// UNSOLVED but functionally matching
RigidBody::Mesh::Mesh(const SimSurface &material, const UMath::Vector4 *verts, unsigned int count, UCrc32 name, bool persistant)
    : mVerts(nullptr), mNumVertices(count), mFlags(0), mMaterial(material.GetConstCollection()), mName(UCrc32(name)) {
    if (persistant) {
        mVerts = const_cast<UMath::Vector4 *>(verts);
    } else {
        mVerts = reinterpret_cast<UMath::Vector4 *>(gFastMem.Alloc(static_cast<unsigned short>(count) * sizeof(*mVerts), "RigidBody::Mesh::Verts"));
        bMemCpy(mVerts, verts, static_cast<unsigned short>(count) * sizeof(*mVerts));
        mFlags |= Mesh::FREEABLE;
    }
}

RigidBody::Mesh::~Mesh() {
    if (mFlags & Mesh::FREEABLE) {
        gFastMem.Free(mVerts, mNumVertices * sizeof(*mVerts), "RigidBody::Mesh::Verts");
    }
}

void RigidBody::Mesh::Enable(bool enable) {
    if (!enable) {
        mFlags |= Mesh::DISABLED;
    } else {
        mFlags &= ~Mesh::DISABLED;
    }
}

bool RigidBody::MeshList::Create(const SimSurface &material, const UMath::Vector4 *verts, unsigned int count, UCrc32 name, bool persistant) {
    Mesh *mesh = new Mesh(material, verts, count, UCrc32(name), persistant);
    AddTail(mesh);
    mVertCount += count;
    mSize++;
    return true;
}

bool RigidBody::PrimList::Create(const UMath::Vector3 &dim, const UMath::Vector3 &offset, const SimSurface &material,
                                 Dynamics::Collision::Geometry::Shape shape, const UMath::Vector4 &orient, unsigned int flags, const UCrc32 &name) {
    Primitive *geom = new Primitive(dim, offset, material, shape, orient, flags, name);

    if (geom->GetShape() == Dynamics::Collision::Geometry::SPHERE) {
        AddHead(geom);
    } else {
        AddTail(geom);
    }

    float r = UMath::Length(dim) + UMath::Length(offset);
    mRadius = UMath::Max(mRadius, r);
    mSize++;
    return true;
}

void RigidBody::Primitive::Enable(bool enable) {
    if (!enable) {
        mFlags |= Primitive::DISABLED;
    } else {
        mFlags &= ~Primitive::DISABLED;
    }
}

// TODO why is the offset of bodyMatrix wrong on Xenon?
void RigidBody::Primitive::Prepare(const Volatile &data) {
    UMath::Rotate(GetOffset(), data.bodyMatrix, mPrevPosition);
    UMath::Add(mPrevPosition, data.position, mPrevPosition);
}

// UNSOLVED
bool RigidBody::Primitive::SetCollision(const Volatile &data, Dynamics::Collision::Geometry &obb) const {
    UMath::Vector3 position;
    UMath::Vector3 dP;

    UMath::Rotate(GetOffset(), data.bodyMatrix, position);
    UMath::Add(position, data.position, position);
    UMath::Sub(position, mPrevPosition, dP);

    if (this->mShape == Dynamics::Collision::Geometry::SPHERE) {
        obb.Set(UMath::Matrix4::kIdentity, position, GetDimension(), Dynamics::Collision::Geometry::SPHERE, dP);
    } else {
        UMath::Matrix4 mat;
        UMath::Vector4 rot;
        UMath::Mult(mOrientation, data.orientation, rot);
        UMath::QuaternionToMatrix4(rot, mat);
        obb.Set(mat, position, GetDimension(), mShape, dP);
    }
    return true;
}

void RigidBody::InitRigidBodySystem() {}

void RigidBody::ShutdownRigidBodySystem() {}

void RBGrid::Remove(RBGrid *grid) {
    if (grid) {
        grid->~RBGrid();
    }
}

// UNSOLVED
RigidBody::~RigidBody() {
    if (mGrid) {
        RBGrid::Remove(mGrid);
    }
    mGrid = nullptr;
    mCount--;
    mMaps[mData->index] = nullptr;
    if (mWCollider) {
        WCollider::Destroy(mWCollider);
    }
    mWCollider = nullptr;
    mMeshes.DeleteAllElements();
    mPrimitives.DeleteAllElements();
    Dynamics::Articulation::Release(this);
    TheRigidBodies.Remove(this);
    // TODO come back to this after we decomped Listable<>Unlist or the inlines it calls
}

void RigidBody::Reset() {
    mWCollider->Clear();
    UpdateCollider();
}

void RigidBody::SetOrientation(const UMath::Vector4 &newOrientation) {
    Volatile &data = *mData;
    UMath::Copy(newOrientation, data.orientation);
    UMath::QuaternionToMatrix4(data.orientation, data.bodyMatrix);
    data.inertiaTensor.GetInverseWorldTensor(data.bodyMatrix, mInvWorldTensor);
}

bool RigidBody::DistributeMass() {
    Volatile &data = *mData;
    UMath::Clear(mCOG);

    float total_volume = 0.0f;
    for (Primitive *primitive = mPrimitives.GetHead(); primitive != mPrimitives.EndOfList(); primitive = primitive->GetNext()) {
        if (primitive->GetFlags() & Primitive::VSOBJECTS) {
            const UMath::Vector3 &dimension = primitive->GetDimension();
            float volume = dimension.x * dimension.y * dimension.z * 8.0f;
            UMath::ScaleAdd(primitive->GetOffset(), volume, mCOG, mCOG);
            total_volume += volume;
        }
    }
    if (total_volume > 0.0f) {
        UMath::Scale(mCOG, 1.0f / total_volume, mCOG);
        return true;
    }
    return false;
}

void RigidBody::EnableCollisionGeometries(UCrc32 name, bool enable) {
    if (name == UCrc32::kNull) {
        return;
    }
    for (Primitive *primitive = mPrimitives.GetHead(); primitive != mPrimitives.EndOfList(); primitive = primitive->GetNext()) {
        if (primitive->GetName() == name) {
            primitive->Enable(enable);
        }
    }
    for (Mesh *mesh = mMeshes.GetHead(); mesh != mMeshes.EndOfList(); mesh = mesh->GetNext()) {
        if (mesh->GetName() == name) {
            mesh->Enable(enable);
        }
    }
}

void RigidBody::SetPosition(const UMath::Vector3 &pos) {
    mData->position = pos;
}

void RigidBody::SetLinearVelocity(const UMath::Vector3 &vel) {
    mData->linearVel = vel;
}

void RigidBody::SetAngularVelocity(const UMath::Vector3 &vel) {
    mData->angularVel = vel;
}

void RigidBody::SetRadius(float radius) {
    mData->radius = radius;
}

void RigidBody::AttachedToWorld(bool b, float detachforce) {
    if (b) {
        mData->SetStatus(Volatile::IS_ATTACHED_TO_WORLD);
    } else {
        mData->RemoveStatus(Volatile::IS_ATTACHED_TO_WORLD);
    }
    mDetachForce = detachforce;
}

void RigidBody::Detach() {
    if (mData->GetStatus(Volatile::IS_ATTACHED_TO_WORLD)) {
        mData->RemoveStatus(Volatile::IS_ATTACHED_TO_WORLD);
        EventSequencer::IEngine *engine = GetOwner()->GetEventSequencer();
        if (engine) {
            engine->ProcessStimulus(0xc91b90d9, Sim::GetTime(), nullptr, EventSequencer::QUEUE_ALLOW);
        }
    }
}

void RigidBody::SetInertiaTensor(const UMath::Vector3 &moment) {
    mData->inertiaTensor = moment;
}

void RigidBody::SetMass(float newMass) {
    Volatile &data = *mData;
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
            if (AddCollisionBox(dim, offset, material, orient, flags, name)) {
                result = true;
            }
        } else if (boundflags & CollisionGeometry::kBounds_Sphere) {
            if (AddCollisionSphere(radius, offset, material, flags, name)) {
                result = true;
            }
        }
    }
    return result;
}

bool RigidBody::AddCollisionMesh(UCrc32 name, const UMath::Vector4 *verts, unsigned int count, const struct SimSurface &material,
                                 CollisionGeometry::BoundFlags flags, bool persistant) {
    return mMeshes.Create(material, verts, count, UCrc32(name), persistant);
}

void RigidBody::CreateGeometries() {
    SimSurface material = SimSurface(mSpecs.BASE_MATERIAL());
    if (mGeoms && mGeoms->fCollection && mGeoms->fCollection->AddTo(this, mGeoms, material, true)) {
        return;
    }
    if (mSpecs.DEFAULT_COL_BOX() == Attrib::StringKey("SPHERE")) {
        AddCollisionSphere(UMath::Max(mDimension.x, UMath::Max(mDimension.y, mDimension.z)), UMath::Vector3::kZero, material, 3, UCrc32::kNull);
    } else {
        AddCollisionBox(mDimension, UMath::Vector3::kZero, material, UMath::Vector4::kIdentity, 7, UCrc32::kNull);
    }
}

void RigidBody::PlaceObject(const UMath::Matrix4 &orientMat, const UMath::Vector3 &initPos) {
    Volatile &data = *mData;
    data.state = 0;
    SetOrientation(orientMat);
    data.RemoveStatus(Volatile::IS_ATTACHED_TO_WORLD);
    UMath::Clear(data.force);
    UMath::Clear(data.torque);
    UMath::Clear(data.linearVel);
    UMath::Clear(data.angularVel);
    data.position = initPos;
    data.leversInContact = 0;
    if (mSimableType == SIMABLE_VEHICLE) {
        data.SetStatus(0x40);
    } else {
        data.RemoveStatus(0x40);
    }
    mData->statusPrev = mData->status;
    mWCollider->Clear();
    UpdateCollider();
    mData->Validate();
}

float RigidBody::GetOrientToGround() const {
    const Volatile &data = *mData;

    return data.bodyMatrix.v1.x * mGroundNormal.x + data.bodyMatrix.v1.y * mGroundNormal.y + data.bodyMatrix.v1.z * mGroundNormal.z;
}

void RigidBody::GetPointVelocity(const UMath::Vector3 &position, UMath::Vector3 &velocity) const {
    const Volatile &data = *mData;
    UMath::Vector3 worldArm;
    UMath::Vector3 cg;

    UMath::Rotate(mCOG, data.bodyMatrix, cg);
    UMath::Add(cg, data.position, cg);
    UMath::Sub(position, cg, worldArm);
    UMath::Cross(data.angularVel, worldArm, velocity);
    UMath::Add(velocity, data.linearVel, velocity);
}

void RigidBody::OnEndFrame(float dT) {
    Volatile &data = *mData;
    if (data.state == 0) {
        UpdateCollider();
        data.RemoveStatus(Volatile::HAS_HAD_WORLD_COLLISION);
        if (!mWCollider->IsEmpty()) {
            DoWorldCollisions(dT);
        }
        DoDrag();
        data.Validate();
        if (ShouldSleep() != 0) {
            data.state = 1;
        }
    } else {
        Behavior::GetOwner()->GetWPos().Update(data.position, mGroundNormal, true, nullptr, true);
    }
    data.RemoveStatus(0x400);
}

void RigidBody::DisableTriggering() {
    mData->SetStatus(1);
}

bool RigidBody::IsTriggering() const {
    return !mData->GetStatus(1);
}

void RigidBody::EnableTriggering() {
    mData->RemoveStatus(1);
}

void RigidBody::DisableModeling() {
    mData->state = 2;
}

void RigidBody::EnableModeling() {
    if (mData->state == 2) {
        Volatile &data = *mData;
        data.Validate();
        data.state = 0;
        Behavior::GetOwner()->GetWPos().Update(data.position, mGroundNormal, false, nullptr, true);
    }
}

void RigidBody::OnBeginFrame(float dT) {
    Volatile &data = *mData;
    data.SetStatus(0x400);
    if (data.state != 2) {
        for (Primitive *collider = mPrimitives.GetHead(); collider != mPrimitives.EndOfList(); collider = collider->GetNext()) {
            collider->Prepare(data);
        }
        DoIntegration(dT);
    }
    UMath::Clear(data.force);
    UMath::Clear(data.torque);
    if (data.state == 0) {
        data.statusPrev = data.status;
        if (!data.GetStatus(Volatile::IS_ATTACHED_TO_WORLD)) {
            data.force.y += mSpecs.GRAVITY() * data.mass;
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

void RigidBody::DoDrag() {
    Volatile &data = *mData;
    if (!data.GetStatus(Volatile::IS_ATTACHED_TO_WORLD)) {
        if (data.GetStatus(0x800)) {
            UMath::Vector3 avel = data.angularVel;
            ConvertWorldToLocal(avel, false);
            UMath::Vector3 cross_section;
            cross_section.x = mDimension.x * 4.0f * UMath::Sqrt(mDimension.y * mDimension.y + mDimension.z * mDimension.z);
            cross_section.y = mDimension.y * 4.0f * UMath::Sqrt(mDimension.x * mDimension.x + mDimension.z * mDimension.z);
            cross_section.z = mDimension.z * 4.0f * UMath::Sqrt(mDimension.x * mDimension.x + mDimension.y * mDimension.y);

            Drag drag((UVector3(avel)), UVector3(UMath::Vector4To3(mSpecs.DRAG_ANGULAR())), UVector3(cross_section), 1.225f);
            ConvertLocalToWorld(drag, false);
            ResolveTorque(drag);
        }
        if (data.GetStatus(0x20)) {
            UMath::Vector3 vel = data.linearVel;
            UMath::Vector3 area;
            UMath::Scale(mDimension, mDimension, area);
            UMath::Scale(area, 4.0f, area);
            ConvertWorldToLocal(vel, false);

            Drag drag((UVector3(vel)), UVector3(UMath::Vector4To3(mSpecs.DRAG())), UVector3(area), 1.225f);
            ConvertLocalToWorld(drag, false);
            ResolveForce(drag);
        }
    }
}

void RigidBody::ConvertLocalToWorld(UMath::Vector3 &val, bool translate) const {
    UMath::Rotate(val, mData->bodyMatrix, val);
    if (translate) {
        UMath::Add(val, mData->position, val);
    }
}

void RigidBody::ConvertWorldToLocal(UMath::Vector3 &val, bool translate) const {
    UMath::Matrix4 InvBodyMatrix;

    if (translate) {
        UMath::Sub(val, mData->position, val);
    }
    UMath::Transpose(mData->bodyMatrix, InvBodyMatrix);
    UMath::Rotate(val, InvBodyMatrix, val);
}

void RigidBody::Resolve(const UMath::Vector3 &force, const UMath::Vector3 &torque) {
    Volatile &data = *mData;
    if (data.state != 2) {
        data.state = 0;
        data.status |= 0x40;
        Detach();
        UMath::Add(force, data.force, data.force);
        UMath::Add(torque, data.torque, data.torque);
    }
}

void RigidBody::ResolveTorque(const UMath::Vector3 &torque) {
    Volatile &data = *mData;
    if (data.state != 2) {
        data.state = 0;
        data.status |= 0x40;
        Detach();
        UMath::Add(torque, data.torque, data.torque);
    }
}

void RigidBody::ResolveForce(const UMath::Vector3 &force) {
    Volatile &data = *mData;
    if (data.state != 2) {
        data.state = 0;
        data.status |= 0x40;
        Detach();
        UMath::Add(force, data.force, data.force);
    }
}

void RigidBody::ResolveTorque(const UMath::Vector3 &force, const UMath::Vector3 &p) {
    Volatile &data = *mData;
    if (data.state != 2) {
        data.state = 0;
        data.status |= 0x40;
        Detach();

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
    Volatile &data = *mData;
    if (data.state != 2) {
        data.state = 0;
        data.status |= 0x40;
        Detach();

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

void RigidBody::SetAnimating(bool animating) {
    if (animating) {
        mData->SetStatus(0x100);
    } else {
        mData->RemoveStatus(0x100);
    }
}

// UNSOLVED, regswaps
void RigidBody::ResolveGroundCollision(const CollisionPacket *bcp, const int numContacts) {
    Volatile &data = *mData;
    bool first = true;
    Dynamics::Collision::Friction obj_friction;
    obj_friction.mUs = mSpecs.GROUND_FRICTION(0);
    obj_friction.mUk = UMath::Min(mSpecs.GROUND_FRICTION(1), mSpecs.GROUND_FRICTION(0));
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
        UMath::Scale(e, UMath::Vector4To3(mSpecs.GROUND_ELASTICITY()), e);
        mo.SetElasticity(UMath::Length(e));
        mo.SetInertiaScale(UMath::Vector4To3(mSpecs.GROUND_MOMENT_SCALE()));
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
    if (mData->GetStatus(0x100) && other.mData->GetStatus(0x100)) {
        return false;
    }
    return true;
}

bool RigidBody::CanCollideWithGround() const {
    if (mSpecs.NO_GROUND_COLLISIONS() || !IsAwake()) {
        return false;
    }
    return true;
}

void RigidBody::ModifyCollision(const SimSurface &other, const Dynamics::Collision::Plane &plane, Dynamics::Collision::Moment &myMoment) {
    myMoment.SetInertiaScale(UMath::Vector4To3(mSpecs.WORLD_MOMENT_SCALE()));
}

bool RigidBody::IsImmobile() const {
    if (mSpecs.IMMOBILE_OBJECT_COLLISIONS() || mData->GetStatus(Volatile::IS_ATTACHED_TO_WORLD)) {
        return true;
    }
    return false;
}

void RigidBody::ModifyCollision(const RigidBody &other, const Dynamics::Collision::Plane &plane, Dynamics::Collision::Moment &myMoment) {
    Volatile &data = *mData;
    if (!other.IsImmobile()) {
        if (mSpecs.IMMOBILE_OBJECT_COLLISIONS()) {
            myMoment.MakeImmobile(true, 0.0f);
        } else {
            if (data.GetStatus(Volatile::IS_ATTACHED_TO_WORLD)) {
                myMoment.MakeImmobile(true, mDetachForce);
            }
        }
    }
    myMoment.SetInertiaScale(UMath::Vector4To3(mSpecs.OBJ_MOMENT_SCALE()));
    if (mSpecs.OBJ_MOMENT_SCALE().w > 0.0f) {
        myMoment.SetMass(myMoment.GetMass() * mSpecs.OBJ_MOMENT_SCALE().w);
    }
    if (data.GetStatus(Volatile::IS_ANCHORED)) {
        myMoment.SetFixedCG(true);
    }
}

bool RigidBody::Separate(RigidBody &objA, bool objAImmobile, RigidBody &objB, bool objBImmobile, const UMath::Vector3 &normal, UMath::Vector3 &point,
                         float overlap, bool APenetratesB) {
    if (overlap <= FLOAT_EPSILON) {
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
        if ((UMath::Abs(closingspeed) > FLOAT_EPSILON) && (objA.mSimableType == objB.mSimableType) && (objA.mSimableType == SIMABLE_SMACKABLE)) {
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

    if (moA.React(moB, plane, 0x20)) {
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

bool RigidBody::ResolveWorldOBBCollision(const UMath::Vector3 &cn, const UMath::Vector3 &cp, COLLISION_INFO *collisionInfo,
                                         const Dynamics::Collision::Geometry *otherGeom, const UMath::Vector3 &linearVel, const SimSurface &rbsurface,
                                         const SimSurface &obbsurface) {
    const float otherMass = 1000000.0f;
    Volatile &data = *mData;
    Dynamics::Collision::Moment moA(this);
    UMath::Vector3 inertia =
        Dynamics::Inertia::Box(1000000.0f, 2 * otherGeom->GetDimension().x, 2 * otherGeom->GetDimension().y, 2 * otherGeom->GetDimension().z);
    UMath::Vector3 cg = {};
    UMath::Vector3 angularVel = {};
    Dynamics::Collision::Moment moB(otherGeom->GetOrientation(), otherMass, inertia, cg, linearVel, angularVel, otherGeom->GetPosition());

    Dynamics::Collision::Plane plane;
    plane.friction = Dynamics::Collision::Friction(UMath::Min(mSpecs.OBJ_FRICTION(0), mSpecs.OBJ_FRICTION(1)), mSpecs.OBJ_FRICTION(0));
    plane.normal = cn;
    plane.point = UVector3(cp.x, cp.y, cp.z);
    plane.friction *= obbsurface.WORLD_FRICTION();

    UMath::Vector3 e;
    UMath::Dot(plane.normal, data.bodyMatrix, e);
    UMath::Scale(e, UMath::Vector4To3(mSpecs.WALL_ELASTICITY()), e);
    moA.SetElasticity(UMath::Length(e));

    ModifyCollision(obbsurface, plane, moA);

    moA.MakeImmobile(false, 0.0f);
    moB.MakeImmobile(true, 0.0f);

    if (moA.React(moB, plane, 0x20)) {
        float force = moA.GetForce().Magnitude();

        collisionInfo->objAVel = data.linearVel;
        collisionInfo->objBVel = linearVel;
        collisionInfo->objAsurface = rbsurface.GetConstCollection();
        collisionInfo->objA = GetOwner()->GetInstanceHandle();
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
        ConvertWorldToLocal(collisionInfo->armA, 0);

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
    Volatile &data = *mData;
    const Attrib::Gen::rigidbodyspecs &specs = *mSpecs;
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
    ModifyCollision(worldSurface, plane, mo);
    mo.SetLinearVelocity(mo.GetLinearVelocity() - worldVel);
    if (mo.React(plane, 0x10)) {
        float force = mo.GetForce().Magnitude();
        if (collisionInfo) {
            collisionInfo->normal = cn;
            collisionInfo->position = cp;
            collisionInfo->objA = GetOwner()->GetInstanceHandle();
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
            ConvertWorldToLocal(collisionInfo->armA, 0);
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
    Volatile &data = *mData;
    float radius = UMath::Max(mPrimitives.GetRadius(), 3.0f);
    mWCollider->Refresh(data.position, radius, true);
    Behavior::GetOwner()->GetWPos().Update(data.position, mGroundNormal, true, mWCollider, true);
}

// UNSOLVED but functionally matching
bool RigidBody::CanCollideWithWorld() const {
    const Volatile &data = *mData;

    if (data.GetStatus(0x40) && !mSpecs.NO_WORLD_COLLISIONS() && !IsSleeping()) {
        if (mPrimitives.Size() == 0) {
            return false;
        }
        return true;
    }
    return false;
}

bool RigidBody::OnWCollide(const WCollisionMgr::WorldCollisionInfo &cInfo, const UMath::Vector3 &cPoint, void *userdata) {
    Volatile &data = *mData;
    data.SetStatus(Volatile::HAS_HAD_WORLD_COLLISION);
    SimSurface surface(cInfo.fBle.fSurfaceRef);
    surface.DebugOverride();
    UVector3 wV;
    if (cInfo.fNormal.w > 0.0f) {
        UVector3 dp(cInfo.fNormal);
        dp *= cInfo.fNormal.w;
        UMath::Add(data.position, dp, data.position);
    }
    COLLISION_INFO collisionInfo;

    if (ResolveWorldCollision(UVector3(cInfo.fNormal), cPoint, &collisionInfo, reinterpret_cast<Primitive *>(userdata)->GetMaterial(), surface, wV) >
        0.0f) {
        new ECollision(collisionInfo);
    }
    return true;
}

void RigidBody::DoWorldCollisions(const float dT) {
    if (CanCollideWithWorld() != 0) {
        DoObbCollision(dT);
        DoBarrierCollision(dT);
    }
    if (CanCollideWithGround() && !mWCollider->GetInstanceList().empty()) {
        DoInstanceCollision(dT);
    }
}

void RigidBody::DoBarrierCollision(float dT) {}

void RigidBody::DoInstanceCollision(float dT) {
    if (!mSpecs.INSTANCE_COLLISIONS_3D()) {
        DoInstanceCollision2d(dT);
    } else {
        DoInstanceCollision3d(dT);
    }
}

void RigidBody::DoInstanceCollision3d(float dT) {
    Volatile &data = *mData;
    WCollisionMgr::WorldCollisionInfo cInfo;
    Dynamics::Collision::Geometry thisGeom;
    const struct WCollisionInstanceCacheList &instances = mWCollider->GetInstanceList();
    for (Primitive *collider = mPrimitives.GetHead(); collider != mPrimitives.EndOfList(); collider = collider->GetNext()) {
        if ((((collider->GetFlags() ^ 1) & 1) == 0) && !(collider->GetFlags() & 8) && collider->SetCollision(data, thisGeom)) {
            WCollisionMgr(mCollisionMask, 3).Collide(&thisGeom, &instances, this, collider);
        }
    }
}

bool RigidBody::ShouldSleep() const {
    const Volatile &data = *mData;
    float sumVel = UMath::Length(data.linearVel) + (UMath::Length(data.angularVel) * data.radius);
    if (sumVel < mSpecs.SLEEP_VELOCITY() && GetNumContactPoints() > 2) {
        return true;
    }
    return false;
}

void RigidBody::Accelerate(const UMath::Vector3 &a, float dT) {
    Volatile &data = *mData;
    if (data.state == 0) {
        UMath::ScaleAdd(a, dT, data.linearVel, data.linearVel);
    }
}

bool RigidBody::AddCollisionSphere(float radius, const UMath::Vector3 &offset, const SimSurface &material, unsigned int flags, const UCrc32 &name) {
    return mPrimitives.Create(UVector3(radius, radius, radius), offset, material, Dynamics::Collision::Geometry::SPHERE, UMath::Vector4::kIdentity,
                              flags | 0x10, name);
}

bool RigidBody::AddCollisionBox(const UMath::Vector3 &dim, const UMath::Vector3 &offset, const SimSurface &material, const UMath::Vector4 &orient,
                                unsigned int flags, const UCrc32 &name) {
    return mPrimitives.Create(dim + mSpecs.COLLISION_BOX_PAD(), offset, material, Dynamics::Collision::Geometry::BOX, orient, flags, name);
}

bool RigidBody::CanCollideWithObjects() const {
    if (mPrimitives.Size() == 0) {
        return false;
    }
    if (mSpecs.NO_OBJ_COLLISIONS() != 0) {
        return false;
    }
    if (mData->state == 2) {
        return false;
    }
    return true;
}

void RigidBody::OnTaskSimulate(float dT) {}

void RigidBody::OnDebugDraw() {}

unsigned int RigidBody::GetTriggerFlags() const {
    if (mData->GetStatus(1) || IsSleeping()) {
        return 0;
    }
    return 0x40;
}

void RigidBody::PushSP(void *workspace) {
    mOnSP = 1;
    ScratchPtr<RigidBody::Volatile>::Push(workspace);
}

void RigidBody::PopSP() {
    mOnSP = 0;
    ScratchPtr<RigidBody::Volatile>::Pop();
}

void RigidBody::Damp(float amount) {
    Volatile &data = *mData;
    float scale = 1.0f - amount;
    UMath::Scale(data.linearVel, scale, data.linearVel);
    UMath::Scale(data.angularVel, scale, data.angularVel);
    UMath::Scale(data.force, scale, data.force);
    UMath::Scale(data.torque, scale, data.torque);
}

void RigidBody::UpdateGrid(int &overlapx, int &overlapz) {
    Volatile &data = *mData;
    data.RemoveStatus(Volatile::HAS_HAD_OBJECT_COLLISION | Volatile::HAS_HAD_WORLD_COLLISION);
    const bool collideable = CanCollideWithObjects();
    if (collideable && !mGrid) {
        mGrid = RBGrid::Add(data.index, *this, data.position, mPrimitives.GetRadius());
        return;
    }
    if (!collideable && mGrid) {
        RBGrid::Remove(mGrid);
        mGrid = nullptr;
        return;
    }
    if (!mGrid) {
        return;
    }
    mGrid->SetPosition(data.position, mPrimitives.GetRadius());

    if (mGrid->GetX().Overlaps()) {
        overlapx++;
    }
    if (mGrid->GetZ().Overlaps()) {
        overlapz++;
    }
}

IRigidBody *RigidBody::Get(unsigned int index) {
    if (index < RIGID_BODY_MAX) {
        return mMaps[index];
    }
    return nullptr;
}

unsigned int RigidBody::AssignSlot() {
    for (std::size_t i = 0; i < RIGID_BODY_MAX; ++i) {
        if (mMaps[i] == nullptr) {
            return i;
        }
    }
    return static_cast<unsigned int>(-1);
}
