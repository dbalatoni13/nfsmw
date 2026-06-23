#include "SimpleRigidBody.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Generated/Hash.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimpleBody.h"
#include "Speed/Indep/Src/Main/ScratchPtr.h"
#include "Speed/Indep/Src/Physics/Behavior.h"
#include "Speed/Indep/Src/Physics/PhysicsObject.h"
#include "Speed/Indep/Src/Sim/SimTypes.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/bWare/Inc/bTypes.hpp"

BIND_BEHAVIOR_FACTORY(SimpleRigidBody);
IMPLEMENT_SCRATCHPTR(SimpleRigidBody::Volatile);

typedef bTList<SimpleRigidBody> SimpleBodyList;
SimpleBodyList TheSimpleBodies;

SimCollisionMap SimpleRigidBody::mCollisionMap[SimpleRigidBody::Volatile::MaxInstances];

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
        // TODO UNSOLVED
        UMath::Mult(aVel, data.orientation, temp_quat);
        UMath::ScaleAdd(temp_quat, 0.5f, data.orientation, data.orientation);
        UMath::Unit(data.orientation, data.orientation);
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

// TODO
unsigned int SimpleRigidBody::GetTriggerFlags() const {
    return 0;
}

void SimpleRigidBody::Update(const float dT, void *workspace) {
    // TODO really ugly output
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
