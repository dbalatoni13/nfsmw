#include "Speed/Indep/Src/Camera/ICE/ICEAnchor.hpp"
#include "Speed/Indep/Src/Camera/ICE/ICEMath.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

void ICEAnchor::Update(float dT, const ICE::Matrix4 &orientpos, const ICE::Vector3 &velocity, const ICE::Vector3 &acceleration) {

    float distance = ICE::DistBetween(&mGeomPos, reinterpret_cast<const ICE::Vector3 *>(&orientpos.v3));

    ICE::Copy(&mGeomRot, &orientpos);

    mGeomRot.v3.x = mGeomRot.v3.y = mGeomRot.v3.z = 0.0f;

    ICE::Copy(&mGeomPos, reinterpret_cast<const ICE::Vector3 *>(&orientpos.v3));

    ICE::Copy(&mVelocity, &velocity);

    float savedVelMag = mVelMag;
    mVelMag = ICE::Length(&mVelocity);

    if (dT > 0.0f && (distance / dT) < 300.0f) {

        ICE::Vector3 acc((mVelMag - savedVelMag) / dT, 0.0f, 0.0f);

        ICE::MulMatrix(&mAccel, &mGeomRot, &acc);

    } else {

        mAccel.x = mAccel.y = mAccel.z = 0.0f;
    }
}

ICEAnchor::ICEAnchor() : mGeomPos(0.0f, 0.0f, 0.0f), mVelocity(0.0f, 0.0f, 0.0f), mAccel(0.0f, 0.0f, 0.0f) {

    mVelMag = 0.0f;
    mTopSpeed = 0.0f;
    mRPM = 0.0f;
    mNumWheels = 0;
    mForwardSlip = 0.0f;
    mSlipAngle = 0.0f;
    mIsTouchingGround = true;
    mIsNosEngaged = false;
    mNosPercentageLeft = 0.0f;

    ICE::Identity(&mGeomRot);
}
