#include "Speed/Indep/Src/Physics/Wheel.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"

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
