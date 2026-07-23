#include "SimpleRigidBody.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Generated/Hash.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimpleBody.h"
#include "Speed/Indep/Src/Main/ScratchPtr.h"
#include "Speed/Indep/Src/Physics/Behavior.h"
#include "Speed/Indep/Src/Physics/Behaviors/RigidBody.h"
#include "Speed/Indep/Src/Physics/PhysicsObject.h"
#include "Speed/Indep/Src/Sim/OBB.h"
#include "Speed/Indep/Src/Sim/SimTypes.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/bWare/Inc/bTypes.hpp"

BIND_BEHAVIOR_FACTORY(SimpleRigidBody);
IMPLEMENT_SCRATCHPTR(SimpleRigidBody::Volatile);

typedef bTList<SimpleRigidBody> SimpleBodyList;
SimpleBodyList TheSimpleBodies;

unsigned int SimpleRigidBody::mCount = 0;
SimCollisionMap SimpleRigidBody::mCollisionMap[Sim::MaxSimpleBodies] = {SimCollisionMap()};

SimpleRigidBody::SimpleRigidBody(const BehaviorParams &bp, const RBSimpleParams &params)
    : Behavior(bp, 0),       //
      IRigidBody(bp.fowner), //
      ISimpleBody(bp.fowner) {
    TheSimpleBodies.AddTail(this);
    this->MakeDebugable(DBG_RIGIDBODY);

    this->mData->index = AssignSlot();
    this->mData->position = params.finitPos;
    this->mData->linearVel = params.finitVel;
    this->mData->angularVel = params.finitAngVel;
    this->mData->radius = params.finitRadius;
    this->mData->mass = params.finitMass;
    this->mData->flags = 0;

    this->mCollisionMap[this->mData->index].Clear();
    this->mMaps[this->mData->index] = this;
    this->mCount++;
    UMath::Matrix4ToQuaternion(params.finitMat, this->mData->orientation);
}

SimpleRigidBody::~SimpleRigidBody() {
    TheSimpleBodies.Remove(this);
    this->mCollisionMap[this->mData->index].Clear();
    this->mMaps[this->mData->index] = nullptr;
    this->mCount--;
}

Behavior *SimpleRigidBody::Construct(const BehaviorParams &params) {
    const RBSimpleParams sp(params.fparams.Fetch<RBSimpleParams>(UCrc32(UCRC32_BASE)));
    return new SimpleRigidBody(params, sp);
}

SimpleRigidBody::Volatile::Volatile() {}

ISimable *SimpleRigidBody::GetOwner() const {
    return Behavior::GetOwner();
}

void SimpleRigidBody::RecalcOrientMat(UMath::Matrix4 &resultMat4) const {
    UMath::QuaternionToMatrix4(mData->orientation, resultMat4);
}

void SimpleRigidBody::GetForwardVector(UMath::Vector3 &vec) const {
    UMath::ExtractZAxis(this->mData->orientation, vec);
}

void SimpleRigidBody::GetRightVector(UMath::Vector3 &vec) const {
    UMath::ExtractXAxis(this->mData->orientation, vec);
}

void SimpleRigidBody::GetUpVector(UMath::Vector3 &vec) const {
    UMath::ExtractYAxis(this->mData->orientation, vec);
}

void SimpleRigidBody::SetOrientation(const UMath::Matrix4 &orientMat) {
    UMath::Matrix4ToQuaternion(orientMat, this->mData->orientation);
}

void SimpleRigidBody::SetOrientation(const UMath::Vector4 &orientation) {
    this->mData->orientation = orientation;
}

float SimpleRigidBody::GetScalarVelocity() const {
    return UMath::Length(this->mData->linearVel);
}

void SimpleRigidBody::Accelerate(const UMath::Vector3 &a, float dT) {
    Volatile &data = *this->mData;
    if (data.flags & 8) {
        UMath::ScaleAdd(a, dT, data.linearVel, data.linearVel);
    }
}

void SimpleRigidBody::ApplyFriction() {
    Volatile &data = *this->mData;
    if (data.flags & 4) {
        UMath::Scale(data.linearVel, 0.99f, data.linearVel);
        UMath::Scale(data.angularVel, 0.98f, data.angularVel);
    }
}

void SimpleRigidBody::OnDebugDraw() {}

void SimpleRigidBody::DoIntegration(const float dT) {
    Volatile &data = *this->mData;
    this->ApplyFriction();
    if (data.flags & 8) {
        UMath::ScaleAdd(data.linearVel, dT, data.position, data.position);
    }
    if (data.flags & 0x200) {
        UMath::Vector4 aVel = {};
        UMath::Scale(data.angularVel, dT, *reinterpret_cast<UMath::Vector3 *>(&aVel));
        UMath::Vector4 temp_quat;
        UMath::Mult(aVel, data.orientation, temp_quat);
        UMath::ScaleAdd(temp_quat, 0.5f, data.orientation, data.orientation);
        UMath::Unit(data.orientation, data.orientation);
    }
}

