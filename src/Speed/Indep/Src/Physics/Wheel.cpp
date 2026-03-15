#include "Wheel.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"

Wheel::Wheel(unsigned int flags) : mWorldPos(0.025f) //
    , mNormal(UMath::Vector4::kZero) //
    , mPosition(UMath::Vector3::kZero) //
    , mFlags(flags) //
    , mForce(UMath::Vector3::kZero) //
    , mAirTime(0.0f) //
    , mLocalArm(UMath::Vector3::kZero) //
    , mCompression(0.0f) //
    , mWorldArm(UMath::Vector3::kZero) //
    , mVelocity(UMath::Vector3::kZero) //
    , mSurface(SimSurface::kNull) //
    , mSurfaceStick(0.0f) //
    , mIntegral(UMath::Vector4::kZero) {
}

void Wheel::UpdateTime(float dT) {
    if (mSurfaceStick > 0.0f && dT < mSurfaceStick) {
        mSurfaceStick = mSurfaceStick - dT;
        return;
    }
    mSurfaceStick = 0.0f;
}

void Wheel::UpdateSurface(const SimSurface &surface) {
    if (mSurfaceStick <= 0.0f) {
        mSurface = surface;
        surface.STICK();
    } else {
        if (!(surface == mSurface)) {
            return;
        }
    }
    mSurfaceStick = surface.STICK();
    mSurface.DebugOverride();
}

void Wheel::Reset() {
    mIntegral = UMath::Vector4::kZero;
    mSurfaceStick = 0.0f;
    mAirTime = 0.0f;
    mVelocity = UMath::Vector3::kZero;
    mCompression = 0.0f;
    mNormal = UMath::Vector4::kZero;
    mForce = UMath::Vector3::kZero;
    mSurface = SimSurface::kNull;
    mWorldPos = WWorldPos(0.025f);
}

bool Wheel::InitPosition(const IRigidBody &rb, float maxcompression) {
    UMath::Matrix4 matrix;
    rb.GetMatrix4(matrix);
    const UMath::Vector3 &pos = rb.GetPosition();
    matrix.v3 = UMath::Vector4Make(pos, 1.0f);
    const UMath::Vector3 &body_av = rb.GetAngularVelocity();
    const UMath::Vector3 &body_lv = rb.GetLinearVelocity();
    const WCollider *collider = rb.GetWCollider();
    UMath::Vector3 dim = rb.GetDimension();
    return UpdatePosition(body_av, body_lv, matrix, UMath::Vector3::kZero, 0.0f, maxcompression, false, collider, dim.y + dim.y);
}

bool Wheel::UpdatePosition(const UMath::Vector3 &body_av, const UMath::Vector3 &body_lv, 
    const UMath::Matrix4 &body_matrix, const UMath::Vector3 &cog,
    float dT, float wheel_radius, bool usecache, const WCollider *collider, float vehicle_height) {
    UMath::Rotate(mLocalArm, body_matrix, mWorldArm);
    UMath::Add(mWorldArm, UMath::Vector4To3(body_matrix.v3), mPosition);

    UMath::Vector3 pVel;
    UMath::Vector3 pos;
    UMath::Sub(mWorldArm, cog, pos);
    UMath::Cross(body_av, pos, pVel);
    UMath::Add(pVel, body_lv, pVel);
    SetVelocity(pVel);

    UMath::Add(mWorldArm, UMath::Vector4To3(body_matrix.v3), mPosition);

    float tolerance = UMath::Max(-pVel.y * dT, 0.0f) + wheel_radius + UMath::Lengthxz(pVel) * dT;
    float prev = vehicle_height * 0.5f;
    mWorldPos.SetTolerance(UMath::Min(tolerance, prev));

    bool result = mWorldPos.Update(mPosition, mNormal, IsOnGround() && usecache, collider, true);
    UpdateSurface(SimSurface(mWorldPos.GetSurface()));
    return result;
}
