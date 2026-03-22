#include "SimpleRigidBody.h"
#include "RigidBody.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Libs/Support/Utility/UVector.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimpleBody.h"
#include "Speed/Indep/Src/Main/ScratchPtr.h"
#include "Speed/Indep/Src/Physics/Behavior.h"
#include "Speed/Indep/Src/Physics/Dynamics/Collision.h"
#include "Speed/Indep/Src/Physics/PhysicsObject.h"
#include "Speed/Indep/Src/Sim/OBB.h"
#include "Speed/Indep/Src/Sim/Simulation.h"

namespace {

struct CollisionMapAccess {
    unsigned long long fBitMap[3];
};

static void SetCollisionMapBit(SimCollisionMap &map, unsigned int index) {
    CollisionMapAccess &access = reinterpret_cast<CollisionMapAccess &>(map);
    access.fBitMap[index / 64] |= 1ULL << (index % 64);
}

} // namespace

ISimpleBody::~ISimpleBody() {}

template <>
void *ScratchPtr<SimpleRigidBody::Volatile>::mWorkSpace = nullptr;

template <>
SimpleRigidBody::Volatile *ScratchPtr<SimpleRigidBody::Volatile>::mPointer[64] = { nullptr };

template <>
SimpleRigidBody::Volatile ScratchPtr<SimpleRigidBody::Volatile>::mRAMBuffer[64];

template <> ScratchPtr<SimpleRigidBody::Volatile>::~ScratchPtr() {
    *mRef = nullptr;
}

SimpleRigidBody::SimpleRigidBody(const BehaviorParams &bp, const RBSimpleParams &params)
    : Behavior(bp, 0), IRigidBody(bp.fowner), ISimpleBody(bp.fowner) {
    TheSimpleBodies.AddTail(this);
    MakeDebugable(DBG_RIGIDBODY);

    mData->index = AssignSlot();
    mData->position = params.finitPos;
    mData->linearVel = params.finitVel;
    mData->angularVel = params.finitAngVel;
    mData->radius = params.finitRadius;
    mData->mass = params.finitMass;
    mData->flags = 0;

    mCollisionMap[mData->index].Clear();
    mMaps[mData->index] = this;
    mCount++;
    UMath::Matrix4ToQuaternion(params.finitMat, mData->orientation);
}

SimpleRigidBody::~SimpleRigidBody() {
    TheSimpleBodies.Remove(this);
    mCollisionMap[mData->index].Clear();
    mMaps[mData->index] = nullptr;
    mCount--;
}

Behavior *SimpleRigidBody::Construct(const struct BehaviorParams &params) {
    const RBSimpleParams sp(params.fparams.Fetch<RBSimpleParams>(UCrc32(0xa6b47fac)));
    return new SimpleRigidBody(params, sp);
}

SimpleRigidBody::Volatile::Volatile() {}

ISimable *SimpleRigidBody::GetOwner() const {
    return Behavior::GetOwner();
}

void SimpleRigidBody::GetForwardVector(UMath::Vector3 &vec) const {
    UMath::ExtractZAxis(mData->orientation, vec);
}

void SimpleRigidBody::GetRightVector(UMath::Vector3 &vec) const {
    UMath::ExtractXAxis(mData->orientation, vec);
}

void SimpleRigidBody::GetUpVector(UMath::Vector3 &vec) const {
    UMath::ExtractYAxis(mData->orientation, vec);
}

void SimpleRigidBody::SetOrientation(const UMath::Matrix4 &orientMat) {
    UMath::Matrix4ToQuaternion(orientMat, mData->orientation);
}

void SimpleRigidBody::SetOrientation(const UMath::Vector4 &orientation) {
    mData->orientation = orientation;
}

float SimpleRigidBody::GetScalarVelocity() const {
    return UMath::Length(mData->linearVel);
}

void SimpleRigidBody::Accelerate(const UMath::Vector3 &a, float dT) {
    Volatile &data = *mData;
    if (data.flags & 8) {
        UMath::ScaleAdd(a, dT, data.linearVel, data.linearVel);
    }
}

void SimpleRigidBody::ApplyFriction() {
    Volatile &data = *mData;
    if (data.flags & 4) {
        UMath::Scale(data.linearVel, 0.99, data.linearVel);
        UMath::Scale(data.angularVel, 0.98, data.angularVel);
    }
}

void SimpleRigidBody::OnDebugDraw() {}

void SimpleRigidBody::DoIntegration(const float dT) {
    Volatile &data = *mData;
    ApplyFriction();
    if (data.flags & 8) {
        UMath::ScaleAdd(data.linearVel, dT, data.position, data.position);
    }
    if (data.flags & 0x200) {
        UMath::Vector4 aVel = {};
        UMath::Scale(data.angularVel, dT, *reinterpret_cast<UMath::Vector3 *>(&aVel));
        UMath::Vector4 temp_quat;
        // TODO UNSOLVED
        UMath::Mult(aVel, data.orientation, temp_quat);
        UMath::ScaleAdd(temp_quat, 0.5f, data.orientation, data.orientation);
        UMath::Unit(data.orientation, data.orientation);
    }
}