void SimpleRigidBody::DoSRBCollisions(SimpleRigidBody *other) {
    Volatile &data = *this->mData;

    if ((this->mData->flags | other->mData->flags) & 0x4000) {
        if (other->GetSimableType() == this->GetSimableType()) {
            return;
        }
    }

    SimCollisionMap &cmap = this->mCollisionMap[this->GetIndex()];
    SimCollisionMap &cmapother = this->mCollisionMap[other->GetIndex()];

    UMath::Vector3 posSRB = other->GetPosition();
    float radiusSRB = UMath::Max(0.25f, other->GetRadius());
    float thisRadius = UMath::Max(0.25f, this->GetRadius());
    UMath::Vector3 vec;

    UMath::Sub(this->GetPosition(), posSRB, vec);

    float distSquared = UMath::LengthSquare(vec);
    float testRadius = thisRadius + radiusSRB;

    if (distSquared >= testRadius * testRadius) {
        return;
    }

    if (((this->mData->flags & 0x80) != 0) && ((other->mData->flags & 0x80) != 0)) {
        OBB obbThis;
        OBB obbOther;
        UMath::Matrix4 orientThis;
        UMath::Matrix4 orientOther;

        UMath::QuaternionToMatrix4(this->GetOrientation(), orientThis);
        UMath::QuaternionToMatrix4(other->GetOrientation(), orientOther);

        const UMath::Vector3 &posThis = this->GetPosition();
        UMath::Vector3 &posOther = posSRB;

        UMath::Vector3 dimThis;
        dimThis.x = dimThis.y = thisRadius;
        dimThis.z = thisRadius * this->GetScalarVelocity();

        UMath::Vector4 dimOther;
        dimOther.x = dimOther.y = dimOther.z = radiusSRB;
        dimOther.z *= other->GetScalarVelocity();

        obbThis.Reset(orientThis, posThis, dimThis);
        obbOther.Reset(orientOther, posOther, reinterpret_cast<UMath::Vector3 &>(dimOther));

        if (obbThis.CheckOBBOverlap(&obbOther)) {
            cmap.SetCollisionWithSRB(other->GetIndex());
            cmapother.SetCollisionWithSRB(this->GetIndex());
        }
    } else {
        cmap.SetCollisionWithSRB(other->GetIndex());
        cmapother.SetCollisionWithSRB(this->GetIndex());
    }
}

void SimpleRigidBody::DoRBCollisions(const float dT) {
    SimCollisionMap &cmap = this->mCollisionMap[this->GetIndex()];
    Volatile &data = *this->mData;
    Dynamics::Collision::Geometry geomSRB;
    const float thisRadius = UMath::Max(0.25f, this->GetRadius());
    UVector3 dimSRB(thisRadius, thisRadius, thisRadius);

    if (data.flags & 0x8000) {
        geomSRB.Set(UMath::Matrix4::kIdentity, this->GetPosition(), dimSRB, Dynamics::Collision::Geometry::SPHERE,
                    UVector3(this->GetLinearVelocity()) * dT);
    } else if (data.flags & 0x40) {
        UMath::Matrix4 orientSimple;

        UMath::QuaternionToMatrix4(this->GetOrientation(), orientSimple);

        geomSRB.Set(orientSimple, this->GetPosition(), dimSRB, Dynamics::Collision::Geometry::BOX, UVector3(this->GetLinearVelocity()) * dT);
    }

    for (RigidBody *body = TheRigidBodies.GetHead(); body != TheRigidBodies.EndOfList(); body = body->GetNext()) {
        if (!body->IsModeling()) {
            continue;
        }

        bool useobb = (data.flags & 0x8040) != 0;
        const float vdist = useobb ? UMath::Distance(this->GetLinearVelocity(), body->GetLinearVelocity()) * dT : 0.0f;
        const float distSquared = UMath::DistanceSquare(this->GetPosition(), body->GetPosition());
        const float testRadius = thisRadius + body->GetRadius() + vdist;

        if (distSquared >= testRadius * testRadius) {
            continue;
        }

        if (useobb) {
            Dynamics::Collision::Geometry geomRB(body->GetMatrix4(), body->GetPosition(), body->GetDimension(), Dynamics::Collision::Geometry::BOX,
                                                 UVector3(body->GetLinearVelocity()) * dT);

            if (!Dynamics::Collision::Geometry::FindIntersection(&geomRB, &geomSRB, &geomSRB)) {
                continue;
            }

            cmap.SetCollisionWithRB(body->GetIndex());
        } else {
            cmap.SetCollisionWithRB(body->GetIndex());
        }
    }
}

