#include "SimpleRigidBody.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Main/ScratchPtr.h"
#include "Speed/Indep/Src/Sim/Simulation.h"

SimpleRigidBody::~SimpleRigidBody() {
    TheSimpleBodies.Remove(this);
    mCollisionMap[mData->index].Clear();
    mCount--;
    mMaps[mData->index] = nullptr;
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

// TODO
unsigned int SimpleRigidBody::GetTriggerFlags() const {}

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