void SimpleRigidBody::GetPointVelocity(const UMath::Vector3 &worldPt, UMath::Vector3 &pv) const {
    const Volatile &data = *mData;
    UMath::Vector3 worldArm;
    UMath::Sub(worldPt, data.position, worldArm);
    UMath::Cross(data.angularVel, worldArm, pv);
    UMath::Add(pv, data.linearVel, pv);
}

void SimpleRigidBody::PlaceObject(const UMath::Matrix4 &orientMat, const UMath::Vector3 &initPos) {
    SetPosition(initPos);
    SetOrientation(orientMat);
    SetAngularVelocity(UMath::Vector3::kZero);
    SetLinearVelocity(UMath::Vector3::kZero);
}

void SimpleRigidBody::ConvertLocalToWorld(UMath::Vector3 &val, bool translate) const {
    const Volatile &data = *mData;
    UMath::Rotate(val, data.orientation, val);
    if (translate) {
        UMath::Add(val, data.position, val);
    }
}

void SimpleRigidBody::ConvertWorldToLocal(UMath::Vector3 &val, bool translate) const {
    const Volatile &data = *mData;
    UMath::Vector4 invorient;

    if (translate) {
        UMath::Sub(val, data.position, val);
    }
    UMath::Transpose(data.orientation, invorient);
    UMath::Rotate(val, invorient, val);
}

void SimpleRigidBody::Resolve(const UMath::Vector3 &force, const UMath::Vector3 &torque) {
    ResolveForce(force);
}

void SimpleRigidBody::ResolveForce(const UMath::Vector3 &force) {
    if (CheckAnyFlags(8)) {
        UMath::Vector3 vec;
        UMath::Scale(force, 1.0f / GetMass(), vec);
        Accelerate(vec, Sim::GetTimeStep());
    }
}

// huh?
void SimpleRigidBody::ResolveForce(const UMath::Vector3 &force, const UMath::Vector3 &) {
    ResolveForce(force);
}

void SimpleRigidBody::ResolveTorque(const UMath::Vector3 &torque) {}

unsigned int SimpleRigidBody::GetTriggerFlags() const {
    if (!CanHitTrigger()) {
        return 0;
    }

    ISimable *owner = GetOwner();
    unsigned int flags = 0x40;

    if (owner->GetSimableType() == SIMABLE_EXPLOSION) {
        flags = 0x50;
    }

    if (owner->GetSimableType() == SIMABLE_HUMAN) {
        flags |= 0x10000;
        if (!owner->IsPlayer()) {
            flags |= 8;
        }
    }

    if (owner->IsPlayer()) {
        flags |= 4;
    }

    return flags;
}

void SimpleRigidBody::RecalcOrientMat(UMath::Matrix4 &resultMat4) const {
    UMath::QuaternionToMatrix4(mData->orientation, resultMat4);
}

void SimpleRigidBody::DoRBCollisions(const float dT) {
    IRigidBody *thisBody = this;
    SimCollisionMap &cmap = mCollisionMap[thisBody->GetIndex()];
    Volatile &data = *mData;
    Dynamics::Collision::Geometry geomSRB;
    const float thisRadius = UMath::Max(thisBody->GetRadius(), 0.25f);
    UVector3 dimSRB(thisRadius, thisRadius, thisRadius);

    if (data.flags & 0x8000) {
        geomSRB.Set(UMath::Matrix4::kIdentity, thisBody->GetPosition(), dimSRB, Dynamics::Collision::Geometry::SPHERE,
                    UVector3(thisBody->GetLinearVelocity()) * dT);
    } else if (data.flags & 0x40) {
        UMath::Matrix4 orientSimple;
        UMath::QuaternionToMatrix4(data.orientation, orientSimple);
        geomSRB.Set(orientSimple, thisBody->GetPosition(), dimSRB, Dynamics::Collision::Geometry::BOX,
                    UVector3(thisBody->GetLinearVelocity()) * dT);
    }

    for (RigidBody *body = TheRigidBodies.GetHead(); body != TheRigidBodies.EndOfList(); body = body->GetNext()) {
        if (!body->CanCollideWithObjects()) {
            continue;
        }

        const bool useObb = (data.flags & 0x8040) != 0;
        float vdist = 0.0f;
        if (useObb) {
            vdist = UMath::Distance(thisBody->GetLinearVelocity(), body->GetLinearVelocity()) * dT;
        }

        const float distSquared = UMath::DistanceSquare(thisBody->GetPosition(), body->GetPosition());
        const float testRadius = thisRadius + body->GetRadius() + vdist;
        if (distSquared >= testRadius * testRadius) {
            continue;
        }

        if (useObb) {
            UMath::Vector3 bodyDimension;
            body->GetDimension(bodyDimension);
            Dynamics::Collision::Geometry geomRB(body->GetMatrix4(), body->GetPosition(), bodyDimension, Dynamics::Collision::Geometry::BOX,
                                                UVector3(body->GetLinearVelocity()) * dT);
            if (!Dynamics::Collision::Geometry::FindIntersection(&geomRB, &geomSRB, &geomSRB)) {
                continue;
            }
        }

        SetCollisionMapBit(cmap, body->GetIndex());
    }
}