// STRIPPED
void SimpleRigidBody::SetCanHitTrigger(bool canHit) {}

void SimpleRigidBody::GetPointVelocity(const UMath::Vector3 &worldPt, UMath::Vector3 &pv) const {
    const Volatile &data = *this->mData;
    UMath::Vector3 worldArm;
    UMath::Sub(worldPt, data.position, worldArm);
    UMath::Cross(data.angularVel, worldArm, pv);
    UMath::Add(pv, data.linearVel, pv);
}

void SimpleRigidBody::PlaceObject(const UMath::Matrix4 &orientMat, const UMath::Vector3 &initPos) {
    this->SetPosition(initPos);
    this->SetOrientation(orientMat);
    this->SetAngularVelocity(UMath::Vector3::kZero);
    this->SetLinearVelocity(UMath::Vector3::kZero);
}

void SimpleRigidBody::ConvertLocalToWorld(UMath::Vector3 &val, bool translate) const {
    const Volatile &data = *this->mData;
    UMath::Rotate(val, data.orientation, val);
    if (translate) {
        UMath::Add(val, data.position, val);
    }
}

void SimpleRigidBody::ConvertWorldToLocal(UMath::Vector3 &val, bool translate) const {
    const Volatile &data = *this->mData;
    UMath::Vector4 invorient;

    if (translate) {
        UMath::Sub(val, data.position, val);
    }
    UMath::Transpose(data.orientation, invorient);
    UMath::Rotate(val, invorient, val);
}

void SimpleRigidBody::Resolve(const UMath::Vector3 &force, const UMath::Vector3 &torque) {
    this->ResolveForce(force);
}

void SimpleRigidBody::ResolveForce(const UMath::Vector3 &force) {
    if (this->CheckAnyFlags(8)) {
        UMath::Vector3 vec;
        UMath::Scale(force, 1.0f / GetMass(), vec);
        this->Accelerate(vec, Sim::GetTimeStep());
    }
}

// huh?
void SimpleRigidBody::ResolveForce(const UMath::Vector3 &force, const UMath::Vector3 &) {
    this->ResolveForce(force);
}

void SimpleRigidBody::ResolveTorque(const UMath::Vector3 &torque) {}

unsigned int SimpleRigidBody::GetTriggerFlags() const {
    if (!this->CanHitTrigger()) {
        return 0;
    }

    bool isplayer = this->GetOwner()->IsPlayer();
    unsigned int flag = 0x40;

    if (this->GetOwner()->GetSimableType() == SIMABLE_EXPLOSION) {
        flag = 0x50;
    }

    if (this->GetOwner()->GetSimableType() == SIMABLE_HUMAN) {
        flag |= 0x10000;

        if (!isplayer) {
            flag |= 8;
        }
    }

    if (isplayer) {
        flag |= 4;
    }

    return flag;
}

void SimpleRigidBody::Update(const float dT, void *workspace) {
    ScratchPtr<Volatile>::Push(workspace);

    bTList<SimpleRigidBody>::iterator beginIter = TheSimpleBodies.begin();
    bTList<SimpleRigidBody>::iterator endIter = TheSimpleBodies.end();
    bTList<SimpleRigidBody>::iterator iter = beginIter;

    for (; iter != endIter; ++iter) {
        (*iter)->DoIntegration(dT);
    }

    for (int i = 0; i < Sim::MaxSimpleBodies; ++i) {
        mCollisionMap[i].Clear();
    }

    for (iter = beginIter; iter != endIter; ++iter) {
        SimpleRigidBody *srb = *iter;

        if (srb->CanCollideWithRB()) {
            srb->DoRBCollisions(dT);
        }

        if (srb->CanCollideWithSRB()) {
            bTList<SimpleRigidBody>::iterator iternext = iter;

            ++iternext;

            for (; iternext != endIter; ++iternext) {
                SimpleRigidBody *srbother = *iternext;

                if (srbother->CanCollideWithSRB()) {
                    srb->DoSRBCollisions(srbother);
                }
            }
        }
    }

    ScratchPtr<Volatile>::Pop();
}

IRigidBody *SimpleRigidBody::Get(unsigned int index) {
    if (index < NUM_ELEMENTS(mMaps)) {
        return mMaps[index];
    }
    return nullptr;
}

unsigned int SimpleRigidBody::AssignSlot() {
    for (std::size_t i = 0; i < NUM_ELEMENTS(mMaps); ++i) {
        if (mMaps[i] == nullptr) {
            return i;
        }
    }
    return static_cast<unsigned int>(-1);
}
