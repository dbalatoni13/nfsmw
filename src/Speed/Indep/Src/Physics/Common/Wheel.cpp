#include "Speed/Indep/Src/Physics/Wheel.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"

Wheel::Wheel(unsigned int flags)
    : mWorldPos(0.025f),                //
      mNormal(UMath::Vector4::kZero),   //
      mPosition(UMath::Vector3::kZero), //
      mFlags(flags),                    //
      mForce(UMath::Vector3::kZero),    //
      mAirTime(0.0f),                   //
      mLocalArm(UMath::Vector3::kZero), //
      mCompression(0.0f),               //
      mWorldArm(UMath::Vector3::kZero), //
      mVelocity(UMath::Vector3::kZero), //
      mSurface(SimSurface::kNull),      //
      mSurfaceStick(0.0f),              //
      mIntegral(UMath::Vector4::kZero) {}

void Wheel::UpdateTime(float dT) {
    if (this->mSurfaceStick > 0.0f && dT < this->mSurfaceStick) {
        this->mSurfaceStick = this->mSurfaceStick - dT;
        return;
    }
    this->mSurfaceStick = 0.0f;
}

void Wheel::UpdateSurface(const SimSurface &surface) {
    if (this->mSurfaceStick <= 0.0f) {
        this->mSurface = surface;
        surface.STICK();
    } else {
        if (!(surface == this->mSurface)) {
            return;
        }
    }
    this->mSurfaceStick = surface.STICK();
    this->mSurface.DebugOverride();
}

void Wheel::Reset() {
    this->mIntegral = UMath::Vector4::kZero;
    this->mSurfaceStick = 0.0f;
    this->mAirTime = 0.0f;
    this->mVelocity = UMath::Vector3::kZero;
    this->mCompression = 0.0f;
    this->mNormal = UMath::Vector4::kZero;
    this->mForce = UMath::Vector3::kZero;
    this->mSurface = SimSurface::kNull;
    this->mWorldPos = WWorldPos(0.025f);
}

bool Wheel::InitPosition(const IRigidBody &rb, float maxcompression) {
    UMath::Matrix4 matrix;
    rb.GetMatrix4(matrix);
    matrix.v3 = UMath::Vector4Make(rb.GetPosition(), 1.0f);
    return this->UpdatePosition(rb.GetAngularVelocity(), rb.GetLinearVelocity(), matrix, UMath::Vector3::kZero, 0.0f, maxcompression, false,
                                rb.GetWCollider(), rb.GetDimension().y * 2.0f);
}

bool Wheel::UpdatePosition(const UMath::Vector3 &body_av, const UMath::Vector3 &body_lv, 
    const UMath::Matrix4 &body_matrix, const UMath::Vector3 &cog,
    float dT, float wheel_radius, bool usecache, const WCollider *collider, float vehicle_height) {
    UMath::Rotate(this->mLocalArm, body_matrix, this->mWorldArm);
    UMath::Add(this->mWorldArm, UMath::Vector4To3(body_matrix.v3), this->mPosition);

    UMath::Vector3 pVel;
    UMath::Vector3 pos;
    UMath::Sub(this->mWorldArm, cog, pos);
    UMath::Cross(body_av, pos, pVel);
    UMath::Add(pVel, body_lv, pVel);
    SetVelocity(pVel);

    UMath::Add(this->mWorldArm, UMath::Vector4To3(body_matrix.v3), this->mPosition);

    float tolerance = UMath::Max(-pVel.y * dT, 0.0f) + wheel_radius + UMath::Lengthxz(pVel) * dT;
    float prev = vehicle_height * 0.5f;
    this->mWorldPos.SetTolerance(UMath::Min(tolerance, prev));

    bool result = this->mWorldPos.Update(this->mPosition, this->mNormal, IsOnGround() && usecache, collider, true);
    UpdateSurface(SimSurface(this->mWorldPos.GetSurface()));
    return result;
}