void SimpleRigidBody::DoSRBCollisions(SimpleRigidBody *other) {
    Volatile &data = *mData;
    Volatile &otherData = *other->mData;

    if ((data.flags | otherData.flags) & 0x4000) {
        ISimable *ownerOther = other->GetOwner();
        ISimable *ownerThis = GetOwner();
        if (ownerOther == ownerThis) {
            return;
        }
    }

    IRigidBody &thisBody = *this;
    IRigidBody &otherBody = *other;
    SimCollisionMap &cmap = mCollisionMap[thisBody.GetIndex()];
    SimCollisionMap &cmapother = mCollisionMap[otherBody.GetIndex()];
    UMath::Vector3 posSRB = otherBody.GetPosition();
    const float radiusSRB = UMath::Max(otherBody.GetRadius(), 0.25f);
    const float thisRadius = UMath::Max(thisBody.GetRadius(), 0.25f);
    UMath::Vector3 vec;
    UMath::Sub(thisBody.GetPosition(), posSRB, vec);
    const float distSquared = UMath::LengthSquare(vec);
    const float testRadius = thisRadius + radiusSRB;
    if (distSquared >= testRadius * testRadius) {
        return;
    }

    if (((data.flags & 0x80) == 0) || ((otherData.flags & 0x80) == 0)) {
        SetCollisionMapBit(cmap, RIGID_BODY_MAX + otherBody.GetIndex());
        SetCollisionMapBit(cmapother, RIGID_BODY_MAX + thisBody.GetIndex());
        return;
    }

    OBB obbThis;
    OBB obbOther;
    UMath::Matrix4 orientThis;
    UMath::Matrix4 orientOther;
    UMath::Vector3 dimThis;
    UMath::Vector3 dimOther;
    UMath::QuaternionToMatrix4(thisBody.GetOrientation(), orientThis);
    UMath::QuaternionToMatrix4(otherBody.GetOrientation(), orientOther);
    dimThis.x = thisRadius;
    dimThis.y = thisRadius;
    dimThis.z = thisRadius * GetScalarVelocity();
    dimOther.x = radiusSRB;
    dimOther.y = radiusSRB;
    dimOther.z = radiusSRB * other->GetScalarVelocity();
    obbThis.Reset(orientThis, thisBody.GetPosition(), dimThis);
    obbOther.Reset(orientOther, posSRB, dimOther);
    if (obbThis.CheckOBBOverlap(&obbOther)) {
        SetCollisionMapBit(cmap, RIGID_BODY_MAX + otherBody.GetIndex());
        SetCollisionMapBit(cmapother, RIGID_BODY_MAX + thisBody.GetIndex());
    }
}

void SimpleRigidBody::Update(const float dT, void *workspace) {
    ScratchPtr<Volatile>::Push(workspace);

    for (SimpleRigidBody *body = TheSimpleBodies.GetHead(); body != TheSimpleBodies.EndOfList(); body = body->GetNext()) {
        body->DoIntegration(dT);
    }

    for (unsigned int i = 0; i < SIMPLE_RIGID_BODY_MAX; ++i) {
        mCollisionMap[i].Clear();
    }

    for (SimpleRigidBody *body = TheSimpleBodies.GetHead(); body != TheSimpleBodies.EndOfList(); body = body->GetNext()) {
        if (body->CanCollideWithRB()) {
            body->DoRBCollisions(dT);
        }

        if (body->CanCollideWithSRB()) {
            for (SimpleRigidBody *other = body->GetNext(); other != TheSimpleBodies.EndOfList(); other = other->GetNext()) {
                if (other->CanCollideWithSRB()) {
                    body->DoSRBCollisions(other);
                }
            }
        }
    }

    ScratchPtr<Volatile>::Pop();
}

IRigidBody *SimpleRigidBody::Get(unsigned int index) {
    if (index < SIMPLE_RIGID_BODY_MAX) {
        return mMaps[index];
    }
    return nullptr;
}

unsigned int SimpleRigidBody::AssignSlot() {
    for (std::size_t i = 0; i < SIMPLE_RIGID_BODY_MAX; ++i) {
        if (mMaps[i] == nullptr) {
            return i;
        }
    }
    return static_cast<unsigned int>(-1);
}
